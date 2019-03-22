/***************************************************************
*   FINDSP2.C   called from FINDSP1.C - select SP range etc
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
#include <sftd.h>

#include <stats.h>         /* wcs */
#include <strun.h>
#include <extern.h>
#include <stxconst.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct HDR_DEF *hd;        /* header  list pointer */
extern struct PRO_DEF *pd;        /* project list pointer */
extern struct OPT_DEF  *opt;      /* pointer to options list */
extern struct EXT_DEF *ext;
extern char *units[];            /* pointer to units list */
extern char *data;               /* pointer to data buffer */
extern char *answer[];          /* YES/NO - + ? etc etc */
extern HWND Wwait;              /* wait window (StWait() & EnWait() */

			/* global declarations */
extern struct ST_SUMM *sum;    /* summary structure pointer */
HWND Wrun;                     /* display window */


/**************************************************************
*   search for the selected SPs - return (1) if succesfull
*   otherwise return (0)
***************************************************************/
find_spr (FILE *fi, int a)
{
	HWND W1;
	register int i = 9;   /* counter for leeol() clear part window */
	int temprec = 1, frec, lrec;	 /* record nos for SPs */
   int fflag = 0, lflag = 0;      /* set if SPs found */
   long temp_pos;                 /* temporary file position */
   long temp_sp;                  /* temporary SP number */
	if(!a) {
		vatputas (Wrun, 2, 13, REVERR, sum->ffsp_bf);
		vatputas (Wrun, 4, 13, REVERR, sum->flsp_bf);
      vratputs(Wrun, 1, 2, REVNORML, "Total Range");
	}
	for (i; i<=15; i++) {  /* clear bottom part of window after */
		lcurset(Wrun, i, 0);                 /* previous searches */
		leeol(Wrun);
	}
	W1 = vcreat (3, 31, NORML, NO);
	vlocate (W1, 13, 24);
	vframe (W1, EMPHNORML, FRDOUBLE);
   fseek (fi, (long)opt->hdr_fld, SEEK_SET);   /* set start byte */
	StWait();
   do {   /* START LOOKING FOR SP RANGE */
      temp_pos = ftell (fi);   /* get current file position */
         /* read current SP data */
      fread (data, sizeof (char), opt->spn_fld+hd->h_len , fi);
         /* move file pointer on if any additional header length */
      if(opt->add_hdr)
         fseek (fi, (long)opt->add_hdr, SEEK_CUR);   /* add bytes */
            /* read current SP - check number */
		if ((temp_sp = find_sp (hd->sp_ofst + opt->spn_fld,
										 hd->sp, data)) == sum->fsp) {
			fflag = 1;
			frec = temprec;
			sum->fsp_pos = temp_pos;
			if (hd->fn)
				sum->ffn = find_fn (hd->fn_ofst + opt->spn_fld, hd->fn, data);
			if(hd->jd)
				sum->fsp_date = find_jd (hd->jd_ofst + opt->spn_fld, hd->jd, data);
			if(hd->tm) {
				find_tm (hd->tm_ofst + opt->spn_fld, hd->tm, data);
				strcpy (sum->fsp_tim, ext->tim);
			if(hd->wa)
				sum->fsp_wat = find_wat(hd->wa_ofst+opt->spn_fld, hd->wa, data)
										 +opt->draft;
			if(hd->syhd)
				sum->fsp_hed = find_hed(hd->syhd_ofst+opt->spn_fld, hd->syhd, data);
			}
		}
		if ((temp_sp = find_sp (hd->sp_ofst + opt->spn_fld,
										  hd->sp, data)) == sum->lsp) {
			lflag = 1;
			lrec = temprec;
			sum->lsp_pos = temp_pos;
			if (hd->fn)
				sum->lfn = find_fn (hd->fn_ofst + opt->spn_fld, hd->fn, data);
			if(hd->jd)
				sum->lsp_date = find_jd (hd->jd_ofst + opt->spn_fld, hd->jd, data);
			if(hd->tm) {
				find_tm (hd->tm_ofst + opt->spn_fld, hd->tm, data);
				strcpy (sum->lsp_tim, ext->tim);
			if(hd->wa)
				sum->lsp_wat = find_wat(hd->wa_ofst+opt->spn_fld, hd->wa, data)
										+opt->draft;
			if(hd->syhd)
				sum->lsp_hed = find_hed(hd->syhd_ofst+opt->spn_fld, hd->syhd, data);
			}
		}
	temprec++;
	} while ((!feof(fi)) && ((!fflag) || (!lflag)));
	EnWait();
	if ((fflag == 1) && (lflag == 1)) {
		if (sum->lsp_pos > sum->fsp_pos) {
		sum->selln_tim = CompareFTime (sum->fsp_tim, sum->lsp_tim, 1);
		if (sum->lsp_date > sum->fsp_date)
			sum->selln_tim = ((sum->lsp_date - sum->fsp_date)*1440) + sum->selln_tim;
		sum->sel_sp = labs (sum->fsp - sum->lsp)
                          +opt->prod_cal-1;   /* calculation method */
      sum->act_sp = (sum->lsp_pos - sum->fsp_pos)/((long)(opt->spn_fld+hd->h_len)) +1L;
		sum->selln_len = ((float)sum->sel_sp)*(pd->sp_dst/1000);
		if(sum->selln_tim) sum->selav_spd = (1000*sum->selln_len)/(((float)sum->selln_tim)*60);  /* M/S */
		if (pd->sp_un == 6) sum->selav_spd = sum->selav_spd * MS_KNOTS;     /* KNOTS */
		if (sum->fsp != sum->lsp) {
			if (sum->fsp > sum->lsp) sum->sp_change = 6;
				else sum->sp_change = 7;
		} else sum->sp_change = 8;
			if (sum->ffn != sum->lfn) {
				if (sum->ffn > sum->lfn) sum->fn_change = 6;
					else sum->fn_change = 7;
		} else sum->fn_change = 8;
		vdelete (W1, NONE);
		return (1);
		} else {
			vatputf (W1, 0, 1, "Found SP %-6ld : Record %-5d", sum->fsp, frec);
			vatputf (W1, 1, 1, "Found SP %-6ld : Record %-5d", sum->lsp, lrec);
			vratputs(W1, 2, 0, REVERR, " LSP BEFORE FSP - SELECT AGAIN ");
			visible (W1, YES, NO);
			vgetkey(UpdateClockWindow);
			vdelete (W1, NONE);
			return (0);
		}
	} else {
		if (sum->lsp == sum->f_lsp) lrec--;
		if (sum->fsp == sum->f_lsp) frec--;
		if (fflag) vatputf (W1, 0, 1, "Found SP %-6ld : Record %-5d", sum->fsp, frec);
		else vratputf  (W1, 0, 1, REVERR, "Unable to Locate SP %-6ld", sum->fsp);
		if (lflag) vatputf (W1, 1, 1, "Found SP %-6ld : Record %-5d", sum->lsp, lrec);
		else vratputf  (W1, 1, 1, REVERR, "Unable to Locate SP %-6ld", sum->lsp);
		vatputf (W1, 2, 1, "Total Records on File : %-5d", temprec-=2);
		visible (W1, YES, NO);
		vgetkey(UpdateClockWindow);
		vdelete (W1, NONE);
		return (0);
	}
}


