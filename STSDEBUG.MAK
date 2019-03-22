sts.exe: sts.obj helpmain.obj maincfg.obj statsdef.obj headcfg.obj \
statlog1.obj procfg.obj srcecfg.obj cabcfg.obj setanal1.obj setanal2.obj \
setspec1.obj setopts1.obj setopts2.obj findsp1.obj findsp2.obj \
statrun1.obj runutil1.obj runutil2.obj runfun1.obj runfun2.obj \
runfun3.obj functs.obj convert.obj statserr.obj datafun1.obj \
statmak1.obj statinit.obj statmak1.obj optfun1.obj wingcfg.obj \
gundpcfg.obj stxmouse.obj wcs_gnlf.obj stsarc1.obj stsarc2.obj \
setgraf1.obj stsgraf1.obj printcfg.obj stsgraf2.obj stsgraf3.obj
   link @c:\c600\make\sts.lnk

sts.obj: c:\c600\source\sts\sts.c
   cl /c /Zi /Od /nologo /V" Headstat V2.40 - Copyright (c) 1992, West Country \
Systems, Cyprus " c:\c600\source\sts\sts.c

helpmain.obj: c:\c600\source\sts\helpmain.c
   cl /c /nologo c:\c600\source\sts\helpmain.c
   
maincfg.obj: c:\c600\source\sts\maincfg.c
   cl /c /nologo c:\c600\source\sts\maincfg.c

statsdef.obj: c:\c600\source\sts\statsdef.c
	cl /c /Zi /nologo c:\c600\source\sts\statsdef.c
   
headcfg.obj: c:\c600\source\sts\headcfg.c
   cl /c /nologo c:\c600\source\sts\headcfg.c
   
procfg.obj: c:\c600\source\sts\procfg.c
   cl /c /nologo c:\c600\source\sts\procfg.c
   
srcecfg.obj: c:\c600\source\sts\srcecfg.c
   cl /c /nologo c:\c600\source\sts\srcecfg.c

cabcfg.obj: c:\c600\source\sts\cabcfg.c
   cl /c /nologo c:\c600\source\sts\cabcfg.c

wingcfg.obj: c:\c600\source\sts\wingcfg.c
   cl /c /nologo c:\c600\source\sts\wingcfg.c
    
setanal1.obj: c:\c600\source\sts\setanal1.c
   cl /c /nologo c:\c600\source\sts\setanal1.c

setanal2.obj: c:\c600\source\sts\setanal2.c
   cl /c /nologo c:\c600\source\sts\setanal2.c

setspec1.obj: c:\c600\source\sts\setspec1.c
   cl /c /nologo c:\c600\source\sts\setspec1.c

setopts1.obj: c:\c600\source\sts\setopts1.c
   cl /c /nologo c:\c600\source\sts\setopts1.c

setopts2.obj: c:\c600\source\sts\setopts2.c
   cl /c /nologo c:\c600\source\sts\setopts2.c

findsp1.obj: c:\c600\source\sts\findsp1.c
	cl /c /Zi /nologo c:\c600\source\sts\findsp1.c

findsp2.obj: c:\c600\source\sts\findsp2.c
	cl /c /Zi /nologo c:\c600\source\sts\findsp2.c

statrun1.obj: c:\c600\source\sts\statrun1.c
	cl /c /Zi /nologo c:\c600\source\sts\statrun1.c
   
runutil1.obj: c:\c600\source\sts\runutil1.c
	cl /c /Zi /nologo c:\c600\source\sts\runutil1.c
   
runutil2.obj: c:\c600\source\sts\runutil2.c
	cl /c /Zi /nologo c:\c600\source\sts\runutil2.c
   
runfun1.obj: c:\c600\source\sts\runfun1.c
	cl /c /Zi /nologo c:\c600\source\sts\runfun1.c
   
runfun2.obj: c:\c600\source\sts\runfun2.c
	cl /c /Zi /nologo c:\c600\source\sts\runfun2.c

runfun3.obj: c:\c600\source\sts\runfun3.c
	cl /c /Zi /nologo c:\c600\source\sts\runfun3.c
    
datafun1.obj: c:\c600\source\sts\datafun1.c
   cl /c /nologo c:\c600\source\sts\datafun1.c

functs.obj: c:\c600\source\sts\functs.c
	cl /c /Zi /nologo c:\c600\source\sts\functs.c

convert.obj: c:\c600\source\sts\convert.c
	cl /c /Zi /nologo c:\c600\source\sts\convert.c

statserr.obj: c:\c600\source\sts\statserr.c
   cl /c /nologo c:\c600\source\sts\statserr.c

statinit.obj: c:\c600\source\sts\statinit.c
	cl /c /Zi /nologo c:\c600\source\sts\statinit.c
      
statlog1.obj: c:\c600\source\sts\statlog1.c
   cl /c /nologo c:\c600\source\sts\statlog1.c

statmak1.obj: c:\c600\source\sts\statmak1.c
   cl /c /nologo c:\c600\source\sts\statmak1.c


optfun1.obj: c:\c600\source\sts\optfun1.c
	cl /c /Zi /nologo c:\c600\source\sts\optfun1.c
  
gundpcfg.obj: c:\c600\source\sts\gundpcfg.c
   cl /c /nologo c:\c600\source\sts\gundpcfg.c

stsarc1.obj: c:\c600\source\sts\stsarc1.c
   cl /c /nologo c:\c600\source\sts\stsarc1.c

stsarc2.obj: c:\c600\source\sts\stsarc2.c
   cl /c /nologo c:\c600\source\sts\stsarc2.c

stxmouse.obj: c:\c600\source\sts\stxmouse.c
   cl /c /nologo c:\c600\source\sts\stxmouse.c

wcs_gnlf.obj: c:\c600\source\sts\wcs_gnlf.c
   cl /c /nologo c:\c600\source\sts\wcs_gnlf.c

setgraf1.obj: c:\c600\source\sts\setgraf1.c
   cl /c /nologo c:\c600\source\sts\setgraf1.c

stsgraf1.obj: c:\c600\source\sts\stsgraf1.c
	cl /c /Zi /nologo c:\c600\source\sts\stsgraf1.c

printcfg.obj: c:\c600\source\sts\printcfg.c
   cl /c /nologo c:\c600\source\sts\printcfg.c

stsgraf2.obj: c:\c600\source\sts\stsgraf2.c
	cl /c /Zi /nologo c:\c600\source\sts\stsgraf2.c

stsgraf3.obj: c:\c600\source\sts\stsgraf3.c
	cl /c /Zi /nologo c:\c600\source\sts\stsgraf3.c
