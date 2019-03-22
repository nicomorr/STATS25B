/***************************************************************
*   INSTALL.C   installation program for HEADSTAT
****************************************************************
*  Currently V2.0 which works from Drives A & B to C & D
*
*  Compiler:  MSC V6.00  /AM /W4 /Os /Gs /Zp /Zl /G2
*  Linker:    MSC V5.10  /NOE /NOD /EXEPACK
*  Libraries: MSC V6.00 MLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWM.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*  (c) West Country Systems Ltd, Cyprus, 1991.
****************************************************************/

#include <stdio.h>                    /* microsoft */
#include <stdlib.h>
#include <dos.h>
#include <direct.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <errno.h>
#include <graph.h>

#include <dw.h>                       /* Greenleaf */
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

         /* function prototypes */
void main (void);                     /* main */
void EscapeKey(TRANSACTION *);
int defldval_drive (TRANSACTION *);
int detranval_glob (TRANSACTION *);
int KeySure (void);
void StatsError (char *);
HWND Message(HWND , char *);
int copyfile( char *, char *);
unsigned _memo(TRANSACTION *, HWND);    /* stub - dw memsaver */
unsigned _subform(TRANSACTION *, HWND);   /* stub - dw memsaver */

         /* global drive letter */
char c = {'C'};

         /* hold HEADSTAT general program version */
char ver[] = {"HEADSTAT V2"};


