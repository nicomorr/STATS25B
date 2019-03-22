/*****************************************************************
*   STSARC1.C    menu for Archiving and DeArchiving
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

#include <dw.h>            /* greenleaf */
#include <dwmenu.h>
#include <sftd.h>

#include <extern.h>        /* wcs */
#include <stxfunct.h>

			/* external global declarations */
extern struct EXT_DEF *ext;


/**************************************************************
*   main archive menu
**************************************************************/
arc_menu()
{
	HWND Wmsg;
	MENUHDR *mmain;
	MENUITEM *arc_make, *arc_ret, *arc_cfg, *dearc_cfg;  /* toplevel */
	MENUITEM *select;   /* selection */
	ext->a_num = 0;
	mmain = MNUCreateHdr(POPUP);       /* create  */
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
      /*# m_arc() d_arc() mcfg_arc() dcfg_arc() arc_help() */
   arc_make = MNUAddItem( "Archive Data & Configuration Files", NULL,
													'A', 0, NULL, mmain, m_arc);
   arc_ret = MNUAddItem(  "DeArchive ZIP File & AutoRun Stats", NULL,
													'D', 0, NULL, mmain, d_arc);
   arc_cfg = MNUAddItem(  "Store Current Configuration Files", NULL,
													'S', 0, NULL, mmain, mcfg_arc);
   dearc_cfg = MNUAddItem("Retrieve Stored Configuration & Update", NULL,
                                       'R', 0, NULL, mmain, dcfg_arc);
         /* leave up these menus while executing */
   arc_ret->leaveup = YES;
   arc_ret->repaint = YES;
   dearc_cfg->leaveup = YES;
   dearc_cfg->repaint = YES;
         /* archive help function - ignore param warnings */
   arc_make->helpfn = arc_help;
   arc_ret->helpfn = arc_help;
   arc_cfg->helpfn = arc_help;
   dearc_cfg->helpfn = arc_help;
         /* bottom line */
	Wmsg = vcreat(1, 80, REVNORML, NO);
	vlocate(Wmsg, 24, 0);
	vratputs(Wmsg, 0, 36, REVERR, "  F1  ");
	visible (Wmsg, YES, NO);
	do {
		select = MNUDoSelect(mmain, UpdateClockWindow);
      if(!ext->a_num && (select == arc_ret) ) {  /* file dearchived */
			MNUDeleteMenu(mmain);
			vdelete(Wmsg, NONE);
         return(1);             /* start autorun */
		}
	} while (select);
	MNUDeleteMenu(mmain);
	vdelete(Wmsg, NONE);
   return(0);                   /* no autorun */
}


/**************************************************************
*   archive help function
**************************************************************/
void arc_help()
{
   string_help("Archiving Files");
}
