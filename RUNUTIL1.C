/***************************************************************
*    RUNUTIL1.C  general runtime utilities
****************************************************************
*
*  Compiler:  MSC V6.00  /AL /W4 /Os /Gs /Zp /Zl /G2
*  Linker:    MSC V5.10  /NOE /SE:256 /NOD /EXEPACK
*  Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWL.LIB
*             Greenleaf Superfunctions (MSC) V1.00 GSFL.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*	(c) West Country Systems Ltd, Cyprus, 1992
****************************************************************/

#include <stdio.h>         /* microsoft */
#include <string.h>
#include <dos.h>
#include <direct.h>
#include <stdlib.h>
#include <time.h>
#include <types.h>
#include <stat.h>

#include <dw.h>            /* greenleaf */
#include <dwdata.h>
#include <dwmenu.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <stxfunct.h>

				/* external global declarations */
extern struct HDR_DEF *hd;       /* header  list pointer */
extern struct PRO_DEF *pd;       /* pointer to project list */
extern struct OPT_DEF  *opt;     /* pointer to options list */
extern struct SPEC_DEF *spc;     /* pointer to specifications list */
extern struct ANAL_DEF *andef;	/* pointer to set/analysis list */
extern struct ST_SUMM *sum;		/* summary structure pointer */
extern struct SRCE_ERR *gun;		/* pointer to integer gunspecs */
extern struct EXT_DEF *ext;      /* external variables */
extern char ver[];					/* master program version */


/***************************************************************
*   find files  -  present as menu to select
*   if dat_fil is 1 then present datafile comment field &
*		 date/time also.	if dat_fil is 0 present filename only
*   copy filename to "pname"
*   return 0 if no file selected, -1 if no file found, 1 if OK
****************************************************************/
find_file(int dat_fil, char *srchfor, unsigned f_drv, char *f_dir)
{
   char orig_dir[51];                  /* current directory */
   unsigned orig_drv;                  /* current drive */
   int num;
   if(check_drive(f_drv, f_dir)) return(0);   /* could not find drive/dir */
   _dos_getdrive(&orig_drv);           /* get current disk drive */
   getcwd(orig_dir, 50);               /* get current directory */
   _dos_setdrive(f_drv, NULL);         /* change to requested drive */
   chdir(f_dir);                       /* change to requested dir */
   num = GetFileName(dat_fil, srchfor, ext->pname);
   _dos_setdrive(orig_drv, NULL);
   chdir(orig_dir);
   if(num == -1) StatsErr("27001");
	return (num);
}


/***************************************************************
*    get a file name from a list to pname
*    if dat_fil is 1 then also present comment field & date/time
*       otherwise just filename
***************************************************************/
GetFileName(int dat_fil, char *filespec, char *pname)
{
   LISTITEM *l;
   char temp[13];                       /* hold fname */
   char title[] = {" Select Data File "};
   struct FNAME *tmp = NULL;
   struct FNAME *p = NULL;
   char *select = NULL;
   register int n = 31;                 /* position listbox column */
   if(!dosfirst(filespec, 0, temp)) {   /* if at least 1 file found */
      l = initlist();
      p = (struct FNAME *) malloc(sizeof(struct FNAME));
      tmp = p;
      SetFileString(tmp, temp);
      if(dat_fil) GetDatFields(temp, tmp);
      adtolist (l, tmp->f);
      while(!dosnext(filespec, 0, temp)) {
         tmp = tmp->next;
         tmp = (struct FNAME *) malloc(sizeof(struct FNAME));
         SetFileString(tmp, temp);
         if(dat_fil) GetDatFields(temp, tmp);
         adtolist (l, tmp->f);
      }
      tmp->next = NULL;
   } else return (-1);
   if(dat_fil) n = 7;      /* move listbox left */
   else strcpy(title, " Select ");
   select = listsel(8, n, 8, NORML, REVNORML, title,
            EMPHNORML, FRSINGLE, EMPHNORML, l, UpdateClockWindow);
   if(select) {
      if(!dat_fil) strcpy(pname, select);
      else {
         for(n=0; n<12; n++)
            pname[n] = select[n];
         pname[12] = '\0';
      }
   }
   freelist(l, 1);
   if(select) return (1);
   else return(0);
}


