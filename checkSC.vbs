Function isSmartCardPresent()
 Dim status
 'launch certutil.exe
 With CreateObject("wscript.shell").Exec("certutil -scinfo")
  'scan through each line of output
  With .StdOut
   Do While Not .AtEndOfStream
    status = .ReadLine
    'we only care about the line with "Status:" in it
    If InStr(status, "Status:") > 0 Then Exit Do
   Loop
  End With
  'if the status line contains this text, a smartcard IS present
  isSmartCardPresent = (InStr(status, "SCARD_STATE_PRESENT") > 0)
 End With
End Function

If isSmartCardPresent Then MsgBox "Smartcard is inserted...make sure you take it out!"
