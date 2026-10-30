# Native OSPI candidate build

30 October 2026. **Compile succeeded; candidate not flashed or hardware-tested.** Source commit `82dd43d` on `spiritbox`, based on upstream v2.45. Owner explicitly authorized compiling after source implementation. Physical custom flashing still requires approval of this candidate.

Command from repository root (local Arduino CLI 1.5.1):

```powershell
& ./.local/tools/arduino-cli-1.5.1/arduino-cli.exe compile --warnings all --profile esp32s3-ospi --build-path .local/build/spiritbox-ospi ats-mini
```

Installed the platform/libraries already pinned by upstream's profile: ESP32 3.3.12, PU2CLR SI4735 2.1.8, LovyanGFX 1.2.33, Async TCP 3.5.0 and ESP Async WebServer 3.12.1. No source correction, profile change or dependency-version change was needed for the build to pass. The first build downloaded the complete Espressif platform tools and performed a lengthy dependency scan.

## Results

- Compiler exit status 0; application and bootloader pass esptool image checksum/hash validation for ESP32-S3, compatible with the detected v0.2 chip revision.
- Confirmed build FQBN includes `PSRAM=opi`, `FlashSize=8M`, `CPUFreq=80`, `USBMode=hwcdc` and the custom partition scheme.
- Application binary: **1,801,728 bytes**, fits both **3,145,728-byte** application slots with 1,344,000 bytes spare per slot. Arduino's summary used a generic 16 MiB maximum; the actual partition fit was checked independently.
- Static globals: **62,508 bytes**, leaving 265,172 bytes in the compiler's 327,680-byte dynamic-memory region. This is not a runtime heap/stack or PSRAM measurement.
- Binary contains the `SB-dev` display marker. The image's application descriptor still shows Arduino core build metadata; verify the actual displayed firmware identity on hardware after an approved install.
- Compiled partition table exactly matches the original receiver dump. Compiled bootloader exactly matches the official v2.45 OSPI release bootloader.
- Two warnings, both in unchanged PU2CLR 2.1.8 `SI4735.cpp`: line 3520 misleading indentation, line 2660 potentially uninitialized `mjd`. No warning from project source or new Spirit Box code was emitted. No dependency was patched for these warnings.

Application SHA-256:

```text
f36c7324a9361fd98dd629a2e7ebf725f7b3f064f477036ce8907a94adbde404
```

## Local artifacts

Build products and log remain ignored:

- `.local/build/spiritbox-ospi/`: binary, ELF/map, bootloader, partitions and build metadata.
- `.local/logs/spiritbox-compile.txt`: complete compile output.
- `.local/releases/ats-mini-spiritbox-v2.45-82dd43d-ospi/`: four separate firmware files at documented offsets, manifest, SHA-256 checksums and operating notes.
- `.local/releases/ats-mini-spiritbox-v2.45-82dd43d-ospi.zip`: candidate package for review; not a published release.

This package includes no device flash dump or credentials. Do not use the generated merged binary for installation: it overwrites settings. A future approved USB install can use the four separate files and retain the original backup. A fresh pre-custom-flash v2.45 settings/flash capture is prudent before that physical operation.

## Outstanding acceptance

Compilation does not verify the rendered UI, tuning cadence/audio, theme restoration, original band/VFO persistence, pending saves, reboot state or sleep/remote/web interactions. These checks are listed in `operating.md` and must be performed on the actual receiver after an approved installation. The radio currently remains on official v2.45, with the original full-flash backup preserved.
