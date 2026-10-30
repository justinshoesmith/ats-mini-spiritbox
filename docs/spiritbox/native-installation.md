# First native installation

30 October 2026 (Australia/Brisbane). **Installed and exercised on the owner's ATS Mini V4. Experimental firmware.** Owner explicitly approved the verified candidate before the physical write. Source: `82dd43d`, v2.45 OSPI, About label `SB-dev`.

## Preservation and installation

Captured current normal-radio baseline: VHF/FM **104.90 MHz**, volume **30**. Before writing, read all **16,777,216 bytes** of current upstream v2.45 flash and verified local and Documents recovery-copy size/SHA-256. Fresh backup:

```text
Documents/ATS Mini Radio Recovery/20261030T090914440Z/original-flash.bin
SHA-256: 177a221b0b2f479e1a327b0d35fe9b21d69e081d984aa828902f1f1b11d49c3d
```

This is a separate capture from the original factory v2.33 dump, which remains intact. The new copy has not been independently uploaded to or checked on Google Drive.

Reverified the four candidate hashes, both current-backup copies, the factory-backup hash and the connected device identity. Wrote only bootloader at `0x0`, partitions at `0x8000`, `boot_app0.bin` at `0xe000` and application at `0x10000`. esptool verified all four data hashes and reset successfully. No erase-all, merged-image write or eFuse operation was used. Application SHA-256:

```text
f36c7324a9361fd98dd629a2e7ebf725f7b3f064f477036ce8907a94adbde404
```

## Observed acceptance

| Check | Actual result |
| --- | --- |
| Boot and identity | About displays v2.45 SB-dev, Oct 30 2026; 80 MHz ESP32-S3, 16M flash, PSRAM 8192k (8078k free), display ID 8183 |
| Normal baseline | Booted with normal theme, 104.90 MHz and volume 30 |
| Entry and drawing | Main menu opens Spirit Box, initially stopped at 87.50 MHz; purple theme, small ghost, interval/direction and field controls render correctly |
| Native FM | Ran approximately 23 seconds at nominal 100 ms, 100 kHz steps; 46 telemetry samples spanned 87.5–108 MHz and showed wrapping into the next pass |
| Native FM audio | Owner reported some fragments, with gaps/static; do not describe continuity as fully validated |
| Stop and Exit | Screenshot confirms stopped state; Exit screenshot and telemetry confirm normal theme and original band/mode/frequency/volume restored |
| Native AM controls | Selected MW, 200 ms, reverse and 1 kHz step; screenshots confirm values. Restored 9 kHz step before sweep |
| Volume and timeout | Temporarily incremented/decremented volume back to 30, waited over 10 seconds; field editor closed without dropping mode, deferred settings save completed |
| Native AM tuning | Eight-second reverse run at 200 ms / 9 kHz: 16 telemetry samples showed 531 kHz wrapping to the high end and descending to 1278 kHz; short press started/stopped. Native AM audio has not been separately confirmed |
| Saved-state protection | Read settings partition while still in the temporary AM mode, then reset. Compared 68 decoded band/VFO and selected settings records with the pre-install backup: **no differences** |
| Reboot recovery | Reboot returned to normal radio, original theme, 104.90 MHz and volume 30; did not resume a sweep |
| Physical controls | Owner explicitly confirmed physical knob field selection, Run/Stop and Exit work, including restoration of normal radio display/tuning |

The settings comparison covered saved band records, VFO slots/active index and selected global keys (Band, Theme, Volume, AmSoftMute, Squelch, UILayout, USBMode, UTCOffset). It is not a claim that every flash byte remained unchanged. Device screenshots and private logs, including the NVS read, remain ignored under `.local/logs/native-install/`.

## Remaining limitations

Sleep/Off Timer, network/BLE control, queued web/OTA interactions, every interval/step combination, alternate layout restoration, entry from SSB and user-mute/squelch edge cases have been reviewed in source but not all exercised on hardware. No automated native unit-test suite was added. Ordinary AM/SSB reception and sample-accurate audio timing remain unverified. Keep this first build experimental and retain both recovery baselines.

See `operating.md` for use and `build.md` for compile warnings and artifact provenance. The existing native candidate remains the exact binary installed; no source fixes or rebuild were needed during these checks.
