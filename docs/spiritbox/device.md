# Device inventory

Recorded 30 October 2026. Distinguish owner reports from device measurements.

| Item | Evidence / status |
| --- | --- |
| Product | Owner reports AMNVOLT ATS Mini V4, received |
| Installed firmware | USB monitor reports version 233 (v2.33), verified after backup; September 2025 date remains owner-reported |
| Modification history | Owner reports no modifications or backup |
| MCU / receiver | esptool confirms ESP32-S3 (QFN56), revision v0.2, 40 MHz crystal; Si4732 remains owner-reported |
| Enclosure / controls | Owner reports ABS case, colour display, rotary encoder, existing controls, USB-C and rechargeable battery |
| Wireless | Owner reports Wi-Fi and Bluetooth capability; operation on installed firmware untested |
| USB detection | Receiver connected as COM3, Espressif VID:PID 303A:1001, USB Serial/JTAG; USB screenshot and monitor commands respond |
| Flash capacity | esptool detects 16 MiB (16,777,216 bytes), manufacturer 46, device 4018, quad flash, 3.3 V |
| PSRAM capacity / bus | esptool identifies embedded 8 MiB AP_3v3 PSRAM; Espressif's chip specification maps this to Octal SPI; use upstream `esp32s3-ospi` |
| Display controller / board revision | Unknown; capture About screen and, only if needed, physical markings |
| BOOT / RESET availability | Unknown; verify access before any future flashing |
| Current settings / memories | Display captured before and after backup: VHF/FM 91.40 MHz, step 100k, BW Auto, AGC On, volume 20. Memory/theme queries returned no data; full flash preserves stored settings and memories |
| Factory flash / SHA-256 / recovery copy | **Complete**, 16 MiB from offset zero, capture `20261030T052039623Z`; original and private outside-repository copy have matching SHA-256 |

## Preservation evidence

Captured 30 October 2026 at 15:20 Australia/Brisbane (05:20 UTC). Full read completed successfully in 75.9 seconds. SHA-256:

```text
fe87b8c315a17ed054dc3e022a34d1c156cda68b9a21812fbb6819ec7f21f592
```

Repository-local dump: `.local/backups/20261030T052039623Z/original-flash.bin`. Recovery copy: `%LOCALAPPDATA%/ATS Mini Spirit Box/Recovery/20261030T052039623Z/original-flash.bin`. Recovery directory permissions allow only the current Windows account and SYSTEM. Both copies are on the same computer/disk; an additional private offline copy is recommended for disk-loss protection. Logs, hash manifests, parsed partition/application metadata and display evidence remain private and are excluded from Git.

Offline partition inspection finds the existing application slots, settings, LittleFS, NVS and coredump within the first 8 MiB. The dump deliberately covers the full physical 16 MiB. The application descriptor contains Arduino core build metadata rather than the radio's displayed version; v2.33 was independently confirmed by the live USB monitor.

After securing the backup, a read-only chip query with hard reset resumed the existing firmware. USB monitor and complete 320x170 display capture succeeded at 91.40 MHz and volume 20. No flash erase/write or eFuse changes occurred. No restore, audio, physical-control or wireless test was performed.

PSRAM bus identification is based on the chip-reported embedded capacity/vendor and the [Espressif ESP32-S3 series specification](https://documentation.espressif.com/esp32_s3_datasheet_en.pdf), which lists the 8 MiB in-package PSRAM variants as Octal SPI. This is separate from the flash chip's quad bus. Runtime PSRAM allocation and display controller ID have not been inspected on an About screen.

Before entering the bootloader, record available About pages (firmware/date, CPU, flash, PSRAM, display ID). Keep photos and serial output in `.local/`, with a recovery copy outside the repository. Do not reset settings or hold the encoder while powering on: upstream uses this gesture for settings reset.

Record band/mode, frequency, tuning step, volume, bandwidth, AGC/attenuation, soft-mute/squelch, theme/layout, backlight, region, antenna settings, saved memories and enabled USB/network modes where available. Do not publish SSIDs, passwords, MAC addresses, Bluetooth bonds or personal station labels. Keep any detailed settings inventory under `.local/backups/`.

For PSRAM identification use the running firmware's About information and upstream's [variant guide](https://esp32-si4732.github.io/ats-mini/flash.html) / [discussion #174](https://github.com/esp32-si4732/ats-mini/discussions/174). Memory capacity alone may be insufficient; confirm bus type using module markings or known-good firmware evidence. `flash-id` measures flash, not the PSRAM bus. Do not install candidate firmware just to identify hardware.
