/***************************************************************
*    STATMAK1.C  produce statistics on screen
****************************************************************
*    notes:
*
*  Compiler:  MSC V6.00  /AL /W4 /Os /Gs /Zp /Zl /G2
*  Linker:    MSC V5.10  /NOE /SE:256 /NOD /EXEPACK
*  Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWL.LIB
*             Greenleaf Superfunctions (MSC) V1.00 GSFL.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Not 8086 compatible
*  Drives     Up to E (floppies + 3 hard/virtual/tape)
*  Dirs       Directory length limited to 50 characters with
*             total "path" (drive\directory\file.extension)
*             limited to 64 characters
****************************************************************
*  (c) West Country Systems Ltd, Cyprus, 1991.
****************************************************************/

#include <stdio.h>
#include <math.h>
#include <string.h>

#include <dw.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct ST_SUMM *sum;      /* summary pointer */
extern struct ST_SPFN *sps;      /* SP stats pointer */
extern struct ST_SPFN *fns;      /* FN ditto */
extern struct ST_DCOM *dco;      /* RU comms ditto */
extern struct ST_CDEP *dp1;      /* depth 1 spec ditto */
extern struct ST_CDEP *dp2;      /* depth 2 spec ditto */
extern struct ST_CDEP *dp3;      /* depth 3 spec ditto */
extern struct ST_CDEP *dp4;      /* depth 4 spec ditto */
extern struct SRCE_ERR *gun;     /* pointer to integer gunspecs */
extern struct ST_SRCE *sr1;      /* source 1 stats */
extern struct ST_SRCE *sr2;      /* source 2 stats */
extern struct ST_SRCE *sr3;      /* source 3 stats */
extern struct ST_SRCE *sr4;      /* source 4 stats */
extern struct ST_XSRC *xsr;      /* source crossfire stats */
extern struct ST_MISF *msf;      /* consecutive & series misfire stats */
extern struct HDR_DEF *hd;       /* header  list pointer */
extern struct PRO_DEF *pd;       /* project list pointer */
extern struct OPT_DEF  *opt;     /* pointer to options list */
extern struct ANAL_DEF *andef;   /* pointer to set/analysis list */
extern struct SPEC_DEF *spc;     /* pointer to specifications list */
extern struct CAB_DEF *stcab;    /* pointer to start of cable list */
extern struct WING_DEF *stwi;    /* ditto wing angles */
extern struct SRCE_DEF *stsrce;  /* pointer to start of source list */
extern struct ST_MAN *man;       /* pressure stats pointer */
extern struct ST_CLK *t_clk;     /* pointer to timing stats */
extern struct ST_GMSK *gmsk;     /* gun mask stats */
extern struct GUNDP_DEF *stgundp; /* pointer to start of gundep list */
extern struct FIR_SEQ *fire;     /* first struct for loop fire sequence */
extern struct FIRE_BUF *fire_buf; /* hold initial fire sequence text */
extern struct FIRE_BUF *fire_buf_tmp;  /* ditto temp */
extern struct GSP_DEF gsp; 		/* hold ground speed stats */
extern struct GSP_DEF ws;			/* hold water speed stats */
extern struct GSP_DEF cu;			/* hold current stats */
extern struct SEQ_DEF st_seq; 	/* hold sequencing stats */
extern struct ST_WAT *wat;       /* water depth analysis */
extern struct EXT_DEF *ext;
extern struct ST_SPTIM *t_int;   /* SP time interval stats */
extern char *units[];            /* pointer to units list */
extern char ln_name[17], cl_name[17],    /* line & client names */
	  vs_name[17], ar_name[17];	/* vessel & area names */


