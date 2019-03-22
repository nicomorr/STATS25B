/***************************************************************
*   SETGRAF1.C  set/view/load/save graphics routines
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
*  (c) West Country Systems Ltd, Cyprus, 1991.
****************************************************************/

#include <stdio.h>         /* microsoft */
#include <malloc.h>

#include <dw.h>            /* greenleaf */
#include <dwmenu.h>
#include <dwdata.h>
#include <sftd.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct GRAF_DEF *graf;      /* pointer to graphics list */
extern struct PRO_DEF *pd;         /* pointer to project list */
extern struct SPEC_DEF *spc;     /* pointer to specifications list */
extern HWND Wspec;                 /* need as global */
extern char *answer[];             /* pointer to YES/NO */
extern char *units[];              /* FEET/METRES PSI/BARS etc */


/*************************************************************
* main graphics configuration function - called from maincfg.c
*	 note that speed/heading/clip all refer to .GRF graphing
**************************************************************/
void setgraf ()
{
	HWND W1;
	MENUHDR *mmain;
   MENUITEM *view, *load, *save;        /* menu items */
   mmain = MNUCreateHdr(POPUP);         /* create  */
   MNUSetGlobalHelp(mmain, graf_help);
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
         /* menu items */
      /*# ch_graf() set_load_graf() save_graf() graf_help() */
   view = MNUAddItem("View/Set Graphics", NULL, 'V', 0, NULL, mmain, ch_graf);
   load = MNUAddItem("Load Graphics", NULL, 'L', 0, NULL, mmain, set_load_graf);
   save = MNUAddItem("Save Graphics", NULL, 'S', 0, NULL, mmain, save_graf);
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
*   graphics help window
**************************************************************/
void graf_help()
{
   string_help("General Graphics Setup");
}


/**************************************************************
*	 set load as it takes an argument
**************************************************************/
void set_load_graf()
{
   load_graf(1);
}


/**************************************************************
*   save graphics settings to disk
**************************************************************/
void save_graf()
{
	FILE *fp;
	if (!KeySure()) return;
   fp = fopen("GRAPHICS.CFG", "wb");
   fwrite(graf, sizeof(struct GRAF_DEF), 1, fp);
	fclose(fp);
   StatsMessage ("SAVED GRAPHICS.CFG");
}


/**************************************************************
*   load graphics settings from disk
**************************************************************/
load_graf(int i)
{
   FILE *fp;
   if(i)
      if (!KeySure())
         return(0);
   if((fp = fopen("GRAPHICS.CFG", "rb")) == NULL) {
     if(i) StatsErr ("19002");
	  return(0);
	}
   if (1 != fread(graf, sizeof(struct GRAF_DEF), 1, fp))
      StatsErr("19003");
   fclose(fp);
   if(i) StatsMessage("LOADED GRAPHICS.CFG");
	return(1);
}


/**************************************************************
*   set/view graphics
**************************************************************/
void ch_graf ()
{
   HWND W1;           /* bottom line window */
	TRANSACTION *tp;   /* transaction structure */
	FIELD *fld; 		 /* field structure */
	register int i;
	char a_str[4];
   char b_str[4];
   char c_str[4];
   char d_str[4];
   char e_str[4];
   char f_str[4];
   char g_str[4];
   char h_str[4];
   char i_str[4];
   char j_str[4];
   char k_str[4];
	char l_str[4];
	char m_str[4];
   _heapmin();               /* defragment the heap */
   strcpy(a_str, answer[graf->dep_anl1]);
   strcpy(b_str, answer[graf->gun_anl1]);
   strcpy(c_str, answer[graf->vol_anl1]);
   strcpy(d_str, answer[graf->man_anl1]);
   strcpy(e_str, answer[graf->clk_anl]);
   strcpy(f_str, answer[graf->sp_anl]);
   strcpy(g_str, answer[graf->fn_anl]);
   strcpy(h_str, answer[graf->gmsk_anl]);
   strcpy(i_str, answer[graf->seq_anl]);
   strcpy(j_str, answer[graf->gd_anl]);
   strcpy(k_str, answer[graf->tim_anl]);
	strcpy(l_str, answer[graf->spd_anl]);
	strcpy(m_str, answer[graf->hed_anl]);
            /* data entry window */
	Wspec = vcreat (17, 70, NORML, NO);
	vshadow(Wspec, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
	vlocate (Wspec, 4, 4);
   vframe (Wspec, EMPHNORML, FRDOUBLE);
   vtitle (Wspec, EMPHNORML, " Graphics Setup ");
            /* bottom line window */
	W1 = vcreat (1, 80, REVNORML, NO);
	vlocate (W1, 24, 0);
	vratputs (W1, 0, 13, REVEMPHNORML, "<SPACEBAR> Toggles");
	vratputs (W1, 0, 1, REVNORML, "<ESC> Exit");
	vratputs (W1, 0, 36, REVERR, "  F1  ");
   vratputs (W1, 0, 60, REVEMPHNORML, "<CTRL-ENTER> Accept");
            /* start organising data entry window */
      /*# defldval_graf() detranval_glob() EscapeKey() StepThrough() */
	tp = vpreload (31, 24, NULLF, defldval_graf, detranval_glob, NULLF);
   tp->helpfn = graf_help;  /* warning on param list can be ignored */
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   vde_keys._irdffn[0] = StepThrough;   /* step through YES/N) */
      /*# defldset_graf() */
   tp->fld_setfn = defldset_graf;        /* as in SETANAL2.C */
	fld = *(tp->def);
	for( i = 0; i < 31; i++ )
		fld[i].use_ext = TRUE;	 /* use/display the preload values */
			/* field names */
   vdefatsa (tp, 0, 0, 1, NORML, "Cable Depth");
   vdefatsa (tp, 1, 1, 1, NORML, "Source Timing");
   vdefatsa (tp, 2, 2, 1, NORML, "Source Volume");
   vdefatsa (tp, 3, 3, 1, NORML, "Air Pressure");
   vdefatsa (tp, 4, 4, 1, NORML, "System Timing");
   vdefatsa (tp, 5, 5, 1, NORML, "SP Integer");
   vdefatsa (tp, 6, 6, 1, NORML, "FN Integer");
   vdefatsa (tp, 7, 7, 1, NORML, "Gun Masks");
   vdefatsa (tp, 8, 8, 1, NORML, "Firing Sequence");
   vdefatsa (tp, 9, 9, 1, NORML, "Gun Depth");
   vdefatsa (tp, 10, 10, 1, NORML, "SP Timing");
	vdefatsa (tp, 11, 12, 1, NORML, "Speed Graphics");
	vdefatsa (tp, 12, 13, 1, NORML, "Heading Graphics");
         /* cable depths */
   vdefstr (tp, 0, 0, 1, NULL, 18, 3, "!!!", a_str, YES);
   vdefdbl (tp, 1, 0, 25, "Scale Range", 38, 0, "99", &graf->cab_scale, YES);
   vdefdbl (tp, 2, 0, 45, "Spec Line", 56, 0, "99", &graf->cab_line, YES);
	vdefatsa (tp, 13, 0, 62, NORML, units[pd->loc_dp]);
         /* source timing */
   vdefstr (tp, 3, 1, 1, NULL, 18, 3, "!!!", b_str, YES);
   vdefdbl (tp, 4, 1, 25, "Scale Range", 38, 0, "99", &graf->gun_scale, YES);
	vdefatsa (tp, 14, 1, 42, NORML, "mS");
         /* source volume */
   vdefstr (tp, 5, 2, 1, NULL, 18, 3, "!!!", c_str, YES);
   vdefuint (tp, 6, 2, 25, "Scale Range", 38, 0, "9999", &graf->vol_scale, YES);
	vdefatsa (tp, 15, 2, 44, NORML, units[pd->vol_un]);
         /* air pressure */
   vdefstr (tp, 7, 3, 1, NULL, 18, 3, "!!!", d_str, YES);
   vdefuint (tp, 8, 3, 25, "Scale Range", 38, 0, "9999", &graf->air_scale, YES);
	vdefatsa (tp, 16, 3, 44, NORML, units[pd->pr_un]);
         /* system Timing */
   vdefstr (tp, 9, 4, 1, NULL, 18, 3, "!!!", e_str, YES);
   vdefdbl (tp, 10, 4, 25, "Scale Range", 38, 0, "99", &graf->tim_scale, YES);
	vdefatsa (tp, 17, 4, 42, NORML, "mS");
			/* SP integer */
   vdefstr (tp, 11, 5, 1, NULL, 18, 3, "!!!", f_str, YES);
         /* FN integer */
   vdefstr (tp, 12, 6, 1, NULL, 18, 3, "!!!", g_str, YES);
         /* gun masks */
   vdefstr (tp, 13, 7, 1, NULL, 18, 3, "!!!", h_str, YES);
         /* firing sequence */
   vdefstr (tp, 14, 8, 1, NULL, 18, 3, "!!!", i_str, YES);
         /* gun depths */
   vdefstr (tp, 15, 9, 1, NULL, 18, 3, "!!!", j_str, YES);
   vdefdbl (tp, 16, 9, 25, "Scale Range", 38, 0, "99", &graf->gud_scale, YES);
   vdefdbl (tp, 17, 9, 45, "Spec Line", 56, 0, "99", &graf->gud_line, YES);
	vdefatsa (tp, 18, 9, 62, NORML, units[pd->loc_dp]);
         /* SP timing */
	vdefstr (tp, 18, 10, 1, NULL, 18, 3, "!!!", k_str, YES);
			/* header for .GRF graphics block */
	vdefatsa (tp, 19, 11, 19, EMPHNORML, "========= VGA GRAPHICS =========");
			/* speed comparison (for current) */
	vdefstr (tp, 19, 12, 1, NULL, 18, 3, "!!!", l_str, YES);
			/* speed clipping window */
	vdefdbl (tp, 20, 12, 25, "Clip Window", 38, 0, "99.9", &graf->spwin, YES);
	vdefatsa (tp, 20, 12, 43, NORML, units[pd->sp_un]);
			/* polynomial curve fitting order */
	vdefuint (tp, 21, 12, 52, "PolyOrder", 65, 0, "9", &graf->polyorder, YES);
			/* heading comparison (for crab angle) */
	vdefstr (tp, 22, 13, 1, NULL, 18, 3, "!!!", m_str, YES);
			/* heading clipping window */
	vdefdbl (tp, 23, 13, 25, "Clip Window", 38, 0, "99.9", &graf->hdwin, YES);
	vdefatsa (tp, 21, 13, 43, NORML, "DEGREES");
			/* printer resolution .GRF graphics */
	vdefuint (tp, 24, 13, 52, "Resolution", 65, 0, "9", &graf->res, YES);
			/* printer X multiplier .GRF graphics */
	vdefdbl (tp, 25, 14, 1, "X Multiplier", 18, 0, "99.9", &graf->xm, YES);
			/* printer Y multiplier .GRF graphics */
	vdefdbl (tp, 26, 14, 25, "Y Multiplier", 38, 0, "99.9", &graf->ym, YES);
			/* top margin */
	vdefuint (tp, 27, 14, 52, "Top Margin", 65, 0, "99", &graf->tmar, YES);
			/* left margin */
	vdefuint (tp, 28, 15, 1, "Left Margin", 18, 0, "99", &graf->lmar, YES);
			/* formfeed */
	vdefuint (tp, 29, 15, 25, "FormFeed After Print", 47, 0, "9", &graf->ff, YES);
	vdefatsa (tp, 22, 15, 50, NORML, "0 = YES  1 = NO");
			/* orientation */
	vdefuint (tp, 30, 16, 1, "Orientation", 18, 0, "9", &graf->or, YES);
	vdefatsa (tp, 23, 16, 22, NORML, "0 = Portrait  1 = Landscape");
				/* visible & read it */
   visible (Wspec, YES, NO);
   visible (W1, YES, NO);
	if(!vread (tp, Wspec, YES)) {  /* only if DWSUCCESS (0) */
      graf->dep_anl1 = (strcmp(a_str, "YES")) ? 0 : 1;
      graf->gun_anl1 = (strcmp(b_str, "YES")) ? 0 : 1;
      graf->vol_anl1 = (strcmp(c_str, "YES")) ? 0 : 1;
      graf->man_anl1 = (strcmp(d_str, "YES")) ? 0 : 1;
      graf->clk_anl = (strcmp(e_str, "YES")) ? 0 : 1;
      graf->sp_anl = (strcmp(f_str, "YES")) ? 0 : 1;
      graf->fn_anl = (strcmp(g_str, "YES")) ? 0 : 1;
      graf->gmsk_anl = (strcmp(h_str, "YES")) ? 0 : 1;
      graf->seq_anl = (strcmp(i_str, "YES")) ? 0 : 1;
      graf->gd_anl = (strcmp(j_str, "YES")) ? 0 : 1;
		graf->tim_anl = (strcmp(k_str, "YES")) ? 0 : 1;
		graf->spd_anl = (strcmp(l_str, "YES")) ? 0 : 1;
		graf->hed_anl = (strcmp(m_str, "YES")) ? 0 : 1;
	}
	USR_KEY1=0x00;		  /* stop YES/NO toggling on another data entry */
	vdelete(Wspec, NONE);
   vdelete(W1, NONE);
   _heapmin();               /* defragment the heap */
	return;
}

/**************************************************************
*	 field set function
**************************************************************/
defldset_graf (TRANSACTION *tp)
{
	FIELD *field;
   field=&((*(tp->def))[tp->cur_fld]);
   if( (tp->cur_fld == 0) || (tp->cur_fld == 3) ||
      (tp->cur_fld == 5) || (tp->cur_fld == 7) ||
      (tp->cur_fld == 9) || (tp->cur_fld == 11) ||
      (tp->cur_fld == 12) || (tp->cur_fld == 13) ||
      (tp->cur_fld == 14) || (tp->cur_fld == 15) ||
		(tp->cur_fld == 18) || (tp->cur_fld == 19) ||
		(tp->cur_fld == 22) ) {
      USR_KEY1=0x20; /* space bar */
      if(!strlen(field->dataptr))
         StepThrough(tp,Wspec);
      return(TRUE);
   }
   USR_KEY1 = 0x00;
   return(TRUE);
}


/**************************************************************
*	 field validation
**************************************************************/
defldval_graf (TRANSACTION *tp)
{
   if(tp->cur_fld == 1) {
      if((graf->cab_scale < 2.0)||(graf->cab_scale > 20.0)) {
         StatsErr("19004");  /* must be between 2 & 20 */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 2) {
      if((graf->cab_line < 1.0)||(graf->cab_line > graf->cab_scale -1.0)) {
         StatsErr("19005");  /* between 1 and scale minus 1 */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 4) {
      if(graf->gun_scale < (double) (spc->gun1) + 1.0) {
         StatsErr("19006");  /* must be spec + 1 at least */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 6) {
      if( (!graf->vol_scale)||(graf->vol_scale >= spc->gvol1) ) {
         StatsErr("19007");  /* must be between 1 & spec */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 8) {
      if( (!graf->air_scale)||(graf->air_scale >= spc->ma1) ) {
         StatsErr("19008");  /* must be between 1 & spec */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 10) {
      if(graf->tim_scale < 1) {
         StatsErr("19011");  /* at least 1 mS */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 16) {
      if((graf->gud_scale < 2)||(graf->gud_scale > 20)) {
         StatsErr("19009");  /* between 2 & 20 */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 17) {
      if((graf->gud_line < 1)||(graf->gud_line > graf->gud_scale -1)) {
         StatsErr("19010");  /* between 1 & scale minus 1 */
         return(FALSE);
      }
	}
	if( ( tp->cur_fld == 20 ) && ( graf->spwin < 0.29F ) ) {
		StatsErr("19001");	/* clipping window too small 1 */
		return(FALSE);
	}
	if( ( tp->cur_fld == 23 ) && ( graf->hdwin < 1.99F ) ) {
		StatsErr("19012");	/* clipping window too small */
		return(FALSE);
	}
	if( ( tp->cur_fld == 25 ) && ( graf->xm < 1.0F ) ) {
		StatsErr("19013");	/* x multiplier must be >= 1.0 */
		return(FALSE);
	}
	if( ( tp->cur_fld == 26 ) && ( graf->ym < 1.0F ) ) {
		StatsErr("19014");	/* y multiplier must be >= 1.0 */
		return(FALSE);
	}
   return(TRUE);
}