/***************************************************************
*   set file string to standard format
***************************************************************/

void SetFileString(struct FNAME *tmp, char *temp)
{
   REGISTER int i,j;
   for(i=j=0; (temp[i]!='\0') && (temp[i]!='.'); i++,j++)
      tmp->f[j] = temp[i];
   while (j<12)
      tmp->f[j++] =' ';
   for(j=8; temp[i] != '\0'; i++,j++)
      tmp->f[j] = temp[i];
   tmp->f[12] = '\0';
}


/**************************************************************
*   get comment field & file date/time
**************************************************************/
GetDatFields(char *temp, struct FNAME *tmp)
{
   FILE *fi;
   char file_time[26];         /* holds file data/time */
   char *buff = NULL;          /* temporary buffer */
   struct stat filestat;       /* file status structure */
   if( (fi = fopen( temp, "rb" )) == NULL ) {   /* open file */
      StatsError("CANNOT OPEN [runutil1]");   /* cannot open */
      return(0);
   }
   if( (buff = (char *)malloc( (size_t) (opt->com_fld + 1) )) == NULL ) {
      fclose(fi);
      StatsError("MEM ALLOC [runutil1]");   /* memory allocation error */
      return(0);
   }
   fseek (fi, opt->com_ofst, SEEK_SET);   /* set to comment field start */
   if(feof(fi) || ( opt->com_fld != fread (buff, sizeof (char),
                            opt->com_fld, fi)) ) {   /* read comment */
      if(!feof(fi)) StatsError("READERR [runutil1]");   /* read error */
      else StatsError("FILE TOO SHORT [runutil1]");            /* EOF error */
      fclose(fi);
      free(buff);
      return(0);
   }
   fclose(fi);
   buff[opt->com_fld] = '\0';     /* append terminator */
	strcat(tmp->f, "  ");			 /* add 2 white space */
	strcat(tmp->f, "\xb3");			 /* add separator */
	strcat(tmp->f, "  ");			 /* add 2 white space */
   strcat(tmp->f, buff);          /* add comment field */
   if( !stat( temp, &filestat ) ) {  /* get file date/time */
		strcat(tmp->f, " ");			 /* add 1 white space */
		strcat(tmp->f, "\xb3");		 /* add separator */
		strcat(tmp->f, " ");			 /* add 1 white space */
      strcpy(file_time, ctime( &filestat.st_atime ) );
      file_time[24] = '\0';   /* get rid of \n character */
      strcat(tmp->f, file_time);   /* add time/date to string */
   }
   free(buff);
   return(1);
}