/***************************************************************
*    assemble and view the line statistics
****************************************************************/
void stat_make()
{
	HWND W1, W2;
   struct CAB_DEF *cstat=NULL;     /* temp struct for RUs */
   struct WING_DEF *wistat=NULL;   /* ditto wing angles */
   struct GUNDP_DEF *gdstat=NULL;  /* ditto gundepths */
   int i=3;                        /* line counter for stats frame */
   int p;                          /* total lines in window */
   register int x, n;              /* general fast counters */
	int r;								  /* quit loop integer */
   float wing_mean;                /* mean wing angles */
   unsigned change=0, reset=0;     /* sequence stats calcs */
   p = stat_frame();               /* find out how many lines to add */
   if(p< 18) p = 18;               /* 4 always used */
   W2 = vcreat (1, 80, REVNORML, NO);  /* bottom line menu window */
	vlocate (W2, 24, 0);
	visible (W2, YES, NO);
   if( (W1 = vcreat (p+=4, 78, NORML, NO)) == DWNOMEMORY) { /* main window */
		vratputs(W2, 0, 7, REVERR,
	"NOT ENOUGH MEMORY FOR VIEW WINDOW - Use Report File Viewer Instead");
		ViewReports();
      vdelete(W2, NONE);
      return;
   }
	vratputs(W2, 0, 1, REVNORML, "<ESC> or F7 Return");
	vratputs(W2, 0, 21, REVNORML, "F2 View Report Files");
	if(p>22)
      vratputs(W2, 0, 44, REVNORML, "Home/End/PgUp/PgDn/Arrows  Up/Down");
         /* process info */
   vratputs(W1, 0, 2, EMPHNORML, "Processing Range");
   vatputf(W1, 1, 2, "File: %s", ext->pname);
   vatputf(W1, 1, 36, "Headstat ID: %s", sum->comment);
   vatputf(W1, 2, 2, "SP Range: %ld - %-ld", sum->fsp, sum->lsp);
	if(hd->fn)
      vatputf(W1, 2, 36, "FN Range: %ld - %-ld", sum->ffn, sum->lfn);
   vatputf(W1, 3, 2, "Processed: %ld", sum->procno);
   vatputf(W1, 3, 36, "Corrupt: %ld", sum->currno);
         /* contractor header info */
   if( (hd->cl)||(hd->ln)||(hd->ar)||(hd->vs) ) {
      vratputs(W1, i+=2, 2, EMPHNORML, "Contractor Information");
   if( (hd->cl)||(hd->ar) ) ++i;
      if(hd->cl)
         vatputf(W1, i, 2, "Client: %s", cl_name);
      if(hd->ar)
         vatputf(W1, i, 36, "Area: %s", ar_name);
   if( (hd->ln)||(hd->vs) ) ++i;
      if(hd->ln)
         vatputf(W1, i, 2, "Line: %s", ln_name);
      if(hd->vs)
         vatputf(W1, i, 36, "Vessel: %s", vs_name);
   }
         /* shotpoint numbering  - all contractors */
   if (andef->sp_anl && hd->sp) {
      vratputs(W1, i+=2, 2, EMPHNORML, "SP Numbering");
      vatputf(W1, ++i, 2, "Jump: %ld  Freeze: %ld",
														 sps->ju_mp, sps->no_ch);
   }
         /* file number numbering - GECO & DIGICON TRACE0 */
   if ( (!hd->contr || hd->contr == 5) && andef->fn_anl && hd->fn) {
      vratputs(W1, --i, 36, EMPHNORML, "FN Numbering");
      vatputf(W1, ++i, 36, "Jump: %ld  Freeze: %ld",
													  fns->ju_mp, fns->no_ch);
   }
         /* cable depth remote unit comms - GECO only */
   if (!hd->contr && andef->dep_com && ext->numberru && hd->ca) {
      vratputs(W1, i+=2, 2, EMPHNORML, "Cable RU Comms");
         vatputf(W1, ++i, 2, "SPs with an Error: %ld", dco->flag_err);
      vatputf(W1, i, 36, "Bad Readings: %ld", dco->tot_err);
   }
         /* cable depths */
   if ( (andef->dep_anl1 || andef->dep_anl2) && ext->numberru && hd->ca
                      && pd->loc_strm) {
         /* cable 1 */
      vratputf(W1, i+=2, 2, EMPHNORML, "%s Cable Depths", pd->locab1);
		if(andef->dep_anl1) vatputf(W1, ++i, 2,
   "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->d1min, spc->d1max, units[pd->loc_dp], dp1->sm_sp1, dp1->gr_sp1);
		if(andef->dep_anl2) vatputf(W1, ++i, 2,
   "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->d2min, spc->d2max, units[pd->loc_dp], dp1->sm_sp2, dp1->gr_sp2);
		if(andef->dep_com) vatputf(W1, ++i, 2,
         "SP with Comms Error: %-12ld Bad Readings: %ld",
            dp1->flag_err, dp1->tot_err);
      vatputf(W1, ++i, 2, "MIN DEPTH: %-22.1f MAX DEPTH: %.1f",
			dp1->min_dep, dp1->max_dep);
         /* cable 2 */
      if (pd->loc_strm > 1) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Cable Depths", pd->locab2);
         if(andef->dep_anl1) vatputf(W1, ++i, 2,
            "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->d1min, spc->d1max, units[pd->loc_dp], dp2->sm_sp1, dp2->gr_sp1);
         if(andef->dep_anl2) vatputf(W1, ++i, 2,
            "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->d2min, spc->d2max, units[pd->loc_dp], dp2->sm_sp2, dp2->gr_sp2);
         if(andef->dep_com) vatputf(W1, ++i, 2,
            "SP with Comms Error: %-12ld Bad Readings: %ld",
               dp2->flag_err, dp2->tot_err);
         vatputf(W1, ++i, 2, "MIN DEPTH: %-22.1f MAX DEPTH: %.1f",
            dp2->min_dep, dp2->max_dep);
      }
         /* cable 3 */
      if (pd->loc_strm > 2) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Cable Depths", pd->locab3);
         if(andef->dep_anl1) vatputf(W1, ++i, 2,
            "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->d1min, spc->d1max, units[pd->loc_dp], dp3->sm_sp1, dp3->gr_sp1);
         if(andef->dep_anl2) vatputf(W1, ++i, 2,
            "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max %ld",
		  spc->d2min, spc->d2max, units[pd->loc_dp], dp3->sm_sp2, dp3->gr_sp2);
		if(andef->dep_com) vatputf(W1, ++i, 2,
            "SP with Comms Error: %-12ld Bad Readings: %ld",
               dp3->flag_err, dp3->tot_err);
         vatputf(W1, ++i, 2, "MIN DEPTH: %-22.1f MAX DEPTH: %.1f",
            dp3->min_dep, dp3->max_dep);
      }
         /* cable 4 */
      if (pd->loc_strm > 3) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Cable Depths", pd->locab4);
         if(andef->dep_anl1) vatputf(W1, ++i, 2,
            "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->d1min, spc->d1max, units[pd->loc_dp], dp4->sm_sp1, dp4->gr_sp1);
         if(andef->dep_anl2) vatputf(W1, ++i, 2,
            "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %-4ld",
		  spc->d2min, spc->d2max, units[pd->loc_dp], dp4->sm_sp2, dp4->gr_sp2);
         if(andef->dep_com) vatputf(W1, ++i, 2,
            "SP with Comms Error: %-12ld Bad Readings: %ld",
               dp4->flag_err, dp4->tot_err);
         vatputf(W1, ++i, 2, "MIN DEPTH: %-22.1f MAX DEPTH: %.1f",
            dp4->min_dep, dp4->max_dep);
      }
         /* individual cable depth units */
		cstat = stcab;
      vratputs(W1, i+=2, 2, EMPHNORML, "Individual Cable Depths");
		vatputs(W1, ++i, 2,
    "UNIT        UNIT   COMMUNICATIONS   OUT-OF-SPEC      DEPTH  RANGE");
		vatputs(W1, ++i, 2,
	 "NAME       NUMBER   BAD   % BAD     SPEC1 SPEC2     MIN   <>   MAX");
		vatputs(W1, ++i, 2,
	 "--------------------------------------------------------------------");
		for(x=0;x<ext->numberru;x++) {
			vatputf(W1, ++i, 2, "%-10s  %-3u", cstat->cab_name, cstat->cab_num);
         if(cstat->min_dep < spc->d1min)   /* if out of spec 1 min */
				vratputf(W1, i, 52, REVNORML, "%5.1f", cstat->min_dep);
			else vratputf(W1, i, 52, NORML, "%5.1f", cstat->min_dep);
			vatputs(W1, i, 60, "<>");
         if(cstat->max_dep > spc->d1max)   /* if out of spec 1 max */
            vratputf(W1, i, 64, REVNORML, "%5.1f",cstat->max_dep);
         else vratputf(W1, i, 64, NORML, "%5.1f",cstat->max_dep);
			if(andef->dep_anl1) vatputf(W1, i, 38, "%-5u", cstat->tot_sp1);
			if(andef->dep_anl2) vatputf(W1, i, 44, "%-5u", cstat->tot_sp2);
			if(andef->dep_com) {
				if(cstat->tot_err) cstat->per_c =
							100.0F*(float)cstat->tot_err/(float)sum->procno;
				else cstat->per_c = 0.0F;
				vatputf(W1, i, 22, "%-5u", cstat->tot_err);
				if(cstat->per_c>(float)spc->dcom)
					vratputf(W1, i, 29, REVNORML, "%-4.1f", cstat->per_c);
				else vratputf(W1, i, 29, NORML, "%-4.1f", cstat->per_c);
			}
			cstat = cstat->next;
		}
   }
         /* cable depth controller wing angles - GECO only */
   if(!hd->contr && andef->wi_anl && ext->numberwing && hd->wi) {
      wistat = stwi;
      vratputs(W1, i+=2, 2, EMPHNORML, "Wing Angles");
		vatputs(W1, ++i, 2,
    "UNIT      UNIT       /---- ANGLES ----\\    OUT-OF    %    AVERAGE");
		vatputs(W1, ++i, 2,
    "NAME      NUMBER     MIN    MEAN    MAX     SPEC %  ACTIVE  CHANGE");
		vatputs(W1, ++i, 2,
	 "--------------------------------------------------------------------");
      for(x=0;x<ext->numberwing;x++) {
         vatputf(W1, ++i, 2, "%-10s %u", wistat->name, wistat->num);
         if( (float)fabs( (double)wistat->min ) > spc->wing )
            vratputf(W1, i, 21, REVNORML, "%+4.1f", wistat->min);
         else vratputf(W1, i, 21, NORML, "%+4.1f", wistat->min);
         if( (float)fabs( (double)wistat->max ) > spc->wing )
            vratputf(W1, i, 38, REVNORML, "%+4.1f",wistat->max);
         else vratputf(W1, i, 38, NORML, "%+4.1f",wistat->max);
         wing_mean = wistat->mean/(float)sum->procno;
         if( (wing_mean > spc->wing) || (wing_mean < (-spc->wing)) )
            vratputf(W1, i, 30, REVNORML, "%+4.1f", wing_mean);
         else vratputf(W1, i, 30, NORML, "%+4.1f", wing_mean);
         vatputf(W1, i, 46, "%4.1f", 100.0F*(float)wistat->spec/(float)sum->procno);
         vatputf(W1, i, 55, "%4.1f", 100.0F*(float)wistat->on/(float)sum->procno);
         vatputf(W1, i, 63, "%4.2f", wistat->change/(float)wistat->on);
         wistat = wistat->next;
		}
   }
         /* overall & individual source timing & volumes */
   i = srce_mak(W1, i);
         /* air pressure - NOT Western */
   if( (hd->contr != 2) && (andef->man_anl1 || andef->man_anl2) && hd->ma) {
      vratputs(W1, i+=2, 2, EMPHNORML, "Air Pressure");
		vatputf(W1, ++i, 2,
            "Min: %-10u Mean: %-8lu Max: %-8u %s", man->min,
				 man->total/(unsigned long)sum->procno,
                 man->max, units[pd->pr_un]);
      vatputf(W1, ++i, 2, "SOL:  %-9u EOL:  %-9u %s",
             man->sol, man->eol, units[pd->pr_un]);
      if(andef->man_anl1) vatputf(W1, ++i, 2,
           "SP < %u %s: %.1f %%",
				  spc->ma1, units[pd->pr_un],
					  ((float) man->man1/(float)sum->procno)*100.0F);
      if(andef->man_anl2)vatputf(W1, i, 33,
           "SP < %u %s: %.1f %%",
				  spc->ma2, units[pd->pr_un],
					  ((float) man->man2/(float)sum->procno)*100.0F);
   }
         /* gun masks - GECO only */
   if(!hd->contr && andef->gmsk_anl && hd->gm) {
      vratputs(W1, i+=2, 2, EMPHNORML, "Gun Masks");
      vatputf(W1, ++i, 2, "Total SP Processed: %ld", sum->procno);
      vatputf(W1, i, 30, "SP NO Source Fired: %u", gmsk->tot_none);
      vatputf(W1, ++i, 2, "SP > 1 Source Fired: %u", gmsk->tot_ovr);
      vatputf(W1, i, 30, "SP < %u Guns Fired: %u",
						spc->gmsk, gmsk->tot_set);
      if(pd->loc_srce) {       /* if at least 1 local source */
		  vatputf(W1, ++i, 2,
        "SP %s Overfired: %-5u SP < Min Guns: %u", pd->loc1,
			  gmsk->ovr_loc1, gmsk->loc1);
		}
      if(pd->loc_srce > 1) {   /* if at least 2 local sources */
		  vatputf(W1, ++i, 2,
        "SP %s Overfired: %-5u SP < Min Guns: %u", pd->loc2,
			  gmsk->ovr_loc2, gmsk->loc2);
		}
      if(pd->rem_srce) {       /* if at least 1 remote source */
		  vatputf(W1, ++i, 2,
        "SP %s Overfired: %-5u SP < Min Guns: %u", pd->rem1,
			  gmsk->ovr_rem1, gmsk->rem1);
		}
      if(pd->rem_srce > 1) {   /* if at least 2 remote sources */
		  vatputf(W1, ++i, 2,
        "SP %s Overfired: %-5u SP < Min Guns: %u", pd->rem2,
			  gmsk->ovr_rem2, gmsk->rem2);
		}
   }
         /* firing sequence  - GECO only */
   if(!hd->contr && andef->seq_anl && hd->gm) {
      vratputf(W1, i+=2, 2, EMPHNORML, "Firing Sequence");
      vatputf(W1, ++i, 2,
    "--------------------------------------------------------------------");
      vatputf(W1, ++i, 2,
    "NOMINAL    SP     FN     TIME      ACTUAL SOURCE(S)");
      vatputf(W1, ++i, 2,
    "--------------------------------------------------------------------");
      fire_buf_tmp = fire_buf;
      for(x=0;x<(int)pd->seq_len; x++) {
         n = (int) spc->f_seq[x] - 48;
         if(n<=(int)pd->loc_srce) {
            if(n==1)
               strcpy(ext->a_str, pd->loc1);
            if(n==2)
               strcpy(ext->a_str, pd->loc2);
            if(n==3)
               strcpy(ext->a_str, pd->loc3);
            if(n==4)
               strcpy(ext->a_str, pd->loc4);
         } else {
            n = n - pd->loc_srce;
            if(n==1)
               strcpy(ext->a_str, pd->rem1);
            if(n==2)
               strcpy(ext->a_str, pd->rem2);
            if(n==3)
               strcpy(ext->a_str, pd->rem3);
            if(n==4)
               strcpy(ext->a_str, pd->rem4);
         }
         vatputf(W1, ++i, 2, ext->a_str);
         vatputf(W1, i, 13, fire_buf_tmp->fire_info);
         fire_buf_tmp++;
      }
      vatputf(W1, ++i, 2,
    "--------------------------------------------------------------------");
      if(st_seq.change)    /* divide by 4 flagged for 1 period */
         change = (st_seq.change - 1)/4 + 1;
      if(st_seq.reset)     /* ditto */
         reset = (st_seq.reset - 1)/4 + 1;
      vatputf(W1, ++i, 2, "Sequence Changes: %u", change);
      vatputf(W1, i, 36, "Sequence Resets: %u", reset);
   }
         /* source depths */
   if (andef->gd_anl && hd->gd && ext->numberdp && pd->loc_srce) {
         /* source 1 */
      vratputf(W1, i+=2, 2, EMPHNORML, "%s Source Depths", pd->loc1);
		vatputf(W1, ++i, 2,
      "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		 spc->gd_min, spc->gd_max, units[pd->loc_dp], sr1->sm_sp, sr1->gr_sp);
      vatputf(W1, ++i, 2, "MIN DEPTH: %-22.1f MAX DEPTH: %.1f",
			sr1->min_dep, sr1->max_dep);
         /* source 2 */
      if (pd->loc_srce > 1) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Source Depths", pd->loc2);
         vatputf(W1, ++i, 2,
         "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->gd_min, spc->gd_max, units[pd->loc_dp], sr2->sm_sp, sr2->gr_sp);
         vatputf(W1, ++i, 2, "MIN DEPTH: %-22.1f MAX DEPTH: %.1f",
            sr2->min_dep, sr2->max_dep);
      }
         /* source 3 */
      if (pd->loc_srce > 2) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Source Depths", pd->loc3);
         vatputf(W1, ++i, 2,
         "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->gd_min, spc->gd_max, units[pd->loc_dp], sr3->sm_sp, sr3->gr_sp);
         vatputf(W1, ++i, 2, "MIN DEPTH: %-22.1f MAX DEPTH: %.1f",
            sr3->min_dep, sr3->max_dep);
      }
         /* source 4 */
      if (pd->loc_srce > 3) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Source Depths", pd->loc4);
         vatputf(W1, ++i, 2,
         "Spec: %4.1f <> %-4.1f %-14s SP < Min: %-6ld SP > Max: %ld",
		  spc->gd_min, spc->gd_max, units[pd->loc_dp], sr4->sm_sp, sr4->gr_sp);
         vatputf(W1, ++i, 2, "MIN DEPTH: %-22.1f MAX DEPTH: %.1f",
            sr4->min_dep, sr4->max_dep);
      }
         /* individual source depth units */
		gdstat = stgundp;
      vratputs(W1, i+=2, 2, EMPHNORML, "Individual Source Depths");
      vatputs(W1, ++i, 4,
    "UNIT        UNIT        SOURCE     SP OUTSIDE      DEPTH  RANGE");
      vatputs(W1, ++i, 4,
    "NAME        NUMBER      NUMBER     WINDOW         MIN   <>   MAX");
		vatputs(W1, ++i, 2,
	 "--------------------------------------------------------------------");
		for(x=0;x<ext->numberdp;x++) {
			vatputf(W1, ++i, 2, "%-10s      %-3u         %-2u",
			gdstat->gundp_name, gdstat->gundp_num, gdstat->srce);
			if(gdstat->min_dep<spc->gd_min)
				vratputf(W1, i, 52, REVNORML, "%5.1f", gdstat->min_dep);
			else vratputf(W1, i, 52, NORML, "%5.1f", gdstat->min_dep);
			vatputs(W1, i, 60, "<>");
			if(gdstat->max_dep>spc->gd_max)
				vratputf(W1, i, 65, REVNORML, "%5.1f",gdstat->max_dep);
			else vratputf(W1, i, 65, NORML, "%5.1f",gdstat->max_dep);
			vatputf(W1, i, 40, "%-5u", gdstat->tot_sp);
			gdstat = gdstat->next;
		}
   }
          /* Intervessel or TB Echo or FTB timing - NOT Western */
   if( (hd->contr != 2) && andef->clk_anl && hd->clk) {
      if( (hd->clk==18)||(hd->clk==19)||(hd->clk==21) )   /* GECO */
      vratputs(W1, i+=2, 2, EMPHNORML, "Intervessel Timing");
      if(hd->clk == 71)      /* DIGICON TRACE0 */
         vratputs(W1, i+=2, 2, EMPHNORML, "TB Echo Timing");
      if(hd->clk == 91)      /* PRAKLA FTB TIMING (1+2+3) */
         vratputs(W1, i+=2, 2, EMPHNORML, "FTB Timing");
		if(t_clk->tot_anl) {
			t_clk->mean = t_clk->total/(float)t_clk->tot_anl;
			t_clk->per_spec = (((float)(t_clk->sm_spec + t_clk->gr_spec))/
										 (float)t_clk->tot_anl)*100.0F;
		} else {
			t_clk->mean = 0.0F;
			t_clk->per_spec = 0.0F;
		}
		if(sum->procno) {
			t_clk->per_corr =
				((float)t_clk->corrupt/(float)sum->procno)*100.0F;
		} else t_clk->per_corr = 0.0F;
		vatputf(W1, ++i, 2,
            "Min: %-9.1f Mean: %-.1f",
             t_clk->min, t_clk->mean);
      vatputf(W1, i, 30, "Max: %-.1f  MSEC", t_clk->max);
      vatputf(W1, ++i, 2, "Corrupt: %u", t_clk->corrupt);
      vatputf(W1, i,   25, "Valid: %-4u", t_clk->tot_anl);
		vatputf(W1, ++i, 2,
            "Corrupt: %-.1f %%", t_clk->per_corr);
      vatputf(W1, i, 25,
            "Spec: %-4.1f mS +/- %-4.1f mS",
				 spc->clk, spc->clk_ra);
		vatputf(W1, ++i, 2,
            "SP < Spec: %u", t_clk->sm_spec);
      vatputf(W1, i, 25, "SP > Spec: %u", t_clk->gr_spec);
		vatputf(W1, ++i, 2,
            "Valid Times Outside Spec: %.1f %%", t_clk->per_spec);
   }
         /* water depth */
   if(andef->wdp_anl && hd->wa) {
      vratputs(W1, i+=2, 2, EMPHNORML, "Water Depths");
		vatputf(W1, ++i, 2,
            "Min: %-10.1f Max: %-6.1f %s", wat->min,
                 wat->max, units[pd->loc_dp]);
		vatputf(W1, ++i, 2,
            "Corrupt: %-6u Valid: %u",
             wat->corr, sum->procno - wat->corr);
		vatputf(W1, ++i, 2,
            "Spec: %5.1f <> %-5.1f %-10s Outside Window: %u",
                 spc->wd_min, spc->wd_max, units[pd->loc_dp], wat->err);
   }
         /* SP time interval */
   if(andef->tim_anl && hd->tm) {
      vratputs(W1, i+=2, 2, EMPHNORML, "SP Time Intervals");
      if(!t_int->tot_anl)
         t_int->mean = 0.0F;
      else
         t_int->mean = t_int->total/ (float)t_int->tot_anl;
		vatputf(W1, ++i, 2,
            "Min: %-10.2f Mean: %-10.2f Max: %-6.2f SEC",
                t_int->tmin, t_int->mean, t_int->tmax);
		vatputf(W1, ++i, 2,
            "Corrupt: %-6u Valid: %-4u",
             t_int->corrupt, t_int->tot_anl);
      vatputf(W1, ++i, 2,
            "%u SP < %-.2f SEC        %u SP > %-.2f SEC",
             t_int->sm_spec, spc->t_min, t_int->gr_spec, spc->t_max);
   }
         /* speed along track - GECO & DIGICON TRACE0 */
   if( (!hd->contr || hd->contr == 5) && andef->gsp_anl && hd->gs) {
      vratputs(W1, i+=2, 2, EMPHNORML, "Speed Along Track");
		vatputf(W1, ++i, 2,
            "Min: %-10.2f Mean: %-10.2f Max: %-6.2f %s",
                gsp.min, gsp.tot/(float)gsp.read, gsp.max, units[pd->sp_un]);
      vatputf(W1, ++i, 2, "Corrupt: %-6u Analysed: %-6u DMean: %-6.2f %s",
                gsp.corr, gsp.read,
                   gsp.change/(float)gsp.read, units[pd->sp_un]);
      vatputf(W1, ++i, 2,
            "SP > %-.2f %s: %-.1f %%      SOL: %-.1f   EOL: %.1f   %s",
         spc->gsp, units[pd->sp_un], 100.0F*(float)gsp.spec/(float)gsp.read,
               gsp.sol, gsp.eol, units[pd->sp_un]);
	}
			/* water speed - GECO only */
	if( !hd->contr && andef->ws_anl && hd->ws) {
		vratputs(W1, i+=2, 2, EMPHNORML, "Water Speed");
		vatputf(W1, ++i, 2,
            "Min: %-10.2f Mean: %-10.2f Max: %-6.2f %s",
					 ws.min, ws.tot/(float)ws.read, ws.max, units[pd->sp_un]);
      vatputf(W1, ++i, 2, "Corrupt: %-6u Analysed: %-6u DMean: %-6.2f %s",
					 ws.corr, ws.read,
						 ws.change/(float)ws.read, units[pd->sp_un]);
      vatputf(W1, ++i, 2,
            "SP > %-.2f %s: %-.1f %%      SOL: %-.1f   EOL: %.1f   %s",
			spc->gsp, units[pd->sp_un], 100.0F*(float)ws.spec/(float)ws.read,
					ws.sol, ws.eol, units[pd->sp_un]);
	}
			/* current - GECO only */
	if( !hd->contr && andef->ws_anl
						&& hd->ws && hd->gs && andef->gsp_anl) {
		vratputs(W1, i+=2, 2, EMPHNORML, "Current");
		vatputf(W1, ++i, 2,
		 "Min: %+-10.2f Max: %+-10.2f SOL: %+-6.2f EOL: %+-8.2f %s",
					cu.min, cu.max, cu.sol, cu.eol, units[pd->sp_un]);
		vatputf(W1, ++i, 2,
				"Corrupt: %-6u Valid: %u", cu.corr, cu.read);
	}
					/* make the display visible */
	vwind (W1, 22, 78, 0, 0);
	vlocate (W1, 1, 1);
	vframe (W1, EMPHNORML, FRDOUBLE);
	visible (W1, YES, NO);
	i=0;
	r=0;
	while( !r ) {
		switch ( vgetkey( UpdateClockWindow ) ) {
				case ESC:
				case F7:
					r = 1;
					break;
				case F2:
				ViewReports(); 	/* use file viewing utility */
				break;
			case CURDN:
				if ((i+22) >= p ) vbeep();
				if ((i+22) < p)
					vloc (W1, ++i, 0);
				break;
			case CURUP:
				if (!i) vbeep();
				if (i)
					vloc (W1, --i, 0);
					break;
			case PGDN:
				if((i+44)<=p) {
					vloc(W1, i+=22, 0);
					break;
				}
				if (((i+44)>p)&&((i+22)<p)) vloc(W1, (i=p-22), 0);
				if(i==p-22) vbeep();
				break;
			case PGUP:
				if(i>=22) {
					vloc(W1, i-=22, 0);
					break;
				}
				if((i)&&(i<22)) vloc(W1, i=0, 0);
				if(!i) vbeep();
				break;
			case ENDKEY:
			case CTRLPGDN:
				vloc(W1, i=p-22, 0);
				vbeep();
				break;
			case HOME:
			case CTRLPGUP:
				vloc(W1, i=0, 0);
				vbeep();
				break;
		}
	}
	vdelete(W1, NONE);
	vdelete(W2, NONE);
	return;
}


/***************************************************************
*    source timing & volumes
****************************************************************/
srce_mak(HWND W1, register int i)
{
   float gun_perc;                 /* gun misfire percents */
   struct SRCE_DEF *gstat=NULL;    /* ditto for guns */
   register int x;
   if( (andef->gun_anl1 || andef->gun_anl2) && hd->gu
                     && ext->numbergun && pd->loc_srce) {
         /* overall source */
      vratputs(W1, i+=2, 2, EMPHNORML, "Overall Source");
      vatputf(W1, ++i, 2, "SP No Local Source Fired: %u", xsr->no_fire);
      vatputf(W1, i, 36, "SP > 1 Local Source Fired: %u", xsr->x_fire);
		vatputf(W1, ++i, 2,
         "SP in Misfire %% (at least 1 local gun fired): %-5u",
				xsr->tot_sp);
		if(andef->gun_anl1) {
         vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun1);
         vatputs(W1, i, 50, "Misfires:");
			if(xsr->tot_sp)
				xsr->p_spec1 = ((float)xsr->spec1)*100.0F/((float)xsr->tot_sp);
			else xsr->p_spec1 = 0.0F;
			if(xsr->p_spec1>(float)spc->mf4)
            vratputf(W1, i, 60, REVNORML, "%.1f %%", xsr->p_spec1);
         else vratputf(W1, i, 60, NORML, "%.1f %%", xsr->p_spec1);
         vatputf(W1, i, 22,
         "SP outside Window: %u",
				xsr->spec1);
		}
		if(andef->gun_anl2) {
         vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun2);
         vatputs(W1, i, 50, "Misfires:");
			if(xsr->tot_sp)
				xsr->p_spec2 = ((float)xsr->spec2)*100.0F/((float)xsr->tot_sp);
			else xsr->p_spec2 = 0.0F;
			if(xsr->p_spec2>(float)spc->mf4)
            vratputf(W1, i, 60, REVNORML, "%.1f %%", xsr->p_spec2);
         else vratputf(W1, i, 60, NORML, "%.1f %%", xsr->p_spec2);
         vatputf(W1, i, 22,
			"SP outside Window: %u",
				xsr->spec2);
      }
         /* source 1 timing & volumes */
      vratputf(W1, i+=2, 2, EMPHNORML, "%s Source", pd->loc1);
      vatputf(W1, ++i, 2, "Source Fired: %u", sr1->en_num);
      vatputf(W1, i, 22, "NOFIRE/AUTOFIRE: %u", sr1->tot_mf);
      if(andef->gun_anl1) {
         vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun1);
         vatputs(W1, i, 50, "Misfires:");
         if(sr1->en_num)
            sr1->p_spec1 = ((float)sr1->sp1)*100.0F/((float)sr1->en_num);
         else sr1->p_spec1 = 0.0F;
         if(sr1->p_spec1>(float)spc->mf4)
            vratputf(W1, i, 60, REVNORML, "%.1f %%", sr1->p_spec1);
         else vratputf(W1, i, 60, NORML, "%.1f %%", sr1->p_spec1);
         vatputf(W1, i, 22,
         "SP outside Window: %u",
            sr1->sp1);
         if (andef->mf1_anl) {
            vatputf(W1, ++i, 2,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u",
                  spc->mf1, spc->gun1, msf->s1ct1);
         }
         if (andef->mf3_anl) {
            vatputf(W1, ++i, 2,
               "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u",
                  spc->mf3, spc->mf3_ra, spc->gun1, msf->s1mt1);
         }
      }
      if(andef->gun_anl2) {
         vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun2);
         vatputs(W1, i, 50, "Misfires:");
         if(sr1->en_num)
            sr1->p_spec2 = ((float)sr1->sp2)*100.0F/((float)sr1->en_num);
         else sr1->p_spec2 = 0.0F;
         if(sr1->p_spec2>(float)spc->mf4)
            vratputf(W1, i, 60, REVNORML, "%.1f %%", sr1->p_spec2);
         else vratputf(W1, i, 60, NORML, "%.1f %%", sr1->p_spec2);
         vatputf(W1, i, 22,
         "SP outside Window: %u",
            sr1->sp2);
         if (andef->mf1_anl) {
            vatputf(W1, ++i, 2,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u",
                  spc->mf1, spc->gun2, msf->s1ct2);
         }
         if (andef->mf3_anl) {
            vatputf(W1, ++i, 2,
               "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u",
                  spc->mf3, spc->mf3_ra, spc->gun2, msf->s1mt2);
         }
      }
      if((andef->vol_anl1)||(andef->vol_anl2)) {
         vatputf(W1, ++i, 2, "Volume SOL: %u", sr1->sol_vol);
         vatputf(W1, i, 22, "EOL: %u  %s" , sr1->sp_vol,
                                            units[pd->vol_un]);
         ++i;
         if(andef->vol_anl1)
            vatputf(W1, i, 2,
               "SP < %u %s: %u",
                  spc->gvol1, units[pd->vol_un], sr1->vol1);
         if(andef->vol_anl2)
            vatputf(W1, i, 36,
               "SP < %-5u %s: %u",
                  spc->gvol2, units[pd->vol_un], sr1->vol2);
      }
         /* source 2 timing & volumes */
      if (pd->loc_srce > 1) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Source", pd->loc2);
         vatputf(W1, ++i, 2, "Source Fired: %u", sr2->en_num);
         vatputf(W1, i, 22, "NOFIRE/AUTOFIRE: %u", sr2->tot_mf);
			if(andef->gun_anl1) {
            vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun1);
            vatputs(W1, i, 50, "Misfires:");
				if(sr2->en_num)
					sr2->p_spec1 = ((float)sr2->sp1)*100.0F/((float)sr2->en_num);
				else sr2->p_spec1 = 0.0F;
				if(sr2->p_spec1>(float)spc->mf4)
               vratputf(W1, i, 60, REVNORML, "%.1f %%", sr2->p_spec1);
            else vratputf(W1, i, 60, NORML, "%.1f %%", sr2->p_spec1);
            vatputf(W1, i, 22,
            "SP outside Window: %u",
               sr2->sp1);
            if (andef->mf1_anl) {
               vatputf(W1, ++i, 2,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u",
                     spc->mf1, spc->gun1, msf->s2ct1);
            }
            if (andef->mf3_anl) {
               vatputf(W1, ++i, 2,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u",
                  spc->mf3, spc->mf3_ra, spc->gun1, msf->s2mt1);
            }
			}
			if(andef->gun_anl2) {
            vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun2);
            vatputs(W1, i, 50, "Misfires:");
				if(sr2->en_num)
					sr2->p_spec2 = ((float)sr2->sp2)*100.0F/((float)sr2->en_num);
				else sr2->p_spec2 = 0.0F;
				if(sr2->p_spec2>(float)spc->mf4)
               vratputf(W1, i, 60, REVNORML, "%.1f %%", sr2->p_spec2);
            else vratputf(W1, i, 60, NORML, "%.1f %%", sr2->p_spec2);
            vatputf(W1, i, 22,
            "SP outside Window: %u",
               sr2->sp2);
            if (andef->mf1_anl) {
               vatputf(W1, ++i, 2,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u",
                     spc->mf1, spc->gun2, msf->s2ct2);
            }
            if (andef->mf3_anl) {
               vatputf(W1, ++i, 2,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u",
                  spc->mf3, spc->mf3_ra, spc->gun2, msf->s2mt2);
            }
			}
			if((andef->vol_anl1)||(andef->vol_anl2)) {
            vatputf(W1, ++i, 2, "Volume SOL: %u", sr2->sol_vol);
            vatputf(W1, i, 22, "EOL: %u %s" , sr2->sp_vol,
                                               units[pd->vol_un]);
            ++i;
				if(andef->vol_anl1)
               vatputf(W1, i, 2,
                  "SP < %u %s: %u",
							spc->gvol1, units[pd->vol_un], sr2->vol1);
				if(andef->vol_anl2)
               vatputf(W1, i, 36,
                  "SP < %u %s: %u",
							spc->gvol2, units[pd->vol_un], sr2->vol2);
			}
      }
         /* source 3 timing & volumes */
      if (pd->loc_srce > 2) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Source", pd->loc3);
         vatputf(W1, ++i, 2, "Source Fired: %u", sr3->en_num);
         vatputf(W1, i, 22, "NOFIRE/AUTOFIRE: %u", sr3->tot_mf);
			if(andef->gun_anl1) {
            vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun1);
            vatputs(W1, i, 50, "Misfires:");
				if(sr3->en_num)
					sr3->p_spec1 = ((float)sr3->sp1)*100.0F/((float)sr3->en_num);
				else sr3->p_spec1 = 0.0F;
				if(sr3->p_spec1>(float)spc->mf4)
               vratputf(W1, i, 60, REVNORML, "%.1f %%", sr3->p_spec1);
            else vratputf(W1, i, 60, NORML, "%.1f %%", sr3->p_spec1);
            vatputf(W1, i, 22,
            "SP outside Window: %u",
               sr3->sp1);
            if (andef->mf1_anl) {
               vatputf(W1, ++i, 2,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u",
                     spc->mf1, spc->gun1, msf->s3ct1);
            }
            if (andef->mf3_anl) {
               vatputf(W1, ++i, 2,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u",
                  spc->mf3, spc->mf3_ra, spc->gun1, msf->s3mt1);
            }
			}
			if(andef->gun_anl2) {
            vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun2);
            vatputs(W1, i, 50, "Misfires:");
				if(sr3->en_num)
					sr3->p_spec2 = ((float)sr3->sp2)*100.0F/((float)sr3->en_num);
				else sr3->p_spec2 = 0.0F;
				if(sr3->p_spec2>(float)spc->mf4)
               vratputf(W1, i, 60, REVNORML, "%.1f %%", sr3->p_spec2);
            else vratputf(W1, i, 60, NORML, "%.1f %%", sr3->p_spec2);
            vatputf(W1, i, 22,
            "SP outside Window: %u",
               sr3->sp2);
            if (andef->mf1_anl) {
               vatputf(W1, ++i, 2,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u",
                     spc->mf1, spc->gun2, msf->s3ct2);
            }
            if (andef->mf3_anl) {
               vatputf(W1, ++i, 2,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u",
                  spc->mf3, spc->mf3_ra, spc->gun2, msf->s3mt2);
            }
			}
			if((andef->vol_anl1)||(andef->vol_anl2)) {
            vatputf(W1, ++i, 2, "Volume SOL: %u", sr3->sol_vol);
            vatputf(W1, i, 22, "EOL: %u %s" , sr3->sp_vol,
                                               units[pd->vol_un]);
            ++i;
				if(andef->vol_anl1)
               vatputf(W1, i, 2,
                  "SP < %u %s: %u",
							spc->gvol1, units[pd->vol_un], sr3->vol1);
				if(andef->vol_anl2)
               vatputf(W1, i, 36,
                  "SP < %u %s: %u",
							spc->gvol2, units[pd->vol_un], sr3->vol2);
			}
      }
         /* source 4 timing & volumes */
      if (pd->loc_srce > 3) {
         vratputf(W1, i+=2, 2, EMPHNORML, "%s Source", pd->loc4);
         vatputf(W1, ++i, 2, "Source Fired: %u", sr4->en_num);
         vatputf(W1, i, 22, "NOFIRE/AUTOFIRE: %u", sr4->tot_mf);
			if(andef->gun_anl1) {
            vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun1);
            vatputs(W1, i, 50, "Misfires:");
				if(sr4->en_num)
					sr4->p_spec1 = ((float)sr4->sp1)*100.0F/((float)sr4->en_num);
				else sr4->p_spec1 = 0.0F;
				if(sr4->p_spec1>(float)spc->mf4)
               vratputf(W1, i, 60, REVNORML, "%.1f %%", sr4->p_spec1);
            else vratputf(W1, i, 60, NORML, "%.1f %%", sr4->p_spec1);
            vatputf(W1, i, 22,
            "SP outside Window: %u",
               sr4->sp1);
            if (andef->mf1_anl) {
               vatputf(W1, ++i, 2,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u",
                     spc->mf1, spc->gun1, msf->s4ct1);
            }
            if (andef->mf3_anl) {
               vatputf(W1, ++i, 2,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u",
                  spc->mf3, spc->mf3_ra, spc->gun1, msf->s4mt1);
            }
			}
			if(andef->gun_anl2) {
            vatputf(W1, ++i, 2, "Spec: +/- %.2f mS", spc->gun2);
            vatputs(W1, i, 50, "Misfires:");
				if(sr4->en_num)
					sr4->p_spec2 = ((float)sr4->sp2)*100.0F/((float)sr4->en_num);
				else sr4->p_spec2 = 0.0F;
				if(sr4->p_spec2>(float)spc->mf4)
               vratputf(W1, i, 60, REVNORML, "%.1f %%", sr4->p_spec2);
            else vratputf(W1, i, 60, NORML, "%.1f %%", sr4->p_spec2);
            vatputf(W1, i, 22,
            "SP outside Window: %u",
               sr4->sp2);
            if (andef->mf1_anl) {
               vatputf(W1, ++i, 2,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u",
                     spc->mf1, spc->gun2, msf->s4ct2);
            }
            if (andef->mf3_anl) {
               vatputf(W1, ++i, 2,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u",
                  spc->mf3, spc->mf3_ra, spc->gun2, msf->s4mt2);
            }
			}
			if((andef->vol_anl1)||(andef->vol_anl2)) {
            vatputf(W1, ++i, 2, "Volume SOL: %u", sr4->sol_vol);
            vatputf(W1, i, 22, "EOL: %u %s" , sr4->sp_vol,
                                               units[pd->vol_un]);
            ++i;
				if(andef->vol_anl1)
               vatputf(W1, i, 2,
                  "SP < %u %s: %u",
							spc->gvol1, units[pd->vol_un], sr4->vol1);
				if(andef->vol_anl2)
               vatputf(W1, i, 36,
                  "SP < %-5u %s: %u",
							spc->gvol2, units[pd->vol_un], sr4->vol2);
			}
      }
      gstat = stsrce;
      vratputs(W1, i+=2, 2, EMPHNORML, "Individual Guns");
      vatputs(W1, ++i, 2,
	 "GUN        GUN  SRCE GUN     SHOTPOINTS      SPEC 1       SPEC 2 ");
      vatputs(W1, ++i, 2,
	 "NAME       NO.  NO.  VOL.  ENABLE NOF/AUT   BAD   MF%    BAD   MF%");
      vatputs(W1, ++i, 2,
	 "--------------------------------------------------------------------");
      for(x=0;x<ext->numbergun;x++) {
         vatputf(W1, ++i, 2, "%-10s %-3d  %-3d  %-4d  %-5u   %-5u",
            gstat->gun_name, gstat->gun_num, gstat->srce,
            gstat->gun_vol, gstat->en_num, gstat->mf_num);
         if(andef->gun_anl1) {
            vatputf(W1, i, 46, "%-5u", gstat->tot_sp1);
            if(gstat->en_num)
               gun_perc = ((float)gstat->tot_sp1)*100.0F/((float)gstat->en_num);
            else gun_perc = 0.0F;
            if(gun_perc>(float)spc->mf4)
               vratputf(W1, i, 52, REVNORML, "%-4.1f", gun_perc);
            else vratputf(W1, i, 52, NORML, "%-4.1f", gun_perc);
         }
         if(andef->gun_anl2) {
            vatputf(W1, i, 59, "%-5u", gstat->tot_sp2);
            if(gstat->en_num)
               gun_perc = ((float)gstat->tot_sp2)*100.0F/((float)gstat->en_num);
            else gun_perc = 0.0F;
            if(gun_perc>(float)spc->mf4)
               vratputf(W1, i, 65, REVNORML, "%-4.1f", gun_perc);
            else vratputf(W1, i, 65, NORML, "%-4.1f", gun_perc);
         }
      gstat = gstat->next;
      }
	}
   return(i);
}


