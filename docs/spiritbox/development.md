# Development setup

Read root `AGENTS.md` and `docs/source/development.md`. No firmware was compiled or uploaded during bootstrap.

## Local tools

- Existing: Git 2.54.0.windows.1, authenticated GitHub CLI, uv 0.12.5, Python 3.10.
- Local esptool environment: Python 3.12.14, esptool 5.6.0, pyserial 3.5 in `.local/esptool/`.
- Arduino CLI 1.5.1: `.local/tools/arduino-cli-1.5.1/arduino-cli.exe`. Official Windows x64 archive SHA-256 verified against the release checksums: `fabe42e0eb04d00e776a66178299ff95a46c623dbc260f997e58fd514853dd40`.
- Repository checks use upstream's locked Python dependencies in `.venv/`. No global Python packages or PATH changes are needed.

Recreate the backup environment from the repository root:

```powershell
uv venv .local/esptool --python 3.12
uv pip install --python .local/esptool/Scripts/python.exe esptool==5.6.0
```

Arduino CLI was downloaded from its [official release](https://github.com/arduino/arduino-cli/releases/tag/v1.5.1). ESP32 platform and firmware libraries are not yet downloaded: upstream profiles install their pinned versions on first compile. Device identification supports `esp32s3-ospi`; see the inventory for chip evidence. No build has been performed.

`ats-mini/sketch.yaml` pins ESP32 core 3.3.12, PU2CLR SI4735 2.1.8, LovyanGFX 1.2.33, Async TCP 3.5.0 and ESP Async WebServer 3.12.1. Both ESP32-S3 profiles specify 8M flash, 80 MHz CPU, QIO flash and custom partitions; these are upstream build defaults, not measurements of this receiver. OSPI uses `PSRAM=opi`; QSPI uses `PSRAM=enabled`. The default profile is OSPI, so explicitly supply the confirmed profile when a future build is requested. Keep `sketch.yaml` and upstream workflow settings consistent.

Future **compile-only** examples, after hardware identification and when asked:

```powershell
& ./.local/tools/arduino-cli-1.5.1/arduino-cli.exe compile --warnings all --profile esp32s3-ospi ats-mini
# OR --profile esp32s3-qspi, once confirmed. Neither command uploads.
```

Run repository checks with `uv run --frozen prek run --all-files`. The installed uv requests a lock refresh with `--locked`; `--frozen` consumes upstream's existing lock without modifying it.

Bootstrap verification: all upstream prek checks passed; backup script PowerShell parsing and rejection of non-USB COM1 passed; esptool read/restore command options were checked against the installed CLI. Full-flash capture/copy succeeded on COM3 with a 16 MiB read and matching local/recovery SHA-256. Subsequently installed the official v2.45 OSPI release with approval; esptool verified all four writes. USB status, display and runtime PSRAM checked, and owner confirms normal FM audio/controls. Firmware builds, restoration, AM/SSB and wireless tests were not run. Local `core.autocrlf=false` keeps the checkout consistent with upstream's LF policy; no source formatting changes are committed.

## Git workflow

- `origin`: `https://github.com/justinshoesmith/ats-mini-spiritbox.git`
- `upstream`: `https://github.com/esp32-si4732/ats-mini.git`
- `main`: upstream history; keep free of custom changes.
- `spiritbox`: project work based on stable v2.45. Fetch upstream and review a future stable tag before merging it into this branch. Do not blindly merge unreleased upstream main or rewrite published history.

## Source inspection map (no integration yet)

| Concern | Existing files |
| --- | --- |
| Main loop and receiver operation | `ats-mini/ats-mini.ino`, `Common.h`, `SI4735-fixed.h`, `Scan.cpp` |
| Menu and physical controls | `Menu.cpp`, `Menu.h`, `Button.cpp`, `Rotary.cpp` |
| Display and themes | `Draw.cpp`, `Layout-Default.cpp`, `Layout-SMeter.cpp`, `Themes.cpp`, `Themes.h` |
| Boot artwork | `Splash.cpp`, `Splash.h`; separate from operating themes |
| Persistent configuration | `Storage.cpp`, `Storage.h`, `partitions.csv`, version constants in `Common.h` |
| Remote and wireless | `Remote.cpp`, `Parser*.cpp`, `Network.cpp`, `TcpMode.cpp`, `Web.cpp`, `Ble*.cpp` |
| Firmware update | `Ota.cpp`, `Ota.h` |

Detailed integration/tuning measurements and commercial receiver research are deferred to Phase 3. Preserve these upstream facilities and offline operation.
