# Spirit Box integration investigation

30 October 2026. Source inspected: upstream v2.45, with PU2CLR SI4735 tag 2.1.8. **Native mode is designed, not implemented.** The installed radio remains official v2.45 OSPI. No compile or custom flash was performed. Owner confirmed completion of the UTC+10 correction.

## Recommendation

Add one Spirit Box item to the existing main menu and one small `SpiritBox.cpp/.h` module. Use a main-loop timer to advance one frequency at a time. Reuse receiver setup, normal tuning, encoder/button dispatch, sprite rendering and the existing theme fields. Keep all sweep preferences session-local initially. No new library, task, timer interrupt, network requirement or hardware is needed.

Start with FM 87.5–108 MHz, 100 kHz steps, forward sweep, 200 ms step interval, initially stopped. Offer 100/150/200/250/300/350 ms intervals. FM 50 kHz and MW 9 kHz steps are useful first alternatives; optional finer steps can follow measurements. Use a clear interval label: time between tuning commands, including tuning overhead, rather than a promise of that much audible dwell. This is an engineering starting point, not a measured minimum receiver settling time.

Use existing VHF and MW2 receiver setup without changing their stored band definitions. A suggested Australian MW window is 531–1602 kHz in 9 kHz steps; a later 10 kHz-region option should use its own aligned endpoints. Mode-specific limits must stay within the selected upstream band's limits. Wrap last valid grid point to first valid grid point, in either direction, without an off-grid endpoint. Do not sweep SSB in the first version. A single Si4732 tunes one frequency/band at a time; simultaneous AM/FM is outside scope.

## Source findings and integration points

| Concern | Existing code and consequence |
| --- | --- |
| Mode entry | `Menu.cpp` main-menu table and `clickMenu()`, command definitions in `Menu.h`. Add an operating command, not a new demodulation value alongside FM/AM/LSB/USB. |
| Tuning | `ats-mini.ino::updateFrequency()` uses Hz, calls the receiver, and updates current band frequency. `Utils.cpp` converts to 10 kHz FM / 1 kHz AM receiver units. Reuse conversions and bounds. |
| Band setup | `Menu.cpp::selectBand()` and `ats-mini.ino::useBand()` configure demodulation, patch state, bandwidth, antenna/GPIO routing, AGC and audio. Do not switch between FM and AM by sending only a raw frequency. |
| Existing Scan | `Scan.cpp::scanRun()` is a blocking signal-strength scan, mutes audio, samples 200 points and restores the prior frequency. It is unsuitable as the audible continuous-sweep engine. |
| Tune latency | PU2CLR 2.1.8 `SI4735::setFrequency()` waits for command readiness and delays 30 ms by default; it does not prove final audio settling. Existing signal scan uses 60 ms FM / 80 ms AM/SSB plus tune-status polling. |
| Event loop | `loop()` polls buttons and USB/BLE/TCP, applies web operations, processes signal/RDS/schedule work and saves preferences. Advance at most one sweep step per iteration; never run a whole-band loop or catch-up burst. |
| Controls | `handleRotation()` and `handleClick()` serve physical and remote input. Scope their Spirit Box handling to the active mode; retain ordinary radio behavior elsewhere. |
| Theme | `Themes.h` defines `TH` as `theme[themeIdx]`. `Storage.cpp` persists `themeIdx`. Add an effective display-theme accessor that returns a dedicated palette only while active; leave the stored index and palette array untouched. |
| Graphics | Existing LovyanGFX sprite, fonts and RGB565 colours suffice. Draw a tiny static ghost with primitives or a short monochrome bitmap; no image-generation/conversion pipeline is necessary. `Splash.cpp` remains separate. |
| Persistence | `prefsSave()` stores globals, band frequencies, VFO slots and memories, and can be called from shutdown. Pending saves may execute after 10 seconds even without new requests. Avoiding save requests alone is insufficient. |
| Connectivity | `Network.cpp` supports Off, AP and client modes; `Web.cpp` queues settings work onto the main task. USB, TCP and BLE share remote command behavior. Keep these facilities serviced; no wireless dependency for sweeping. |

## State and restoration design

Treat mode entry/exit as a small transaction. Before applying sweep state, flush any pending ordinary settings and capture current band/mode/frequency, relevant band records, both VFO slots and active index, bandwidth/step, audio/DSP state, sleep policy and user mute. Do not treat `captureVfo()` as a complete snapshot: it omits themes, mute, squelch, soft-mute and other settings.

