$global:isPresent = $false
$global:oldStatus = $false
$global:isLocked = $false
$global:oldLockStatus = $false
$global:funcSmartCard;

function checkSmartCardChange() {
    # Write-Host "Checking Smart Card Status..."
    $cutil = Start-Job -ScriptBlock {certutil -scinfo -silent}
    while ($cutil.State -eq 'Running') {
        $output = Receive-Job $cutil
        ForEach ($line in $output) {
            if ($line -match "CardOS") {
                $global:isPresent = $true
                Stop-Job $cutil
                break
            } else {
                $global:isPresent = $false
            }
        }
    } <#
    If ($isPresent) {
        Write-Host "Smart Card Present"
    } Else {
        Write-Host "Smart Card Not Present"
    }  #>
    if ($global:isPresent -and !$global:oldStatus) {
        #Write-Host "Smart Card Inserted"
        $global:oldStatus = $true
    } elseif (!$global:isPresent -and $global:oldStatus) {
        #Write-Host "Smart Card Removed"
        $global:oldStatus = $false
    }
}

function isWorkstationLocked() {
    #Write-Host "Checkin workstation lock status..."
    If (Get-Process -Name "LogonUI" -ErrorAction SilentlyContinue) {
        $global:isLocked = $true
    } Else {
        $global:isLocked = $false
    }
    if ($global:isLocked -and !$global:oldLockStatus) {
        $global:oldLockStatus = $true
        #Write-Host "Workstation Locked"
        if ($global:isPresent) {
            while ($global:isPresent) {
                $alert = New-Object System.Media.SoundPlayer
                $alert.SoundLocation = "./button.wav"
                $alert.PlaySync()
                checkSmartCardChange
            }
        }
    } elseif (!$global:isLocked -and $global:oldLockStatus) {
        $global:oldLockStatus = $false
        #Write-Host "Workstation Unlocked"
        $voice = NEw-Object -ComObject SAPI.SpVoice
        $voice.Speak("Workstation unlocked")
    }
}

while ($true) {
    checkSmartCardChange
    isWorkstationLocked
    Start-Sleep -Seconds 2
}