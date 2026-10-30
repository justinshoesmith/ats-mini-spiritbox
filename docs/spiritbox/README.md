# ATS Mini Spirit Box

Phase 1 bootstrap, 30 October 2026 (Australia/Brisbane). **Factory flash backup pending: no radio USB device detected. Do not erase, upgrade or flash.**

- [Device baseline and outstanding evidence](device.md)
- [Backup, validation and recovery](backup-recovery.md)
- [Development environment and source map](development.md)
- [Scope and decisions](decisions.md)

Repository: <https://github.com/justinshoesmith/ats-mini-spiritbox> (fork of <https://github.com/esp32-si4732/ats-mini>).

`spiritbox` starts at stable `v2.45`, commit `a45a27d4ca0d9ce4230469248a4025819393fbac`. This is the latest stable release checked through GitHub's release API on 30 October 2026; published 22 October 2026. This source baseline is distinct from the receiver's reported installed v2.33.

Upstream source layout, MIT license and attribution are retained. Project additions live in `docs/spiritbox/` and `tools/spiritbox/`. Dumps, diagnostics, local tools and environments are excluded from Git. Never force-add device data.

Phase 1 is incomplete until the original full-flash backup and its external recovery copy are verified. No physical identification, backup, restoration, upgrade or radio functionality test has been completed.
