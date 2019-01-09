
$IniFile = $PSScriptRoot + "\provider\provider.ini"

$IniExists = Test-Path $IniFile

if ($IniExists -eq $True) {
	$VerLine = Get-Item $IniFile | Select-String -CaseSensitive -Pattern "^ *version *="
	$VerNum = $VerLine.ToString().Split("{#}")[0].Split("{=}")[1].Trim()
	echo $VerNum
}
else {
	echo 0
}