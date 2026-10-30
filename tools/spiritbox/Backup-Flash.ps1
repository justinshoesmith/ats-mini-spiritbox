# Full-flash read and integrity check only. There is deliberately no restore action.
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^COM[0-9]+$')]
    [string]$Port,
    [Parameter(Mandatory = $true)]
    [string]$RecoveryDirectory,
    [ValidateSet(115200, 460800, 921600)]
    [int]$Baud = 460800
)

$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$python = Join-Path $repo '.local/esptool/Scripts/python.exe'
if (-not (Test-Path -LiteralPath $python)) { throw 'Install the local esptool environment first; see docs/spiritbox/backup-recovery.md.' }
if (-not [IO.Path]::IsPathRooted($RecoveryDirectory)) { throw 'RecoveryDirectory must be an absolute path outside this repository.' }
$recovery = [IO.Path]::GetFullPath($RecoveryDirectory).TrimEnd('\', '/')
if ($recovery -eq $repo -or $recovery.StartsWith($repo.TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'The recovery copy must be outside the repository.'
}

# Explicit USB port selection prevents accidentally probing the built-in COM1.
$usbPorts = & $python -c 'import serial.tools.list_ports; print(" ".join(p.device for p in serial.tools.list_ports.comports() if p.vid is not None))'
if ($LASTEXITCODE -ne 0 -or $Port -notin ($usbPorts -split '\s+')) { throw 'Selected port is not an enumerated USB serial device. Recheck the cable, power and port.' }

$stamp = [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssfffZ')
$backupDir = Join-Path $repo ".local/backups/$stamp"
$logDir = Join-Path $repo ".local/logs/$stamp"
$copyDir = Join-Path $recovery $stamp
# Establish both destinations before interacting with the receiver.
New-Item -ItemType Directory -Path $backupDir, $logDir, $copyDir | Out-Null

& $python -m esptool --chip esp32s3 --port $Port --baud $Baud flash-id 2>&1 | Tee-Object -FilePath (Join-Path $logDir 'flash-id.txt')
if ($LASTEXITCODE -ne 0) { throw 'Device identification failed. No backup was completed.' }
$identity = Get-Content -LiteralPath (Join-Path $logDir 'flash-id.txt') -Raw
if ($identity -notmatch 'Detected flash size:\s*(\d+)\s*MB') { throw 'Cannot establish flash capacity from flash-id. Inspect the log before proceeding.' }
$expectedBytes = [long]$Matches[1] * 1MB

$dump = Join-Path $backupDir 'original-flash.bin'
# Leave the CPU in the loader until files and hashes have been secured.
& $python -m esptool --chip esp32s3 --port $Port --baud $Baud --after no-reset read-flash 0x0 ALL $dump 2>&1 | Tee-Object -FilePath (Join-Path $logDir 'read-flash.txt')
if ($LASTEXITCODE -ne 0) { throw 'Flash read failed. Any partial dump is NOT a valid backup.' }
if ((Get-Item -LiteralPath $dump).Length -ne $expectedBytes) { throw 'Backup length does not match detected flash capacity.' }
$digest = (Get-FileHash -LiteralPath $dump -Algorithm SHA256).Hash.ToLowerInvariant()
$manifest = Join-Path $backupDir 'SHA256SUMS.txt'
Set-Content -LiteralPath $manifest -Value "$digest  original-flash.bin" -Encoding ascii

$copy = Join-Path $copyDir 'original-flash.bin'
Copy-Item -LiteralPath $dump -Destination $copy
if ((Get-Item -LiteralPath $copy).Length -ne $expectedBytes -or (Get-FileHash -LiteralPath $copy -Algorithm SHA256).Hash.ToLowerInvariant() -ne $digest) {
    throw 'External recovery copy verification failed. Do not flash the receiver.'
}
Copy-Item -LiteralPath $manifest -Destination $copyDir
Copy-Item -LiteralPath (Join-Path $logDir 'flash-id.txt'), (Join-Path $logDir 'read-flash.txt') -Destination $copyDir
@{
    captured_utc = $stamp
    port = $Port
    bytes = $expectedBytes
    sha256 = $digest
    esptool = '5.6.0'
    recovery_copy = $copy
    status = 'Full read completed; size and local/external SHA-256 verified. Restore not tested.'
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $backupDir 'manifest.json') -Encoding UTF8
Copy-Item -LiteralPath (Join-Path $backupDir 'manifest.json') -Destination $copyDir
Write-Output "Backup verified: $dump"
Write-Output "Recovery copy verified: $copy"
Write-Output "Bytes: $expectedBytes; SHA-256: $digest"
Write-Output 'Power-cycle the radio after the command completes. Record the result in the device inventory.'
