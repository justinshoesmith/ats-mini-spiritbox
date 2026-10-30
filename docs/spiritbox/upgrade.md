# Upstream upgrade: v2.33 to v2.45

30 October 2026. **Upgraded successfully to upstream v2.45 OSPI.** The owner approved proceeding after reporting that the original recovery folder had been copied to Google Drive. That cloud copy is owner-reported, not independently inspected. The verified original dump and its local recovery copies remain intact.

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

## Update performed

Files are local under `.local/firmware/v2.45-ospi/ats-mini-v2.45-ospi/`. Digests and inspection evidence are under `.local/logs/upgrade-v2.45/`. No firmware source changes or compilation are needed.

Use the four separate release files at the upstream offsets: bootloader `0x0`, partitions `0x8000`, `boot_app0.bin` `0xe000`, application `0x10000`. Include `boot_app0.bin` so an old OTA selection cannot boot the wrong slot. Do not use the merged image, which overwrites settings, or erase all flash. Preserve the remaining flash and original backups. Reconfirm device identity and backup hashes immediately before writing.

Approval for the physical update is recorded. The owner could not locate BOOT/RESET buttons and reported a side power switch with an unidentified middle position. No case opening, pad shorting or switch experiment was performed. Manual recovery-button access remains unverified. Proceeded using the already-tested automatic USB Serial/JTAG ROM-loader route after reviewing [Espressif's hardware download-mode documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/usb-serial-jtag-console.html). This route does not prove that manual recovery would be available if firmware disabled the USB controller or reassigned its pins.

Immediately before writing, reverified both original dump copies, the release archive and every extracted image digest. A fresh USB chip/flash query matched the original device identity, revision, PSRAM and flash capacity. Wrote only the four separate release images at the documented offsets. esptool exited successfully, verified all four data hashes, and hard-reset the device. No erase-all, merged image, custom firmware, eFuse operation or source compilation was used.

## Observed baseline

- Owner enabled Settings -> USB Port -> Ad hoc. USB monitor then reported version 245, and binary screenshots worked.
- About/System screen displays ATS-Mini F/W v2.45, Oct 22 2026, ESP32-S3 r2 at 80 MHz, 16M flash, PSRAM 8192k (8078k free), and display ID 8183.
- Main screen renders correctly with the prior default theme and volume 20. Per-band tuning reset as expected; restored 91.40 MHz using the native frequency command before the owner's reception test.
- Owner explicitly confirmed: audio and controls work normally on FM. AM/SSB reception and Wi-Fi/Bluetooth have not been tested. The firmware is the official upstream release; no Spirit Box feature is installed yet.
- Original saved squelch was zero and FM region was EU/JP/AU. The original UTC offset index was 19, meaning UTC+10 in v2.33; in v2.45 index 19 means UTC+3:30, while UTC+10 is index 32. Asked the owner to set Settings -> UTC Offset -> UTC+10. Completion of that manual correction has not yet been confirmed.

Full logs, image manifests, monitor output and screenshots remain under `.local/logs/upgrade-v2.45/`. The original full-flash backup has not been overwritten or restored. A downgrade would require reviewing compatibility or restoring that original dump; do not write only the old application over migrated settings.
