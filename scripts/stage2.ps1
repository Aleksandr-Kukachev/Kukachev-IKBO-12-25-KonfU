param([string]$Exe = "$PSScriptRoot/../build/shell-emulator.exe")
# Close each GUI window to continue to the next test.
& $Exe
& $Exe --vfs "$PSScriptRoot/../data/minimal.csv"
& $Exe --script "$PSScriptRoot/stage2-start.txt"
& $Exe --vfs "$PSScriptRoot/../data/minimal.csv" --script "$PSScriptRoot/stage2-start.txt"
& $Exe --script
