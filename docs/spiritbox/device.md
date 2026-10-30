# Device inventory

Recorded 30 October 2026. Distinguish owner reports from device measurements.

| Item | Evidence / status |
| --- | --- |
| Product | Owner reports AMNVOLT ATS Mini V4, received |
| Installed firmware | Owner reports ESP32-SI4732 v2.33, September 2025, displayed on receiver |
| Modification history | Owner reports no modifications or backup |
| MCU / receiver | Owner reports ESP32-S3 / Si4732; consistent with upstream family, not measured on this unit |
| Enclosure / controls | Owner reports ABS case, colour display, rotary encoder, existing controls, USB-C and rechargeable battery |
| Wireless | Owner reports Wi-Fi and Bluetooth capability; operation on installed firmware untested |
| USB detection | Windows CIM and pyserial show only ACPI COM1; no USB serial receiver detected |
| Flash capacity / chip revision | Unknown; obtain from esptool identification |
| PSRAM capacity / bus | Unknown; do not choose OSPI or QSPI based on V4 branding |
| Display controller / board revision | Unknown; capture About screen and, only if needed, physical markings |
| BOOT / RESET availability | Unknown; verify access before any future flashing |
| Current settings / memories | Not captured; no serial or physical access |
| Factory flash / SHA-256 / recovery copy | **Not captured; pending device connection** |

Before entering the bootloader, record available About pages (firmware/date, CPU, flash, PSRAM, display ID). Keep photos and serial output in `.local/`, with a recovery copy outside the repository. Do not reset settings or hold the encoder while powering on: upstream uses this gesture for settings reset.

Record band/mode, frequency, tuning step, volume, bandwidth, AGC/attenuation, soft-mute/squelch, theme/layout, backlight, region, antenna settings, saved memories and enabled USB/network modes where available. Do not publish SSIDs, passwords, MAC addresses, Bluetooth bonds or personal station labels. Keep any detailed settings inventory under `.local/backups/`.

For PSRAM identification use the running firmware's About information and upstream's [variant guide](https://esp32-si4732.github.io/ats-mini/flash.html) / [discussion #174](https://github.com/esp32-si4732/ats-mini/discussions/174). Memory capacity alone may be insufficient; confirm bus type using module markings or known-good firmware evidence. `flash-id` measures flash, not the PSRAM bus. Do not install candidate firmware just to identify hardware.
