/*************************************************************
*   HEADCFG.C   header configuration
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
#include <string.h>

#include <dw.h>            /* greenleaf */
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct HDR_DEF *hd;   /* header list pointer */
extern struct EXT_DEF *ext;
extern char *formats[];      /* format definitions */


/**************************************************************
*	 select/change  header configuration
**************************************************************/
void ch_head()
{
	HWND W1, W2;				  /* data entry/message windows */
	TRANSACTION *tp;			  /* transaction structure */
	FIELD *fld; 				  /* field structure */
   unsigned i=0;
   char sp_fmt[21], fn_fmt[21];      /* temp formats */
	char jd_fmt[21], tm_fmt[21];
	char ln_fmt[21], ar_fmt[21];
	char cl_fmt[21], vs_fmt[21];
	char ca_fmt[21], gu_fmt[21];
	char gm_fmt[21], ma_fmt[21];
	char gs_fmt[21], wa_fmt[21];
	char gd_fmt[21], syhd_fmt[21];
	char clk_fmt[21], wi_fmt[21];
	char ws_fmt[21], gy_fmt[21];
         /* check for corrupt format numbers */
   while (formats[i]) i++;   /* get the max format number */
   if(hd->sp < i)
      ext->a_num = hd->sp;   /* set to 0 unless within range */
   else ext->a_num = hd->sp = 0;  /* otherwise invalid pointers to lists */
   if(hd->fn < i)
      ext->b_num = hd->fn;
   else ext->b_num = hd->fn = 0;
   if(hd->jd < i)
      ext->c_num = hd->jd;
   else ext->c_num = hd->jd = 0;
   if(hd->tm < i)
      ext->d_num = hd->tm;
   else ext->d_num = hd->tm = 0;
   if(hd->ln < i)
      ext->e_num = hd->ln;
   else ext->e_num = hd->ln = 0;
   if(hd->ar < i)
      ext->f_num = hd->ar;
   else ext->f_num = hd->ar = 0;
   if(hd->cl < i)
      ext->g_num = hd->cl;
   else ext->g_num = hd->cl = 0;
   if(hd->vs < i)
      ext->h_num = hd->vs;
   else ext->h_num = hd->vs = 0;
   if(hd->ca < i)
      ext->i_num = hd->ca;
   else ext->i_num = hd->ca = 0;
   if(hd->gu < i)
      ext->j_num = hd->gu;
   else ext->j_num = hd->gu = 0;
   if(hd->gm < i)
      ext->k_num = hd->gm;
   else ext->k_num = hd->gm = 0;
   if(hd->ma < i)
      ext->l_num = hd->ma;
   else ext->l_num = hd->ma = 0;
   if(hd->gs < i)
      ext->m_num = hd->gs;
   else ext->m_num = hd->gs = 0;
   if(hd->wa < i)
      ext->n_num = hd->wa;
   else ext->n_num = hd->wa = 0;
   if(hd->gd < i)
      ext->o_num = hd->gd;
   else ext->o_num = hd->gd = 0;
   if(hd->syhd < i)
      ext->p_num = hd->syhd;
   else ext->p_num = hd->syhd = 0;
   if(hd->clk < i)
      ext->q_num = hd->clk;
   else ext->q_num = hd->clk = 0;
   if(hd->wi < i)
      ext->r_num = hd->wi;
	else ext->r_num = hd->wi = 0;
	if(hd->ws < i)
		ext->s_num = hd->ws;
	else ext->s_num = hd->ws = 0;
	if(hd->gy < i)
		ext->t_num = hd->gy;
	else ext->t_num = hd->gy = 0;
   strcpy(sp_fmt, formats[hd->sp]);  /* copy names across */
	strcpy(fn_fmt, formats[hd->fn]);
	strcpy(jd_fmt, formats[hd->jd]);
	strcpy(tm_fmt, formats[hd->tm]);
	strcpy(ln_fmt, formats[hd->ln]);
	strcpy(ar_fmt, formats[hd->ar]);
	strcpy(cl_fmt, formats[hd->cl]);
	strcpy(vs_fmt, formats[hd->vs]);
	strcpy(ca_fmt, formats[hd->ca]);
	strcpy(gu_fmt, formats[hd->gu]);
	strcpy(gm_fmt, formats[hd->gm]);
	strcpy(ma_fmt, formats[hd->ma]);
	strcpy(gs_fmt, formats[hd->gs]);
	strcpy(wa_fmt, formats[hd->wa]);
	strcpy(gd_fmt, formats[hd->gd]);
	strcpy(syhd_fmt, formats[hd->syhd]);
   strcpy(clk_fmt, formats[hd->clk]);
	strcpy(wi_fmt, formats[hd->wi]);
	strcpy(ws_fmt, formats[hd->ws]);
	strcpy(gy_fmt, formats[hd->gy]);
      /* make the data entry window */
	W1 = vcreat (23, 50, NORML, NO);
	vwind (W1, 21, 50, 0, 0);
	vlocate (W1, 2, 2);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " Geco Headers ");
      /* make the bottom line message window */
	W2 = vcreat (1, 80, REVNORML, NO);
	vlocate (W2, 24, 0);
	vratputs (W2, 0, 1, REVNORML, "<ESC> Exit");
	vratputs (W2, 0, 36, REVERR, "  F1  ");
   vratputs (W2, 0, 60, REVEMPHNORML, "<CTRL-ENTER> Accept");
         /* set up the transaction */
      /*# defldval_hdr() detranval_glob() EscapeKey() head_help_gec() */
	tp = vpreload (45, 2, NULLF, defldval_hdr, detranval_glob, NULLF);
   tp->cancfn=EscapeKey;
   tp->helpfn = head_help_gec;   /* param warning can be ignored */
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
	fld = *(tp->def);
	for(i=0;i<45;i++)
		fld[i].use_ext = TRUE;	 /* use/display the preload value */
	vdefatsa (tp, 0, 1, 16, NORML, "Offset   Units      Format");
	vdefatsa (tp, 1, 0, 24, NORML, "Bytes");
      /* data fields */
	vdefuint (tp, 0, 0, 1, "Header Length", 18, 4, "9999", &hd->h_len, YES);
	vdefuint (tp, 1, 2, 1, "Shotpoint", 18, 4, "9999", &hd->sp_ofst, YES);
	vdefstr	(tp, 2, 2, 34, NULL, 36, 20, NULL, sp_fmt, YES);
	vdefuint (tp, 3, 3, 1, "File Number", 18, 4, "9999", &hd->fn_ofst, YES);
	vdefstr	(tp, 4, 3, 34, NULL, 36, 20, NULL, fn_fmt, YES);
	vdefuint (tp, 5, 4, 1, "Julian Date", 18, 4, "9999", &hd->jd_ofst, YES);
	vdefstr	(tp, 6, 4, 34, NULL, 36, 20, NULL, jd_fmt, YES);
	vdefuint (tp, 7, 5, 1, "Time", 18, 4, "9999", &hd->tm_ofst, YES);
	vdefstr	(tp, 8, 5, 34, NULL, 36, 20, NULL, tm_fmt, YES);
	vdefuint (tp, 9, 6, 1, "Line Name", 18, 4, "9999", &hd->ln_ofst, YES);
	vdefstr	(tp, 10, 6, 34, NULL, 36, 20, NULL, ln_fmt, YES);
	vdefuint (tp, 11, 7, 1, "Area", 18, 4, "9999", &hd->ar_ofst, YES);
	vdefstr	(tp, 12, 7, 34, NULL, 36, 20, NULL, ar_fmt, YES);
	vdefuint (tp, 13, 8, 1, "Client", 18, 4, "9999", &hd->cl_ofst, YES);
	vdefstr	(tp, 14, 8, 34, NULL, 36, 20, NULL, cl_fmt, YES);
	vdefuint (tp, 15, 9, 1, "Vessel", 18, 4, "9999", &hd->vs_ofst, YES);
	vdefstr	(tp, 16, 9, 34, NULL, 36, 20, NULL, vs_fmt, YES);
	vdefuint (tp, 17, 10, 1, "Cable Depths", 18, 4, "9999", &hd->ca_ofst, YES);
	vdefuint (tp, 18, 10, 24, NULL, 26, 3, "999", &hd->ca_num, YES);
	vdefstr	(tp, 19, 10, 34, NULL, 36, 20, NULL, ca_fmt, YES);
	vdefuint (tp, 20, 11, 1, "Gun Controller", 18, 4, "9999", &hd->gu_ofst, YES);
	vdefuint (tp, 21, 11, 24, NULL, 26, 3, "999", &hd->gu_num, YES);
	vdefstr	(tp, 22, 11, 34, NULL, 36, 20, NULL, gu_fmt, YES);
	vdefuint (tp, 23, 12, 1, "Remote Guns", 18, 4, "9999", &hd->gm_ofst, YES);
	vdefstr	(tp, 24, 12, 34, NULL, 36, 20, NULL, gm_fmt, YES);
	vdefuint (tp, 25, 13, 1, "Manifold Air", 18, 4, "9999", &hd->ma_ofst, YES);
	vdefstr	(tp, 26, 13, 34, NULL, 36, 20, NULL, ma_fmt, YES);
	vdefuint (tp, 27, 14, 1, "Ground Speed", 18, 4, "9999", &hd->gs_ofst, YES);
	vdefstr	(tp, 28, 14, 34, NULL, 36, 20, NULL, gs_fmt, YES);
	vdefuint (tp, 29, 15, 1, "Water Depth", 18, 4, "9999", &hd->wa_ofst, YES);
	vdefstr	(tp, 30, 15, 34, NULL, 36, 20, NULL, wa_fmt, YES);
	vdefuint (tp, 31, 16, 1, "Gun Depths", 18, 4, "9999", &hd->gd_ofst, YES);
	vdefuint (tp, 32, 16, 24, NULL, 26, 3, "999", &hd->gd_num, YES);
	vdefstr	(tp, 33, 16, 34, NULL, 36, 20, NULL, gd_fmt, YES);
	vdefuint (tp, 34, 17, 1, "System Heading", 18, 4, "9999", &hd->syhd_ofst, YES);
	vdefstr	(tp, 35, 17, 34, NULL, 36, 20, NULL, syhd_fmt, YES);
	vdefuint (tp, 36, 18, 1, "System Timing", 18, 4, "9999", &hd->clk_ofst, YES);
   vdefstr  (tp, 37, 18, 34, NULL, 36, 20, NULL, clk_fmt, YES);
   vdefuint (tp, 38, 19, 1, "Wing Angles", 18, 4, "9999", &hd->wi_ofst, YES);
   vdefuint (tp, 39, 19, 24, NULL, 26, 3, "999", &hd->wi_num, YES);
	vdefstr	(tp, 40, 19, 34, NULL, 36, 20, NULL, wi_fmt, YES);
	vdefuint (tp, 41, 20, 1, "Water Speed", 18, 4, "9999", &hd->ws_ofst, YES);
	vdefstr	(tp, 42, 20, 34, NULL, 36, 20, NULL, ws_fmt, YES);
	vdefuint (tp, 43, 21, 1, "Gyro Heading", 18, 4, "9999", &hd->gy_ofst, YES);
	vdefstr	(tp, 44, 21, 34, NULL, 36, 20, NULL, gy_fmt, YES);
		/*# defldset_hdr() */
	deSetFieldSetfn(tp, defldset_hdr);
   visible (W1, YES, NO);
   visible (W2, YES, NO);
	if(!vread (tp, W1, YES)) { /* only if DWSUCCESS (0) */
		hd->sp = ext->a_num;
		hd->fn = ext->b_num;
		hd->jd = ext->c_num;
		hd->tm = ext->d_num;
		hd->ln = ext->e_num;
		hd->ar = ext->f_num;
		hd->cl = ext->g_num;
		hd->vs = ext->h_num;
		hd->ca = ext->i_num;
		hd->gu = ext->j_num;
		hd->gm = ext->k_num;
		hd->ma = ext->l_num;
		hd->gs = ext->m_num;
		hd->wa = ext->n_num;
		hd->gd = ext->o_num;
		hd->syhd = ext->p_num;
      hd->clk = ext->q_num;
		hd->wi = ext->r_num;
		hd->ws = ext->s_num;
		hd->gy = ext->t_num;
	}
	vdelete(W1, NONE);
	vdelete(W2, NONE);
	return;
}


