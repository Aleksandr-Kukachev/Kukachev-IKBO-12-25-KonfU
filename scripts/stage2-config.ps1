param([string]$Exe = "$PSScriptRoot/../build/shell-emulator.exe")
# Test alternative syntax, help, missing and unknown options.
& $Exe "--vfs=$PSScriptRoot/../data/minimal.csv"
& $Exe "--script=$PSScriptRoot/stage2-start.txt"
& $Exe --help
& $Exe --vfs
& $Exe --unknown
