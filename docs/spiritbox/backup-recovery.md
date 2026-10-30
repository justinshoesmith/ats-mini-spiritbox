# Backup and recovery

**Status: prepared, not performed.** No radio USB port was present on 30 October 2026. A SHA-256 manifest must come from actual device data, never from an empty or sample file.

Based on upstream [backup/recovery](https://esp32-si4732.github.io/ats-mini/recovery.html) and Espressif [esptool commands](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/esptool/basic-commands.html). Local tooling uses esptool 5.6.0 and its current hyphenated command names.

## Capture

1. Charge the radio, capture its settings and About information, power it on and connect with a USB **data** cable. Close serial monitors. Identify the new USB port; do not use ACPI COM1.
2. Select a private recovery directory outside this checkout, preferably on encrypted removable storage. Flash dumps can contain Wi-Fi credentials and personal configuration. A second folder on the same disk is not protection against disk loss. Do not place dumps in public or shared cloud folders.
3. From the repository root, enumerate ports, then run the backup utility with the actual port and private recovery path:

   ```powershell
   & ./.local/esptool/Scripts/python.exe -m serial.tools.list_ports -v
   # Replace both placeholders; this is a template, not a command already run.
   ./tools/spiritbox/Backup-Flash.ps1 -Port COM_REPLACE -RecoveryDirectory 'E:\PrivateRadioRecovery'
   ```

The utility rejects non-USB serial ports and recovery paths inside the checkout. It runs `flash-id`, derives flash capacity from the device response, then reads **all** flash from `0x0` with `read-flash 0x0 ALL`. It validates byte count, calculates SHA-256, copies the dump and evidence outside the repository, and hashes that copy again. Each run has a new UTC timestamp directory. Failed/partial reads do not count as backups.

Default baud is 460800; retry with `-Baud 115200` if the connection is unreliable. Entering the ROM loader resets the receiver and may load esptool's temporary RAM stub; the utility contains no flash-write, erase or eFuse operation. It leaves the receiver in loader mode after reading so it does not resume saving settings during verification. Power-cycle it after completion. If security/encryption errors appear, stop and inspect; never use force or alter eFuses.

If automatic loader entry fails, verify which buttons are actually present. Upstream documents holding BOOT, pressing RESET, then releasing BOOT; for a BOOT-only unit, hold BOOT while powering on and then release. Do not open the case or short undocumented pads as a routine first step.

## Acceptance evidence

- Successful `flash-id` and complete read logs under `.local/logs/<UTC>/`.
- `.local/backups/<UTC>/original-flash.bin` length equals measured flash capacity (8 MiB would be 8,388,608 bytes; **do not assume 8 MiB**).
- `SHA256SUMS.txt` and `manifest.json` identify the captured data.
- Independent file hash of the external recovery copy matches the original.
- Save settings/About evidence alongside the external copy; update `device.md` with non-sensitive results.

A file hash proves subsequent file integrity, not that a restoration has been physically tested. Full-flash backup includes stored partitions/settings but does not capture eFuses, hardware state or secrets that cannot be read out. A second full read while the application remains stopped can provide additional confidence if needed; avoid allowing the radio to save settings between reads.

## Restore (future operation; approval required)

Do not run restoration in Phase 1. Obtain explicit approval before any physical write. Confirm this is the same receiver, security state is compatible, both copies are intact and the dump length matches its flash. Check the manifest hash again first. Use the **original complete dump**, not an OTA or merged release image. From the repository root, the esptool 5.6.0 command template is:

```powershell
& ./.local/esptool/Scripts/python.exe -m esptool --chip esp32s3 --port COM_REPLACE --baud 460800 --before default-reset --after hard-reset write-flash --flash-mode keep --flash-freq keep --flash-size keep 0x0 'ABSOLUTE_PATH_TO_VERIFIED_ORIGINAL_FLASH.bin'
```

There is no need for a separate erase-all operation. Do not use `--force`. After a future approved restore, power-cycle and verify v2.33/date, reception/audio, controls, display, memories/settings and previously used connectivity. Record actual observations; restoration has not yet been tested.

Do not substitute upstream's merged release image for a backup: it is a fresh installation and can reset settings. Bootloader/partition/settings migration review belongs to Phase 2 after the backup is secured.
