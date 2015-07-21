Dim objFSO
Dim objFile
Dim objShell
Dim objSource
Dim objZip
Dim strVer
Dim strZip

'
' initialize objects
'
Set objShell = CreateObject("Shell.Application") 
Set objScriptShell = CreateObject("Wscript.Shell") 
Set objFSO = CreateObject("Scripting.FileSystemObject")

'
' read version file
'
Set objFile = objFSO.OpenTextFile("VERSION.TXT", 1)
strVer = objFile.ReadLine()
Set objFile = Nothing

'
' create zip file name with version
'
strZip = Replace(WScript.Arguments(1), "{VERSION}", Trim(strVer))

'
' create zip file
'
Set objFile = objFSO.CreateTextFile(strZip, True)
objFile.Write("PK" & Chr(5) & Chr(6) & String(18, vbNullChar))
Set objFile = Nothing

'
' copy files to zip folder
'
Set objSource = objShell.NameSpace(objFSO.GetAbsolutePathName(WScript.Arguments(0))).Items
Set objZip = objShell.NameSpace(objFSO.GetAbsolutePathName(strZip))
Set objFSO = Nothing
objZip.CopyHere(objSource) 

'
' Wait for zip window to open
'
Do 
  WScript.Sleep(1) 
Loop While Not objScriptShell.AppActivate("Compressing...")

'
' Wait for window to close
'
Do 
  WScript.Sleep(1)
Loop While objScriptShell.AppActivate("Compressing...")

Set objScriptShell = Nothing
Set objShell = Nothing