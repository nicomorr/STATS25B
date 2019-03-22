/***************************************************************
*   HELPMAIN.C   help from ASCII text file "STATMAIN.HLP"
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

#include <stdio.h>					  /* microsoft */
#include <stdlib.h>
#include <malloc.h>
#include <direct.h>

#include <dw.h>						  /* greenleaf */
#include <dwmenu.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>					  /* wcs */
#include <stxfunct.h>

			/* external global declarations */
extern struct OPT_DEF  *opt;		  /* options list pointer */


/***************************************************************
*	 main help function - called from sts.c
****************************************************************/
void help_main()
{
	MENUHDR	*mmain;
	MENUITEM *AA, *BA, *CA, *DA, *EA, *FA, *GA, *HA, *IA, *JA, *KA,
				*LA, *MA, *NA, *OA, *PA, *QA, *RA, *SA, *TA, *UA,
            *VA, *WA, *XA, *YA, *ZA, *AB, *BB, *CB, *DB, *EB,
            *FB, *GB, *HB, *IB, *JB, *KB, *LB, *MB, *NB, *OB,
            *PB, *QB, *RB, *SB, *TB, *UB, *VB, *WB, *XB, *YB,
            *ZB, *AC, *BC, *CC, *DC, *EC, *FC, *GC, *HC, *IC,
            *JC, *KC, *LC, *MC, *NC, *OC, *PC, *QC, *RC, *SC,
            *TC, *UC, *VC, *WC, *XC, *YC, *ZC, *AD, *BD, *CD,
				*DD, *ED, *FD, *GD, *HD, *ID, *JD, *KD, *LD, *MD,
				*ND, *OD, *PD, *QD;
   MENUITEM *result;   /* result string pointer */
   mmain = MNUCreateHdr(POPUP);      /* create */
	mmain->toprow = 0;					  /* top row */
   mmain->topcol = 5;                /* top column */
   mmain->poprows = 3;               /* next level pop row */
   mmain->popcols = 14;              /* next level pop column */
   mmain->d3 = 1;                    /* turn on 3D */
   mmain->shadowchar = SHADOW3D;     /* set 3D shading % */
   mmain->shadowcor = BOTTOMRIGHT;   /* set 3D direction */
   mmain->hattr = BACK3D;            /* set 3D attribute */
   mmain->battr = NORML;             /* background attribute */
   mmain->fattr = EMPHNORML;         /* frame attribute */
   mmain->uattr = NORML;             /* unselected item attribute */
   mmain->sattr = REVNORML;          /* selected item attribute */
   mmain->dattr = NORML;             /* description line attribute */
   mmain->lattr = REVERR;            /* selection letter attribute */
   mmain->mouseselect = YES;         /* auto mouse select */
   mmain->pulldown = NO;             /* auto pulldown */
			/* top level selections */
	AA = MNUAddItem("General HELP", NULL, 'G', 0, NULL, mmain, NULLF);
	QD = MNUAddItem("NEW Additions", NULL, 'N', 0, NULL, mmain, NULLF);
   BA = MNUAddItem("External Headers", NULL, 'E', 0, NULL, mmain, NULLF);
   CA = MNUAddItem("Headstat Program", NULL, 'H', 0, NULL, mmain, NULLF);
   PC = MNUAddItem("Using Headstat", NULL, 'U', 0, NULL, mmain, NULLF);
   DA = MNUAddItem("Headstat Reports", NULL, 'H', 0, NULL, mmain, NULLF);
   EA = MNUAddItem("Algorithms", NULL, 'A', 0, NULL, mmain, NULLF);
   RA = MNUAddItem("Units", NULL, 'U', 0, NULL, mmain, NULLF);
   TA = MNUAddItem("Common Problems", NULL, 'C', 0, NULL, mmain, NULLF);
   FA = MNUAddItem("Configuration", NULL, 'C', 0, NULL, mmain, NULLF);
   GA = MNUAddItem("Analysis Settings", NULL, 'A', 0, NULL, mmain, NULLF);
   HA = MNUAddItem("Options", NULL, 'O', 0, NULL, mmain, NULLF);
   IA = MNUAddItem("Specifications", NULL, 'S', 0, NULL, mmain, NULLF);
   JA = MNUAddItem("Graphics", NULL, 'G', 0, NULL, mmain, NULLF);
   LA = MNUAddItem("Viewing Statistics", NULL, 'V', 0, NULL, mmain, NULLF);
   MA = MNUAddItem("Printing Graphics", NULL, 'P', 0, NULL, mmain, NULLF);
   NA = MNUAddItem("Printing Reports", NULL, 'P', 0, NULL, mmain, NULLF);
   OA = MNUAddItem("Archiving Files", NULL, 'A', 0, NULL, mmain, NULLF);
   PA = MNUAddItem("Listing SP/FN", NULL, 'L', 0, NULL, mmain, NULLF);
   QA = MNUAddItem("Keyboard/Mouse", NULL, 'K', 0, NULL, mmain, NULLF);
   SA = MNUAddItem("Contractor Data", NULL, 'C', 0, NULL, mmain, NULLF);
   UA = MNUAddItem("Video Modes", NULL, 'V', 0, NULL, mmain, NULLF);
			/* second level - general HELP */
	MD = MNUAddItem("General HELP", NULL, 'G', 0, AA, mmain, NULLF);
	ND = MNUAddItem("Browse Manuals", NULL, 'B', 0, AA, mmain, br_man);
         /* second level - using headstat */
   KA = MNUAddItem("Overall Procedure", NULL, 'O', 0, PC, mmain, NULLF);
   QC = MNUAddItem("Doing A Stats Run", NULL, 'D', 0, PC, mmain, NULLF);
         /* second level - algorithms */
   VA = MNUAddItem("SP/FN Numbering", NULL, 'S', 0, EA, mmain, NULLF);
   WA = MNUAddItem("Cables", NULL, 'C', 0, EA, mmain, NULLF);
   XA = MNUAddItem("Sources", NULL, 'S', 0, EA, mmain, NULLF);
   YA = MNUAddItem("System Timing", NULL, 'S', 0, EA, mmain, NULLF);
   ZA = MNUAddItem("Water Depth", NULL, 'W', 0, EA, mmain, NULLF);
   AB = MNUAddItem("SP Time Interval", NULL, 'S', 0, EA, mmain, NULLF);
   BB = MNUAddItem("Speed Along Track", NULL, 'S', 0, EA, mmain, NULLF);
         /* third level - algorithms - cables */
   CB = MNUAddItem("Cable Depth & Comms", NULL, 'C', 0, WA, mmain, NULLF);
   DB = MNUAddItem("Bird Wing Angles", NULL, 'B', 0, WA, mmain, NULLF);
         /* third level - algorithms - sources */
   EB = MNUAddItem("Gun Timing", NULL, 'G', 0, XA, mmain, NULLF);
   FB = MNUAddItem("Misfires", NULL, 'M', 0, XA, mmain, NULLF);
   UC = MNUAddItem("Xfiring", NULL, 'X', 0, XA, mmain, NULLF);
   GB = MNUAddItem("Source Volume", NULL, 'S', 0, XA, mmain, NULLF);
   HB = MNUAddItem("Source Depth", NULL, 'S', 0, XA, mmain, NULLF);
   IB = MNUAddItem("Air Pressure", NULL, 'A', 0, XA, mmain, NULLF);
   JB = MNUAddItem("Gun Mask", NULL, 'G', 0, XA, mmain, NULLF);
   LB = MNUAddItem("Firing Sequence", NULL, 'F', 0, XA, mmain, NULLF);
   MB = MNUAddItem("Litton LRS-100", NULL, 'L', 0, XA, mmain, NULLF);
   NB = MNUAddItem("Geco GUNDA", NULL, 'G', 0, XA, mmain, NULLF);
   OB = MNUAddItem("Prakla VZAD", NULL, 'P', 0, XA, mmain, NULLF);
   ID = MNUAddItem("Digicon MDSL", NULL, 'D', 0, XA, mmain, NULLF);
   MB->separator = YES;  /* separator line above 'Litton' */
         /* fourth level - algorithms - sources - misfires  */
   FC = MNUAddItem("Misfire Definition", NULL, 'M', 0, FB, mmain, NULLF);
   PB = MNUAddItem("Consecutive Misfires", NULL, 'C', 0, FB, mmain, NULLF);
   QB = MNUAddItem("Series Misfires", NULL, 'S', 0, FB, mmain, NULLF);
   RB = MNUAddItem("Running Misfire %", NULL, 'R', 0, FB, mmain, NULLF);
   PB->separator = YES;  /* separator line above 'Consecutive' */
         /* fourth level - algorithms - sources - gun mask  */
   KB = MNUAddItem("Geco Gun Mask", NULL, 'G', 0, JB, mmain, NULLF);
   OC = MNUAddItem("Western Status", NULL, 'W', 0, JB, mmain, NULLF);
         /* second level - units */
   JC = MNUAddItem("Depth Units", NULL, 'D', 0, RA, mmain, NULLF);
   KC = MNUAddItem("Other Units", NULL, 'O', 0, RA, mmain, NULLF);
         /* second level - common problems */
   GC = MNUAddItem("General", NULL, 'G', 0, TA, mmain, NULLF);
   AD = MNUAddItem("Memory", NULL, 'M', 0, TA, mmain, NULLF);
   HC = MNUAddItem("Numbering", NULL, 'N', 0, TA, mmain, NULLF);
   IC = MNUAddItem("Read After Write", NULL, 'R', 0, TA, mmain, NULLF);
   LC = MNUAddItem("Debugging", NULL, 'D', 0, TA, mmain, NULLF);
         /* second level - configuration */
   MC = MNUAddItem("How To Configure", NULL, 'H', 0, FA, mmain, NULLF);
   ZB = MNUAddItem("Drives/Directories", NULL, 'D', 0, FA, mmain, NULLF);
   NC = MNUAddItem("Storage", NULL, 'S', 0, FA, mmain, NULLF);
   SB = MNUAddItem("Header", NULL, 'H', 0, FA, mmain, NULLF);
   TB = MNUAddItem("Project", NULL, 'P', 0, FA, mmain, NULLF);
   UB = MNUAddItem("Source", NULL, 'S', 0, FA, mmain, NULLF);
   VB = MNUAddItem("Cable Depth", NULL, 'C', 0, FA, mmain, NULLF);
   WB = MNUAddItem("Wing Angle", NULL, 'W', 0, FA, mmain, NULLF);
   XB = MNUAddItem("Gun Depth", NULL, 'G', 0, FA, mmain, NULLF);
   YB = MNUAddItem("Printer", NULL, 'P', 0, FA, mmain, NULLF);
   ZB->separator = YES;  /* separator line above 'Drives' */
   SB->separator = YES;  /* separator line above 'Header' */
         /* third level - configuration - header */
   RC = MNUAddItem("On Setting Formats", NULL, 'O', 0, SB, mmain, NULLF);
   SC = MNUAddItem("More On Formatting", NULL, 'M', 0, SB, mmain, NULLF);
   BD = MNUAddItem("Geco Formats", NULL, 'G', 0, SB, mmain, NULLF);
   CD = MNUAddItem("Western Formats", NULL, 'W', 0, SB, mmain, NULLF);
   DD = MNUAddItem("Prakla Formats", NULL, 'P', 0, SB, mmain, NULLF);
   ED = MNUAddItem("SSL Formats", NULL, 'S', 0, SB, mmain, NULLF);
   FD = MNUAddItem("CGG Formats", NULL, 'C', 0, SB, mmain, NULLF);
   GD = MNUAddItem("Digicon Formats", NULL, 'D', 0, SB, mmain, NULLF);
   HD = MNUAddItem("HGS Formats", NULL, 'H', 0, SB, mmain, NULLF);
   BD->separator = YES;  /* separator line above 'Geco Formats' */
         /* fourth level - configuration - header - Geco formats */
   JD = MNUAddItem("Geco Formats 1", NULL, 'G', 0, BD, mmain, NULLF);
   KD = MNUAddItem("Geco Formats 2", NULL, 'G', 0, BD, mmain, NULLF);
   LD = MNUAddItem("Geco Formats 3", NULL, 'G', 0, BD, mmain, NULLF);
         /* second level - function keys */
   AC = MNUAddItem("Keyboard/Function Keys", NULL, 'K', 0, QA, mmain, NULLF);
   BC = MNUAddItem("Mouse/More Keys", NULL, 'M', 0, QA, mmain, NULLF);
         /* second level - graphics */
   CC = MNUAddItem("Fixed Stikograms", NULL, 'F', 0, JA, mmain, NULLF);
   DC = MNUAddItem("Variable Stikograms", NULL, 'V', 0, JA, mmain, NULLF);
   VC = MNUAddItem("No Error Blocks", NULL, 'N', 0, JA, mmain, NULLF);
   EC = MNUAddItem("Other Graphics", NULL, 'O', 0, JA, mmain, NULLF);
   TC = MNUAddItem("Graphics Setup", NULL, 'G', 0, JA, mmain, NULLF);
   TC->separator = YES;  /* separator line above 'Graphics Setup' */
         /* third level graphics */
   WC = MNUAddItem("General Graphics Setup", NULL, 'G', 0, TC, mmain, NULLF);
   XC = MNUAddItem("Printer Resolutions", NULL, 'P', 0, TC, mmain, NULLF);
   YC = MNUAddItem("Printer Multipliers", NULL, 'P', 0, TC, mmain, NULLF);
	ZC = MNUAddItem("Other Printer Settings", NULL, 'O', 0, TC, mmain, NULLF);
			/* second level viewing statistics */
	OD = MNUAddItem("Statistics Window", NULL, 'S', 0, LA, mmain, NULLF);
	PD = MNUAddItem("Report Files", NULL, 'R', 0, LA, mmain, NULLF);
         /* make the menu & select */
   do {
		result = MNUDoSelect(mmain, UpdateClockWindow);
		if(result) run_help(result);
	} while (result);
   MNUDeleteMenu(mmain);
}


