/***************************************************************
*    RUNUTIL2.C    various run utilities
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
#include <dwdata.h>
#include <sftd.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <strun.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct ANAL_DEF *andef;   /* pointer to set/analysis list */
extern struct HDR_DEF *hd;       /* header  list pointer */
extern struct PRO_DEF *pd;       /* project list pointer */
extern struct OPT_DEF  *opt;     /* pointer to options list */
extern struct ST_SUMM *sum;      /* summary structure pointer */
extern struct EXT_DEF *ext;
extern char *data;                /* pointer to data buffer */


/***************************************************************
*   select a new SP range for processing
****************************************************************/
select_sp ()
{
	HWND W1;
	TRANSACTION *tp;
	FIELD *fld;
	W1 = vcreat (3, 27, NORML, NO);
	vlocate (W1, 20, 25);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " Select SP Range ");
      /*# defldval_sp() EscapeKey() */
	tp = vpreload (2, 1, NULLF, defldval_sp, NULLF, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
	fld = *(tp->def);
	fld[0].use_ext = TRUE;
	fld[1].use_ext = TRUE;
	vdeflong (tp, 0, 0, 1, "FSP To Process", 20, 6, "999999", &sum->fsp, YES);
	vdeflong (tp, 1, 1, 1, "LSP To Process", 20, 6, "999999", &sum->lsp, YES);
	vdefatsa (tp, 0, 2, 2, HIGHNORML, "<Ctrl-Enter> To Accept");
	visible (W1, YES, NO);
	vread (tp, W1, YES);
	vdelete (W1, NONE);
	return (0);
}


/*************************************************************
* field validation
**************************************************************/
defldval_sp ()
{
	if (sum->fsp == sum->lsp) return (FALSE);
	else return (TRUE);
}