/***************************************************************
*   main calling function
****************************************************************/
void main ()
{
   HWND Wtop, Wbot, Wbord, Wfake, Wmsg=0, Wmissing, W1, W2;
   TRANSACTION *tp;
	FIELD *fld;
   FILE *fp;
   char buff[_MAX_PATH];
   char copy[_MAX_PATH];
   char orig_dir[_MAX_PATH];
   unsigned key;
   register int i=0;
   int error;
   unsigned orig_drv, drv_count;
   _dos_getdrive(&orig_drv);                 /* get current disk drive */
   getcwd(orig_dir, _MAX_PATH);              /* get current directory */
         /* check that source drive is A:\ or B:\ */
   if( (strcmp(orig_dir, "A:\\")!=0) &&
       (strcmp(orig_dir, "B:\\")!=0) ) {
		_clearscreen(_GCLEARSCREEN);
		_settextposition(3, 0);
      printf("INSTALL CAN ONLY BE RUN FROM DRIVES A:\\ OR B:\\ \n");
      printf("\t\tSEE ECLSTAT.DOC FOR DETAILS\n\n");
      exit(1);
   }
         /* on correct drive - so start installation */
   Wbord = vcreat (23, 78, NORML, NO);       /* border window */
   Wtop = vcreat (1, 80, REVNORML, NO);      /* top line window */
   Wbot = vcreat (1, 80, REVNORML, NO);      /* bottom line window */
   Wfake = vcreat (1, 80, REVNORML, NO);     /* vredir() DOS window */
   Wmissing = vcreat (20, 74, REVNORML, NO);   /* status window at end */
   W1 = vcreat (1, 27, NORML, NO);           /* data input window */
   W2 = vcreat (7, 74, NORML, NO);           /* information window */
   vlocate (Wbord, 1, 1);
   vlocate (Wtop, 0, 0);
   vlocate (Wbot, 24, 0);
   vlocate (W1, 3, 15);
   vlocate (W2, 11, 3);
   vframe (Wbord, NORML, FRDOUBLE);
      /* here define INSTALL VERSION NUMBER */
   vratputf (Wtop, 0, 1, REVNORML, "%s INSTALLATION PROGRAM V2.0", ver);
   vratputs (Wbot, 0, 14, REVNORML,
      "Intera ECL Petroleum Technologies Ltd   -   Henley UK");
   breakchk(OFF);   /* <CTRL-C checking */
   vframe (W1, NORML, FRSINGLE);
   vframe (W2, NORML, FRSINGLE);
   vratputf(W2, 0, 0, REVNORML,
      "%s can be installed on to drives C or D only.", ver);
   vatputs(W2, 1, 0,
      "Program files will be copied to \\ECLSTAT directory.");
   vatputs(W2, 2, 0,
      "Stored example configurations will be put into \\ECLSTAT\\CONFIG.");
   vatputs(W2, 3, 0,
      "These directories will be created if they do not already exist.");
   vatputs(W2, 4, 0,
"An empty data directory \\ECLHEAD\\DATA will be made if it does not exist.");
   vatputs(W2, 5, 0,
      "An empty reporting directory \\ECLSTAT\\TEXT will also be created.");
   vatputs(W2, 6, 0,
"You can change these directories in HeadStat Options later if you wish.");
      /*# defldval_drive() detranval_glob() EscapeKey() UpdateClockWindow() */
   tp = vpreload (1, 1, NULLF, defldval_drive, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
   tp->inp_idle = UpdateClockWindow;
	fld = *(tp->def);
	fld[0].use_ext = TRUE;   /* use/display the preload values */
   vdefchar (tp, 0, 0, 22, NULL, 24, 1, "!", &c, YES);
   vdefatsa (tp, 0, 0, 1, NORML, "Stats Program Drive ..");
         /* make the usual clock/date window */
   BuildClockWindow (0, -1, REVNORML, 0, 10, 5, 2);
         /* make it all visible */
   visible (Wbord, YES, NO);
   visible (Wtop, YES, NO);
   visible (Wbot, YES, NO);
   visible(W1, YES, NO);
   visible(W2, YES, NO);
   ControlClockWindow (YES, NO);
   curdrag(W1, ON);
   vread(tp, W1, YES);
   vdelete(W1, NONE);
   W1 = vcreat (2, 60, NORML, NO);
   vlocate (W1, 3, 9);
   vratputf(W1, 0, 0, REVNORML,
      "%s WILL BE INSTALLED ON DRIVE %c:\\ECLSTAT       ", ver, c);
   vratputs(W1, 1, 0, REVERR,
      "PRESS <ENTER> TO CONTINUE INSTALLATION - ANY OTHER KEY EXITS");
   visible(W1, YES, NO);
   if( (key = vgetkey(UpdateClockWindow)) != CR) {
      pcuron();
      breakchk(ON);
      vexit(1);
   }
   vdelete (W2, NONE);
   vdelete (W1, NONE);
      /* check for existing installation */
   _dos_setdrive(c - 'A' + 1, NULL);  /* change to requested drive */
   sprintf(buff, "%c:\\ECLSTAT", c);
   if(!chdir("\\ECLSTAT")) {           /* request directory */
      sprintf(buff, "%c:\\ECLSTAT\\STS.EXE", c);
      if((fp = fopen(buff, "rb")) != NULL) {
         fclose(fp);
         W1 = vcreat (3, 60, NORML, NO);
         vlocate (W1, 3, 9);
         vratputf(W1, 0, 0, REVERR,
            "STS.EXE ALREADY EXISTS ON %c:\\ECLSTAT", c);
         vratputs(W1, 1, 0, HIGHNORML,
            "BACK UP YOUR CURRENT VERSION BEFORE INSTALLING NEW V2");
         vratputs(W1, 2, 0, EMPHNORML,
            "PRESS <ENTER> TO CONTINUE - ANY OTHER KEY EXITS INSTALL");
         visible(W1, YES, NO);
         if( (key = vgetkey(UpdateClockWindow)) != CR) {
            _dos_setdrive(orig_drv, NULL);  /* back to original drive */
            chdir(orig_dir);                /* back to original directory */
            pcuron();
            breakchk(ON);
            vexit(1);
         }
         vdelete (W1, NONE);
      }
   }
   _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
   chdir(orig_dir);                /* back to original directory */
      /* make program directory & copy program files */
   sprintf(buff,
   "Making Directory %c:\\ECLSTAT & Installing Program Files", c );
   Wmsg = Message(Wmsg, buff);
   vredir(ON, Wfake);

   _dos_setdrive(c - 'A' + 1, &drv_count);  /* change to requested drive */
   sprintf(buff, "%c:\\ECLSTAT", c);
   mkdir(buff);
   if(chdir("\\ECLSTAT")) {           /* request directory */
      vatputf(Wmissing, i, 1,
         "Unable to Create %c:\\ECLSTAT - cannot copy program files", c);
      i++;
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
   } else {
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */

      sprintf(copy, "%c:\\STS.EXE", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\STS.EXE", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\STS.EXE - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\DONGLE.EXE", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\DONGLE.EXE", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\DONGLE.EXE - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\PKZIP.EXE", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\PKZIP.EXE", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\PKZIP.EXE - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\PKUNZIP.EXE", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\PKUNZIP.EXE", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\PKUNZIP.EXE - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\STATERR.ERR", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\STATERR.ERR", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\STATERR.ERR - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\BKDRIVE.SET", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\BKDRIVE.SET", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\BKDRIVE.SET - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\PRINTER.SET", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\PRINTER.SET", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\PRINTER.SET - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\STATMAIN.HLP", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\STATMAIN.HLP", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
			vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\STATMAIN.HLP - check installation diskette", c);
			i++;
      }

      sprintf(copy, "%c:\\MODERN.FON", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\MODERN.FON", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\MODERN.FON - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\README.DOC", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\README.DOC", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\README.DOC - check installation diskette", c);
         i++;
      }

      sprintf(copy, "%c:\\ECLSTAT.DOC", 'A' + orig_drv - 1);
      sprintf(buff, "%c:\\ECLSTAT\\ECLSTAT.DOC", c);
      if(error = copyfile(copy, buff)) {
         strcat(copy, ": ");
         strcat(copy, strerror(error));
         StatsError(copy);
         vatputf(Wmissing, i, 1,
"Unable to Copy %c:\\ECLSTAT\\ECLSTAT.DOC - check installation diskette", c);
         i++;
      }
         /* copy any configuration (.CFG) files */
      sprintf(buff, "copy *.cfg %c:\\ECLSTAT", c);
      system(buff);
   }
   vredir(OFF, Wfake);
   vdelete(Wmsg, NONE);
      /* make configuration directory & copy .ZFG files */
   sprintf(buff,
"Making Directory %c:\\ECLSTAT\\CONFIG & Copying Stored Configurations", c);
   Wmsg = Message(Wmsg, buff);
   vredir(ON, Wfake);

   _dos_setdrive(c - 'A' + 1, &drv_count);  /* change to requested drive */
   sprintf(buff, "%c:\\ECLSTAT\\CONFIG", c);
   mkdir(buff);
   if(chdir("\\ECLSTAT\\CONFIG")) {   /* request directory */
      vatputf(Wmissing, i, 1,
      "Unable to Create %c:\\ECLSTAT\\CONFIG - cannot copy .ZFG files", c);
      i++;
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
   } else {
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
      /* copy any stored configuration files (.ZFG) */
      sprintf(buff, "copy *.zfg %c:\\ECLSTAT\\CONFIG", c);
      system(buff);
   }
   vredir(OFF, Wfake);
   vdelete(Wmsg, NONE);
      /* make text & data directories */
   sprintf(buff,
   "Making Directories %c:\\ECLSTAT\\TEXT & c:\\ECLHEAD\\DATA", c, c);
   Wmsg = Message(Wmsg, buff);
   vredir(ON, Wfake);

   _dos_setdrive(c - 'A' + 1, &drv_count);  /* change to requested drive */
   sprintf(buff, "%c:\\ECLSTAT\\TEXT", c);
   mkdir(buff);
   if(chdir("\\ECLSTAT\\TEXT")) {     /* request directory */
      vatputf(Wmissing, i, 1,
      "Unable to Create %c:\\ECLSTAT\\TEXT reporting directory", c);
      i++;
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
   } else {
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
   }

   _dos_setdrive(c - 'A' + 1, &drv_count);  /* change to requested drive */
   sprintf(buff, "%c:\\ECLHEAD", c);
   mkdir(buff);
   if(chdir("\\ECLHEAD")) {           /* request directory */
      vatputf(Wmissing, i, 1,
      "Unable to Create %c:\\ECLHEAD - data base directory", c);
      i++;
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
   } else {
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
   }

   _dos_setdrive(c - 'A' + 1, &drv_count);  /* change to requested drive */
   sprintf(buff, "%c:\\ECLHEAD\\DATA", c);
   mkdir(buff);
   if(chdir("\\ECLHEAD\\DATA")) {     /* request directory */
      vatputf(Wmissing, i, 1,
      "Unable to Create %c:\\ECLHEAD\\DATA data directory", c);
      i++;
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
   } else {
      _dos_setdrive(orig_drv, &drv_count);  /* back to original drive */
      chdir(orig_dir);                /* back to original directory */
   }
   _dos_setdrive(c - 'A' + 1, &drv_count);  /* change to requested drive */
   sprintf(buff, "%c:\\ECLSTAT", c);
   chdir(buff);                       /* leave in \ECLSTAT if poss */
   _dos_setdrive(orig_drv, &drv_count);     /* back to original drive */
   chdir(orig_dir);                   /* back to original directory */

   vredir(OFF, Wfake);
   vbeep();
   if(!i) {
      W2 = vcreat (5, 74, NORML, NO);
      vlocate (W2, 8, 3);
      vframe (W2, NORML, FRSINGLE);
      vatputs(W2, 0, 1, "Your installation floppy may contain example archived data lines (.ZIP)");
      vatputs(W2, 1, 1, "You can  test the installation by retrieving/autorunning stats");
      vatputs(W2, 2, 1, "from F7 Main Menu.  This will extract the data file (.DAT) to the data");
      vatputs(W2, 3, 1, "directory, & the configuration files to the program directory, update");
      vatputs(W2, 4, 1, "the configuration & stop at the run-time summary.");
      vatputs(W2, 6, 1, "Archive Drive is presently set to A: - if you wish to use drive B, then");
      vatputs(W2, 7, 1, "go to Options F5, change to drive B & save the Option Configuration.");
      vatputs(W2, 8, 1, "NOTE: ECL Dongle must be installed before running HEADSTAT V2");
      visible(W2, YES, NO);
      vdelete(Wmsg, NONE);
      W1 = vcreat (1, 39, NORML, NO);
      vlocate (W1, 4, 9);
      vframe (W1, EMPHNORML, FRSINGLE);
      vratputs(W1, 0, 0, HIGHNORML, "INSTALLATION COMPLETE - ANY KEY TO EXIT");
      visible(W1, YES, NO);
      _dos_setdrive(c - 'A' + 1, &drv_count);  /* change to requested drive */
   } else {
      vdelete(Wmsg, NONE);
      ++i;
      vratputs(Wmissing, i, 17, REVERR,
                "INSTALLATION INCOMPLETE - CHECK ERRORS");
      vwind(Wmissing, i+1, 74, 0, 0);
      vlocate(Wmissing, 20 - i, 3);
      vframe (Wmissing, REVERR, FRSINGLE);
      visible(Wmissing, YES, NO);
   }
   vgetkey(UpdateClockWindow);
	pcuron();
   breakchk(ON);
	vexit(0);
}