/***************************************************************
*   do it - uses "STATMAIN.HLP" (ASCII text file).
****************************************************************/
void run_help(MENUITEM *result)
{
   HWND W1, W2, Wmsg;
   char *hlpstr;
   _heapmin();
   if((hlpstr = ExtractFromHelpFile("STATMAIN.HLP", result->item0)) == NULL) {
		StatsErr("95001");
		return;
	}
   W1 = vcreat(19, 78, EMPHNORML, NO);   /* actual Help */
	vlocate(W1, 3, 1);
   vframe(W1, NORML, FRSINGLE);
   vtitle (W1, EMPHNORML, result->item0);
   Wmsg = vcreat(1, 80, REVNORML, NO);   /* bottom line message */
   vlocate(Wmsg, 24, 0);
   vatputs(Wmsg, 0, 35, "<Any Key>");
   vdispstr(W1, hlpstr);
   W2 = vcreat(23, 80, NORML, NO);       /* background blanking */
   vlocate(W2, 1, 0);
   visible(W2, YES, NO);
   visible(Wmsg, YES, NO);
	visible(W1, YES, NO);
   MouseKey(1, 1);
   vdelete(Wmsg, NONE);
   vdelete(W2, NONE);
   vdelete(W1, NONE);
   _heapmin();
}


/***************************************************************
*   call run_help with a string
****************************************************************/
void string_help(char *string)
{
	MENUHDR	*mmain;
   MENUITEM *AA;
	mmain = MNUCreateHdr(POPUP);		  /* create */
   AA = MNUAddItem(string, NULL, ' ', 0, NULL, mmain, NULLF);
   run_help(AA);
   MNUDeleteMenu(mmain);
   _heapmin();
}


/***************************************************************
*	 browse documentation (ECLSTAT & README .DOCs)
****************************************************************/
void br_man() {
	char cur_dir[_MAX_PATH+1];
	getcwd(cur_dir, _MAX_PATH);
	ViewFile (opt->work_dr, cur_dir, "*.DOC");
}
