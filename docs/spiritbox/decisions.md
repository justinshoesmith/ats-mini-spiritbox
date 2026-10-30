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
- Phase 3 investigation: source architecture and commercial manuals reviewed; two bounded FM USB sweep trials audible throughout. Two additional MW2/AM trials completed with verified MW and starting FM tuning restoration; owner listening result pending. Native design and acceptance gates are recorded in `integration.md`. Native implementation remains pending; no firmware changes compiled or installed.

## Investigation decisions

5. Reuse normal tuning in a main-loop sweep timer rather than the muted, blocking signal scan. Start at 200 ms, offer 100–350 ms pending further audio/tune-complete measurements. Keep a single-band offline mode.
6. Select a temporary effective display theme without changing persistent `themeIdx` or user palette data. A small embedded static ghost suffices.
7. Explicitly preserve normal band/VFO/global state through deferred and direct save paths; avoiding new save requests alone does not prevent temporary sweep state from persisting.
8. USB protocol experiments remain bounded and restore starting tuning. They measure host cadence and sampled receiver status, not RF/audio settling time. Physical custom firmware flashing requires its own explicit approval after a concrete build is ready.
