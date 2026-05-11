Get-ChildItem .\scripts\*.ps1 | Unblock-File

Get-ChildItem .\scripts\*.ps1 | Get-Item -Stream Zone.Identifier -ErrorAction SilentlyContinue