/*************************************************************
*   general header help function
**************************************************************/
void head_help()
{
   string_help("On Setting Formats");
}


/**************************************************************
*   set up the fixed menu selection for the formats field
**************************************************************/
defldset_hdr (TRANSACTION *tp)
{
	register int sel;							 /* format number */
	unsigned i;
	ext->ee = ext->ff = TRUE;            /* set the bool flags */
	if (((tp->cur_fld == 2)||(tp->cur_fld == 4)||
		  (tp->cur_fld == 6)||(tp->cur_fld == 8)||
		  (tp->cur_fld == 10)||(tp->cur_fld == 12)||
		  (tp->cur_fld == 14)||(tp->cur_fld == 16)||
		  (tp->cur_fld == 19)||(tp->cur_fld == 22)||
		  (tp->cur_fld == 24)||(tp->cur_fld == 26)||
		  (tp->cur_fld == 28)||(tp->cur_fld == 30)||
		  (tp->cur_fld == 33)||(tp->cur_fld == 35)||
		  (tp->cur_fld == 37)||(tp->cur_fld == 40)||
		  (tp->cur_fld == 42)||(tp->cur_fld == 44))
         && (tp->thiskey != ESC)) {  /* if at field */
		if(tp->cur_fld == 2) i = ext->a_num;		/* pass to select */
		if(tp->cur_fld == 4) i = ext->b_num;
		if(tp->cur_fld == 6) i = ext->c_num;
		if(tp->cur_fld == 8) i = ext->d_num;
		if(tp->cur_fld == 10) i = ext->e_num;
		if(tp->cur_fld == 12) i = ext->f_num;
		if(tp->cur_fld == 14) i = ext->g_num;
		if(tp->cur_fld == 16) i = ext->h_num;
		if(tp->cur_fld == 19) i = ext->i_num;
		if(tp->cur_fld == 22) i = ext->j_num;
		if(tp->cur_fld == 24) i = ext->k_num;
		if(tp->cur_fld == 26) i = ext->l_num;
		if(tp->cur_fld == 28) i = ext->m_num;
		if(tp->cur_fld == 30) i = ext->n_num;
		if(tp->cur_fld == 33) i = ext->o_num;
		if(tp->cur_fld == 35) i = ext->p_num;
      if(tp->cur_fld == 37) i = ext->q_num;
		if(tp->cur_fld == 40) i = ext->r_num;
		if(tp->cur_fld == 42) i = ext->s_num;
		if(tp->cur_fld == 44) i = ext->t_num;
		if((sel = select_fmt(i)) != -1) {	/* get a format */
			if(tp->cur_fld == 2) ext->a_num = sel;	/* temporary format number */
			if(tp->cur_fld == 4) ext->b_num = sel;
			if(tp->cur_fld == 6) ext->c_num = sel;
			if(tp->cur_fld == 8) ext->d_num = sel;
			if(tp->cur_fld == 10) ext->e_num = sel;
			if(tp->cur_fld == 12) ext->f_num = sel;
			if(tp->cur_fld == 14) ext->g_num = sel;
			if(tp->cur_fld == 16) ext->h_num = sel;
			if(tp->cur_fld == 19) ext->i_num = sel;
			if(tp->cur_fld == 22) ext->j_num = sel;
			if(tp->cur_fld == 24) ext->k_num = sel;
			if(tp->cur_fld == 26) ext->l_num = sel;
			if(tp->cur_fld == 28) ext->m_num = sel;
			if(tp->cur_fld == 30) ext->n_num = sel;
			if(tp->cur_fld == 33) ext->o_num = sel;
			if(tp->cur_fld == 35) ext->p_num = sel;
         if(tp->cur_fld == 37) ext->q_num = sel;
         if(tp->cur_fld == 40) ext->r_num = sel;
			if(tp->cur_fld == 42) ext->s_num = sel;
			if(tp->cur_fld == 44) ext->t_num = sel;
			tp->inp_get = KeySim;						/* do the actual keysim */
			ext->s_input = formats[sel];
		} else { 											/* do only nextfield jump */
			ext->ee = FALSE;								/* see KeySim() function */
			tp->inp_get = KeySim;
		}
	}
	return (TRUE);
}


