# Upstream upgrade: v2.33 to v2.45

30 October 2026. **Prepared, not flashed.** The owner approved proceeding after reporting that the original recovery folder had been copied to Google Drive. That cloud copy is owner-reported, not independently inspected. The verified original dump and its local recovery copies remain intact.

Latest stable rechecked through GitHub's release API: [v2.45, released 22 October 2026](https://github.com/esp32-si4732/ats-mini/releases/tag/v2.45). Use the standard ATS Mini **OSPI** release for the chip-identified embedded 8 MiB Octal PSRAM. Physical flash is 16 MiB; upstream's 8 MiB layout fits within it. Do not change the upstream build profile just to use the unused capacity.

## Compatibility review

- Downloaded `ats-mini-v2.45-ospi.zip` from the upstream release. SHA-256 matches GitHub's published digest: `68e7cf6c6181ad7fdd759c71efa963b2a9e4a5a314e897c25addfab7d537d8a3`.
- Extracted application SHA-256 matches the published OSPI OTA application digest: `08cc7837e812b48c7e6b52d4e8aed5f66dad09afd414a63564ab377178e8a33b`. The application fits its 3 MiB slot.
- esptool validates application and bootloader checksums/hashes; both target ESP32-S3 and permit the detected v0.2 chip revision.
- The released partition table exactly matches the corresponding bytes from the original device dump. Partition offsets and sizes did not change between the source tags either.
- ESP32 core advances from 3.3.0 to 3.3.12. Use the released bootloader together with the released application. The original v2.33 lacks the later firmware-update interface/compatibility metadata; use USB for this upgrade.
- Settings and memories version remain 71. Bands version changes from 72 to 73: per-band tuning/VFO state resets because frequencies now use Hz internally. Global settings and compatible memories should survive a separate-file USB update.
- v2.34 changed UTC offset indices: check and set the correct UTC offset after upgrade. Squelch is now per-mode, with a different stored value type; inspect its value after upgrade.
- USB remote control defaults to Off in newer firmware. Expect to enable Settings -> USB Port -> Ad hoc using the encoder before remote checks will work. TCP remote control is also optional and defaults to Off.
- Display rendering changes to LovyanGFX; verify the physical display and controls after upgrade. Audio clamp configuration is now on the Config web page; inspect only if audible band-change/mute clicks occur.

Review sources: repository `CHANGELOG.md`, `ats-mini/Common.h`, `ats-mini/Storage.cpp`, `ats-mini/partitions.csv`, both source-tag profiles, and upstream [flashing instructions](https://esp32-si4732.github.io/ats-mini/flash.html).

## Prepared update

Files are local under `.local/firmware/v2.45-ospi/ats-mini-v2.45-ospi/`. Digests and inspection evidence are under `.local/logs/upgrade-v2.45/`. No firmware source changes or compilation are needed.

Use the four separate release files at the upstream offsets: bootloader `0x0`, partitions `0x8000`, `boot_app0.bin` `0xe000`, application `0x10000`. Include `boot_app0.bin` so an old OTA selection cannot boot the wrong slot. Do not use the merged image, which overwrites settings, or erase all flash. Preserve the remaining flash and original backups. Reconfirm device identity and backup hashes immediately before writing.

Approval for the physical update is recorded. **Pending hardware detail:** owner confirmation of accessible BOOT/RESET controls, as requested by the upstream pre-flash recovery check. Automatic USB loader entry worked during backup, but manual recovery-button access has not been verified. No physical write has been attempted.

## Verification after a future write

Confirm successful esptool verification, reboot, displayed v2.45, non-zero PSRAM, readable display, encoder/buttons and audible normal FM/AM reception. Enable USB Ad hoc if needed and capture serial status and display evidence. Recheck UTC offset, squelch, theme, volume, memories and current tuning. Restore the recorded normal tuning where appropriate. Report remote checks separately from owner-observed audio/physical-control checks. Do not begin custom Spirit Box firmware changes until the upstream baseline operates normally.
