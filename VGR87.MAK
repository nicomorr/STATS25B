vgr.exe: vgr.obj
   link @c:\c600\make\vgr87.lnk

vgr.obj: c:\c600\source\sts\vgr.c
   cl /c /nologo /V" Copyright (c) WCS Cyprus 1992 " c:\c600\source\sts\vgr.c
 
