/***************************************************************
*   STSARC2.C   Archiving and DeArchiving
****************************************************************
*
*	Compiler:  MSC V6.00  /AL /W4 /Os /Gs /Zp /Zl /G2 /ST:3072
*  Linker:    MSC V5.10  /NOE /SE:256 /NOD /EXEPACK
*  Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWL.LIB
*             Greenleaf Superfunctions (MSC) V1.00 GSFL.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*  NOTES:
*      This file contains the file compression & extraction
*      functions.   They work by spawning PKWare's ZIP programs
*      PKZIP.EXE & PKUNZIP.EXE
*
****************************************************************
*	(c) West Country Systems Ltd, Cyprus, 1992
****************************************************************/

#include <stdio.h>         /* microsoft */
#include <dos.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <direct.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>          /* O_ constant definitions */

#include <dw.h>            /* greenleaf */
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct OPT_DEF  *opt;      /* pointer to options list */
extern struct EXT_DEF *ext;
extern HWND Wwait;					 /* working window */


/**************************************************************
*   store current configuration as a .ZFG file
**************************************************************/
void mcfg_arc (void)
{
   HWND W1;                       /* data entry & DOS redir windows */
   HWND Work;                     /* message window */
   TRANSACTION *tp;               /* transaction structure */
   FIELD *fld;                    /* field structure */
	register int i, errnum;
	unsigned drive;
   char dat_buf[65];              /* path/file buffer */
	char orig_dir[41];
		/* check drive/dir */
   _dos_getdrive(&drive);         /* get current disk drive */
	getcwd(orig_dir, 40);
	if(chdir(opt->cfg_dir)) {
		errnum = errno;
		sprintf(dat_buf, "%c:%s", 'A' + drive -1, opt->cfg_dir);
		StatsDosError(dat_buf, errnum);
		return;
	}
	chdir(orig_dir);
		/* start doing the set up */
   W1 = vcreat (6, 37, NORML, NO);
   vlocate (W1, 8, 20);
   vframe (W1, NORML, FRDOUBLE);
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
   sprintf(dat_buf, " Store At %c:%s ", 'A' +opt->work_dr -1, opt->cfg_dir);
   vtitle (W1, NORML, dat_buf);
      /*# defldval_mcfg_arc() detranval_glob() EscapeKey() arc_help() */
   tp = vpreload (1, 3, NULLF, defldval_mcfg_arc, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
   tp->paint_fl = TRUE;
   tp->helpfn = arc_help;   /* ignore param warning */
	tp->inp_idle = UpdateClockWindow;
	fld = *(tp->def);
   vdefstr (tp, 0, 1, 1, "Filename", 20, 8,
         "!!!!!!!!", ext->a_str, YES);
	vdefatsa (tp, 0, 3, 1, HIGHNORML, "EIGHT CHAR FILENAME WITHOUT SPACES");
   vdefatsa (tp, 1, 5, 3, NORML, "<ESC> Exit");
   vdefatsa (tp, 2, 5, 24, HIGHNORML, "<CR> Store");
   visible (W1, YES, NO);
   if(!vread (tp, W1, YES)) {   /* only if DWSUCCESS (0) */
      vdelete(W1, NONE);
      sprintf(dat_buf, "%c:%s\\%s.ZFG", 'A' + opt->work_dr -1, opt->cfg_dir, ext->a_str);
      W1 = vcreat(5, 78, NORML, NO);   /* dummy window for redir() */
      vredir(ON, W1);
      curdrag(W1, ON);
      Work =  vcreat(1, 20 + strlen(dat_buf), NORML, NO);  /* msg window */
      vlocate(Work, 20, 2);
      vframe (Work, EMPHNORML, FRSINGLE);
      vtitle (Work, EMPHNORML, " Current Configuration Files ");
      vatputf(Work, 0, 0, "ARCHIVING *.CFG TO: %s", dat_buf);
      visible(Work, YES, NO);
      _heapmin();
		if((i = spawnlp(P_WAIT, "PKZIP.EXE", "PKZIP.EXE",
							 "-a", dat_buf, "*.CFG", NULL)) == -1) {
			errnum = errno;      /* IF A DOS ERROR */
         vredir(OFF, W1);
         vdelete(W1, NONE);
         vdelete(Work, NONE);
			StatsDosError("PKZIP.EXE", errnum);
			return;
		}
      if(i) {                     /* IF A PKZIP.EXE ERROR CODE */
         vredir(OFF, W1);
         vdelete(W1, NONE);
         vdelete(Work, NONE);
			if(i == 1) StatsErr("90002");
			if((i == 2)||(i == 3)) StatsErr("90003");
			if((i > 3)&&(i < 12)) StatsErr("90004");
			if(i == 12) StatsErr("90005");
			if(i == 13) StatsErr("90006");
			if(i == 14) StatsErr("90007");
			if(i == 15) StatsErr("90008");
			if(i == 16) StatsErr("90009");
			if(i == 17) StatsErr("90010");
			return;
		}
      vredir(OFF, W1);
      vdelete(Work, NONE);
	}
   vdelete(W1, NONE);
	return;
}


/**************************************************************
*	transaction validation for store configuration mcfg_arc()
**************************************************************/
defldval_mcfg_arc()
{
	register int i;
	for(i=0;i<8;i++) {
		if((ext->a_str[i] == ' ')||(ext->a_str[i] == '.')
				||(ext->a_str[i] == ',')||(ext->a_str[i] == '\0')) {
			StatsErr("90001");
			return(FALSE);
		}
	}
	return(TRUE);
}


/**************************************************************
*   retrieve a stored set of configuration files & update
**************************************************************/
void dcfg_arc (void)
{
   HWND W1;                       /* data entry window */
   HWND Work;                     /* message window */
	register int i, errnum;
   char dat_buf[65];              /* .ZFG path/file buffer */
	W1 = vcreat(4, 47, NORML, NO);
	vlocate(W1, 19, 17);
	vframe (W1, HIGHNORML, FRDOUBLE);
   vatputs(W1, 0, 2, "Current Configuration will be deleted prior");
   vatputs(W1, 1, 7, "to retrieval of stored .ZFG file");
   vratputs(W1, 2, 0, HIGHNORML,
                     "  ??? ARCHIVE  CURRENT CONFIGURATION FIRST ??? ");
   vatputs(W1, 3, 16, "<ESC> to EXIT");
	visible (W1, YES, NO);
   if(find_file(0, "*.ZFG", opt->work_dr, opt->cfg_dir) != 1) {
		vdelete(W1, NONE);
		return;
	}
	vdelete(W1, NONE);
	sprintf(dat_buf, "%c:%s\\%s", 'A' + opt->work_dr -1, opt->cfg_dir, ext->pname);
   W1 = vcreat(5, 78, NORML, NO); /* dummy window for redir() */
   remove("HEADER.CFG");          /* delete old .CFG files */
   remove("ANALYSIS.CFG");
   remove("OPTIONS.CFG");
   remove("GRAPHICS.CFG");
   remove("PROJECT.CFG");
   remove("SOURCE.CFG");
   remove("CABLE.CFG");
   remove("GUNDEPTH.CFG");
   remove("WINGS.CFG");
	vredir(ON, W1);
   curdrag(W1, ON);
   Work =  vcreat(1, 21 + strlen(dat_buf), NORML, NO);  /* msg window */
   vlocate(Work, 20, 2);
   vframe (Work, EMPHNORML, FRSINGLE);
   vtitle (Work, EMPHNORML, " Retrieve Configuration Files ");
   vatputf(Work, 0, 0, "UPDATING *.CFG FROM: %s", dat_buf);
   visible(Work, YES, NO);
   _heapmin();
	if((i = spawnlp(P_WAIT, "PKUNZIP.EXE", "PKUNZIP.EXE",
							"-o", dat_buf, "*.CFG", NULL)) == -1) {
		errnum = errno;
		vredir(OFF, W1);
      vdelete(W1, NONE);
      vdelete(Work, NONE);
		StatsDosError("PKUNZIP.EXE", errnum);
		return;
	}
   if(i) {                        /* IF A PKUNZIP.EXE ERROR CODE */
		vredir(OFF, W1);
      vdelete(W1, NONE);
      vdelete(Work, NONE);
		if(i == 1) StatsErr("91002");
		if((i == 2)||(i == 3)) StatsErr("91003");
		if((i > 3)&&(i < 9)) StatsErr("91004");
		if(i == 9) StatsErr("91005");
		if(i == 10) StatsErr("91006");
		if(i == 11) StatsErr("91007");
		if(i == 50) StatsErr("91008");
		if(i == 51) StatsErr("91009");
		return;
	}
	vredir(OFF, W1);
   vdelete(W1, NONE);
   vdelete(Work, NONE);
	loaddef( 0, 0 );
	return;
}


/**************************************************************
*   dearchive a .ZIP file;
*   include the configuration files & update the configuration
*   and auto run stats  (if file found)
**************************************************************/
d_arc (void)
{
   HWND W1;                       /* zip window */
   HWND Work;                     /* message window */
	register int i, errnum;
   char dat_buf[65];              /* path/file buffer */
   char orig_dir[51];             /* original dir */
   char dat_dir[51];              /* data directory */
   unsigned drive, num_drives;    /* original drive */
   ext->a_num = 0;                /* stop autorun if no file found */
   getcwd(orig_dir, 50);
   _dos_getdrive(&drive);         /* get current disk drive */
	_dos_setdrive(ext->fl_drive, &num_drives);
    if(chdir("\\")) {             /* check for floppy media */
		_dos_setdrive(drive, &num_drives);
      ext->a_num = 1;             /* stop autorun */
		StatsErr("91010");
		return(0);
	}
	if(chdir(ext->back_dir)) {
		_dos_setdrive(drive, &num_drives);
		chdir(orig_dir);
		ext->a_num = 1;
		StatsErr("91001");
		return(0);
	}
	_dos_setdrive(drive, &num_drives);
	chdir(orig_dir);
   if(find_file(0, "*.ZIP", ext->fl_drive, ext->back_dir) != 1) {
      ext->a_num = 1;             /* stop autorun */
		return(0);
	}
	StWait();
   remove("HEADER.CFG");          /* delete old .CFG files */
   remove("ANALYSIS.CFG");
   remove("OPTIONS.CFG");
   remove("GRAPHICS.CFG");
   remove("PROJECT.CFG");
   remove("SOURCE.CFG");
   remove("CABLE.CFG");
   remove("GUNDEPTH.CFG");
   remove("WINGS.CFG");
	if(chdir(opt->dat_dir)) {
		StatsErr("91013");
      ext->a_num = 1;             /* stop autorun */
		return(0);
   }
   chdir(orig_dir);
	if(1 == strlen(ext->back_dir))
		sprintf(dat_buf, "%c:\\%s", 'A' + ext->fl_drive -1, ext->pname);
   else sprintf(dat_buf, "%c:%s\\%s", 'A' + ext->fl_drive -1,
                         ext->back_dir, ext->pname);
   sprintf(dat_dir, "%c:%s", 'A' + opt->work_dr -1, opt->dat_dir);
   W1 = vcreat(5, 78, NORML, NO);   /* dummy window for redir() */
	vredir(ON, W1);
   curdrag(W1, ON);
   Work =  vcreat(1, 24 + strlen(dat_buf), NORML, NO);  /* msg window */
   vlocate(Work, 20, 2);
   vframe (Work, EMPHNORML, FRSINGLE);
   vtitle (Work, EMPHNORML, " Retrieve Data & Configuration ");
   vatputf(Work, 0, 0, " DEARCHIVING DATA FROM: %s", dat_buf);
   visible(Work, YES, NO);
   _heapmin();
	if((i = spawnlp(P_WAIT, "PKUNZIP.EXE", "PKUNZIP.EXE",
                   "-o", dat_buf, dat_dir, "*.DAT", NULL)) == -1) {
		errnum = errno;
		vredir(OFF, W1);
      vdelete(W1, NONE);
      vdelete(Work, NONE);
		EnWait();
		StatsDosError("PKUNZIP.EXE", errnum);
		ext->a_num = 1;				 /* stop autorun */
		return(0);
	}
   if(i) {                        /* IF A PKUNZIP.EXE ERROR CODE */
		vredir(OFF, W1);
      vdelete(W1, NONE);
      vdelete(Work, NONE);
		EnWait();
		if(i == 1) StatsErr("91002");
		if((i == 2)||(i == 3)) StatsErr("91003");
		if((i > 3)&&(i < 9)) StatsErr("91004");
		if(i == 9) StatsErr("91005");
		if(i == 10) StatsErr("91006");
		if(i == 11) StatsErr("91007");
		if(i == 50) StatsErr("91008");
		if(i == 51) StatsErr("91009");
		ext->a_num = 1;	/* stop autorun */
		return(0);
   }
   vatputf(Work, 0, 0, "UPDATING CONFIGURATION: %s", dat_buf);
	if((i = spawnlp(P_WAIT, "PKUNZIP.EXE", "PKUNZIP.EXE",
						 "-o", dat_buf, "*.CFG", NULL)) == -1) {
		errnum = errno;
		vredir(OFF, W1);
      vdelete(W1, NONE);
      vdelete(Work, NONE);
		EnWait();
		StatsDosError("PKUNZIP.EXE", errnum);
		ext->a_num = 1;	/* stop autorun */
		return(0);
	}
   if(i) {                        /* IF A PKUNZIP.EXE ERROR CODE */
		vredir(OFF, W1);
      vdelete(W1, NONE);
      vdelete(Work, NONE);
		EnWait();
		if(i == 1) StatsErr("91002");
		if((i == 2)||(i == 3)) StatsErr("91003");
		if((i > 3)&&(i < 9)) StatsErr("91004");
		if(i == 9) StatsErr("91005");
		if(i == 10) StatsErr("91006");
		if(i == 11) StatsErr("91007");
		if(i == 50) StatsErr("91008");
		if(i == 51) StatsErr("91009");
		ext->a_num = 1;	/* stop autorun */
		return(0);
	}
	vredir(OFF, W1);
   vdelete(W1, NONE);
   vdelete(Work, NONE);
	EnWait();
	loaddef( 0, 100 );	/* check .CFG integrity but suppress load banner */
   vbeep();
   Delay18(1);
   vbeep();
	return(1);
}


/**************************************************************
*   archive a .DAT file to floppy using PKZIP.EXE
*   include the configuration files
**************************************************************/
void m_arc (void)
{
   HWND W1;
   HWND Work;                     /* message window */
   char orig_dir[65];             /* original directory */
   char dat_buf[65];              /* .DAT path/file buffer */
   char zip_name[65];             /* floppy zipfile path/file */
   char logbuf[13];               /* filename buffer */
	register int i, errnum;
   unsigned drive, num_drives;
   float avail, total;            /* available & total disk space */
   int handle;                    /* for filelength handle */
   long length;                   /* filelength */
		/* check that archive drive is ready */
   _dos_getdrive(&drive);         /* get current disk drive */
   getcwd(orig_dir, 64);          /* get current directory */
	_dos_setdrive(ext->fl_drive, &num_drives);
    if(chdir("\\")) {             /* check for floppy media */
		_dos_setdrive(drive, &num_drives);
		StatsErr("91010");
		return;
	}
   if(chdir(ext->back_dir)) {     /* check archive dr/dir exists */
		_dos_setdrive(drive, &num_drives);
		chdir(orig_dir);
		StatsErr("91001");
		return;
	}
	_dos_setdrive(drive, &num_drives);
	chdir(orig_dir);
		/* look for a .DAT file */
   if(find_file(1, "*.DAT", opt->work_dr, opt->dat_dir) != 1)
		return;
   for(i=0;i<8;i++) {             /* check for 8 chars/no spaces */
		if((ext->pname[i] == ' ')||(ext->pname[i] == '.')
				||(ext->pname[i] == ',')||(ext->pname[i] == '\0')) {
			StatsErr("91014");
			return;
		}
	}
	for(i=0;i<8;i++) logbuf[i] = ext->pname[i];
	logbuf[8] = '\0';
	strcat(logbuf, ".ZIP");
	if(1 == strlen(ext->back_dir))
		sprintf(zip_name, "%c:\\%s", 'A' + ext->fl_drive -1, logbuf);
	else sprintf(zip_name, "%c:%s\\%s", 'A' + ext->fl_drive -1,
														  ext->back_dir, logbuf);
	sprintf(dat_buf, "%c:%s\\%s", 'A' + opt->work_dr -1,
                                opt->dat_dir, ext->pname);
      /* check that space exists on hard drive for intermediate file */
   if( (handle = open( dat_buf, O_BINARY | O_RDONLY )) == -1) { /* open file */
      StatsErr("91015");   /* could not open data file */
      return;
   }
   length = filelength( handle );         /* get .DAT file length */
   close( handle );                       /* clode .DAT file */
   get_free( (char) ('A' + opt->work_dr -1), &avail, &total);  /* get free space on work drive */
   if( ( ((long)avail) * 1024L) < length) {   /* equal space ??? */
      StatsErr("91016");   /* not enough room on hard drive (inter file) */
      return;
   }
      /* check that 1/4 .DAT filelength free on archive drive */
   get_free( (char) ('A' + ext->fl_drive -1), &avail, &total);
   if( ( ((long)avail) * 1024L) < (length/4L) ) {
      StatsErr("91017");   /* probably not enough room on archive drive */
      return;
	}
	StWait();
   W1 = vcreat(5, 78, NORML, NO);  /* dummy window for redir() */
	vredir(ON, W1);
   curdrag(W1, ON);
   Work =  vcreat(1, 16 + strlen(dat_buf), NORML, NO);  /* msg window */
   vlocate(Work, 20, 2);
   vframe (Work, EMPHNORML, FRSINGLE);
			/* will make temporary file in the work drive root */
   sprintf(logbuf, "-b%c:", 'A' + opt->work_dr - 1);
   _heapmin();
         /* archive configuration */
   vtitle(Work, EMPHNORML, " Archive *.CFG ");
   vatputf(Work, 0, 0, "STORE CONFIGURATION: %s", zip_name);
   visible(Work, YES, NO);
	if((i = spawnlp(P_WAIT, "PKZIP.EXE", "PKZIP",
							 "-a", logbuf, zip_name, "*.CFG", NULL)) == -1) {
		errnum = errno;
		vredir(OFF, W1);
      vdelete(W1, NONE);
		vdelete(Work, NONE);
		EnWait();
		StatsDosError("PKZIP.EXE", errnum);
		return;
	}
   if(i) {                        /* IF A PKZIP.EXE ERROR CODE */
		vredir(OFF, W1);
      vdelete(W1, NONE);
         vdelete(Work, NONE);
			EnWait();
         if(i == 1) StatsErr("90002");
         if((i == 2)||(i == 3)) StatsErr("90003");
         if((i > 3)&&(i < 12)) StatsErr("90004");
         if(i == 12) StatsErr("90005");
         if(i == 13) StatsErr("90006");
         if(i == 14) StatsErr("90007");
         if(i == 15) StatsErr("90008");
         if(i == 16) StatsErr("90009");
         if(i == 17) StatsErr("90010");
         return;
   }
   vclear(Work);
         /* archive data */
   vtitle (Work, EMPHNORML, " Archive Data ");
   vatputf(Work, 0, 0,      "ARCHIVING FILE: %s", dat_buf);
	if((i = spawnlp(P_WAIT, "PKZIP.EXE", "PKZIP",
							 "-m", logbuf, zip_name, dat_buf, NULL)) == -1) {
		errnum = errno;
		vredir(OFF, W1);
      vdelete(W1, NONE);
      vdelete(Work, NONE);
		EnWait();
		StatsDosError("PKZIP.EXE", errnum);
		return;
	}
   if(i) {                        /* IF A PKZIP.EXE ERROR CODE */
		vredir(OFF, W1);
      vdelete(W1, NONE);
         vdelete(Work, NONE);
			EnWait();
         if(i == 1) StatsErr("90002");
         if((i == 2)||(i == 3)) StatsErr("90003");
         if((i > 3)&&(i < 12)) StatsErr("90004");
         if(i == 12) StatsErr("90005");
         if(i == 13) StatsErr("90006");
         if(i == 14) StatsErr("90007");
         if(i == 15) StatsErr("90008");
         if(i == 16) StatsErr("90009");
         if(i == 17) StatsErr("90010");
         return;
   }
	vredir(OFF, W1);
   vdelete(W1, NONE);
   vdelete(Work, NONE);
	EnWait();
	vbeep();
   Delay18(1);
   vbeep();
	return;
}
