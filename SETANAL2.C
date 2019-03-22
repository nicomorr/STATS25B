/***************************************************************
*   SETANAL2.C  turn analysis routines on or off
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
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct HDR_DEF *hd;         /* header list pointer */
extern struct ANAL_DEF *andef;     /* pointer to set/analysis list */
extern char *answer[];             /* pointer to YES/NO */
extern struct EXT_DEF *ext;        /* various global externals */

HWND Wspec;   /* need as global unfortunately */


/**************************************************************
*	 view/change analyses
**************************************************************/
ch_anl ()
{
   HWND W1;           /* message window for control */
   HWND Wborder;      /* border windiw for scrollable data entry */
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
	char n_str[4];
	char o_str[4];
	char p_str[4];
	char q_str[4];
	char r_str[4];
	char s_str[4];
	char t_str[4];
	char u_str[4];
	char v_str[4];
	char w_str[4];
	char x_str[4];
	char y_str[4];
	char z_str[4];
	char aa_str[4];
	char ab_str[4];
	char ac_str[4];
	char ad_str[4];
	char ae_str[4];
	char af_str[4];
	char ag_str[4];
	char ah_str[4];
	char ai_str[4];
	char aj_str[4];
	char ak_str[4];
	char al_str[4];
	char am_str[4];
   char an_str[4];
   char ao_str[4];
   char ap_str[4];
   char aq_str[4];
   char ar_str[4];
	char as_str[4];
	char at_str[4];
	char au_str[4];
	char av_str[4];
	strcpy(a_str, answer[andef->dep_anl1]);
	strcpy(b_str, answer[andef->ldep_anl1]);
	strcpy(c_str, answer[andef->dep_anl2]);
	strcpy(d_str, answer[andef->ldep_anl2]);
	strcpy(e_str, answer[andef->dep_com]);
	strcpy(f_str, answer[andef->ldep_com]);
	strcpy(g_str, answer[andef->gun_anl1]);
	strcpy(h_str, answer[andef->lgun_anl1]);
	strcpy(i_str, answer[andef->gun_anl2]);
	strcpy(j_str, answer[andef->lgun_anl2]);
	strcpy(k_str, answer[andef->vol_anl1]);
	strcpy(l_str, answer[andef->lvol_anl1]);
	strcpy(m_str, answer[andef->vol_anl2]);
	strcpy(n_str, answer[andef->lvol_anl2]);
	strcpy(o_str, answer[andef->man_anl1]);
	strcpy(p_str, answer[andef->lman_anl1]);
	strcpy(q_str, answer[andef->man_anl2]);
	strcpy(r_str, answer[andef->lman_anl2]);
	strcpy(s_str, answer[andef->mf1_anl]);
	strcpy(t_str, answer[andef->lmf1_anl]);
	strcpy(u_str, answer[andef->clk_anl]);
	strcpy(v_str, answer[andef->lclk_anl]);
	strcpy(w_str, answer[andef->mf3_anl]);
	strcpy(x_str, answer[andef->lmf3_anl]);
	strcpy(y_str, answer[andef->mf4_anl]);
	strcpy(z_str, answer[andef->lmf4_anl]);
	strcpy(aa_str, answer[andef->sp_anl]);
	strcpy(ab_str, answer[andef->lsp_anl]);
	strcpy(ac_str, answer[andef->fn_anl]);
	strcpy(ad_str, answer[andef->lfn_anl]);
	strcpy(ae_str, answer[andef->gsp_anl]);
	strcpy(af_str, answer[andef->lgsp_anl]);
	strcpy(ag_str, answer[andef->gmsk_anl]);
	strcpy(ah_str, answer[andef->lgmsk_anl]);
	strcpy(ai_str, answer[andef->seq_anl]);
	strcpy(aj_str, answer[andef->lseq_anl]);
	strcpy(ak_str, answer[andef->wdp_anl]);
	strcpy(al_str, answer[andef->lwdp_anl]);
	strcpy(am_str, answer[andef->gd_anl]);
   strcpy(an_str, answer[andef->lgd_anl]);
   strcpy(ao_str, answer[andef->tim_anl]);
   strcpy(ap_str, answer[andef->ltim_anl]);
   strcpy(aq_str, answer[andef->wi_anl]);
	strcpy(ar_str, answer[andef->lwi_anl]);
	strcpy(as_str, answer[andef->ws_anl]);   /* water speed */
	strcpy(at_str, answer[andef->lws_anl]);
	strcpy(au_str, answer[andef->cu_anl]);   /* current analysis */
	strcpy(av_str, answer[andef->cr_anl]);   /* crab angle analysis */
            /* border for data entry window with fixed lines */
   Wborder = vcreat (21, 50, NORML, NO);
   vlocate (Wborder, 2, 15);
   vframe (Wborder, EMPHNORML, FRDOUBLE);
   vtitle (Wborder, EMPHNORML, " Analysis ");
   vratputs (Wborder, 0, 25, REVERR, "DO STATS     LOG ERRORS");
   vratputs (Wborder, 20, 7, REVERR, "Page Up/Page Down for more Selections");
            /* actual data entry window (scrollable) */
	Wspec = vcreat (25, 50, NORML, NO);
   vwind (Wspec, 19, 50, 0, 0);
   vlocate (Wspec, 3, 15);
            /* bottom line window */
	W1 = vcreat (1, 80, REVNORML, NO);
	vlocate (W1, 24, 0);
	vratputs (W1, 0, 13, REVEMPHNORML, "<SPACEBAR> Toggles");
	vratputs (W1, 0, 1, REVNORML, "<ESC> Exit");
	vratputs (W1, 0, 36, REVERR, "  F1  ");
   vratputs (W1, 0, 60, REVEMPHNORML, "<CTRL-ENTER> Accept");
            /* start organising data entry window */
      /*# defldval_anl() detranval_glob() EscapeKey() anl_help() */
	tp = vpreload (48, 27, NULLF, defldval_anl, detranval_glob, NULLF);
   tp->cancfn=EscapeKey;
   tp->helpfn = anl_help;   /* param warning can be ignored */
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   vde_keys._irdffn[0] = StepThrough;
      /*# defldset_anl() */
	tp->fld_setfn = defldset_anl;
	fld = *(tp->def);
	for(i=0;i<48;i++)
		fld[i].use_ext = TRUE;	 /* use/display the preload values */
			/* field names */
   vdefatsa (tp, 0, 0, 1, NORML, "Cable Depths Spec 1");
   vdefatsa (tp, 1, 1, 1, NORML, "Cable Depths Spec 2");
   vdefatsa (tp, 2, 2, 1, NORML, "Remote Unit Comms");
   vdefatsa (tp, 3, 3, 1, NORML, "Gun Timings Spec 1");
   vdefatsa (tp, 4, 4, 1, NORML, "Gun Timings Spec 2");
   vdefatsa (tp, 5, 5, 1, NORML, "Source Volume Spec 1");
   vdefatsa (tp, 6, 6, 1, NORML, "Source Volume Spec 2");
   vdefatsa (tp, 7, 7, 1, NORML, "Manifold Pressure 1");
   vdefatsa (tp, 8, 8, 1, NORML, "Manifold Pressure 2");
   vdefatsa (tp, 9, 9, 1, NORML, "Consecutive Misfires");
   vdefatsa (tp, 10, 10, 1, NORML, "System Timing");
   vdefatsa (tp, 11, 11, 1, NORML, "Misfire Series");
   vdefatsa (tp, 12, 12, 1, NORML, "Running Misfire %");
   vdefatsa (tp, 13, 13, 1, NORML, "Shotpoint Integer");
   vdefatsa (tp, 14, 14, 1, NORML, "File Number Integer");
   vdefatsa (tp, 15, 15, 1, NORML, "Ground Speed Limit");
   vdefatsa (tp, 16, 16, 1, NORML, "Gun Mask Checking");
   vdefatsa (tp, 17, 17, 1, NORML, "Firing Sequence Errors");
   vdefatsa (tp, 18, 18, 1, NORML, "Water Depth Errors");
   vdefatsa (tp, 19, 19, 1, NORML, "Gun Depth Errors");
   vdefatsa (tp, 20, 20, 1, NORML, "SP Timing Errors");
   vdefatsa (tp, 21, 21, 1, NORML, "Wing Angle Errors");
	vdefatsa (tp, 22, 22, 1, NORML, "Water Speed Limit");
	vdefatsa (tp, 23, 23, 1, NORML, "Analyse Current");
	vdefatsa (tp, 24, 24, 1, NORML, "Analyse Crab Angle");
	vdefatsa (tp, 25, 23, 33, NORML, "Ground\\Water");
	vdefatsa (tp, 26, 24, 33, NORML, "System\\Gyro");
			/* string fields */
   vdefstr (tp, 0, 0, 1, NULL, 28, 3, "!!!", a_str, YES);
   vdefstr (tp, 1, 0, 1, NULL, 41, 3, "!!!", b_str, YES);
   vdefstr (tp, 2, 1, 1, NULL, 28, 3, "!!!", c_str, YES);
   vdefstr (tp, 3, 1, 1, NULL, 41, 3, "!!!", d_str, YES);
   vdefstr (tp, 4, 2, 1, NULL, 28, 3, "!!!", e_str, YES);
   vdefstr (tp, 5, 2, 1, NULL, 41, 3, "!!!", f_str, YES);
   vdefstr (tp, 6, 3, 1, NULL, 28, 3, "!!!", g_str, YES);
   vdefstr (tp, 7, 3, 1, NULL, 41, 3, "!!!", h_str, YES);
   vdefstr (tp, 8, 4, 1, NULL, 28, 3, "!!!", i_str, YES);
   vdefstr (tp, 9, 4, 1, NULL, 41, 3, "!!!", j_str, YES);
   vdefstr (tp, 10, 5, 1, NULL, 28, 3, "!!!", k_str, YES);
   vdefstr (tp, 11, 5, 1, NULL, 41, 3, "!!!", l_str, YES);
   vdefstr (tp, 12, 6, 1, NULL, 28, 3, "!!!", m_str, YES);
   vdefstr (tp, 13, 6, 1, NULL, 41, 3, "!!!", n_str, YES);
   vdefstr (tp, 14, 7, 1, NULL, 28, 3, "!!!", o_str, YES);
   vdefstr (tp, 15, 7, 1, NULL, 41, 3, "!!!", p_str, YES);
   vdefstr (tp, 16, 8, 1, NULL, 28, 3, "!!!", q_str, YES);
   vdefstr (tp, 17, 8, 1, NULL, 41, 3, "!!!", r_str, YES);
   vdefstr (tp, 18, 9, 1, NULL, 28, 3, "!!!", s_str, YES);
   vdefstr (tp, 19, 9, 1, NULL, 41, 3, "!!!", t_str, YES);
   vdefstr (tp, 20, 10, 1, NULL, 28, 3, "!!!", u_str, YES);
   vdefstr (tp, 21, 10, 1, NULL, 41, 3, "!!!", v_str, YES);
   vdefstr (tp, 22, 11, 1, NULL, 28, 3, "!!!", w_str, YES);
   vdefstr (tp, 23, 11, 1, NULL, 41, 3, "!!!", x_str, YES);
   vdefstr (tp, 24, 12, 1, NULL, 28, 3, "!!!", y_str, YES);
   vdefstr (tp, 25, 12, 1, NULL, 41, 3, "!!!", z_str, YES);
   vdefstr (tp, 26, 13, 1, NULL, 28, 3, "!!!", aa_str, YES);
   vdefstr (tp, 27, 13, 1, NULL, 41, 3, "!!!", ab_str, YES);
   vdefstr (tp, 28, 14, 1, NULL, 28, 3, "!!!", ac_str, YES);
   vdefstr (tp, 29, 14, 1, NULL, 41, 3, "!!!", ad_str, YES);
   vdefstr (tp, 30, 15, 1, NULL, 28, 3, "!!!", ae_str, YES);
   vdefstr (tp, 31, 15, 1, NULL, 41, 3, "!!!", af_str, YES);
   vdefstr (tp, 32, 16, 1, NULL, 28, 3, "!!!", ag_str, YES);
   vdefstr (tp, 33, 16, 1, NULL, 41, 3, "!!!", ah_str, YES);
   vdefstr (tp, 34, 17, 1, NULL, 28, 3, "!!!", ai_str, YES);
   vdefstr (tp, 35, 17, 1, NULL, 41, 3, "!!!", aj_str, YES);
   vdefstr (tp, 36, 18, 1, NULL, 28, 3, "!!!", ak_str, YES);
   vdefstr (tp, 37, 18, 1, NULL, 41, 3, "!!!", al_str, YES);
   vdefstr (tp, 38, 19, 1, NULL, 28, 3, "!!!", am_str, YES);
   vdefstr (tp, 39, 19, 1, NULL, 41, 3, "!!!", an_str, YES);
   vdefstr (tp, 40, 20, 1, NULL, 28, 3, "!!!", ao_str, YES);
   vdefstr (tp, 41, 20, 1, NULL, 41, 3, "!!!", ap_str, YES);
   vdefstr (tp, 42, 21, 1, NULL, 28, 3, "!!!", aq_str, YES);
   vdefstr (tp, 43, 21, 1, NULL, 41, 3, "!!!", ar_str, YES);
	vdefstr (tp, 44, 22, 1, NULL, 28, 3, "!!!", as_str, YES);
	vdefstr (tp, 45, 22, 1, NULL, 41, 3, "!!!", at_str, YES);
	vdefstr (tp, 46, 23, 1, NULL, 28, 3, "!!!", au_str, YES); /* speeds */
	vdefstr (tp, 47, 24, 1, NULL, 28, 3, "!!!", av_str, YES); /* headings */
   visible (Wborder, YES, NO);
   visible (Wspec, YES, NO);
	visible (W1, YES, NO);
	if(!vread (tp, Wspec, YES)) {  /* only if DWSUCCESS (0) */
		andef->dep_anl1 = (strcmp(a_str, "YES")) ? 0 : 1;
		andef->ldep_anl1 = (strcmp(b_str, "YES")) ? 0 : 1;
		andef->dep_anl2 = (strcmp(c_str, "YES")) ? 0 : 1;
		andef->ldep_anl2 = (strcmp(d_str, "YES")) ? 0 : 1;
		andef->dep_com = (strcmp(e_str, "YES")) ? 0 : 1;
		andef->ldep_com = (strcmp(f_str, "YES")) ? 0 : 1;
		andef->gun_anl1 = (strcmp(g_str, "YES")) ? 0 : 1;
		andef->lgun_anl1 = (strcmp(h_str, "YES")) ? 0 : 1;
		andef->gun_anl2 = (strcmp(i_str, "YES")) ? 0 : 1;
		andef->lgun_anl2 = (strcmp(j_str, "YES")) ? 0 : 1;
		andef->vol_anl1 = (strcmp(k_str, "YES")) ? 0 : 1;
		andef->lvol_anl1 = (strcmp(l_str, "YES")) ? 0 : 1;
		andef->vol_anl2 = (strcmp(m_str, "YES")) ? 0 : 1;
		andef->lvol_anl2 = (strcmp(n_str, "YES")) ? 0 : 1;
		andef->man_anl1 = (strcmp(o_str, "YES")) ? 0 : 1;
		andef->lman_anl1 = (strcmp(p_str, "YES")) ? 0 : 1;
		andef->man_anl2 = (strcmp(q_str, "YES")) ? 0 : 1;
		andef->lman_anl2 = (strcmp(r_str, "YES")) ? 0 : 1;
		andef->mf1_anl = (strcmp(s_str, "YES")) ? 0 : 1;
		andef->lmf1_anl = (strcmp(t_str, "YES")) ? 0 : 1;
		andef->clk_anl = (strcmp(u_str, "YES")) ? 0 : 1;
		andef->lclk_anl = (strcmp(v_str, "YES")) ? 0 : 1;
		andef->mf3_anl = (strcmp(w_str, "YES")) ? 0 : 1;
		andef->lmf3_anl = (strcmp(x_str, "YES")) ? 0 : 1;
		andef->mf4_anl = (strcmp(y_str, "YES")) ? 0 : 1;
		andef->lmf4_anl = (strcmp(z_str, "YES")) ? 0 : 1;
		andef->sp_anl = (strcmp(aa_str, "YES")) ? 0 : 1;
		andef->lsp_anl = (strcmp(ab_str, "YES")) ? 0 : 1;
		andef->fn_anl = (strcmp(ac_str, "YES")) ? 0 : 1;
		andef->lfn_anl = (strcmp(ad_str, "YES")) ? 0 : 1;
		andef->gsp_anl = (strcmp(ae_str, "YES")) ? 0 : 1;
		andef->lgsp_anl = (strcmp(af_str, "YES")) ? 0 : 1;
		andef->gmsk_anl = (strcmp(ag_str, "YES")) ? 0 : 1;
		andef->lgmsk_anl = (strcmp(ah_str, "YES")) ? 0 : 1;
		andef->seq_anl = (strcmp(ai_str, "YES")) ? 0 : 1;
		andef->lseq_anl = (strcmp(aj_str, "YES")) ? 0 : 1;
		andef->wdp_anl = (strcmp(ak_str, "YES")) ? 0 : 1;
		andef->lwdp_anl = (strcmp(al_str, "YES")) ? 0 : 1;
		andef->gd_anl = (strcmp(am_str, "YES")) ? 0 : 1;
      andef->lgd_anl = (strcmp(an_str, "YES")) ? 0 : 1;
      andef->tim_anl = (strcmp(ao_str, "YES")) ? 0 : 1;
      andef->ltim_anl = (strcmp(ap_str, "YES")) ? 0 : 1;
      andef->wi_anl = (strcmp(aq_str, "YES")) ? 0 : 1;
      andef->lwi_anl = (strcmp(ar_str, "YES")) ? 0 : 1;
		andef->ws_anl = (strcmp(as_str, "YES")) ? 0 : 1;
		andef->lws_anl = (strcmp(at_str, "YES")) ? 0 : 1;
		andef->cu_anl = (strcmp(au_str, "YES")) ? 0 : 1;
		andef->cr_anl = (strcmp(av_str, "YES")) ? 0 : 1;
      vdelete(Wspec, NONE);
      vdelete(W1, NONE);
		vdelete(Wborder, NONE);
		USR_KEY1=0x00;   /* stop YES/NO toggling on another data entry */
      return(1); /* return value used in changing anals during stats run */
	}
	USR_KEY1=0x00; 	  /* stop YES/NO toggling on another data entry */
	vdelete(Wspec, NONE);
   vdelete(W1, NONE);
   vdelete(Wborder, NONE);
   return(0);
}