/**************************************************************
*   keyboard interrupt processing routine during run stats
**************************************************************/
int key_int(HWND Werroll, long sp, long fn, char *errbuf, FILE *flog)
{
   HWND W1;         /* menu window */
   HWND Wklone;     /* blank background window */
	unsigned c=0;
	if(gfkbhit()) c = getkey();
   if( (c == ESC) || ( (sp == sum->way_sp) && (sum->way_fl) ) ) {
         /* beep if a waypoint reached - reset waypoint flag */
      if( (sp == sum->way_sp) && (sum->way_fl) ) {
         vbeep();
         sum->way_fl=0;
      }
      W1 = vcreat(8, 26, NORML, NO);
		vlocate (W1, 1, 53);
		vframe (W1, EMPHNORML, FRDOUBLE);
		vatputs(W1, 0, 1, "F2  Set Next Waypoint");
		vatputs(W1, 1, 1, "F3  Change Configuration");
		vatputs(W1, 2, 1, "F4  Change Analyses Set");
		vatputs(W1, 3, 1, "F6  Change Specifications");
		vatputs(W1, 4, 1, "F7  Interim Statistics");
		vratputs(W1, 5, 0, REVNORML, " <CR> Continue Processing ");
      vatputs(W1, 6, 1, "<ESC>  Quit  Processing");
      vratputs(W1, 7, 13, REVERR, "F1");
		visible(W1, YES, YES);
		do {
			c = vgetkey(UpdateClockWindow);
			switch (c) {
            case ESC: /* escape processing altogether */
					if (KeySure()) {
						vdelete (W1, NONE);
						return (1);
					} else {
						c = 0;
						break;
               }
            case F1: /* help window */
               go_help();
               break;
            case CR: /* continue processing */
					vdelete (W1, NONE);
					return (0);
				case F2:	/* set next waypoint */
					set_waypoint();
						if ((sum->way_sp<sp && sum->way_sp<sum->lsp)
							  || (sum->way_sp>sp && sum->way_sp>sum->lsp)) {
                     StatsErr("27003");
							sum->way_fl=0;
							break;
						} else {
							sum->way_fl =1;
							break;
						}
            case F3: /* change configuration */
               Wklone = vcreat(25, 80, NORML, NO);
               vlocate (Wklone, 0, 0);
               visible(Wklone, YES, YES);
               maincfg();
               vdelete(Wklone, NONE);
					scr_error("CONFIGURATION CHANGED",
                                   errbuf, sp, fn, Werroll, flog);
					break;
				case F4:	/* change analyses */
               if(ch_anl())
                  scr_error("ANALYSIS CHANGED",
                                   errbuf, sp, fn, Werroll, flog);
					break;
				case F6:	/* change specs */
					if(ch_spec()) {
                     /* initialise integer gun specs */
                  if((hd->gu==9)||(hd->gu==8)||
                     (hd->gu==63)||(hd->gu==64)) { /* LITTON */
							gun->sp1 = (int) (4.0F*spc->gun1);
							gun->sp2 = (int) (4.0F*spc->gun2);
						}
                  if((hd->gu==10)||(hd->gu==11)) { /* GUNDA */
							gun->sp1 = (int) (10.0F*spc->gun1);
							gun->sp2 = (int) (10.0F*spc->gun2);
						}
						scr_error("SPECIFICATIONS CHANGED",
                                       errbuf, sp, fn, Werroll, flog);
					}
					break;
				case F7:
					stat_make();
					break;
#ifdef DEBUG
				case ALTM:
					heapstat();
					break;
#endif
			}
      } while ((c != ESC) && (c != CR));
	} else return(0);
}


/**************************************************************
* write errors to screen & to the .LOG error disk file
***************************************************************/
void scr_error(char *err, char *errbuf, long sp, long fn,
               HWND Werroll, FILE *flog)
{
	if(hd->fn && hd->tm)
      sprintf(errbuf, "SP %-6ld FN %-6ld %-8s  %s", sp, fn, ext->tim, err);
	else if(hd->fn)
		sprintf(errbuf, "SP %-6ld FN %-6ld  %s", sp, fn, err);
	else if(hd->tm)
      sprintf(errbuf, "SP %-6ld %-8s  %s", sp, ext->tim, err);
	else sprintf(errbuf, "SP %-6ld  %s", sp, err);
	vscrolup(Werroll, 1);
   vatputs(Werroll, 14, 2, errbuf);
	fwrite(errbuf, sizeof (char), strlen(errbuf), flog);
	fwrite("\n", sizeof (char), 1, flog);
	return;
}


/****************************************************************
*   check that a drive & directory exist - returns 0 if OK
*****************************************************************/
check_drive (unsigned drv, char *dir)
{
   register int i, errnum;             /* return value & error num */
   char orig_dir[51];
   unsigned orig_drv, temp_drv;
   _dos_getdrive(&orig_drv);           /* get current disk drive */
   getcwd(orig_dir, 50);               /* get current directory */
   _dos_setdrive(drv, NULL);           /* change to requested drive */
   _dos_getdrive(&temp_drv);           /* get current drive */
   if (temp_drv != drv) {              /* check it */
      _dos_setdrive(orig_drv, NULL);
      sprintf(ext->a_str, "UNABLE TO LOCATE DRIVE %c:", 'A' + drv -1);
		StatsError(ext->a_str);
      return(1);                       /* error return */
	}
   if(i = chdir(dir)) {                 /* request directory */
		errnum = errno;
      sprintf(ext->a_str, "%c:%s", 'A' + drv -1, dir);
      StatsDosError(ext->a_str, errnum);
   }
   _dos_setdrive(orig_drv, NULL);     /* back to original drive */
   chdir(orig_dir);                   /* back to original directory */
   return(i);                         /* if (i),  did not find it */
}


