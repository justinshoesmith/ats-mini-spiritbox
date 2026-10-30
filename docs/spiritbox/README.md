# ATS Mini Spirit Box

30 October 2026 (Australia/Brisbane). **Original full-flash backup completed and approved upstream upgrade to v2.45 OSPI performed successfully.** The 16 MiB original dump and its SHA-256-verified recovery copies remain intact. No custom Spirit Box firmware has been installed.

- [Device baseline and outstanding evidence](device.md)
- [Backup, validation and recovery](backup-recovery.md)
- [Development environment and source map](development.md)
- [Scope and decisions](decisions.md)
- [Upstream upgrade and verified v2.45 baseline](upgrade.md)

Repository: <https://github.com/justinshoesmith/ats-mini-spiritbox> (fork of <https://github.com/esp32-si4732/ats-mini>).

`spiritbox` starts at stable `v2.45`, annotated tag object `a45a27d4ca0d9ce4230469248a4025819393fbac`, resolving to release commit `21f7397`. This is the latest stable release checked through GitHub's release API on 30 October 2026; published 22 October 2026. The receiver now runs this official release; the original v2.33 baseline remains in the full-flash backup.

Upstream source layout, MIT license and attribution are retained. Project additions live in `docs/spiritbox/` and `tools/spiritbox/`. Dumps, diagnostics, local tools and environments are excluded from Git. Never force-add device data.

The connected receiver is an ESP32-S3 revision v0.2 with 16 MiB flash and 8 MiB PSRAM. USB status and About confirm v2.45, non-zero PSRAM and a working display. The owner confirms normal FM audio and physical controls. UTC offset needs correction to UTC+10 after migration. Restoration, AM/SSB and wireless operation remain untested; manual recovery-button access is unverified. See the device inventory for evidence and limitations.
