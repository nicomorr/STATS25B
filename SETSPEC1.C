/***************************************************************
*   SETSPEC1.C  set specifications
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

#include <dw.h>            /* greenleaf */
#include <dwmenu.h>
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct SPEC_DEF *spc;     /* pointer to specifications list */
extern struct PRO_DEF *pd;       /* project list pointer */
extern struct EXT_DEF *ext;      /* various external variables ptr */
extern char *units[];            /* FEET/METRES  PSI/BARS  etc */


/*************************************************************
* main specifications configuration - called from sts.c
**************************************************************/
void setspec ()
{
	HWND W1;
	MENUHDR *mmain;
	MENUITEM *view, *load, *save;		  /* top */
	mmain = MNUCreateHdr(POPUP);       /* create  */
	MNUSetGlobalHelp(mmain, specs_help);
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
      /*# ch_spec() set_load_spec() save_spec() specs_help() */
	view = MNUAddItem("View/Change Specs", NULL, 'V', 0, NULL, mmain, ch_spec);
	load = MNUAddItem("Load Specs", NULL, 'L', 0, NULL, mmain, set_load_spec);
	save = MNUAddItem("Save Specs", NULL, 'S', 0, NULL, mmain, save_spec);
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
*   change specifications
**************************************************************/
ch_spec ()
{
	register int i=0;
   HWND W1, W2;             /* data entry window / message window */
   HWND Wborder;            /* border for scrollable data entry window */
	TRANSACTION *tp;         /* transaction structure */
	FIELD *fld;
            /* define field variables */
	double d1min,            /* cable depths */
			 d1max,				 /* two spec levels */
			 d2min,
          d2max,
          gun1,             /* gun timing */
          gun2,             /* two spec levels */
          clk,              /* nominal clock time */
          clk_ra,           /* variation permitted */
          gsp,              /* ground speed */
          wd_min,           /* min/max water depths - checks fatho */
          wd_max,
          gd_min,           /* min/max gun depths */
          gd_max,
          wing,             /* wing angle (+/-) */
          t_min,            /* min & max SP time interval */
          t_max;
            /* initialise (double) conversion field variables */
	d1min = (double)spc->d1min;            /* cable depths */
	d1max = (double)spc->d1max;				/* two spec levels */
	d2min = (double)spc->d2min;
	d2max = (double)spc->d2max;
   gun1 =  (double)spc->gun1;             /* gun timing */
   gun2 =  (double)spc->gun2;             /* two spec levels */
   clk =   (double)spc->clk;              /* nominal clock timing */
   clk_ra = (double)spc->clk_ra;          /* variation permitted */
   gsp =   (double)spc->gsp;              /* ground speed */
   wd_min = (double)spc->wd_min;          /* min/max water depths */
	wd_max = (double)spc->wd_max;
	gd_min = (double)spc->gd_min; 			/* min and max gun depths */
   gd_max = (double)spc->gd_max;
   t_min = (double)spc->t_min;            /* min & max SP time interval */
   t_max = (double)spc->t_max;
   wing = (double) spc->wing;
            /* bottom line window */
	W2 = vcreat (1, 80, REVNORML, NO);
	vlocate (W2, 24, 0);
	vratputs (W2, 0, 1, REVNORML, "<ESC> Exit");
	vratputs (W2, 0, 36, REVERR, "  F1  ");
   vratputs (W2, 0, 60, REVEMPHNORML, "<CTRL-ENTER> Accept");
            /* border for data entry window with fixed lines */
	Wborder = vcreat (18, 70, NORML, NO);
	vshadow(Wborder, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
   vlocate (Wborder, 3, 5);
   vframe (Wborder, EMPHNORML, FRDOUBLE);
   vtitle (Wborder, EMPHNORML, " Specifications ");
	vratputs (Wborder, 17, 17, REVERR, "Page Up/Page Down for more Selections");
            /* actual data entry window (scrollable) */
   W1 = vcreat (21, 70, NORML, NO);
   vwind (W1, 16, 70, 0, 0);
   vlocate (W1, 4, 5);
            /* set up data entry structure */
   tp = vpreload (29, 37, NULLF, defldval_spec, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
   tp->paint_fl = TRUE;
      /*# specs_help() defldval_spec() detranval_glob() EscapeKey() */
   tp->helpfn = specs_help;         /* param warning can be ignored */
   tp->inp_idle = UpdateClockWindow;
	fld = *(tp->def);
   for (i;i<29;i++)
		fld[i].use_ext = TRUE;   /* use/display the preload values */
            /* DECLARE FIELDS */
	vdefdbl (tp, 0, 0, 1, "Cable Depths Spec 1", 30, 0, "999.9", &d1min, YES);
	vdefdbl (tp, 1, 0, 41, NULL, 42, 0, "999.9", &d1max, YES);
	vdefdbl (tp, 2, 1, 1, "Cable Depths Spec 2", 30, 0, "999.9", &d2min, YES);
	vdefdbl (tp, 3, 1, 41, NULL, 42, 0, "999.9", &d2max, YES);
	vdefuint (tp, 4, 2, 1, "Remote Unit Comms %", 35, 0, "99", &spc->dcom, YES);
	vdefdbl (tp, 5, 3, 1, "Gun Timings Spec 1", 37, 0, "99.99", &gun1, YES);
	vdefdbl (tp, 6, 4, 1, "Gun Timings Spec 2", 37, 0, "99.99", &gun2, YES);
	vdefuint (tp, 7, 5, 1, "Source Volume Spec 1", 38, 0, "9999", &spc->gvol1, YES);
	vdefuint (tp, 8, 6, 1, "Source Volume Spec 2", 38, 0, "9999", &spc->gvol2, YES);
	vdefuint (tp, 9, 7, 1, "Manifold Pressure 1", 38, 0, "9999", &spc->ma1, YES);
	vdefuint (tp, 10, 8, 1, "Manifold Pressure 2", 38, 0, "9999", &spc->ma2, YES);
	vdefuint (tp, 11, 9, 1, "Consecutive Misfires", 38, 0, "99", &spc->mf1, YES);
	vdefdbl (tp, 12, 10, 1, "System Timing", 38, 0, "999.9", &clk, YES);
	vdefdbl (tp, 13, 10, 48, NULL, 48, 0, "99.9", &clk_ra, YES);
	vdefuint (tp, 14, 11, 1, "Misfire Series", 38, 0, "999", &spc->mf3, YES);
	vdefuint (tp, 15, 11, 47, NULL, 46, 0, "999", &spc->mf3_ra, YES);
	vdefuint (tp, 16, 12, 1, "Permitted Misfires", 38, 0, "99", &spc->mf4, YES);
	vdefuint (tp, 17, 13, 1, "Shotpoint Integer", 30, 0, "99", &spc->spint, YES);
	vdefuint (tp, 18, 13, 38, "File Number Integer", 62, 0, "99", &spc->fnint, YES);
   vdefdbl (tp, 19, 14, 1, "Ground Speed", 38, 0, "99.9", &gsp, YES);
	vdefuint (tp, 20, 15, 1, "Gun Mask Minimum", 38, 0, "99", &spc->gmsk, YES);
	vdefstr (tp, 21, 16, 1, "Firing Sequence Order", 30, pd->seq_len,
		NULL, spc->f_seq, YES);
   vdefdbl (tp, 22, 17, 1, "Water Depth", 30, 0, "9999.9", &wd_min, YES);
	vdefdbl (tp, 23, 17, 41, NULL, 42, 0, "9999.9", &wd_max, YES);
   vdefdbl (tp, 24, 18, 1, "Gun Depth", 30, 0, "99.9", &gd_min, YES);
   vdefdbl (tp, 25, 18, 41, NULL, 42, 0, "99.9", &gd_max, YES);
   vdefdbl (tp, 26, 19, 1, "SP Time Interval", 30, 0, "99.9", &t_min, YES);
   vdefdbl (tp, 27, 19, 41, NULL, 42, 0, "99.9", &t_max, YES);
   vdefdbl (tp, 28, 20, 1, "Wing Angles", 30, 0, "99.9", &wing, YES);
            /* DECLARE ATSAYS */
	vdefatsa (tp, 0, 0, 38, NORML, "<>");
	vdefatsa (tp, 1, 1, 38, NORML, "<>");
	vdefatsa (tp, 2, 0, 50, NORML, units[pd->loc_dp]);
	vdefatsa (tp, 3, 1, 50, NORML, units[pd->loc_dp]);
	vdefatsa (tp, 4, 2, 30, NORML, "<=");
	vdefatsa (tp, 5, 2, 40, NORML, "PerCent Bad");
	vdefatsa (tp, 6, 3, 30, NORML, "<= +/-");
	vdefatsa (tp, 7, 4, 30, NORML, "<= +/-");
	vdefatsa (tp, 8, 3, 43, NORML, "mS");
	vdefatsa (tp, 9, 4, 43, NORML, "mS");
	for(i=0;i<4;i++)
		vdefatsa (tp, 10+i, 5+i, 30, NORML, "Minimum");
	for(i=0;i<2;i++)
		vdefatsa (tp, 14+i, 5+i, 43, NORML, units[pd->vol_un]);
	for(i=0;i<2;i++)
		vdefatsa (tp, 16+i, 7+i, 43, NORML, units[pd->pr_un]);
	vdefatsa (tp, 18, 9, 30, NORML, "Maximum");
	vdefatsa (tp, 19, 10, 30, NORML, "Nominal");
	vdefatsa (tp, 20, 11, 30, NORML, "Maximum");
	vdefatsa (tp, 21, 12, 30, NORML, "Maximum");
	vdefatsa (tp, 22, 10, 44, NORML, "+/-");
	vdefatsa (tp, 23, 11, 42, NORML, "in");
	vdefatsa (tp, 24, 12, 42, NORML, "PerCent");
	vdefatsa (tp, 25, 14, 30, NORML, "Maximum");
	vdefatsa (tp, 26, 14, 44, NORML, units[pd->sp_un]);
	vdefatsa (tp, 27, 15, 30, NORML, "Minimum");
	vdefatsa (tp, 28, 15, 44, NORML, "Guns Set");
	vdefatsa (tp, 29, 17, 38, NORML, "<>");
	vdefatsa (tp, 30, 18, 38, NORML, "<>");
	vdefatsa (tp, 31, 17, 50, NORML, units[pd->loc_dp]);
	vdefatsa (tp, 32, 18, 50, NORML, units[pd->loc_dp]);
   vdefatsa (tp, 33, 10, 56, NORML, "mS");
   vdefatsa (tp, 34, 19, 38, NORML, "<>");
   vdefatsa (tp, 35, 19, 50, NORML, "Seconds");
   vdefatsa (tp, 36, 20, 36, NORML, "Degrees");

            /* MAKE VISIBLE & DO TRANSACTION */
   visible (W2, YES, NO);
   visible (Wborder, YES, NO);
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
            /* have to convert back to float */
		spc->d1min = (float)d1min;     /* cable depths */
		spc->d1max = (float)d1max;     /* two spec levels */
		spc->d2min = (float)d2min;
		spc->d2max = (float)d2max;
		spc->gun1 = (float)gun1;		 /* gun timing */
		spc->gun2 = (float)gun2;       /* two spec levels */
		spc->clk = (float)clk;			 /* nominal clock timing */
		spc->clk_ra = (float)clk_ra;	 /* variation permitted */
		spc->gsp = (float)gsp;			 /* ground speed */
		spc->wd_min = (float)wd_min;	 /* min/max water depths */
		spc->wd_max = (float)wd_max;
		spc->gd_min = (float)gd_min;   /* min and max gun depths */
      spc->gd_max = (float)gd_max;
      spc->t_min = (float)t_min;
      spc->t_max = (float)t_max;
      spc->wing = (float) wing;
      vdelete (W1, NONE);
      vdelete (Wborder, NONE);
      vdelete (W2, NONE);
		return(1); /* return value used in changing specs during stats run */
	}
   vdelete (W1, NONE);
   vdelete (Wborder, NONE);
	vdelete (W2, NONE);
	return(0);
}


/**************************************************************
*   field validation
**************************************************************/
defldval_spec (TRANSACTION *tp)
{
	char err_str[41];
	char t_str[2];
	if(tp->cur_fld == 21) {   /* firing sequence order validation */
		if(fir_seq()) {
         strcpy(err_str, "SRCE NOS FROM 1 TO ");
			strcat(err_str, itoa(pd->seq_len, t_str, 10));
         strcat(err_str, " ONLY");
			StatsError(err_str);
			return(FALSE);
		}
	}
	return (TRUE);
}


/*************************************************************
* firing length validation - MORE NEEDED HERE
**************************************************************/
fir_seq()
{
	int i = 0;
	for (i; i<(int)pd->seq_len; i++) {
		if (((int)spc->f_seq[i] < 49)||((int)spc->f_seq[i] > (48+pd->seq_len)))
				return(1);
	}
	return(0);
}


/**************************************************************
*   save specifications to disk
**************************************************************/
void save_spec(void)
{
	FILE *fp;
	if (!KeySure()) return;
	fp = fopen("SPECSET.CFG", "wb");
	fwrite(spc, sizeof(struct SPEC_DEF), 1, fp);
	fclose(fp);
   StatsMessage ("SAVED SPECSET.CFG");
}


/**************************************************************
*   load specifications from disk
**************************************************************/
load_spec(int i)
{
   FILE *fp;
   if(i)
   if (!KeySure())
       return(0);
	if((fp = fopen("SPECSET.CFG", "rb")) == NULL) {
	  if(i) StatsErr("31001");
	  return(0);
	}
	if (1 != fread(spc, sizeof(struct SPEC_DEF), 1, fp))
		StatsErr("31002");
   fclose(fp);
   if(i) StatsMessage("LOADED SPECSET.CFG");
	return(1);
}


/**************************************************************
*   global specs help window
**************************************************************/
void specs_help()
{
   string_help("Specifications");
}


/**************************************************************
*	 set load as it takes an argument
**************************************************************/
void set_load_spec()
{
	load_spec(1);
}