Keep a working sweep frequency separate from saved normal-radio state where practical. If tuning temporarily changes shared band/current-frequency fields, persistence must explicitly serialize the captured normal band/VFO/global state while active. Cover direct `prefsSave()` calls as well as deferred `prefsTickTime()`, including shutdown. Avoid globally disabling saves, which would also discard legitimate unrelated settings changes. Do not bump settings-layout versions merely to store temporary sweep state.

On explicit exit, stop advancing first, restore normal band/VFO/tuning and receiver audio settings, remove the display override, clear stale station information, and redraw. Failure to enter must unwind the same changes. On reboot always start in normal radio, with the last normal state persisted. Sleep should stop sweeping and restore normal state before invoking the existing sleep behavior; preserve the user's sleep configuration rather than persisting a disabled value. Off Timer continues to work through the same restoration path.

Remote commands that change frequency, band, mode, memories or receiver settings should leave Spirit Box cleanly before the existing command runs, so the sweep cannot immediately overwrite them. Apply the same policy to queued web mutations. Read-only screenshots and telemetry can remain active. Theme editor commands should exit the temporary theme first, so they continue targeting the user's actual palette. Dedicated remote Spirit Box commands are a later enhancement.

## Controls and display

Use an existing-style field list: Run/Stop, Band, Interval, Direction, Step, Volume, Exit. Rotation selects a field; click activates/edits it; rotation edits; click returns to selection. Short press provides Stop/Start when not editing, with explicit Exit always visible as an option. Keep long press available for existing sleep behavior through clean mode exit. Avoid timeout silently dropping the command into normal tuning while the sweep continues; retain the active-mode context while the field editor can time out back to its mode screen.

Display a clear Spirit Box title, readable frequency/unit, Running/Stopped, direction and interval. Reuse existing fonts and status indicators. Dark purple/black with high-contrast light text and a small ghost provides identity without motion. The temporary theme must never appear as the saved global theme or mutate a shared user palette. Use the mode-specific draw path for either normal layout preference and restore the prior layout unchanged on exit.

## Audio behavior

Do not call `muteOn()` on every step: it has explicit 50 ms delays and controls the hardware clamp/amplifier. Respect user mute, and do not attempt to synthesize white noise. The receiver's own tuning behavior determines the audio fragments and noise heard.

The first FM USB trial was audible throughout with the existing receiver defaults. This supports leaving that tuning path unchanged initially. If needed, mode-only FM soft-mute can use `setFmSoftMuteMaxAttenuation(0)` and AM can use `setAmSoftMuteMaxAttenuation(0)`. Snapshot/read the actual FM property before overriding it rather than assuming a default; reapply the user's AM setting on exit. Bypass squelch only for the mode's sweep, clear its active squelch mute on entry if necessary, and restore normal threshold behavior on exit. Retain user volume/mute safety and AGC; do not force the amplifier on or patch the library's FAST/FREEZE fields without evidence.

## Commercial references

These are documented product controls, not evidence of paranormal communication. Manufacturer manuals contain paranormal claims that are not engineering premises for this project.

