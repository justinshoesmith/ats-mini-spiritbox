# Native Spirit Box mode

**Installed experimental v2.45 SB-dev OSPI build.** Entry, drawing, FM/MW tuning, state persistence/reboot and owner-operated controls/Exit have been checked. See `native-installation.md` for actual results and remaining limitations; `build.md` records source, sizes, warnings and artifacts.

The installed custom build shows `SB-dev` in its About version string. The normal serial protocol retains the upstream numeric version for compatibility.

## Controls

Open the existing main menu and select **Spirit Box**. The mode enters stopped, on FM 87.50 MHz, forward direction, 100 kHz steps and a nominal 100 ms interval.

- Turn the encoder to select Run/Stop, Band, Interval, Direction, Step, Volume or Exit.
- Click Run/Stop to start or stop; click Exit to return to normal radio.
- Click another field to edit, turn to change it, then click to finish. An asterisk marks editing. After ten seconds without input, editing ends but the mode stays open.
- Short press starts/stops sweeping. Long press exits the mode and uses the radio's normal sleep action.
- Band offers FM or AM/MW. Changing it stops the sweep; start again when ready.

FM sweeps 87.5–108 MHz, with 100 or 50 kHz steps. MW sweeps 531–1602 kHz, with 9 or 1 kHz steps. Direction wraps at the last aligned frequency. Interval choices are 100, 150, 200, 250, 300 and 350 ms. These are nominal start-to-start command intervals, not guaranteed audible dwell times. Display and receiver work may lengthen them.

Volume changes apply to the ordinary radio volume and are saved normally. User mute is respected. AM soft-mute remains at its existing setting; no synthetic noise is added. Squelch is bypassed only inside this operating mode; its stored thresholds are not changed. Reception depends on the antenna and local broadcasts. This is an experimental frequency-sweeping radio, not an established paranormal communication method.

## Returning to radio

Exit stops the sweep, restores the previous band/frequency, band settings and normal squelch behavior, and removes the purple display override. The existing theme preference and layout stay unchanged. The mode does not replace or alter the boot splash.

Sleep, Off Timer shutdown, remote commands that change ordinary radio settings, queued web configuration, DSP patch selection and firmware updates also leave the mode first. USB/BLE encoder gestures operate the mode controls; screenshots, logging and memory reads remain available. Web memory-slot edits do not retune the radio and can still be saved without leaving the mode. Networking remains optional, and operation is fully offline.

Sweep intervals, step selections, direction, Run state and temporary tuning are session-only. Saves while sweeping retain the original normal-radio band/VFO state, including when another setting is saved. Reboot starts normal radio, not a running sweep.

## Validation still required

The OSPI build passed with two warnings in unchanged PU2CLR library code and was subsequently installed with explicit approval. Native FM audio is reported as some fragments with gaps/static; native AM audio still needs a separate listening check. Additional acceptance remains for entry/exit from every normal modulation, both layouts, all interval/step combinations, mute/squelch, sleep/off-timer and web/remote interactions. These cases must not be inferred from the successful basic persistence/reboot and physical-control checks. Future physical flashes still require explicit approval of the prepared build.
