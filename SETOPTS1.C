/**************************************************************
*   SETOPTS1.C   set the options - also various utilities
****************************************************************
*
*
*  Compiler:  MSC V6.00  /AL /W4 /Os /Gs /Zp /Zl /G2
*  Linker:    MSC V5.10  /NOE /SE:256 /NOD /EXEPACK
*  Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWL.LIB
*             Greenleaf Superfunctions (MSC) V1.00 GSFL.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*  (c) West Country Systems Ltd, Cyprus, 1991.
****************************************************************/

#include <stdio.h>         /* microsoft */
#include <stdlib.h>
#include <string.h>

#include <dw.h>            /* greenleaf */
#include <dwmenu.h>
#include <sftd.h>
#include <ibmkeys.h>
#include <color.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* global declarations */
long lastline;					/* number of lines read into a screen */

			/* external global declarations */
extern struct OPT_DEF  *opt;		  /* options list pointer */
extern struct EXT_DEF *ext;


/**************************************************************
*	 MAIN options set function - called from sts.c
**************************************************************/
void set_opts()
{
	HWND Wmsg;
	MENUHDR *mmain;
   MENUITEM *dirs, *syst, *util,     /* toplevel */
				*view, *save, *load,
				*txt, *log, *seq, *prn, *spn,	/* file viewing */
            *select;                        /* select pointer */
   mmain = MNUCreateHdr(POPUP);             /* create */
	mmain->toprow = 1;
	mmain->topcol = 1;
	mmain->poprows = 0;
	mmain->popcols = 36;
	mmain->battr = NORML;
	mmain->fattr = EMPHNORML;
	mmain->uattr = NORML;
	mmain->sattr = REVNORML;
	mmain->dattr = NORML;
   mmain->lattr = REVERR;
	mmain->d3 = 1;                    /* turn on 3D */
	mmain->shadowchar = SHADOW3D;     /* set 3D shading % */
	mmain->shadowcor = BOTTOMRIGHT;   /* set 3D direction */
	mmain->hattr = BACK3D;            /* set 3D attribute */
      /*# set_dirs() st_mem() pr_spfn() save_opt() set_load_opt() */
	dirs = MNUAddItem("View/Change Options", NULL, 'V', 0, NULL, mmain, set_dirs);
	syst = MNUAddItem("Examine System Status", NULL, 'E', 0, NULL, mmain, st_mem);
	view = MNUAddItem("View Report Files", NULL, 'V', 0, NULL, mmain, NULLF);
	util = MNUAddItem("List SP/FN/Gun Mask", NULL, 'L', 0, NULL, mmain, pr_spfn);
	load = MNUAddItem("Load Option Config", NULL, 'L', 0, NULL, mmain, set_load_opt);
	save = MNUAddItem("Save Option Config", NULL, 'S', 0, NULL, mmain, save_opt);
	dirs->leaveup = NO;
	dirs->repaint = NO;
	syst->leaveup = NO;
	syst->repaint = NO;
   util->leaveup = YES;
	util->repaint = YES;
			/* second level (file viewers) menus */
		/*# v_txt() v_log() v_seq() v_prn() v_spn() */
	txt = MNUAddItem("Statistics (.TXT)", NULL, 'S', 0, view, mmain, v_txt);
	log = MNUAddItem("Error Log (.LOG)", NULL, 'E', 0, view, mmain, v_log);
	seq = MNUAddItem("Firing Sequence (.SEQ)", NULL, 'F', 0, view, mmain, v_seq);
	prn = MNUAddItem("System Timing (.PRN)", NULL, 'S', 0, view, mmain, v_prn);
	spn = MNUAddItem("SP/FN/Time/Mask (.SPN)", NULL, 'S', 0, view, mmain, v_spn);
         /* organise help functions & windows  - ignore param warnings */
		/*# opt_help() syst_help() spfn_help() view_hlp() */
   dirs->helpfn = opt_help;
   syst->helpfn = syst_help;
   util->helpfn = spfn_help;
	view->helpfn = view_help;
   save->helpfn = opt_help;
	load->helpfn = opt_help;
	txt->helpfn = view_help;
	log->helpfn = view_help;
	seq->helpfn = view_help;
	prn->helpfn = view_help;
	spn->helpfn = view_help;
			/* start making the window */
	Wmsg = vcreat(1, 80, REVNORML, NO);
	vlocate(Wmsg, 24, 0);
	vratputs(Wmsg, 0, 36, REVERR, "  F1  ");
	visible (Wmsg, YES, NO);
	while(select = MNUDoSelect(mmain, UpdateClockWindow));
	MNUDeleteMenu(mmain);
	vdelete(Wmsg, NONE);
	return;
}


/**************************************************************
*	 call load_opt with argument
**************************************************************/
void set_load_opt()
{
	load_opt(1);
}