| Reference | Documented controls | Useful lesson |
| --- | --- | --- |
| [P-SB7 Rev8 manufacturer manual](https://fccid.io/2AVMZ-P-SB7/User-Manual/User-manual-8105758.pdf), pp. 6–8 | AM/FM, 50/100/150/200/250/300/350 ms rates, forward/reverse and stop | A small interval list, visible state and straightforward direction controls are sufficient. |
| [PSB7-PRO Rev2 manufacturer manual](https://fccid.io/2AVMZ-PSB7-PRO/User-Manual/User-manual-8106155.pdf), pp. 3–6 | 17 rates from 30 to 350 ms; FM steps 10/20/40/60/80/100 kHz, AM steps 1/2/4/6/8/10 kHz; mute and sensor-trigger options | Step size and interval are separate controls. Fine steps revisit overlapping portions of a station; extra sensors are unnecessary here. |
| [Manufacturer comparison, P-SB11](https://pro-measure.com/post/p-sb7-vs-p-sb11-vs-psb7-pro) | Describes simultaneous AM/FM, 50–350 ms and forward/reverse | Dual-band simultaneous operation requires different hardware; omit it from the single-receiver scope. |

Commercial maximum speeds do not establish what this Si4732/firmware combination can do. We have not independently measured those commercial radios or verified their internal audio/noise implementation.

## USB experiment and observed evidence

`tools/spiritbox/usb-sweep.py` uses existing v2.45 `F<Hz>` commands and `t` status monitoring. It does not flash firmware or alter themes, volume, soft-mute or band. It accepts explicitly selected Espressif USB ports, only VHF/FM or MW2/AM, aligned in-band endpoints and a maximum 30-second run. It checks telemetry, avoids command bursts, and attempts to restore and verify the starting frequency on completion or Ctrl+C. Disconnect/power loss can prevent restoration; logs and the captured starting state remain under `.local/logs/usb-sweep/`. Existing `F` commands update the upstream band's live state and schedule a save, so this is an operational prototype, not a persistence-free native implementation.

Two five-second trials completed on COM3, 30 October 2026 around 16:05 Brisbane time. Sweep range 90.8–92.0 MHz, 100 kHz steps; starting frequency **106.50 MHz** restored and confirmed by telemetry after each trial.

| Requested interval | Direction | Commands sent | Host send interval min / median / max | Result |
| --- | --- | --- | --- | --- |
| 200 ms | Forward | 25 | 203 / 203 / 204 ms | Status continued; original tuning restored |
| 100 ms | Reverse | 46 | 109 / 109 / 110 ms | Status continued; original tuning restored |

Owner explicitly reported audio audible throughout the FM trials. Host scheduling and serial overhead explain why the observed cadence exceeds the requested interval; no sample-accurate audio or tune-complete measurement was taken. Status updates are only every 500 ms, so these results do not certify every intermediate tuning command, exact receiver latency, all-band coverage, or native-mode timing. Soft-mute/squelch overrides have not been tested.

### MW trial

Two five-second MW2/AM trials completed on COM3 on 30 October 2026 around 16:09 Brisbane time, sweeping 702–810 kHz in 9 kHz steps. At requested 200 ms forward, 25 commands were sent at 203/203/204 ms min/median/max host intervals; at requested 100 ms reverse, 46 commands were sent at 109/109/110 ms. Both restored and verified the saved MW2 frequency of 783 kHz. The owner missed listening to those first trials, so they do not establish an audio result.

The surrounding session returned to the original VHF/FM band and 106.50 MHz, confirmed by fresh telemetry. The first return check failed to confirm the band immediately after batched band commands; a new connection/status query already found VHF/FM, and the original frequency was reasserted and verified. For future multi-band trials, wait for fresh telemetry after each band change rather than checking a buffered status sample. The private session result is `.local/logs/mw-trial/result.json`; individual sweep logs remain in `.local/logs/usb-sweep/`. No firmware write or receiver audio-property change was performed.

Repeated the same range and directions for eight seconds each around 16:17 Brisbane time while the owner listened. The forward trial sent 40 commands, with host intervals 203/203/204 ms; reverse sent 74 commands, at 109/109/110 ms. Each restored 783 kHz, and the surrounding session restored and verified VHF/FM 106.50 MHz with volume 20. Owner reported **audible, with silent gaps**. The report does not distinguish which interval had more gaps or establish their cause. The repeat session result is `.local/logs/mw-trial/repeat-result.json`.

### Controlled MW soft-mute comparison

Completed the approved comparison around 16:21 Brisbane time on 30 October 2026. Menu screenshots confirmed AM soft-mute maximum attenuation **4 dB** and MW squelch **Off (rssi)** before the change. Changed only AM soft-mute to **0 dB**, verified by a second screenshot; squelch remained Off. Repeated the same 702–810 kHz range and 9 kHz steps for eight seconds forward at requested 200 ms, then eight seconds reverse at requested 100 ms.

Forward sent 40 commands at host intervals 203/203/204 ms min/median/max; reverse sent 74 at 109/109/110 ms. Each restored saved MW tuning, 783 kHz. Afterward screenshots verified soft-mute restored to **4 dB** and squelch still **Off (rssi)**. Fresh telemetry then confirmed original VHF/FM **106.50 MHz**, volume **20**. No firmware write was performed. Private evidence is under `.local/logs/mw-softmute/`; trial logs are `20261030T062125442795Z.json` and `20261030T062137000088Z.json` under `.local/logs/usb-sweep/`.

The owner selected the combined answer **"Worse, or I missed it"**. This does not distinguish worsened audio from a missed observation, so the comparison is inconclusive. There is no evidence from this trial to justify disabling AM soft-mute by default; retain the user's existing setting in the first native implementation and make any bypass an explicit experimental choice. Do not conclude that soft-mute caused or fixed the gaps from command/telemetry results: receiver tune settling and reception conditions remain possible contributors. The earlier 4 dB trial was audible with gaps; no audio recording or instrumented latency measurement was taken.

Repeated the soft-mute-zero comparison at the owner's request around 17:30 Brisbane time on 30 October 2026, with the same two eight-second sweeps. Screenshots again confirmed original soft-mute 4 dB, trial value 0 dB and unchanged squelch Off. Forward 200 ms sent 40 commands (203/203/204 ms host intervals); reverse 100 ms sent 74 (109/109/110 ms). Both restored 783 kHz. Screenshots verified soft-mute restored to 4 dB and squelch Off afterward; fresh telemetry verified starting VHF/FM 106.50 MHz and the owner's current volume **30**, unchanged by the experiment. Evidence is under `.local/logs/mw-softmute-repeat/` and the corresponding `20261030T073005176032Z.json` / `20261030T073016729192Z.json` sweep logs.

The owner explicitly reported that the last (100 ms reverse) sweep sounded better, with fewer gaps, but no voices. This supports useful MW audio with soft-mute disabled under those conditions, but does not isolate soft-mute's effect: interval and direction differ between the two passes and no matched, recorded 4 dB/0 dB comparison was made. No conclusion about local station availability or absence can be drawn. Speech depends on tuning through a receivable broadcast carrying speech; sweep noise alone does not establish useful speech reception. Retain the user's default setting until a matched comparison supports changing it. A future FM demonstration around a confirmed speaking station would distinguish speech availability from sweep continuity; earlier FM trials were already reported audible throughout.

## Acceptance before installing a native build

### Additional FM listening trial

At the owner's request, repeated FM sweeping around the current 106.50 MHz tuning on 30 October 2026 around 17:34 Brisbane time. Used 106.20–106.80 MHz, 100 kHz steps, eight seconds at 200 ms forward followed by eight seconds at 100 ms reverse. Both completed with telemetry-confirmed restoration of 106.50 MHz. No audio properties, volume or firmware were changed. The owner instructed proceeding but did not confirm that the centre frequency carried clear speech, so this is not a confirmed speech-station trial. Session evidence is under `.local/logs/fm-centred-trial/`, with sweep logs `20261030T073405174364Z.json` and `20261030T073418737183Z.json`.

Owner rejected the listening result: it sounded mostly like static and long stretches around 106.5 MHz, rather than useful broadcast fragments. Log inspection shows multiple changing frequencies across the requested range in both passes; the radio was not stationary in the sampled status. However, the test comprised only seven frequency points, repeating the same station neighbourhood about every 1.4 seconds at the slower cadence or 0.76 seconds at the faster cadence. Telemetry acquisition and restoration also added stationary periods around the eight-second sweeps. These design choices made this a poor demonstration of a broad-band spirit-box sweep. Sampled tuning changes do not contradict the owner's audio assessment and do not prove useful audible fragments.

Next proposed FM trial: a single forward pass across 87.5–108 MHz in 100 kHz steps at nominal 100 ms, about 22–23 seconds using observed host cadence, with explicit start/finish cues and no repeated narrow loop. Confirm reception at a normal station first if speech fragmentation is the criterion. Do not treat the earlier "audible throughout" report as proof of satisfying audio quality. This wider trial has not yet been performed.

### Remaining checks

1. Retain the user's AM soft-mute setting initially; repeat a clearly observed comparison only if audio optimization needs it. FM is audible throughout at the tested intervals; MW continuity has not yet met that standard. Finish ordinary AM/SSB baseline checks separately.
2. Implement the small native module and integration points above; review the persistence paths before compiling. A future compile requires an explicit build request under root `AGENTS.md`.
3. Check boundaries/wrap, both directions, interval changes, Stop/Start, field timeout, exit, sleep/off-timer, remote/web conflicts, pending saves and power-cycle restoration. Verify global theme/VFO/preferences remain unchanged, including unrelated saves during the mode.
4. With a concrete verified build prepared, obtain explicit approval before any custom physical flash. Test FM and MW audio, controls, offline use, clean exit and ordinary radio regression on the actual unit. No such custom build/flash approval has been inferred from the earlier upstream-update approval.
