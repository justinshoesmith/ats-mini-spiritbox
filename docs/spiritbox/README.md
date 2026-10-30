# ATS Mini Spirit Box

Phase 1 preservation, 30 October 2026 (Australia/Brisbane). **Original full-flash backup completed: 16 MiB, size and SHA-256 verified against a private recovery copy outside the repository.** No firmware was erased, written or upgraded.

- [Device baseline and outstanding evidence](device.md)
- [Backup, validation and recovery](backup-recovery.md)
- [Development environment and source map](development.md)
- [Scope and decisions](decisions.md)

Repository: <https://github.com/justinshoesmith/ats-mini-spiritbox> (fork of <https://github.com/esp32-si4732/ats-mini>).

`spiritbox` starts at stable `v2.45`, commit `a45a27d4ca0d9ce4230469248a4025819393fbac`. This is the latest stable release checked through GitHub's release API on 30 October 2026; published 22 October 2026. This source baseline is distinct from the receiver's reported installed v2.33.

Upstream source layout, MIT license and attribution are retained. Project additions live in `docs/spiritbox/` and `tools/spiritbox/`. Dumps, diagnostics, local tools and environments are excluded from Git. Never force-add device data.

The connected receiver is an ESP32-S3 revision v0.2 with 16 MiB flash and 8 MiB embedded PSRAM (OSPI profile, based on Espressif's chip specification). USB status confirms installed firmware v2.33. After the backup, the receiver was reset into its existing firmware; serial status and display capture succeeded at the original frequency and volume. Restoration, audio, physical controls and wireless operation have not been tested. Recovery-button access and display controller identity remain unverified. See the device inventory for evidence and limitations.
