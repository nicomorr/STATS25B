/***************************************************************
*   FINDSP1.C   main runtime selection engine
****************************************************************
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
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <strun.h>
#include <extern.h>
#include <stxfunct.h>	/* function prototypes */
#include <stxconst.h>	/* constant definitions */

			/* global declarations */
struct ST_SUMM *sum = NULL;       /* summary structure pointer */
HWND Wrun;                        /* display window */
char ln_name[17], cl_name[17],    /* line & client names */
     vs_name[17], ar_name[17];    /* vessel & area names */

			/* external global declarations */
extern struct HDR_DEF *hd;        /* header  list pointer */
extern struct PRO_DEF *pd;        /* project list pointer */
extern struct OPT_DEF  *opt;      /* pointer to options list */
extern struct EXT_DEF *ext;       /* general variables */
extern char *units[];             /* pointer to units list */
extern char *data;                /* pointer to data buffer */
extern char *answer[];            /* YES/NO - + ? etc etc */
extern HWND Wwait;                /* wait window (StWait() & EnWait() */


/*************************************************************
*   select a data file - set up runtime summary
**************************************************************/
runfind (int i)
{
	FILE *fi;                /* file to analyse */
	int rr;						 /* keycode selections */
   char prog_buf[65];       /* .DAT path/file buffer */
         /* look for a file - then allocate buffers */
	data = NULL;
	sum = NULL;
	if(i) {
      if(find_file(1, "*.DAT", opt->work_dr, opt->dat_dir) != 1)
			return (1);
	} else {
		ext->pname[9] = 'D';
		ext->pname[10] = 'A';
		ext->pname[11] = 'T';
	}
	if (!(data = (char *)malloc(hd->h_len +opt->spn_fld +1))) {
			StatsErr("10002");
			return(2);
	}
	if (!(sum = (struct ST_SUMM *)malloc(sizeof(struct ST_SUMM)))) {
			StatsErr("10003");
			if(data) free (data);
			return(3);
	}
	sum->way_fl = 0;   /* initialise waypoint flag (0 if no waypoint) */
	sum->way_sp = 0L;
	sum->ffsp_wat = 0.0F;
	sum->flsp_wat = 0.0F;     /* water depths */
	sum->fsp_wat = 0.0F;
	sum->lsp_wat = 0.0F;      /* ditto */
	sum->ffsp_hed = 0.0F;
	sum->flsp_hed = 0.0F;     /* system headings */
	sum->fsp_hed = 0.0F;
	sum->lsp_hed = 0.0F;      /* headings */
	sum->ffsp_date = 0;       /* dates */
	sum->flsp_date = 0;
	sum->fsp_date = 0;
	sum->lsp_date = 0;
	sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr -1, opt->dat_dir);
	if(strlen(opt->dat_dir)>1) strcat(prog_buf, "\\");
	strcat(prog_buf, ext->pname);
	if ((fi = fopen (prog_buf, "rb")) == NULL) {
		StatsErr ("10004");
		if(sum) free(sum);
		if(data) free (data);
		return(4);
	}
			/* read comment field after initialising it */
	memset (sum->comment, '\0', opt->com_fld +1);
	fseek (fi, (long)opt->com_ofst, SEEK_SET);   /* go to comment offset */
	if (fread (sum->comment, sizeof (char), opt->com_fld, fi) != opt->com_fld) {
		StatsErr ("10005");	 /* no data found - file corrupt ??? */
		fclose(fi);
		if(sum) free(sum);
		if(data) free(data);
		return (5);
   }
         /* LOOK FOR FIRST FILE RECORD DETAILS */
	fseek (fi, (long)opt->hdr_fld, SEEK_SET);
	sum->fsp_pos = sum->ffsp_pos = ftell (fi);
	if (fread (data, sizeof (char), opt->spn_fld+hd->h_len , fi)
											  != opt->spn_fld + hd->h_len) {
		StatsErr ("10006");
		fclose(fi);
		if(sum) free(sum);
		if(data) free(data);
		return (6);
	}
	if(hd->sp)
	sum->fsp = sum->f_fsp = find_sp (hd->sp_ofst + opt->spn_fld, hd->sp, data);
	if (hd->fn)
	sum->ffn = sum->f_ffn = find_fn (hd->fn_ofst + opt->spn_fld, hd->fn, data);
	if(hd->jd)
		sum->fsp_date = sum->ffsp_date = find_jd (hd->jd_ofst + opt->spn_fld, hd->jd, data);
	if(hd->tm) {
		find_tm (hd->tm_ofst + opt->spn_fld, hd->tm, data);
		strcpy (sum->ffsp_tim, ext->tim);
		strcpy (sum->fsp_tim, ext->tim);
	}
	if(hd->wa)
	sum->ffsp_wat = sum->fsp_wat = find_wat(hd->wa_ofst+opt->spn_fld, hd->wa, data)
											+opt->draft;
	if(hd->syhd)
	sum->ffsp_hed = sum->fsp_hed = find_hed(hd->syhd_ofst+opt->spn_fld, hd->syhd, data);
         /* LOOK FOR LAST FILE RECORD DETAILS */
   fseek (fi, -(long)(hd->h_len + opt->spn_fld + opt->add_hdr), SEEK_END);
	sum->lsp_pos = sum->flsp_pos = ftell (fi);
	fread(data, sizeof (char), opt->spn_fld+hd->h_len, fi);
	if(hd->sp)
	sum->lsp = sum->f_lsp = find_sp (hd->sp_ofst + opt->spn_fld, hd->sp, data);
	if (hd->fn)
	sum->lfn = sum->f_lfn = find_fn (hd->fn_ofst + opt->spn_fld, hd->fn, data);
	if(hd->jd)
		sum->lsp_date = sum->flsp_date = find_jd (hd->jd_ofst + opt->spn_fld, hd->jd, data);
	if(hd->tm) {
		find_tm (hd->tm_ofst + opt->spn_fld, hd->tm, data);
		strcpy (sum->flsp_tim, ext->tim);
		strcpy (sum->lsp_tim, ext->tim);
	if(hd->wa)
	sum->flsp_wat = sum->lsp_wat = find_wat(hd->wa_ofst+opt->spn_fld, hd->wa, data)
											+opt->draft;
	if(hd->syhd)
	sum->flsp_hed = sum->lsp_hed = find_hed(hd->syhd_ofst+opt->spn_fld, hd->syhd, data);
   }
   if(hd->ln)
      find_ln(hd->ln_ofst+opt->spn_fld, hd->ln, data, ln_name);
   if(hd->vs)
      find_vs(hd->vs_ofst+opt->spn_fld, hd->vs, data, vs_name);
   if(hd->cl)
      find_cl(hd->cl_ofst+opt->spn_fld, hd->cl, data, cl_name);
   if(hd->ar)
      find_ar(hd->ar_ofst+opt->spn_fld, hd->ar, data, ar_name);
	sum->selln_tim = sum->ln_tim = CompareFTime (sum->ffsp_tim, sum->flsp_tim, 1);
	if (sum->flsp_date > sum->ffsp_date)
		sum->selln_tim = sum->ln_tim = ((sum->flsp_date - sum->ffsp_date)*1440) + sum->ln_tim;
	sum->sel_sp = sum->file_sp = labs (sum->f_fsp - sum->f_lsp)
                                +opt->prod_cal -1; /* calculation method */
   sum->act_sp = (sum->lsp_pos - sum->fsp_pos)/((long)(opt->spn_fld+hd->h_len)) +1L;
	sum->selln_len = sum->ln_len = ((float)sum->file_sp)*(pd->sp_dst/1000);
	if(sum->ln_tim) sum->selav_spd = sum->av_spd = (1000*sum->ln_len)/(((float)sum->ln_tim)*60);  /* M/S */
	if (pd->sp_un == 6) sum->selav_spd = sum->av_spd = sum->av_spd * MS_KNOTS;     /* KNOTS */
	if (sum->f_fsp != sum->f_lsp) {
      if (sum->f_fsp > sum->f_lsp) sum->sp_change = 6; /* decrementing SPs */
      else sum->sp_change = 7;                         /* incrementing SPs */
   } else sum->sp_change = 8;                          /* no change SPs ?? */
	if (sum->f_ffn != sum->f_lfn) {
      if (sum->f_ffn > sum->f_lfn) sum->fn_change = 6; /* decrementing FNs */
      else sum->fn_change = 7;                         /* incrementing FNs */
   } else sum->fn_change = 8;                          /* no change FNs ?? */
			/* format to screen window */
	Wrun = vcreat (20, 58, NORML, NO);
   vatputf(Wrun, 0, 2, "File: %-12s", ext->pname);
   vatputf(Wrun, 0, 22,"Headstat ID: %s", sum->comment);
   vratputs(Wrun, 1, 2, REVNORML, "Total Range");
   if(hd->ln)
      vatputf(Wrun, 1, 20, "Contractor ID: %s", ln_name);
	vatputf(Wrun, 2, 2, "File  FSP  %-6ld", sum->f_fsp);
	if(hd->fn)
		vatputf(Wrun, 2, 22, "FFN %-6ld", sum->f_ffn);
	ltoa (sum->f_fsp, sum->ffsp_bf, 10);
	vatputas (Wrun, 2, 13, REVERR, sum->ffsp_bf);
	if (hd->jd)
		vatputf(Wrun, 2, 33, "JD %3.3d", sum->ffsp_date);
	if (hd->tm)
		vatputf(Wrun, 2, 43, "Time %s", sum->ffsp_tim);
	if (hd->wa)
		vatputf(Wrun, 3, 2, "Water Depth %-1.1f %-8s", sum->ffsp_wat,
															  units[pd->loc_dp]);
	if (hd->syhd)
		vatputf(Wrun, 3, 30, "SysHead %05.1f Degrees", sum->ffsp_hed);
	vatputf(Wrun, 4, 2, "File  LSP  %-6ld", sum->f_lsp);
	if(hd->fn)
	vatputf(Wrun, 4, 22, "LFN %-6ld", sum->f_lfn);
	ltoa (sum->f_lsp, sum->flsp_bf, 10);
	vatputas (Wrun, 4, 13, REVERR, sum->flsp_bf);
	if (hd->jd)
		vatputf(Wrun, 4, 33, "JD %3.3d", sum->flsp_date);
	if (hd->tm)
		vatputf(Wrun, 4, 43, "Time %s", sum->flsp_tim);
	if (hd->wa)
		vatputf(Wrun, 5, 2, "Water Depth %-1.1f %-8s", sum->flsp_wat,
																units[pd->loc_dp]);
	if(hd->syhd)
		vatputf(Wrun, 5, 30, "SysHead %05.1f Degrees", sum->flsp_hed);
	if(hd->tm)
	vatputf(Wrun, 6, 2, "Line Minutes %-4d", sum->ln_tim);
	vatputf(Wrun, 6, 22, "Total SPs %-6ld  Kms %-8.3f", sum->file_sp, sum->ln_len);
	if(hd->tm)
		vatputf(Wrun, 7, 2, "Average Speed %-5.2f %-7s", sum->av_spd,
																  units[pd->sp_un]);
	vatputf(Wrun, 7, 30, "Change Flag: SPs %01s", answer[sum->sp_change]);
	if(hd->fn)
      vatputf(Wrun, 7, 50, "FNs %01s", answer[sum->fn_change]);
   if(hd->cl)
      vatputf(Wrun, 8, 2, "%s", cl_name);
   if(hd->ar)
      vatputf(Wrun, 8, 20, "%s", ar_name);
   if(hd->vs)
      vatputf(Wrun, 8, 38, "%s", vs_name);
	vlocate (Wrun, 3, 10);
	vframe (Wrun, EMPHNORML, FRDOUBLE);
	vtitle (Wrun, EMPHNORML, " Run Time Summary ");
	vratputs (Wrun, 18, 0, REVNORML,
		"   <ESC> Abort     <CR> Run Stats     F8 Select SP Range  ");
	vratputs (Wrun, 19, 0, REVNORML,
      "          F1                      F2 Set Waypoint         ");
   pcuroff();
   vshadow(Wrun, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
	visible (Wrun, YES, NO);
	do {
		rr = vgetkey(UpdateClockWindow);
         switch (rr) {
            case F1:
               go_help();
               break;
				case F2:      /* set a waypoint to halt at */
					set_waypoint();
					if(sum->way_fl)
						vatputf(Wrun, 17, 2,
							"Processing Waypoint Set at SP  %-6ld", sum->way_sp);
					else {
						lcurset(Wrun, 17, 2);  /* clear waypoint display */
						leeol(Wrun);
					}
					break;
				case F8:      /* select SP range to analyse */
					select_sp();
					if ((sum->fsp != sum->f_fsp) || (sum->lsp != sum->f_lsp)) {
						if (find_spr(fi, 0)) {
							lcurset(Wrun, 17, 2);  /* clear waypoint display */
							leeol(Wrun);
							sum->way_fl =0;
							view_spr();
						}
						else {
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
	vdelete(Wrun, NONE);
	if (rr == CR)
		statrun(fi);
	fclose(fi);
	if(sum) free(sum);
	if(data) free(data);
	return(0);
}


/*************************************************************
*   run time help function
**************************************************************/
void go_help()
{
   string_help("Doing A Stats Run");
}