/**************************************************************
*	 view statistics report files
**************************************************************/
void v_txt()
{
	ViewFile (opt->work_dr, opt->txt_dir, "*.TXT");
}


/**************************************************************
*	 view error log report files
**************************************************************/
void v_log()
{
	ViewFile (opt->work_dr, opt->txt_dir, "*.LOG");
}


/**************************************************************
*	 view firing sequence report files
**************************************************************/
void v_seq()
{
	ViewFile (opt->work_dr, opt->txt_dir, "*.SEQ");
}


/**************************************************************
*	 view system timing report files
**************************************************************/
void v_prn()
{
	ViewFile (opt->work_dr, opt->txt_dir, "*.PRN");
}


/**************************************************************
*	 view SP/FN/TIME/GMASK report files
**************************************************************/
void v_spn()
{
	ViewFile (opt->work_dr, opt->txt_dir, "*.SPN");
}


/**************************************************************
*	 view files
**************************************************************/
void ViewFile( unsigned s_drive, char *s_directory, char *extension )
{
	HWND topW, botW, viewW;
	FILE *fi;
	int quit = 0;
	int curpage= -1;			 /* current index to fileptrs[] */
	int lastpage;				 /* last index to fileptrs[] */
	long curline;				 /* current linenumber */
	long totlines;				 /* total lines to display */
	long *fileptrs = NULL;	 /* array of file position pointers */
	char dirbuf[_MAX_PATH];  /* file full pathname */
	if(find_file(0, extension, s_drive, s_directory) != 1)
		return;
		/* make a top window and put the full filename in it */
	topW = vcreat(1, 80, YELGRN, NO);
	vlocate(topW, 1, 0);
	if(s_directory[0] == '\x5c')	 /* a fudge to get past getcwd() */
	sprintf(dirbuf, "%c:%s\\%s", 'A' + s_drive - 1,
											  s_directory, ext->pname);
	else
		sprintf(dirbuf, "%s\\%s", s_directory, ext->pname);
	vatputs(topW, 0, 38 - ((int)strlen(dirbuf)/2), dirbuf);
	visible(topW, YES, NO);
	if ((fi = fopen (dirbuf, "rb")) == NULL) {
		quit = errno;	 /* use quit as temp error number */
		StatsDosError( dirbuf, quit);
		vdelete(topW, NONE);
		return;
	}
		/* make the viewing window */
	viewW = vcreat(40, 83, NORML, NO);	/* 40 line page buffer */
	vwind(viewW, 20, 78, 0, 0);
	vlocate(viewW, 3, 1);
	vframe(viewW, EMPHNORML, FRDOUBLE);
		/* make the bottom line window for line numbering & messages */
	botW = vcreat(1, 80, NORML, NO);
	vlocate(botW, 24, 0);
		/* make windows visible & show first (top) screen page */
	visible(botW, YES, NO);
	visible(viewW, YES, NO);
	ShowTop(fi, viewW);
	curline = (long) lastline;
		/* allocate memory for the array of file pointers (4096) */
	if( (fileptrs = (long *) calloc( 4096, sizeof(long) )) == NULL ) {
		StatsErr("18001");
		fclose(fi);
		vdelete(topW, NONE);
		vdelete(botW, NONE);
		vdelete(viewW, NONE);
		return;
	}
		/* note - lastpage is the last index for fileptrs[] for fseek() */
	lastpage = GetPointers(fi, fileptrs);	/* get fptrs (after EOF is -1) */
	rewind(fi);   /* reset filepointer to top */
	totlines = (1L + ((long) lastpage))*20L + (long) lastline;
	LineMessage(botW, fi, curline, totlines);
	while( !quit ) {
		switch( vgetkey(UpdateClockWindow) ) {
			case ESC:
				quit = 1;
				break;
			case ENDKEY:
			case CTRLPGDN:
				ShowBottom(fi, viewW, fileptrs, lastpage);
				LineMessage(botW, fi, totlines, totlines);
				curpage = lastpage;
				curline = totlines;
				break;
			case HOME:
			case CTRLPGUP:
				ShowTop(fi, viewW);
				LineMessage(botW, fi, lastline, totlines);
				curpage = -1;
				curline = lastline;
				break;
			case PGUP:
				if( curpage < 1 ) {	/* if 1 page from the top */
					ShowTop(fi, viewW);
					LineMessage(botW, fi, lastline, totlines);
					curpage = -1;
					curline = lastline;
				} else {
					ShowPage(fi, viewW,fileptrs[--curpage]);
					LineMessage(botW, fi, curline -= 20L, totlines);
				}
				break;
			case PGDN:
				if( curpage >= lastpage - 1 ) {	/* if 1 page from the end */
				ShowBottom(fi, viewW, fileptrs, lastpage);
				LineMessage(botW, fi, totlines, totlines);
				curpage = lastpage;
				curline = totlines;
				} else {
					ShowPage(fi, viewW,fileptrs[++curpage]);
					LineMessage(botW, fi, curline += 20L, totlines);
				}
				break;
		}
	}
	fclose(fi);
	free( fileptrs );
	vdelete(topW, NONE);
	vdelete(botW, NONE);
	vdelete(viewW, NONE);
	return;
}