/**************************************************************
*   view the selected SPs - as another window added
*   below the original file limits (f_fsp - f_lsp)
*   uses global window handle Wrun
***************************************************************/
void view_spr (void)
{
   vratputs(Wrun, 1, 2, NORML, "Total Range");
	vatputas (Wrun, 2, 13, NORML, sum->ffsp_bf);
	vatputas (Wrun, 4, 13, NORML, sum->flsp_bf);
   vratputs(Wrun, 9, 2, REVNORML, "Selected Range");
	vatputf(Wrun, 10, 2, "Process FSP %-6ld", sum->fsp);
	if(hd->fn)
	vatputf(Wrun, 10, 22, "FFN %-6ld", sum->ffn);
	ltoa (sum->fsp, sum->fsp_bf, 10);
	vatputas (Wrun, 10, 14, REVERR, sum->fsp_bf);
	if (hd->jd)
		vatputf(Wrun, 10, 33, "JD %3.3d", sum->fsp_date);
	if (hd->tm)
		vatputf(Wrun, 10, 43, "Time %s", sum->fsp_tim);
	if(hd->wa)
		vatputf(Wrun, 11, 2, "Water Depth %-1.1f %-8s", sum->fsp_wat,
																units[pd->loc_dp]);
	if(hd->syhd)
		vatputf(Wrun, 11, 30, "SysHead %05.1f Degrees", sum->fsp_hed);
	vatputf(Wrun, 12, 2, "Process LSP %-6ld", sum->lsp);
	if(hd->fn)
	vatputf(Wrun, 12, 22, "LFN %-6ld", sum->lfn);
	ltoa (sum->lsp, sum->lsp_bf, 10);
	vatputas (Wrun, 12, 14, REVERR, sum->lsp_bf);
	if (hd->jd)
		vatputf(Wrun, 12, 33, "JD %3.3d", sum->lsp_date);
	if (hd->tm)
		vatputf(Wrun, 12, 43, "Time %s", sum->lsp_tim);
	if(hd->wa)
		vatputf(Wrun, 13, 2, "Water Depth %-1.1f %-8s", sum->lsp_wat,
																units[pd->loc_dp]);
	if(hd->syhd)
		vatputf(Wrun, 13, 30, "SysHead %05.1f Degrees", sum->lsp_hed);
	if(hd->tm)
		vatputf(Wrun, 14, 2, "Line Minutes %-4d", sum->selln_tim);
	vatputf(Wrun, 14, 22, "Total SPs %-6ld  Kms %-8.3f",
											  sum->sel_sp, sum->selln_len);
	if(hd->tm)
		vatputf(Wrun, 15, 2, "Average Speed %-5.2f %-7s",
											 sum->selav_spd, units[pd->sp_un]);
	vatputf(Wrun, 15, 30, "Change Flag: SPs %01s", answer[sum->sp_change]);
	if(hd->fn)
		vatputf(Wrun, 15, 50, "FNs %01s", answer[sum->fn_change]);
	return;
}