/**************************************************************
*  escape from data entry
**************************************************************/
void EscapeKey(TRANSACTION *tp)
{
   if(tp->thiskey==ESCAPE_KEY || tp->thiskey==REV_ESCAPE)
      vexit(1);
}


/*************************************************************
* field validate
**************************************************************/
defldval_drive(TRANSACTION *tp)
{
   if(tp) {
      char buff[50];
      unsigned drv=0, orig_drv, temp_drv, drv_count;
      if( (c != 'C') && (c != 'D') )
         return (FALSE);
      if(c == 'C') drv = 3;
      if(c == 'D') drv = 4;
      _dos_getdrive(&orig_drv);           /* get current disk drive */
      _dos_setdrive(drv, &drv_count);           /* change to requested drive */
      _dos_getdrive(&temp_drv);           /* get current drive */
      if (temp_drv != drv) {              /* check it */
         _dos_setdrive(orig_drv, &drv_count);
         sprintf(buff, "UNABLE TO LOCATE DRIVE %c:", 'A' + drv - 1);
         StatsError(buff);
         return(FALSE);                       /* error return */
      }
      _dos_setdrive(orig_drv, &drv_count);
      return(TRUE);
   }
}


/*************************************************************
*   transaction validate function
**************************************************************/
detranval_glob (TRANSACTION *tp)
{
   if(tp->thiskey==ESCAPE_KEY || tp->thiskey==REV_ESCAPE)
      return FALSE;
	if (KeySure()) return (TRUE);
	else return (FALSE);
}


