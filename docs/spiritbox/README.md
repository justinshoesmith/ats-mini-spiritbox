# ATS Mini Spirit Box

30 October 2026 (Australia/Brisbane). **First experimental native Spirit Box build installed with approval and exercised on the radio.** Current About label: v2.45 SB-dev. Both the original v2.33 full-flash backup and a fresh pre-custom-install v2.45 backup are verified and preserved.

- [Device baseline and outstanding evidence](device.md)
- [Backup, validation and recovery](backup-recovery.md)
- [Development environment and source map](development.md)
- [Scope and decisions](decisions.md)
- [Upstream upgrade and verified v2.45 baseline](upgrade.md)
- [Native-mode investigation and completed USB sweep trials](integration.md)
- [Native-mode controls and outstanding validation](operating.md)
- [Successful OSPI build and candidate artifacts](build.md)
- [Native installation and actual hardware checks](native-installation.md)

Repository: <https://github.com/justinshoesmith/ats-mini-spiritbox> (fork of <https://github.com/esp32-si4732/ats-mini>).

`spiritbox` starts at stable `v2.45`, annotated tag object `a45a27d4ca0d9ce4230469248a4025819393fbac`, resolving to release commit `21f7397`. This was the latest stable release checked through GitHub's release API on 30 October 2026; published 22 October 2026. The installed custom source is `82dd43d`, based on this release; both original v2.33 and pre-custom v2.45 flash baselines remain backed up.

Upstream source layout, MIT license and attribution are retained. Project additions live in `docs/spiritbox/` and `tools/spiritbox/`. Dumps, diagnostics, local tools and environments are excluded from Git. Never force-add device data.

The receiver has 16 MiB flash and working 8 MiB PSRAM. Native mode renders correctly, sweeps FM/MW, restores the normal theme/tuning and returns to normal radio on reboot. A comparison of 68 saved band/VFO and selected preference records after an in-mode save found no differences from the pre-install backup. Owner confirms physical controls and Exit work. Native FM produces fragments with gaps/static; native AM audio and the remaining interaction cases are still provisional. See the installation record for the exact checks and limitations.
