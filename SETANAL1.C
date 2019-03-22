/***************************************************************
*   SETANAL1.C  turn analysis routines on or off
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

#include <dw.h>
#include <dwmenu.h>        /* greenleaf */
#include <sftd.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct ANAL_DEF *andef;     /* pointer to set/analysis list */


/*************************************************************
* main analysis configuration function - called from maincfg.c
**************************************************************/
void setanal ()
{
	HWND W1;
	MENUHDR *mmain;
	MENUITEM *view, *load, *save;	/* top */
	mmain = MNUCreateHdr(POPUP);       /* create  */
   MNUSetGlobalHelp(mmain, anl_help);   /* help screen function */
	mmain->toprow = 1;
	mmain->topcol = 1;
	mmain->poprows = 0;
	mmain->popcols = 0;
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
         /* TOPLEVEL MENU ITEMS */
      /*# ch_anl() set_load_anl() save_anl() anl_help() */
	view = MNUAddItem("View/Change Analysis", NULL, 'V', 0, NULL, mmain, ch_anl);
	load = MNUAddItem("Load Analysis", NULL, 'L', 0, NULL, mmain, set_load_anl);
	save = MNUAddItem("Save Analysis", NULL, 'S', 0, NULL, mmain, save_anl);
	view->leaveup = NO;
	view->repaint = NO;
	W1 = vcreat (1, 80, REVNORML, NO);
	vlocate (W1, 24, 0);
	vratputs (W1, 0, 36, REVERR, "  F1  ");
	visible(W1, YES, NO);
	while(MNUDoSelect(mmain, UpdateClockWindow));
	MNUDeleteMenu(mmain);
	vdelete(W1, NONE);
}


/**************************************************************
*	 set load as it takes an argument
**************************************************************/
void set_load_anl()
{
	load_anl(1);
}


/**************************************************************
*   save analysis settings to disk
**************************************************************/
void save_anl()
{
	FILE *fp;
	if (!KeySure()) return;
	fp = fopen("ANALYSIS.CFG", "wb");
	fwrite(andef, sizeof(struct ANAL_DEF), 1, fp);
	fclose(fp);
   StatsMessage ("SAVED ANALYSIS.CFG");
}


/**************************************************************
*   load analysis settings from disk
**************************************************************/
load_anl(int i)
{
   FILE *fp;
   if(i)
      if (!KeySure())
         return(0);
	if((fp = fopen("ANALYSIS.CFG", "rb")) == NULL) {
	  if(i) StatsErr ("28002");
	  return(0);
	}
	if (1 != fread(andef, sizeof(struct ANAL_DEF), 1, fp))
		StatsErr("28003");
   fclose(fp);
   if(i) StatsMessage("LOADED ANALYSIS.CFG");
	return(1);
}