/***************************************************************
*   KeySure()  displays  "Are You Sure?  .........."
*   returns 0 if NO - 1 if YES
*   if mouse pressed within (including frame) returns 1
*   if mouse pressed outside frame returns 0
****************************************************************/
KeySure()
{
	unsigned c;
   HWND Wkey;
	Wkey = vcreat (1, 26, NORML, NO);
   vlocate (Wkey, 7, 26);
   vframe (Wkey, NORML, FRSINGLE);
	vatputs (Wkey, 0, 0, "Are You Sure!  ........  ");
	vratputc (Wkey, 0, 25, REVEMPHNORML, 'N');
	vwind (Wkey, 1, 27, 0, 0);
   visible (Wkey, YES, NO);
   while(c = vgetkey(UpdateClockWindow)) {
      if ((c != 89) && (c != 121)) {
         vdelete (Wkey, NONE);
         return (0);
       } else {
          vratputc (Wkey, 0, 25, REVEMPHNORML, (char)toupper(c));
          if(vgetkey(UpdateClockWindow) != 13) {
             vdelete (Wkey, NONE);
             return (0);
          } else {
             vdelete (Wkey, NONE);
             return (1);
          }
       }
   }
}



/***************************************************************
*   StatsError()  displays error message in window
*   calculates window width based on strlen()
*   takes a string argument
****************************************************************/
void StatsError (char *arg)
{
	HWND Werr;
	Werr = vcreat (1, 70, HIGHERR, NO);
   vlocate (Werr, 21, 5);
	vframe (Werr, REVERR, FRSINGLE);
	vtitle (Werr, REVERR, " Error ");
	vatputf (Werr, 0, 0, "%s", arg);
	vwind (Werr, 1, strlen(arg), 0, 0);
	visible (Werr, YES, NO);
   vgetkey(UpdateClockWindow);
	vdelete (Werr, NONE);
}


