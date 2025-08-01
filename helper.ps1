# cd desktop
# Set-ExecutionPolicy Bypass -Scope Process -Force; ./sendkeys.ps1
# Adjust sleep timer (seconds) to increase/decrease interval between command repeats

$wshell = New-Object -ComObject wscript.shell
$wshell.AppActivate('Untitled - Notepad')
while ($true) {
    # adjust timer to desired interval
    Start-Sleep 3
    $wshell.SendKeys('a');
    Start-Sleep 3
    $wshell.SendKeys("{BACKSPACE}");
}