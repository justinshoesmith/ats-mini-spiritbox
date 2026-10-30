# Spirit Box tools

Run commands from the repository root. Device data and results belong under ignored `.local/`.

- `Backup-Flash.ps1`: full original-flash capture, size/SHA-256 verification and outside-repository recovery copy. See `docs/spiritbox/backup-recovery.md`.
- `usb-sweep.py`: bounded audible tuning experiment using official v2.45 firmware. Uses pyserial already installed in the local esptool environment. No compilation or flashing.

## USB sweep trial

Power on, connect a USB data cable, enable Settings -> USB Port -> Ad hoc, and select normal VHF/FM (or MW2/AM) using the radio. Keep volume comfortable. Leave the controls untouched while the trial runs. Check the actual USB port:

```powershell
& ./.local/esptool/Scripts/python.exe -m serial.tools.list_ports -v
& ./.local/esptool/Scripts/python.exe tools/spiritbox/usb-sweep.py --port COM3 --lower-hz 90800000 --upper-hz 92000000 --step-hz 100000 --interval-ms 200 --seconds 5
```

Add `--direction reverse` for reverse sweep. The upper endpoint must align with the lower endpoint and step. The utility does not switch bands. For MW2/AM, a short Australian-grid example is `--lower-hz 702000 --upper-hz 810000 --step-hz 9000 --interval-ms 200 --seconds 5`. Execution and restoration have been exercised on the radio; the owner reports MW audio with silent gaps. No soft-mute/squelch overrides were applied. See the integration record for observations and next measurements.

Ctrl+C stops and attempts restoration, as does normal completion. Wait for the restoration result and check the displayed tuning. It preserves detected monitor enablement on the current connection, but disconnecting can reset the firmware's per-session monitor state. A USB disconnect or power failure may prevent restoration; inspect the starting status in the JSON log and restore manually if necessary. Upstream's normal tuning commands schedule band/VFO persistence; the prototype cannot guarantee recovery after forced termination.

Reports under `.local/logs/usb-sweep/` contain command counts, host timing, sampled receiver status and restoration results. Host timing is not receiver/audio latency. See `docs/spiritbox/integration.md` for actual trials and design conclusions.