/**************************************************************
*	 select the first screen page
**************************************************************/
void ShowTop(FILE *fi, HWND viewW)
{
	rewind(fi);
	ShowPage(fi, viewW, 0L);
}

/**************************************************************
*	 select the last screen page
**************************************************************/
void ShowBottom(FILE *fi, HWND viewW, long *fileptrs, int lastpage)

{
	ShowPage(fi, viewW, fileptrs[lastpage]);
}


/**************************************************************
*	 actually display the screen
*		 sets lastline (number of lines read & displayed)
**************************************************************/
void ShowPage(FILE *fi, HWND viewW, long fpoint)
{
	register int i;
	char linebuf[83];
	char *comp; 					 /* compare pointer (remove CR/LF) */
	fseek(fi, fpoint, SEEK_SET);
	lastline = 0;
	vclear( viewW );
	for(i=0; i<20; i++) {		/* display 20 lines */
		fgets(linebuf, 82, fi);
		lastline++;
			/* remove the CRLF characters by replacing with \0 */
		comp = memchr( linebuf, '\r', sizeof( linebuf ) );
		*comp = '\0';
		vatputs(viewW, i, 0, linebuf);
			if(feof(fi)) {		/* if at end of file */
			break;
		}
	}
}


/**************************************************************
*	 get top of screen file pointers (max 4096 pages)
*	 returns integer to last index to fileptrs[]
**************************************************************/
GetPointers(FILE *fi, long *fileptrs)
{
	register int i=0, n;
	char linebuf[83];
	while( !feof(fi) ) {	/* while !EOF */
		fileptrs[i++] = ftell( fi );
		lastline = 0;	 /* lastline only counts the extra in last window */
		for(n=0; n<20; n++) {		/* read next 20 lines */
			if( feof(fi) ) break;
			else {
				lastline++;	/* extra lines in last window */
				fgets(linebuf, 82, fi);
			}
		}
		if(i >= 4095) {	/* if too many page handles (file too big) */
			StatsErr("18002");	/* file too long - show as much as possible */
			break;
		}
	}
	return (--i);
}


/**************************************************************
*	 do the bottom line message
**************************************************************/
void LineMessage(HWND botW, FILE *fi, long curline, long totlines)
{
	vclear( botW );
	vatputs(botW, 0, 2, "LINE NUMBER:        of");
	vratputf(botW, 0, 16, REVERR, "%ld", curline);
	vratputf(botW, 0, 26, REVERR, "%ld", totlines);
	if( feof( fi ) )
		vratputs(botW, 0, 66, REVERR, "<END OF FILE>");
}


/****************************************************************
*	 View Report Files on Disk
*****************************************************************/
void ViewReports()
{
	MENUHDR *mmain;
	MENUITEM *txt, *log, *seq, *prn, *spn;   /* file viewing */
   mmain = MNUCreateHdr(POPUP);             /* create */
	mmain->toprow = 2;
	mmain->topcol = 1;
	mmain->poprows = 0;
	mmain->popcols = 36;
	mmain->battr = NORML;
	mmain->fattr = EMPHNORML;
	mmain->uattr = NORML;
	mmain->sattr = REVNORML;
	mmain->dattr = NORML;
   mmain->lattr = REVERR;
   mmain->d3 = 1;                    /* turn on 3D */
   mmain->shadowchar = SHADOW3D;     /* set 3D shading % */
   mmain->shadowcor = BOTTOMRIGHT;   /* set 3D direction */
   mmain->hattr = BACK3D;            /* set 3D attribute */
			/* first level (file viewers) menus */
		/*# v_txt() v_log() v_seq() v_prn() v_spn() */
	txt = MNUAddItem("Statistics (.TXT)", NULL, 'S', 0, NULL, mmain, v_txt);
	log = MNUAddItem("Error Log (.LOG)", NULL, 'E', 0, NULL, mmain, v_log);
	seq = MNUAddItem("Firing Sequence (.SEQ)", NULL, 'F', 0, NULL, mmain, v_seq);
	prn = MNUAddItem("System Timing (.PRN)", NULL, 'S', 0, NULL, mmain, v_prn);
	spn = MNUAddItem("SP/FN/Time/Mask (.SPN)", NULL, 'S', 0, NULL, mmain, v_spn);
	while( MNUDoSelect( mmain, UpdateClockWindow ) );
	MNUDeleteMenu(mmain);
	return;
}
