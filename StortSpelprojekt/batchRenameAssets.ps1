cd "$Home\Google Drive\Stort spelprojekt\Assets\"
Get-ChildItem -File -Recurse | % { Rename-Item -Path $_.PSPath -NewName $_.Name.replace(" (1)","")}