/**************************************************************
*   print a range of SP/FN/TIME sets
**************************************************************/
void pr_spfn (void)    /* make a file of SP/FN/Time Range */
{
   HWND W1;                 /* selection window */
   FILE *fi=NULL;           /* file to analyse */
	FILE *flog = NULL;       /* handle for .SPN file */
	long sp, fn;
	int rr;                  /* keycode selections */
	int errnum;              /* for strerror() */
   char prog_buf[65];       /* .DAT path/file buffer */
      /* look for a data file */
   if(find_file(1, "*.DAT", opt->work_dr, opt->dat_dir) != 1) {
		return;
   }
   if(check_drive(opt->work_dr, opt->txt_dir))  /* no txt dr/dir */
      return;
	if (!(data = (char *)malloc(hd->h_len +opt->spn_fld +1))) {
         StatsErr("50002");
			return;
	}
	if (!(sum = (struct ST_SUMM *)malloc(sizeof(struct ST_SUMM)))) {
         StatsErr("50003");
			if(data) free (data);
			return;
	}
	sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr - 1, opt->dat_dir);
	if(strlen(opt->dat_dir)>1) strcat(prog_buf, "\\");
	strcat(prog_buf, ext->pname);
	if ((fi = fopen (prog_buf, "rb")) == NULL) {
		errnum = errno;
		StatsDosError (ext->pname, errnum);
		if(sum) free(sum);
		if(data) free (data);
		return;
	}
			/* read comment field after initialising it */
	memset (sum->comment, '\0', opt->com_fld +1);
	fseek (fi, (long)opt->com_ofst, SEEK_SET);   /* go to comment offset */
	if (fread (sum->comment, sizeof (char), opt->com_fld, fi) != opt->com_fld) {
      StatsErr ("50004");
		fclose(fi);
		if(sum) free(sum);
		if(data) free(data);
		return;
	}
         /* LOOK FOR FIRST FILE RECORD DETAILS */
	fseek (fi, (long)opt->hdr_fld, SEEK_SET);
	sum->fsp_pos = sum->ffsp_pos = ftell (fi);
	if (fread (data, sizeof (char), opt->spn_fld+hd->h_len , fi)
											  != opt->spn_fld + hd->h_len) {
      StatsErr("50005");
		fclose(fi);
		if(sum) free(sum);
		if(data) free(data);
		return;
	}
	if(hd->sp)
	sum->fsp = sum->f_fsp = find_sp (hd->sp_ofst + opt->spn_fld, hd->sp, data);
	if(hd->fn)
   sum->ffn = sum->f_ffn = find_fn (hd->fn_ofst + opt->spn_fld, hd->fn, data);
      /* LOOK FOR FILE LAST RECORD DETAILS */
   fseek (fi, -(long)(hd->h_len + opt->spn_fld + opt->add_hdr), SEEK_END);
	sum->lsp_pos = sum->flsp_pos = ftell (fi);
	fread(data, sizeof (char), opt->spn_fld+hd->h_len, fi);
	if(hd->sp)
	sum->lsp = sum->f_lsp = find_sp (hd->sp_ofst + opt->spn_fld, hd->sp, data);
	if (hd->fn)
	sum->lfn = sum->f_lfn = find_fn (hd->fn_ofst + opt->spn_fld, hd->fn, data);
         /* format to screen */
   W1 = vcreat(8, 58, NORML, NO);
	vlocate(W1, 14, 10);
   vframe (W1, EMPHNORML, FRDOUBLE);
   strcpy(prog_buf, " List: SP");
   if(hd->fn) strcat(prog_buf, "/FN");
   if(hd->tm) strcat(prog_buf, "/TIME");
   if( hd->gm && andef->gmsk_anl ) strcat(prog_buf, "/SOURCE");
   strcat(prog_buf, " ");
   vtitle (W1, EMPHNORML, prog_buf);
   vatputf(W1, 0, 2, "Data File: %-18s ID: %-21s", ext->pname, sum->comment);
   vratputs(W1, 1, 10, REVNORML, "Range");
	vatputf(W1, 2, 2, "File  FSP  %-6ld", sum->f_fsp);
	if(hd->fn)
		vatputf(W1, 2, 22, "FFN %-6ld", sum->f_ffn);
	ltoa (sum->f_fsp, sum->ffsp_bf, 10);
	vatputas (W1, 2, 13, REVERR, sum->ffsp_bf);
	vatputf(W1, 3, 2, "File  LSP  %-6ld", sum->f_lsp);
	if(hd->fn)
	vatputf(W1, 3, 22, "LFN %-6ld", sum->f_lfn);
	ltoa (sum->f_lsp, sum->flsp_bf, 10);
	vatputas (W1, 3, 13, REVERR, sum->flsp_bf);
	pcuroff();
	vratputs(W1, 7, 0, REVNORML,
            "   <CR> Make List    F8 Select SP Range    <ESC> To Exit   ");
	visible (W1, YES, NO);
	do {
		rr = vgetkey(UpdateClockWindow);
      switch (rr) {
         case F1:
            spfn_help();
            break;
         case F8:      /* select SP range to analyse */
            lcurset(W1, 5, 0);     /* previous searches */
            leeol(W1);
            select_sp();
            if ((sum->fsp != sum->f_fsp) || (sum->lsp != sum->f_lsp)) {
               if (find_spr(fi, 1 )) {
                  vratputf(W1, 5, 11, REVNORML,
                     "PROCESS SP RANGE:  %6ld  -  %-ld", sum->fsp, sum->lsp);
                  vratputs(W1, 1, 10, NORML, "Range");
                  vatputas (W1, 2, 13, NORML, sum->ffsp_bf);
                  vatputas (W1, 3, 13, NORML, sum->flsp_bf);
               } else {
                  sum->fsp = sum->f_fsp;
                  sum->lsp = sum->f_lsp;
                  sum->fsp_pos = sum->ffsp_pos;
                  sum->lsp_pos = sum->flsp_pos;
               }
            }
            break;
         default:
            break;
      }
   } while ((rr != ESC) && (rr != CR));
	if (rr == CR) {
		vdelete(W1, NONE);
      flog = spn_file();
      fseek (fi, sum->fsp_pos, SEEK_SET);
      W1 = vcreat(2, 20, NORML, NO);
      vwind (W1, 2, 20, 0, 0);
      vlocate(W1, 18, 21);
      sprintf(prog_buf, " %s ", ext->pname);
      vtitle (W1, EMPHNORML, prog_buf);
      vframe (W1, EMPHNORML, FRDOUBLE);
      visible (W1, YES, NO);
      vratputs(W1, 1, 4, REVNORML, "<ESC> Aborts");
      vatputs(W1, 0, 1, "Listing SP:");
      do {
         rr = 0;     /* check for ESCAPE to abort */
         if(gfkbhit()) rr = vgetkey(UpdateClockWindow); /* if keystroke */
         if(rr == ESC) if (KeySure()) break;
                      /* master read function */
         if((opt->spn_fld+hd->h_len)!=fread (data, sizeof (char),
                                   opt->spn_fld+hd->h_len , fi)) {
            StatsErr("50006");
            if(flog) fclose(flog);
            if(sum) free(sum);
            if(data) free(data);
            return;
         }
            /* move file pointer on if any additional header length */
         if(opt->add_hdr)
            fseek (fi, (long)opt->add_hdr, SEEK_CUR);   /* add bytes */
         if(hd->sp)
            sp = find_sp (hd->sp_ofst + opt->spn_fld, hd->sp, data);
         vratputf(W1, 0, 13, REVERR, "%ld", sp);
         if (hd->fn)
            fn = find_fn (hd->fn_ofst + opt->spn_fld, hd->fn, data);
         if(hd->tm)
            find_tm ((hd->tm_ofst + opt->spn_fld), hd->tm, data);
         if((hd->sp)&&(hd->fn)&&(hd->tm))
                  fprintf(flog, "SP %-6ld  FN %-6ld  %s", sp, fn, ext->tim);
         else if ((hd->sp)&&(hd->tm))
                  fprintf(flog, "SP %-6ld  %s", sp, ext->tim);
         else if ((hd->sp)&&(hd->fn))
                  fprintf(flog, "SP %-6ld  FN %-6ld", sp, fn);
         else fprintf(flog, "SP %-6ld", sp);
         if(andef->gmsk_anl)   /* do gun mask reporting */
            proc_gmsk((HWND)NULL, (long)NULL, (char *)NULL, (long)NULL, (FILE *)flog, 1);
         fwrite("\n", sizeof (char), 1, flog);
      } while (ftell(fi) <= sum->lsp_pos);
      vdelete(W1, NONE);
      fwrite("\n", sizeof (char), 1, flog);
      if(flog) fclose(flog);
      if(sum) free(sum);
      if(data) free(data);
      return;
	}  else {
		vdelete(W1, NONE);
	}
	fclose(fi);
	if(sum) free(sum);
	if(data) free(data);
	return;
}


/*************************************************************
*   SP/FN list help function
**************************************************************/
void spfn_help()
{
   string_help("Listing SP/FN");
}
