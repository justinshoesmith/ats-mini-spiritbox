"""Bounded sweep experiment using the existing v2.45 USB protocol; no flashing."""

import argparse
from datetime import datetime, timezone
import json
import math
from pathlib import Path
import statistics
import time

import serial
from serial.tools import list_ports


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', required=True)
    parser.add_argument('--lower-hz', type=int, required=True)
    parser.add_argument('--upper-hz', type=int, required=True)
    parser.add_argument('--step-hz', type=int, required=True)
    parser.add_argument('--interval-ms', type=int, choices=(100, 150, 200, 250, 300, 350), default=200)
    parser.add_argument('--seconds', type=float, default=8)
    parser.add_argument('--direction', choices=('forward', 'reverse'), default='forward')
    args = parser.parse_args()
    if not math.isfinite(args.seconds) or not 0 < args.seconds <= 30:
        parser.error('--seconds must be greater than zero and at most 30')
    if args.step_hz <= 0 or args.lower_hz >= args.upper_hz:
        parser.error('require positive step and lower < upper')
    if (args.upper_hz - args.lower_hz) % args.step_hz:
        parser.error('range endpoints must align with the step')
    ports = [p for p in list_ports.comports() if p.device.upper() == args.port.upper()]
    if not ports or (ports[0].vid, ports[0].pid) != (0x303A, 0x1001):
        parser.error('select the Espressif USB Serial/JTAG port explicitly')

    stamp = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
    log = Path(__file__).resolve().parents[2] / '.local/logs/usb-sweep' / (stamp + '.json')
    log.parent.mkdir(parents=True, exist_ok=True)
    report = dict(started_utc=stamp, parameters=vars(args), status_samples=[], restored=False)
    # Set line states before opening; do not use the ESP reset sequence.
    port = serial.Serial(port=None, baudrate=115200, timeout=0, write_timeout=2)
    port.dtr = False
    port.rts = False
    port.port = args.port
    buffer = bytearray()
    latest = None
    original = None
    log_enabled_here = False
    sends = []
    latest_at = 0

    def collect(seconds):
        nonlocal latest, latest_at
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            buffer.extend(port.read(port.in_waiting or 1))
            if len(buffer) > 65536:
                raise RuntimeError('Unexpected serial output exceeded the receive buffer limit.')
            while b'\n' in buffer:
                line, _, rest = buffer.partition(b'\n')
                buffer[:] = rest
                text = line.decode('ascii', errors='replace').strip()
                fields = text.split(',')
                if len(fields) == 15 and fields[0].isdigit() and fields[1].isdigit():
                    latest = fields
                    latest_at = time.monotonic()
                    report['status_samples'].append(dict(at=time.monotonic(), fields=fields))
                elif text.startswith('Error') or 'out of range' in text.lower():
                    raise RuntimeError(text)
            time.sleep(.002)

    def tune(hz):
        port.write(f'F{hz}\n'.encode('ascii'))
        port.flush()

    try:
        port.open()
        collect(.7)
        if latest is None:
            port.write(b't')
            port.flush()
            log_enabled_here = True
            collect(1.2)
        if latest is None:
            raise RuntimeError('No status received. Enable Settings -> USB Port -> Ad hoc.')
        original = latest[:]
        if original[0] != '245':
            raise RuntimeError('This prototype targets only the upstream v2.45 protocol.')
        if (original[4], original[5]) == ('VHF', 'FM'):
            low, high, unit = 64000000, 108000000, 10000
        elif (original[4], original[5]) == ('MW2', 'AM'):
            low, high, unit = 495000, 1701000, 1000
        else:
            raise RuntimeError('Select normal VHF/FM or MW2/AM before running; no band switching is performed.')
        if not low <= args.lower_hz < args.upper_hz <= high:
            raise RuntimeError('Requested range exceeds the supported current-band limits.')
        if any(value % unit for value in (args.lower_hz, args.upper_hz, args.step_hz)):
            raise RuntimeError('Frequencies/step must align to receiver units: FM 10 kHz, AM 1 kHz.')
        report['original_status'] = original
        report['original_frequency_hz'] = int(original[1]) * unit
        freq = args.lower_hz if args.direction == 'forward' else args.upper_hz
        step = args.step_hz * (1 if args.direction == 'forward' else -1)
        end = time.monotonic() + args.seconds
        next_send = time.monotonic()
        print(f"Sweeping {original[4]}/{original[5]} for {args.seconds:g}s; Ctrl+C stops and restores.", flush=True)
        while time.monotonic() < end:
            now = time.monotonic()
            if now >= next_send:
                tune(freq)
                sends.append(time.monotonic())
                # No catch-up bursts when Windows scheduling runs late.
                next_send = sends[-1] + args.interval_ms / 1000
                freq += step
                if freq > args.upper_hz:
                    freq = args.lower_hz
                elif freq < args.lower_hz:
                    freq = args.upper_hz
            collect(min(.01, max(0, end - time.monotonic())))
            if latest[4:6] != original[4:6]:
                raise RuntimeError('Band/mode changed during the experiment; stopping.')
            if time.monotonic() - latest_at > 2:
                raise RuntimeError('Receiver status stopped arriving; stopping the sweep.')
    except KeyboardInterrupt:
        report['interrupted'] = True
    except Exception as exc:
        report['error'] = str(exc)
        raise
    finally:
        try:
            if port.is_open and 'original_frequency_hz' in report:
                # Stop the sweep first; allow any last command to drain.
                collect(.2)
                if latest[4:6] != original[4:6]:
                    raise RuntimeError('Band/mode changed; restore manually to the recorded original status.')
                tune(report['original_frequency_hz'])
                latest = None
                collect(1.2)
                report['restored'] = latest is not None and latest[1:6] == original[1:6]
                if not report['restored']:
                    raise RuntimeError('Original frequency restoration was not confirmed; inspect the radio.')
                print('Original tuning restored and verified by receiver telemetry.', flush=True)
        except Exception as exc:
            report['restoration_error'] = str(exc)
            raise
        finally:
            if port.is_open:
                try:
                    if log_enabled_here:
                        port.write(b't')
                        port.flush()
                finally:
                    port.close()
            gaps = [(b - a) * 1000 for a, b in zip(sends, sends[1:])]
            report['commands_sent'] = len(sends)
            if gaps:
                report['host_send_interval_ms'] = dict(min=min(gaps), median=statistics.median(gaps), max=max(gaps))
            report['limitation'] = 'Host command cadence and 500 ms status samples; not RF tune-complete or audio timing.'
            log.write_text(json.dumps(report, indent=2), encoding='utf-8')
            print(f'Experiment log: {log}', flush=True)


if __name__ == '__main__':
    main()