/****************************************************************
*   open & initialise logging (.LOG) file
*****************************************************************/
FILE * log_file()
{
   FILE *flog;
	char prog_buf[65];           /* .LOG path/file buffer */
   char logbuf[13];             /* file buffer */
	register int i;
	for (i=0; i<8; i++)
	logbuf[i] = ext->pname[i];
	logbuf[8] = '\0';
	strcat(logbuf, ".LOG");
	sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr - 1, opt->txt_dir);
	if(strlen(opt->txt_dir)>1) strcat(prog_buf, "\\");
	strcat(prog_buf, logbuf);
   if( (flog = fopen(prog_buf, "w")) == NULL ) {
      StatsErr("27004");  /* could not open .log file */
      return(NULL);
   }
   fprintf(flog,
    "==================================================================");
   fwrite("\n", sizeof (char), 1, flog);
   fprintf(flog, "LINE ERROR LOG %-36s %s", " ", ver);
   fwrite("\n", sizeof (char), 1, flog);
	fprintf(flog,
		"------------------------------------------------------------------");
	fwrite("\n", sizeof (char), 1, flog);
   fprintf(flog, "ID: %-42s FILE:  %s", sum->comment, ext->pname);
   fwrite("\n", sizeof (char), 1, flog);
   fprintf(flog,
    "==================================================================");
   fwrite("\n", sizeof (char), 1, flog);
	return (flog);
}


/****************************************************************
*	 open & initialise graphics (.GRF) file
*****************************************************************/
FILE * grf_file()
{
   FILE *flog;
	char prog_buf[65];        /* .PRN path/file buffer */
   char logbuf[13];          /* file buffer */
	register int i;
	for (i=0; i<8; i++)
		logbuf[i] = ext->pname[i];
	logbuf[8] = '\0';
	strcat(logbuf, ".GRF");
	sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr - 1, opt->txt_dir);
	if(strlen(opt->txt_dir)>1) strcat(prog_buf, "\\");
   strcat(prog_buf, logbuf);
	if( (flog = fopen(prog_buf, "wb")) == NULL ) {
		StatsErr("27010");  /* could not open .grf file */
      return(NULL);
   }
	return (flog);
}


/****************************************************************
*   open & initialise timing (.PRN) file
*****************************************************************/
FILE * prn_file()
{
   FILE *flog;
	char prog_buf[65];        /* .PRN path/file buffer */
   char logbuf[13];          /* file buffer */
	register int i;
	for (i=0; i<8; i++)
		logbuf[i] = ext->pname[i];
	logbuf[8] = '\0';
	strcat(logbuf, ".PRN");
	sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr - 1, opt->txt_dir);
	if(strlen(opt->txt_dir)>1) strcat(prog_buf, "\\");
   strcat(prog_buf, logbuf);
   if( (flog = fopen(prog_buf, "w")) == NULL ) {
      StatsErr("27005");  /* could not open .prn file */
      return(NULL);
   }
	if((hd->clk == 19)||(hd->clk == 21))   /* if GM2B or PULSECLK */
		fwrite("\"SP\",\"TSTAMP\",\"CLOSURE\",\"LINK\"\n",
					 sizeof (char), 31, flog);
	if(hd->clk==91)
		fwrite("\"SP\",\"FTB1\",\"FTB2\",\"FTB3\",\"FTB\"\n",
					 sizeof (char), 32, flog);
	return (flog);
}


/****************************************************************
*   open & initialise SP/FN listing (.SPN) file
*****************************************************************/
FILE * spn_file()
{
   FILE *flog;
	char prog_buf[65];        /* .SPN path/file buffer */
   char logbuf[13];          /* file buffer */
	register int i;
	for (i=0; i<8; i++)
		logbuf[i] = ext->pname[i];
	logbuf[8] = '\0';
	strcat(logbuf, ".SPN");
	sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr - 1, opt->txt_dir);
	if(strlen(opt->txt_dir)>1) strcat(prog_buf, "\\");
   strcat(prog_buf, logbuf);
   if( (flog = fopen(prog_buf, "w")) == NULL ) {
      StatsErr("27006");  /* could not open .spn file */
      return(NULL);
   }
	fprintf(flog,
		"==================================================================");
	fwrite("\n", sizeof (char), 1, flog);
   fprintf(flog, "SP");
   if(hd->fn) fprintf(flog, "/FN");
   if(hd->tm) fprintf(flog, "/TIME");
	if(andef->gmsk_anl)   /* do gun mask reporting */
      fprintf(flog, " & GUN MASK ");
   fprintf(flog, " LISTING              %s", ver);
	fwrite("\n", sizeof (char), 1, flog);
	fprintf(flog,
		"------------------------------------------------------------------");
   fwrite("\n", sizeof (char), 1, flog);
   fprintf(flog, "LINE (ID):  %-28s File:  %s", sum->comment, ext->pname);
   fwrite("\n", sizeof (char), 1, flog);
	fprintf(flog,
		"==================================================================");
	fwrite("\n", sizeof (char), 1, flog);
	return (flog);
}


