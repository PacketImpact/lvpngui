Param([Parameter(Mandatory=$true)][string[]$Dir)

function Hash-Files([string]$dir) {
	$basePath = Resolve-Path $dir
	$files = Get-ChildItem -Path $dir -File -Force -Recurse |
		% {join-path -Path $dir -ChildPath $_.Name}
	$hashes = Get-FileHash $files -Algorithm SHA1 | Select "Hash", "Path"
	foreach ($row in $hashes) {
		$row.Path = $row.Path.replace("$basePath", "")
		if ($row.Path.StartsWith("\")) {
			$row.Path = $row.Path.substring(1)
		}
		$row.Path = $row.Path.replace("\", "/")
	}

	$hashes |
		ConvertTo-Csv -delimiter ' ' -NoTypeInformation |
		% {$_.Replace('"','').ToLower()}  |
		Select -skip 1 |
		Out-File $dir/index.txt
}

Hash-Files $Dir