/**************************************************************
*	 field validation
**************************************************************/
defldval_anl (TRANSACTION *tp)
{
	FIELD *fld;
   fld=&((*(tp->def))[tp->cur_fld]);
      /* only GECO can do gun mask or firing sequence analysis
         at present - Western RDC, Prakla source flags etc are
         to be added at a later date */
	if( hd->contr && (
				 (tp->cur_fld == 32) || (tp->cur_fld == 34) ) ) {
      if(!strcmp(fld->fldbuf, "YES")) {
         StatsErr("12001");  /* Gmask & firseq on Geco only */
         return(FALSE);
      }
	}
		/* current comparisons only if both speeds are ON */
	if( (tp->cur_fld == 46) && (!hd->ws || !hd->gs) ) {
      if(!strcmp(fld->fldbuf, "YES")) {
			StatsErr("12002");  /* Current only if both speeds ON */
         return(FALSE);
      }
	}
		/* crab angles only if both headings are ON */
	if( (tp->cur_fld == 47) && (!hd->gy || !hd->syhd) ) {
      if(!strcmp(fld->fldbuf, "YES")) {
			StatsErr("12003");  /* Crab angle only if both headings ON	*/
         return(FALSE);
      }
	}
   return(TRUE);
}


/**************************************************************
*	 field set function
**************************************************************/
defldset_anl (TRANSACTION *tp)
{
	FIELD *field;
	field=&((*(tp->def))[tp->cur_fld]);
	USR_KEY1=0x20;	/* space bar */
	if(!strlen(field->dataptr))
		StepThrough(tp,Wspec);
	return(TRUE);
}


/*************************************************************
* cycle through YES/NO
**************************************************************/
StepThrough(TRANSACTION *tp, HWND Wspec)
{
	FIELD *fld;
	STUFF *stuff;
	unsigned index;
	fld=&((*(tp->def))[tp->cur_fld]);
	stuff=(STUFF *)tp->tmp;

      index = (strcmp(fld->fldbuf, "YES")) ? 1 : 0;
	strcpy(fld->fldbuf,answer[index]);
	strcpy(stuff[tp->cur_fld].image,answer[index]);
	curdrag(Wspec,OFF);
	vratputs(Wspec,fld->row,fld->f_col,FLD_SEL0,
		 fld->fldbuf);
	curdrag(Wspec,ON);
	lcurset(Wspec,fld->row,fld->f_col);
	return(0);
}


/*************************************************************
*   analysis help function
**************************************************************/
void anl_help()
{
   string_help("Analysis Settings");
}
