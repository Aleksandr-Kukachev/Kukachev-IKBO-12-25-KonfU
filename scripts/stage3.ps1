param([string]$Exe = "$PSScriptRoot/../build/shell-emulator.exe")
# Close each window manually (except the exit-script case).
$Data = "$PSScriptRoot/../data"
& $Exe
& $Exe --vfs "$Data/minimal.csv" --script "$PSScriptRoot/stage3-minimal.txt"
& $Exe --vfs "$Data/files.csv" --script "$PSScriptRoot/stage3-files.txt"
& $Exe --vfs "$Data/deep.csv" --script "$PSScriptRoot/stage3-start.txt"
& $Exe --script "$PSScriptRoot/stage3-exit.txt"
& $Exe --vfs "$Data/missing.csv"