/****************************************************************
*   open & initialise Firing Sequence (.SEQ) file
*****************************************************************/
FILE * seq_file()
{
   FILE *firs;
   char prog_buf[65];              /* .SEQ path/file buffer */
   char logbuf[13];                /* file buffer */
   register int i;
	for (i=0; i<8; i++)
		logbuf[i] = ext->pname[i];
	logbuf[8] = '\0';
   strcat(logbuf, ".SEQ");
	sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr - 1, opt->txt_dir);
	if(strlen(opt->txt_dir)>1) strcat(prog_buf, "\\");
   strcat(prog_buf, logbuf);
   if( (firs = fopen(prog_buf, "w")) == NULL ) {
      StatsErr("27007");  /* could not open .seq file */
      return(NULL);
   }
   fprintf(firs,
   "==================================================================");
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs, "SOURCE FIRING SEQUENCE ANALYSIS %-19s %s", " ", ver);
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs,
   "------------------------------------------------------------------");
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs, "ID: %-42s FILE:  %s", sum->comment, ext->pname);
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs,
   "------------------------------------------------------------------");
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs, "SP Range:  %6ld - %-6ld", sum->fsp, sum->lsp);
   if(hd->fn) fprintf(firs, "    FN Range:  %6ld - %-6ld", sum->ffn, sum->lfn);
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs, "Sequence Length: %u    Local Sources: %u    Remote Sources: %u",
              pd->seq_len, pd->loc_srce, pd->rem_srce);
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs,
   "==================================================================");
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs,
    "REFERENCE  SP     FN   TIME     ACTUAL SOURCE(S) FIRED");
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs,
    "SEQUENCE    NUMBERS    (UTC)    CHANGE & ERROR FLAGS");
   fwrite("\n", sizeof (char), 1, firs);
   fprintf(firs,
   "==================================================================");
   fwrite("\n", sizeof (char), 1, firs);
   return (firs);
}


/****************************************************************
*   set a processing waypoint to stop at during run
*   sets waypoint flag if requested SP is in range
*****************************************************************/
void set_waypoint()
{
	HWND W1;
	TRANSACTION *tp;
	FIELD *fld;
	sum->way_fl = 0;
	W1 = vcreat (2, 27, NORML, NO);
	vlocate (W1, 21, 25);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " Set Waypoint ");
      /*# detranval_way() EscapeKey() */
	tp = vpreload (1, 1, NULLF, NULLF, detranval_way, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
	fld = *(tp->def);
	fld[0].use_ext = TRUE;   /* use/display the preload values */
	vdeflong (tp, 0, 0, 1, "Waypoint SP", 20, 6, "999999", &sum->way_sp, YES);
	vdefatsa (tp, 0, 1, 3, HIGHNORML, "<ESC> Exit No Change");
	visible (W1, YES, NO);
	vread (tp, W1, YES);
	vdelete (W1, NONE);
	return;
}


/*************************************************************
* transaction validation
**************************************************************/
detranval_way (TRANSACTION *tp)
{
   if(tp->thiskey==ESCAPE_KEY || tp->thiskey==REV_ESCAPE)
      return FALSE;
	if ((sum->way_sp<sum->fsp && sum->way_sp<sum->lsp)
		  || (sum->way_sp>sum->fsp && sum->way_sp>sum->lsp)) {
      StatsErr("27003");
		return(FALSE);
	} else {
		sum->way_fl =1;
		return (TRUE);
	}
}