/**************************************************************
*   field validation function  - NOT USEFUL YET
**************************************************************/
defldval_hdr ()
{
	return (TRUE);
}


/**************************************************************
*   offer up a formats list to select
**************************************************************/
select_fmt(unsigned n)
{
	LISTITEM *l;
	LBOXSTA *b;
	register int i = 0;
	unsigned key;
	char *s;
	l = initlist();
	while (formats[i])
		adtolist (l, formats[i++]);
	b = WCS_lboxinit (3, 58, 19, NORML, REVNORML, NULL, EMPHNORML,
						 FRDOUBLE, EMPHNORML, l);	 /* WCS clone of DW function */
	for(i=0; i<(int)n; i++) lboxctrl(b, CURDN);
	visible(b->lb, YES, NO);
	while((key=vgetkey(UpdateClockWindow)) != ESC && key != 0x0d)
		lboxctrl(b, key);
	if(key == ESC) {
		lboxdel(b);
		return(-1);
	}
	s = lboxsel(b);
	lboxdel(b);
	i = 0;
	while (formats[i]) {
	  if(!strcmp(formats[i], s)) return (i);
	  i++;
	}
	return (-1);
}


/**************************************************************
*   save header definition to disk
**************************************************************/
void save_hdr ()
{
	FILE *fp;
	if (!KeySure()) return;
	fp = fopen("HEADER.CFG", "wb");
	fwrite(hd, sizeof(struct HDR_DEF), 1, fp);
	fclose(fp);
   StatsMessage ("SAVED HEADER.CFG");
}


/**************************************************************
*   load header definition from disk
**************************************************************/
load_hdr (int i)
{
   FILE *fp;
   if(i)
      if (!KeySure())
         return(0);
	if((fp = fopen("HEADER.CFG", "rb")) == NULL) {
	  if(i) StatsErr("22001"); 			  /* not found */
	  return(0);
	}
	if (1 != fread(hd, sizeof(struct HDR_DEF), 1, fp))
		StatsErr("22002");
   fclose(fp);
   if(i) StatsMessage("LOADED HEADER.CFG");
	return(1);
}
