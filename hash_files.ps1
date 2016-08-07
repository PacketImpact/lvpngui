
$files = @(
	"libpkcs11-helper-1.dll"
	"openvpn.exe"
	"ssleay32.dll"
	"libeay32.dll"
	"liblzo2-2.dll"
	"tap-windows.exe"
)

function Hash-Files([string]$dir) {
	$basePath = $PSScriptRoot + "\" + $dir
	cd $dir
	$hashes = Get-FileHash $files -Algorithm SHA1 | Select "Hash", "Path"
	cd ..
	foreach ($row in $hashes) {
		$row.Path = $row.Path.replace("$basePath", "")
		if ($row.Path.StartsWith("\")) {
			$row.Path = $row.Path.substring(1)
		}
		$row.Path = $row.Path.replace("\", "/")
	}

	$hashes | ConvertTo-Csv -delimiter ' ' -NoTypeInformation | % {$_.Replace('"','').ToLower()}  | Select -skip 1 | Out-File $dir/index.txt
}

Hash-Files "openvpn-32"
Hash-Files "openvpn-64"