/***************************************************************
*   message window
****************************************************************/
HWND Message(HWND Wmes, char *buff)
{
   Wmes = vcreat (1, 75, NORML, NO);
   vlocate (Wmes, 3, 4);
   vframe (Wmes, NORML, FRSINGLE);
   vatputf (Wmes, 0, 0, "%s", buff);
   vwind (Wmes, 1, strlen(buff), 0, 0);
   visible (Wmes, YES, NO);
   return(Wmes);
}


/***************************************************************
*   file copy function
****************************************************************/
copyfile( char *source, char *target )
{
    char *buf;
    int hsource, htarget;
    unsigned count = 0xff00;

      /* Open source file and create target, overwriting if necessary. */
    if( (hsource = open( source, O_BINARY | O_RDONLY )) == - 1 )
        return errno;
    if( (htarget = open( target, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC,
                            S_IREAD | S_IWRITE )) == -1)
        return errno;
    if( filelength( hsource ) < (long) count )
        count = (int)filelength( hsource );
    /* Dynamically allocate a large file buffer. If there's not enough
     * memory for it, find the largest amount available on the near heap
     * and allocate that. This can't fail, no matter what the memory model.
     */
    if( (buf = (char *)malloc( (size_t)count )) == NULL )
    {
        count = _memmax();
        if( (buf = (char *)malloc( (size_t)count )) == NULL )
            return ENOMEM;
    }
          /* Read-write until there's nothing left. */
    while( !eof( hsource ) )
    {
          /* Read and write input. */
        if( (count = read( hsource, buf, count )) == -1 )
            return errno;
        if( (count = write( htarget, buf, count )) == - 1 )
            return errno;
    }
          /* Close files and release memory. */
    close( hsource );
    close( htarget );
    free( buf );
    return( 0 );
}


/***************************************************************
*   declare two datawindows stubs to reduce .EXE size
*   this can only be done if Memo & Subform type fields are
*   not used in the program
*   these two stubs replace low level service routines
****************************************************************/
unsigned _memo(TRANSACTION *t1, HWND t2) {}
unsigned _subform(TRANSACTION *t1, HWND t2) {}
