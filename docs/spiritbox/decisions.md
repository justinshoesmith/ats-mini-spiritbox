# Scope and decisions

Recorded 30 October 2026.

1. Initial bootstrap covered Phase 1. The owner subsequently reported the Google Drive recovery copy and approved proceeding with Phase 2. Physical backup remains a hard prerequisite for upgrade work. Never erase or flash without a verified original backup, secured external copy and explicit user approval for the physical update.
2. Start custom work from stable upstream v2.45. Preserve full Git history, MIT license, attribution and source structure. Keep project work on `spiritbox`; keep upstream maintenance separate.
3. Treat AMNVOLT V4 branding and owner-reported hardware as inventory evidence, not proof of flash capacity, PSRAM bus, display type or recovery-button layout. Subsequent USB identification measured 16 MiB flash and embedded 8 MiB PSRAM; Espressif's chip specification supports the OSPI profile. Display type and recovery-button access remain unverified.
4. Use upstream Arduino CLI profiles and Espressif esptool. Keep local data/tools out of Git. Do not add a framework or cloud requirement.

## Retained feature brief (future phases)

Add a Spirit Box entry to the existing menu, with an offline dedicated mode: FM and AM/MW sweep, adjustable interval, forward/reverse, start/stop, appropriate steps, current frequency/status and clean exit. Preserve normal reception, existing controls, Wi-Fi and remote interfaces. Measure Si4732 tuning latency and audio continuity, including soft-mute/squelch behaviour, before choosing intervals. Consider the existing USB protocol for a tuning experiment first.

Use a temporary dark purple/black mode theme and a small embedded ghost graphic through existing graphics mechanisms. Restore the prior radio theme on exit without persisting the temporary choice. Keep it distinct from boot artwork and avoid elaborate animation.

Study P-SB7/SB7, SB7 Pro and comparable AM/FM sweep receivers in Phase 3 for useful controls, intervals, steps and audio handling. They are references, not required specifications. This project is an experimental sweeping radio; paranormal communication is not an established technical capability.

## Gates

- Phase 1 preservation complete: USB identification, current display/settings evidence, full 16 MiB original dump, measured size and SHA-256, private verified recovery copy outside the repository, OSPI profile supported by chip evidence. Restoration is documented but untested. Recovery-button access and display controller identity remain to be checked before a future physical update. Both backup copies currently share the same computer/disk.
- Phase 2: approved v2.45 OSPI update completed and all four written image hashes verified. USB status, About/System, PSRAM and display checked; owner confirms normal FM audio and controls and completion of the UTC+10 correction. AM/SSB and wireless checks remain outstanding. Manual recovery buttons could not be located; proceeded via verified hardware USB download access, with limitations recorded in `upgrade.md`.
- Phase 3 investigation: source architecture and commercial manuals reviewed; two bounded FM USB sweep trials audible throughout. MW2/AM trials and a longer listening repeat completed with verified MW and starting FM tuning restoration; owner reports MW audible with silent gaps. Investigate soft-mute/squelch and tuning behavior before claiming continuous MW audio. Native design and acceptance gates are recorded in `integration.md`. Native implementation remains pending; no firmware changes compiled or installed.

## Investigation decisions

5. Reuse normal tuning in a main-loop sweep timer rather than the muted, blocking signal scan. Start at 200 ms, offer 100–350 ms pending further audio/tune-complete measurements. Keep a single-band offline mode.
6. Select a temporary effective display theme without changing persistent `themeIdx` or user palette data. A small embedded static ghost suffices.
7. Explicitly preserve normal band/VFO/global state through deferred and direct save paths; avoiding new save requests alone does not prevent temporary sweep state from persisting.
8. USB protocol experiments remain bounded and restore starting tuning. They measure host cadence and sampled receiver status, not RF/audio settling time. Physical custom firmware flashing requires its own explicit approval after a concrete build is ready.
9. The controlled AM soft-mute-zero listening comparison was inconclusive (owner chose "Worse, or I missed it"). Retain the user's AM soft-mute setting by default; do not claim disabling it improves continuity. Original 4 dB soft-mute, squelch Off, FM 106.50 MHz and volume 20 were restored and verified.
10. On a later explicitly requested repeat, owner heard fewer gaps on the last 100 ms reverse MW pass with soft-mute zero, but no speech. Treat this as a positive listening observation, not isolated proof of a soft-mute improvement: speed/direction were also different. Restored soft-mute 4 dB, squelch Off and starting FM 106.50 MHz; preserved the now-current volume 30. Earlier FM tests were audible throughout. Do not infer local AM station availability from a sweep lacking speech.
11. Owner rejected the narrow 106.2–106.8 MHz FM trial as static/long stretches near 106.5. Telemetry confirms tuning changes, but seven-point repeated loops and stationary setup/restore periods are inadequate evidence of useful sweep audio. Next demonstration should use one broad FM pass with clear cues; do not confuse audible output with acceptable broadcast fragmentation.
12. Full-band FM pass across 87.5–108 MHz, 100 kHz steps and nominal 100 ms interval succeeded; owner confirmed clear station fragments. All 206 frequency commands were logged, sampled receiver status progressed across the band, and original FM 106.50 MHz/volume 30 was restored. Use this setting as the initial native FM default, superseding the provisional 200 ms starting point; keep intervals adjustable. Native implementation and physical custom flashing remain outstanding.
