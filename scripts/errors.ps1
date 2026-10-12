param([string]$Exe = "$PSScriptRoot/../build/shell-emulator.exe")
Get-ChildItem "$PSScriptRoot/errors/*.txt" | Sort-Object Name | ForEach-Object {
    & $Exe --vfs "$PSScriptRoot/../data/deep.csv" --script $_.FullName
}