/****************************************************************
*   calculate size of logical window for stats display
*   return integer window height
*****************************************************************/
stat_frame()
{
   register int i=0;
            /* contractor information  - GECO only */
   if(hd->cl || hd->ln || hd->ar || hd->vs) i+=2;
   if(hd->cl || hd->ar) ++i;     /* client & area */
   if(hd->ln || hd->vs) ++i;     /* line & vessel */
            /* shotpoint & file numbering */
   if (andef->sp_anl || (andef->fn_anl && hd->fn) ) i +=3;
            /* cable depth remote unit comms - GECO only */
   if (!hd->contr && andef->dep_com && ext->numberru && hd->ca) i +=3;
            /* cable depths */
   if ( (andef->dep_anl1 || andef->dep_anl2) && ext->numberru && hd->ca
                      && pd->loc_strm) {
            /* cable 1 */
      i += 3;
		if(andef->dep_anl1) ++i;
		if(andef->dep_anl2) ++i;
		if(andef->dep_com) ++i;
            /* cable 2 */
      if (pd->loc_strm > 1) {
         i += 3;
         if(andef->dep_anl1) ++i;
         if(andef->dep_anl2) ++i;
         if(andef->dep_com) ++i;
      }
            /* cable 3 */
      if (pd->loc_strm > 2) {
         i += 3;
         if(andef->dep_anl1) ++i;
         if(andef->dep_anl2) ++i;
         if(andef->dep_com) ++i;
      }
            /* cable 4 */
      if (pd->loc_strm > 3) {
         i += 3;
         if(andef->dep_anl1) ++i;
         if(andef->dep_anl2) ++i;
         if(andef->dep_com) ++i;
      }
            /* individual depth units */
      i += (5 + ext->numberru);
   }
            /* depth controller wing angles - GECO only */
   if(!hd->contr && andef->wi_anl && ext->numberwing && hd->wi)
      i += (5 + ext->numberwing);
            /* source timing & volumes */
   if( (andef->gun_anl1 || andef->gun_anl2) && hd->gu
                            && pd->loc_srce && ext->numbergun) {
            /* overall sources */
      i+=4;
      if(andef->gun_anl1) ++i;
      if(andef->gun_anl2) ++i;
            /* source 1 */
      i += 3;
      if(andef->gun_anl1) {
         ++i;
         if (andef->mf1_anl) ++i;
         if (andef->mf3_anl) ++i;
      }
      if(andef->gun_anl2) {
         ++i;
         if (andef->mf1_anl) ++i;
         if (andef->mf3_anl) ++i;
      }
      if(andef->vol_anl1 || andef->vol_anl2) i+=2;
            /* source 2 */
      if (pd->loc_srce > 1) {
         i += 3;
         if(andef->gun_anl1) {
            ++i;
            if (andef->mf1_anl) ++i;
            if (andef->mf3_anl) ++i;
         }
         if(andef->gun_anl2) {
            ++i;
            if (andef->mf1_anl) ++i;
            if (andef->mf3_anl) ++i;
         }
         if(andef->vol_anl1 || andef->vol_anl2) i+=2;
      }
            /* source 3 */
      if (pd->loc_srce > 2) {
         i += 3;
         if(andef->gun_anl1) {
            ++i;
            if (andef->mf1_anl) ++i;
            if (andef->mf3_anl) ++i;
         }
         if(andef->gun_anl2) {
            ++i;
            if (andef->mf1_anl) ++i;
            if (andef->mf3_anl) ++i;
         }
         if(andef->vol_anl1 || andef->vol_anl2) i+=2;
      }
            /* source 4 */
      if (pd->loc_srce > 3) {
         i += 3;
         if(andef->gun_anl1) {
            ++i;
            if (andef->mf1_anl) ++i;
            if (andef->mf3_anl) ++i;
         }
         if(andef->gun_anl2) {
            ++i;
            if (andef->mf1_anl) ++i;
            if (andef->mf3_anl) ++i;
         }
         if(andef->vol_anl1 || andef->vol_anl2) i+=2;
      }
            /* individual gun timings */
      i +=  (5 + ext->numbergun);
   }
            /* air pressure */
   if( (hd->contr != 2) && (andef->man_anl1 || andef->man_anl2) && hd->ma) {
      i+=4;
      if(andef->man_anl1) ++i;
   }
            /* system timing */
   if( (hd->contr != 2) && andef->clk_anl && hd->clk) i += 7;
            /* gun masks */
   if(!hd->contr && andef->gmsk_anl && hd->gm) {
      i += 4;
		if(pd->loc_srce) ++i;          /* if at least 1 local source */
		if(pd->loc_srce > 1) ++i;      /* if at least 2 local sources */
		if(pd->rem_srce) ++i;          /* if at least 1 remote source */
		if(pd->rem_srce > 1) ++i;      /* if at least 2 remote sources */
   }
            /* firing sequence */
   if(!hd->contr && andef->seq_anl && hd->gm)
      i += 7 + pd->seq_len;
            /* source depths */
   if(andef->gd_anl && ext->numberdp && hd->gd && pd->loc_srce) {
            /* source 1 */
      i+=4;
            /* source 2 */
      if(pd->loc_srce > 1) i+=4;
            /* source 3 */
      if(pd->loc_srce > 2) i+=4;
            /* source 4 */
      if(pd->loc_srce > 3) i+=4;
            /* individual source unit depths */
      i += (5 + ext->numberdp);
   }
            /* water depth */
   if(andef->wdp_anl && hd->wa) i += 5;
            /* SP time intervals */
   if(andef->tim_anl && hd->tm) i += 6;
            /* speed along track - GECO & DIGICON TRACE0 */
   if( (!hd->contr || hd->contr == 5) && andef->gsp_anl && hd->gs) i += 5;
				/* water speed - GECO only */
	if( !hd->contr && andef->ws_anl && hd->ws) i += 5;
				/* current - GECO only */
	if( !hd->contr && andef->ws_anl
						&& hd->ws && hd->gs && andef->gsp_anl) i += 4;
	return (i);
}
