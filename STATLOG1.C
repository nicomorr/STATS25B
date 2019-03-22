/***************************************************************
*    STATLOG1.C  master file for logging stats to disk
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

#include <sftd.h>          /* greenleaf */

#include <stats.h>         /* wcs */
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
extern struct SRCE_DEF *stsrce;  /* pointer to start of source list */
extern struct ST_MAN *man;       /* pressure stats pointer */
extern struct ST_CLK *t_clk;     /* pointer to timing stats */
extern struct ST_GMSK *gmsk;     /* gun mask stats */
extern struct GUNDP_DEF *stgundp;   /* pointer to start of gundep list */
extern struct WING_DEF *stwi;    /* ditto wing angles */
extern struct FIR_SEQ *fire;     /* first struct for loop fire sequence */
extern struct FIRE_BUF *fire_buf; /* hold initial fire sequence text */
extern struct FIRE_BUF *fire_buf_tmp;  /* ditto temp */
extern struct SEQ_DEF st_seq; 	/* hold sequencing stats */
extern struct GSP_DEF gsp; 		/* hold track speed stats */
extern struct GSP_DEF ws;			/* hold water speed stats */
extern struct GSP_DEF cu;			/* hold current speed stats */
extern struct ST_WAT *wat;       /* water depth analysis */
extern struct ST_SPTIM *t_int;   /* SP time interval stats */
extern struct EXT_DEF *ext;      /* general variables */
extern char *units[];            /* pointer to units list */
extern char *answer[];           /* YES/NO - + ? etc etc */
extern char ver[];               /* master program version */
extern char *formats[];          /* format definitions */
extern char ln_name[17], cl_name[17],    /* line & client names */
     vs_name[17], ar_name[17];   /* vessel & area names */


/***************************************************************
*   open/write/close an ASCII stats log disk file
****************************************************************/
void stat_log()
{
   FILE *flog = NULL;               /* logging file */
   struct GUNDP_DEF *gdstat=NULL;   /* ditto gundepths */
   struct WING_DEF *wistat=NULL;    /* ditto wing angles */
   struct FIR_SEQ *info=NULL;       /* pointer for initial sequence */
   register int i, p;
   unsigned change=0, reset=0;      /* sequence stats calcs */
   float wing_mean;                 /* mean wing angles */
   char prog_buf[65];               /* .TXT path/file buffer */
   char logbuf[71];                 /* line log buffer */
   for (i=0; i<8; i++)
      logbuf[i] = ext->pname[i];
   logbuf[8] = '\0';
   strcat(logbuf, ".TXT");
   sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr -1, opt->txt_dir);
   if(strlen(opt->txt_dir)>1) strcat(prog_buf, "\\");
   strcat(prog_buf, logbuf);
   flog = fopen(prog_buf, "wt");
   fprintf(flog,
   "==================================================================\n");
   fprintf(flog, "LINE ANALYSIS %-24s %s - INTERA ECL\n", " ", ver);
   fprintf(flog,
   "------------------------------------------------------------------\n");
   fprintf(flog, "ID: %-42s FILE:  %s\n", sum->comment, ext->pname);
   fprintf(flog,
   "==================================================================\n");
   fprintf(flog, "     Process Information\n");
   fprintf(flog, "PROCESS FSP %-6ld", sum->fsp);
   if(hd->fn)
      fprintf(flog, "  FFN %-6ld", sum->ffn);
   if (hd->jd)
      fprintf(flog, "  JD %3.3d", sum->fsp_date);
   if (hd->tm)
      fprintf(flog, "  Time %s", sum->fsp_tim);
   fwrite("\n", sizeof (char), 1, flog);
   if(hd->wa)
      fprintf(flog, "Water Depth %-5.1f %-14s", sum->fsp_wat,
                                             units[pd->loc_dp]);
   if(hd->syhd)
      fprintf(flog, "SysHead %05.1f Degrees", sum->fsp_hed);
   if((hd->wa)||(hd->syhd)) fwrite("\n", sizeof (char), 1, flog);
   fprintf(flog, "PROCESS LSP %-6ld", sum->lsp);
   if(hd->fn)
      fprintf(flog, "  LFN %-6ld", sum->lfn);
   if (hd->jd)
      fprintf(flog, "  JD %3.3d", sum->lsp_date);
   if (hd->tm)
      fprintf(flog, "  Time %s", sum->lsp_tim);
   fwrite("\n", sizeof (char), 1, flog);
   if(hd->wa)
      fprintf(flog, "Water Depth %-5.1f %-14s", sum->lsp_wat,
                                             units[pd->loc_dp]);
   if(hd->syhd)
      fprintf(flog, "SysHead %05.1f Degrees", sum->lsp_hed);
   if((hd->wa)||(hd->syhd)) fwrite("\n", sizeof (char), 1, flog);
   if(hd->tm)
      fprintf(flog, "Line Minutes: %-7d", sum->selln_tim);
   fprintf(flog, "Total SP: %-6ld   Kms: %.3f", sum->sel_sp, sum->selln_len);
   fwrite("\n", sizeof (char), 1, flog);
   if(hd->tm)
      fprintf(flog, "Average Speed %-5.2f %-7s",
                               sum->selav_spd, units[pd->sp_un]);
   fprintf(flog, "   Change Flag: SPs %01s", answer[sum->sp_change]);
   if(hd->fn)
      fprintf(flog, "  FNs %01s", answer[sum->fn_change]);
   fwrite("\n", sizeof (char), 1, flog);
   fprintf(flog, "SP Processed: %-16ld", sum->procno);
   fprintf(flog, "SP Corrupt: %ld\n", sum->currno);
   fprintf(flog,
   "------------------------------------------------------------------\n");
   if(hd->cl || hd->ln || hd->ar || hd->vs) {
      fprintf(flog, "     Contractor Header Data\n");
      if(hd->cl)
         fprintf(flog, "Client: %-19s", cl_name);
      if(hd->ar)
         fprintf(flog, "Area: %s", ar_name);
      if(hd->cl || hd->ar)
         fwrite("\n", sizeof (char), 1, flog);
      if(hd->ln)
         fprintf(flog, "Line: %-19s", ln_name);
      if(hd->vs)
         fprintf(flog, "Vessel: %s", vs_name);
      if(hd->ln || hd->vs)
         fwrite("\n", sizeof (char), 1, flog);
      fprintf(flog,
   "------------------------------------------------------------------\n");
   }
   fprintf(flog, "     Headstat Configured Data\n");
   fprintf(flog, "%-30s %s\n", pd->cli_name, pd->pro_name);
   fprintf(flog, "%-30s %s\n", pd->con_name, pd->qc_com1);
   fprintf(flog, "%-30s %s\n", pd->qc_com2, pd->qc_com3);
   fprintf(flog, "%-30s Vessels: %u\n", pd->qc_com4, pd->vsl_numb);
   fprintf(flog,
   "------------------------------------------------------------------\n");
   if(pd->loc_strm || pd->rem_strm)
      fprintf(flog, "     Cable Data\n");
   if(pd->loc_strm) {
      fprintf(flog, "Local Cables:   %u", pd->loc_strm);
      for(i=1;i<=(int)pd->loc_strm;i++) {
         if(i==1) fprintf(flog, "  %s", pd->locab1);
         if(i==2) fprintf(flog, "  %s", pd->locab2);
         if(i==3) fprintf(flog, "  %s", pd->locab3);
         if(i==4) fprintf(flog, "  %s", pd->locab4);
      }
      fwrite("\n", sizeof (char), 1, flog);
   }
   if(pd->rem_strm) {
      fprintf(flog, "Remote Cables:  %u", pd->rem_strm);
      for(i=1;i<=(int)pd->rem_strm;i++) {
         if(i==1) fprintf(flog, "  %s", pd->remcab1);
         if(i==2) fprintf(flog, "  %s", pd->remcab2);
         if(i==3) fprintf(flog, "  %s", pd->remcab3);
         if(i==4) fprintf(flog, "  %s", pd->remcab4);
      }
      fwrite("\n", sizeof (char), 1, flog);
   }
   if(pd->loc_strm) {
      fprintf(flog, "Cable Controller: ");
      if (hd->contr == 0)   /* if GECO */
         fprintf(flog, "%-14s", formats[hd->ca]);
      if (hd->contr == 1)   /* if PRAKLA */
         fprintf(flog, "SYN V2.3      ");
      if (hd->contr == 2)   /* if WESTERN */
         fprintf(flog, "WESTERN %-6u", hd->west + 1);
      if (hd->contr == 5)   /* if DIGICON TRACE0 */
         fprintf(flog, "TRACE0        ");
      fprintf(flog, "Units: %u\n", ext->numberru);
      if(andef->dep_anl1 && hd->ca)
         fprintf(flog, "Depth Spec 1:  %.1f <> %-.1f %s\n",
                            spc->d1min, spc->d1max, units[pd->loc_dp]);
      if(andef->dep_anl2 && hd->ca)
         fprintf(flog, "Depth Spec 2:  %.1f <> %-.1f %s\n",
                            spc->d2min, spc->d2max, units[pd->loc_dp]);
      if(!hd->contr && andef->wi_anl && hd->ca) {
         fprintf(flog, "Wing Angles Controller: %-14s Units: %u\n",
               formats[hd->ca], ext->numberwing);
         fprintf(flog, "Spec:  +/- %.1f Degrees\n", spc->wing);
      }
   }
   fprintf(flog,
   "------------------------------------------------------------------\n");
   if(pd->loc_srce || pd->rem_srce)
      fprintf(flog, "     Source Data\n");
   if(pd->loc_srce) {
      fprintf(flog, "Local Sources:  %u", pd->loc_srce);
      for(i=1;i<=(int)pd->loc_srce;i++) {
         if(i==1) fprintf(flog, "  %s", pd->loc1);
         if(i==2) fprintf(flog, "  %s", pd->loc2);
         if(i==3) fprintf(flog, "  %s", pd->loc3);
         if(i==4) fprintf(flog, "  %s", pd->loc4);
      }
      fwrite("\n", sizeof (char), 1, flog);
   }
   if(pd->rem_srce) {
      fprintf(flog, "Remote Sources: %u", pd->rem_srce);
      for(i=1;i<=(int)pd->rem_srce;i++) {
         if(i==1) fprintf(flog, "  %s", pd->rem1);
         if(i==2) fprintf(flog, "  %s", pd->rem2);
         if(i==3) fprintf(flog, "  %s", pd->rem3);
         if(i==4) fprintf(flog, "  %s", pd->rem4);
      }
      fwrite("\n", sizeof (char), 1, flog);
   }
   if(pd->loc_srce && hd->gu) {
      fprintf(flog, "Gun Controller: ");
      if (hd->contr == 0)   /* if GECO */
         fprintf(flog, "%-14s", formats[hd->gu]);
      if (hd->contr == 1)   /* if PRAKLA */
         fprintf(flog, "SYN V2.3      ");
      if (hd->contr == 2)   /* if WESTERN */
         fprintf(flog, "WESTERN %-6u", hd->west + 1);
      if (hd->contr == 5)  /* if DIGICON TRACE0 */
         fprintf(flog, "TRACE0        ");
      fprintf(flog, "Guns: %u\n", ext->numbergun);
      if( (andef->gun_anl1)||(andef->gun_anl2) )
         fprintf(flog, "Timing");
      if(andef->gun_anl1)
         fprintf(flog, " Spec 1: +/- %.1f mS", spc->gun1);
      if(andef->gun_anl2)
         fprintf(flog, "    Spec 2: +/- %.1f mS", spc->gun2);
      if(andef->gun_anl1 || andef->gun_anl2)
         fwrite("\n", sizeof (char), 1, flog);
      if(andef->vol_anl1 || andef->vol_anl2)
         fprintf(flog, "Volume");
      if(andef->vol_anl1)
         fprintf(flog, " Spec 1: %u %s",
                             spc->gvol1, units[pd->vol_un]);
      if(andef->vol_anl2)
         fprintf(flog, "   Spec 2: %u %s",
                             spc->gvol2, units[pd->vol_un]);
      if( (andef->vol_anl1)||(andef->vol_anl2) )
         fwrite("\n", sizeof (char), 1, flog);
      if(hd->ma && (hd->contr != 2) ) { /* NOT Western */
         if(andef->man_anl1 || andef->man_anl2)
            fprintf(flog, "Pressure");
         if(andef->man_anl1)
            fprintf(flog, " Spec 1: %u %s",
                                spc->ma1, units[pd->pr_un]);
         if(andef->man_anl2)
            fprintf(flog, "   Spec 2: %u %s",
                                spc->ma2, units[pd->pr_un]);
         if(andef->man_anl1 || andef->man_anl2)
            fwrite("\n", sizeof (char), 1, flog);
      }
      if(hd->gd && andef->gd_anl) {
         fprintf(flog, "Gun Depths: ");
         if (hd->contr == 0)   /* if GECO */
            fprintf(flog, "%-14s", formats[hd->gd]);
         if (hd->contr == 1)   /* if PRAKLA */
            fprintf(flog, "SYN V2.3      ");
         if (hd->contr == 2)   /* if WESTERN */
            fprintf(flog, "WESTERN %-6u", hd->west + 1);
         if (hd->contr == 5)   /* if DIGICON TRACE0 */
            fprintf(flog, "TRACE0        ");
         fprintf(flog, "Units: %u\n", ext->numberdp);
         fprintf(flog, "Gun Depth Spec: %4.1f <> %-4.1f %s\n",
                    spc->gd_min, spc->gd_max, units[pd->loc_dp]);
      }
   }
   if(pd->seq_len)
      fprintf(flog, "Sequence Length: %u\n", pd->seq_len);
   if(!hd->contr && andef->seq_anl && hd->gm) {
      fprintf(flog, "Nominal Initial Sequence\n");
      info = fire;
      for(i=0;i<(int)pd->seq_len; i++) {
         p = (int) spc->f_seq[i] - 48;
         if(p<=(int)pd->loc_srce) {
            if(p==1)
               fprintf(flog, "    %s", pd->loc1);
            if(p==2)
               fprintf(flog, "    %s", pd->loc2);
            if(p==3)
               fprintf(flog, "    %s", pd->loc3);
            if(p==4)
               fprintf(flog, "    %s", pd->loc4);
         } else {
            p = p - pd->loc_srce;
            if(p==1)
               fprintf(flog, "    %s", pd->rem1);
            if(p==2)
               fprintf(flog, "    %s", pd->rem2);
            if(p==3)
               fprintf(flog, "    %s", pd->rem3);
            if(p==4)
               fprintf(flog, "    %s", pd->rem4);
         }
      }
      fwrite("\n", sizeof (char), 1, flog);
   }
   fprintf(flog,
   "------------------------------------------------------------------\n");
   fprintf(flog, "     General Information\n");
   fprintf(flog, "Depth: %s  Pressure: %s",
      units[pd->loc_dp], units[pd->pr_un]);
   fprintf(flog, "  Volume: %s  Speed: %s\n",
      units[pd->vol_un], units[pd->sp_un]);
   fprintf(flog, "SP Interval: %.2f  Metres\n", pd->sp_dst);
   if(hd->wa)
      fprintf(flog,
			"Water Depth Multiplier: %-.2f  +  Draft: %+-.2f  %s\n",
            opt->mul_wadep, opt->draft, units[pd->loc_dp]);
   if(andef->gd_anl && hd->gd)
      fprintf(flog,
         "Gun Depth Multiplier: %-.2f  +  Offset: %-.2f  %s\n",
            opt->mul_gundep, opt->gundep_ofst, units[pd->loc_dp]);
   if( (andef->dep_anl1 || andef->dep_anl2) && hd->ca)
      fprintf(flog, "Cable Depth Multiplier: %-.2f\n", opt->mul_cabdep);
	if( andef->ws_anl && hd->ws )
		fprintf(flog, "Water Speed Correction: %+-4.2f  %s\n",
							opt->wasp_ofst, units[pd->sp_un]);
	fprintf(flog, "Analysis run on %s @ %s\n",
                     GetFDate(ext->a_str, 26), GetFTime(ext->b_str, 6) );
   fprintf(flog,
   "------------------------------------------------------------------\n\f");
         /* page 2 - actual statistics */
   fprintf(flog,
   "==================================================================\n");
   fprintf(flog, "LINE STATISTICS\n");
   fprintf(flog,
   "==================================================================\n");
      /* SP & FN numbering */
   if(andef->sp_anl || andef->fn_anl) {
      if (andef->sp_anl)
         fprintf(flog, "     SP Numbering");
            /* FN numbering Geco & Digicon Trace0 only */
      if ( (!hd->contr || hd->contr == 5) && andef->fn_anl && hd->fn)
         fprintf(flog, "                  FN Numbering");
      fwrite("\n", sizeof (char), 1, flog);
      if (andef->sp_anl)
         fprintf(flog, "Jump: %-6ld Freeze: %-9ld",
                                           sps->ju_mp, sps->no_ch);
            /* FN numbering Geco & Digicon Trace0 only */
      if ( (!hd->contr || hd->contr == 5) && andef->fn_anl && hd->fn)
         fprintf(flog, "Jump: %-6ld Freeze: %ld",
                                         fns->ju_mp, fns->no_ch);
      fwrite("\n\n", sizeof (char), 2, flog);
   }
      /* cable depth controller comms status */
   if (!hd->contr && andef->dep_com && ext->numberru && hd->ca) {
      fprintf(flog, "     Cable RU Comms\n");
      fprintf(flog,
       "SPs with an Error: %-6ld    Bad Readings: %ld\n\n",
               dco->flag_err, dco->tot_err);
   }
      /* cable & individual controller depths */
   cab_log (flog);
      /* cable depth controller wing angles */
   if(!hd->contr &&  andef->wi_anl && ext->numberwing && hd->wi) {
      wistat = stwi;
      fprintf(flog, "     Wing Angles\n");
      fprintf(flog,
   "------------------------------------------------------------------\n");
      fprintf(flog,
 "UNIT      UNIT     /---- ANGLES ----\\     OUT-OF    %%     AVERAGE\n");
      fprintf(flog,
 "NAME      NUMBER   MIN     MEAN    MAX    SPEC %%  ACTIVE  CHANGE\n");
      fprintf(flog,
   "------------------------------------------------------------------\n");
      for(i=0;i<ext->numberwing;i++) {
         fprintf(flog, "%-10s %-7u", wistat->name, wistat->num);
         fprintf(flog, "%+-8.1f", wistat->min);
         wing_mean = wistat->mean/(float)sum->procno;
         fprintf(flog, "%+-8.1f", wing_mean);
         fprintf(flog, "%+-8.1f",wistat->max);
         fprintf(flog, "%-8.1f", 100.0F*(float)wistat->spec/(float)sum->procno);
         fprintf(flog, "%-8.1f", 100.0F*(float)wistat->on/(float)sum->procno);
         fprintf(flog, "%-8.2f\n", wistat->change/(float)wistat->on);
         wistat = wistat->next;
      }
   fprintf(flog,
   "------------------------------------------------------------------\n\n");
   }
      /* source timing & volumes */
   srce_log (flog);
      /* air pressure */
   if(hd->ma &&  (hd->contr != 2 ) && (andef->man_anl1 || andef->man_anl2) ) {
      fprintf(flog, "     Air Pressure\n");
		fprintf(flog,
         "Min: %-8u Mean: %-8lu Max: %-6u %s\n", man->min,
            man->total/(unsigned long)sum->procno,
               man->max, units[pd->pr_un]);
      fprintf(flog, "SOL: %-8u EOL: %-6u %s\n",
             man->sol, man->eol, units[pd->pr_un]);
      if(andef->man_anl1)
        fprintf(flog, "SP < %u %s: %.1f %%       ",
           spc->ma1, units[pd->pr_un],
              ((float) man->man1/(float)sum->procno)*100.0F);
      if(andef->man_anl2)
        fprintf(flog, "SP < %u %s: %.1f %%",
           spc->ma2, units[pd->pr_un],
              ((float) man->man2/(float)sum->procno)*100.0F);
		fwrite("\n\n", sizeof (char), 2, flog);
   }
      /* gun masks */
   if(!hd->contr && andef->gmsk_anl && hd->gm) {
      fprintf(flog,"     Gun Masks\n");
      fprintf(flog, "Processed: %-20ld", sum->procno);
      fprintf(flog, "SP > 1 Source Fired: %-4u\n", gmsk->tot_ovr);
      fprintf(flog, "SP NO Source Fired: %-11u", gmsk->tot_none);
      fprintf(flog, "SP < %3u Guns Fired: %u\n",
         spc->gmsk, gmsk->tot_set);
      if(pd->loc_srce)           /* if at least 1 local source */
		  fprintf(flog,
        "%s  SP Overfired: %-4u & SP < Min Guns: %u\n", pd->loc1,
			  gmsk->ovr_loc1, gmsk->loc1);
      if(pd->loc_srce > 1)           /* if at least 2 local sources */
		  fprintf(flog,
        "%s  SP Overfired: %-4u & SP < Min Guns: %u\n", pd->loc2,
			  gmsk->ovr_loc2, gmsk->loc2);
      if(pd->rem_srce)           /* if at least 1 remote source */
		  fprintf(flog,
        "%s  SP Overfired: %-4u & SP < Min Guns: %u\n", pd->rem1,
			  gmsk->ovr_rem1, gmsk->rem1);
      if(pd->rem_srce > 1)           /* if at least 2 remote sources */
		  fprintf(flog,
        "%s  SP Overfired: %-4u & SP < Min Guns: %u\n", pd->rem2,
			  gmsk->ovr_rem2, gmsk->rem2);
		  fwrite("\n", sizeof (char), 1, flog);
   }
         /* firing sequence */
   if(!hd->contr && andef->seq_anl && hd->gm) {
      fprintf(flog, "     Firing Sequence\n");
      fprintf(flog,
   "------------------------------------------------------------------\n");
      fprintf(flog,
    "NOMINAL     SP     FN     TIME      ACTUAL SOURCE(S) FIRED\n");
      fprintf(flog,
   "------------------------------------------------------------------\n");
      fire_buf_tmp = fire_buf;
      for(i=0;i<(int)pd->seq_len; i++) {
         p = (int) spc->f_seq[i] - 48;
         if(p<=(int)pd->loc_srce) {
            if(p==1)
               strcpy(ext->a_str, pd->loc1);
            if(p==2)
               strcpy(ext->a_str, pd->loc2);
            if(p==3)
               strcpy(ext->a_str, pd->loc3);
            if(p==4)
               strcpy(ext->a_str, pd->loc4);
         } else {
            p = p - pd->loc_srce;
            if(p==1)
               strcpy(ext->a_str, pd->rem1);
            if(p==2)
               strcpy(ext->a_str, pd->rem2);
            if(p==3)
               strcpy(ext->a_str, pd->rem3);
            if(p==4)
               strcpy(ext->a_str, pd->rem4);
         }
         fprintf(flog, "%s", ext->a_str);
         fprintf(flog, "    %s\n", fire_buf_tmp->fire_info);
         fire_buf_tmp++;
      }
      fprintf(flog,
   "------------------------------------------------------------------\n");
      if(st_seq.change)
         change = (st_seq.change - 1)/4 + 1;
      if(st_seq.reset)
         reset = (st_seq.reset - 1)/4 + 1;
      fprintf(flog,  "Sequence Changes: %-8u", change);
      fprintf(flog, "Sequence Resets: %u\n\n", reset);
   }
         /* source depths */
   if (andef->gd_anl && hd->gd && ext->numberdp && pd->loc_srce) {
         /* source 1 */
      fprintf(flog, "     %s Source Gun Depths\n", pd->loc1);
		fprintf(flog,
         "Spec: %4.1f <> %-4.1f %-10s SP < Min: %-8ld SP > Max: %ld\n",
		 spc->gd_min, spc->gd_max, units[pd->loc_dp], sr1->sm_sp, sr1->gr_sp);
      fprintf(flog, "MIN DEPTH: %-12.1f MAX DEPTH: %.1f\n",
			sr1->min_dep, sr1->max_dep);
         /* source 2 */
      if (pd->loc_srce > 1) {
         fprintf(flog, "     %s Source Gun Depths\n", pd->loc2);
         fprintf(flog,
            "Spec: %4.1f <> %-4.1f %-10s SP < Min: %-8ld SP > Max: %ld\n",
		  spc->gd_min, spc->gd_max, units[pd->loc_dp], sr2->sm_sp, sr2->gr_sp);
         fprintf(flog, "MIN DEPTH: %-12.1f MAX DEPTH: %.1f\n",
            sr2->min_dep, sr2->max_dep);
      }
         /* source 3 */
      if (pd->loc_srce > 2) {
         fprintf(flog, "     %s Source Gun Depths\n", pd->loc3);
         fprintf(flog,
            "Spec: %4.1f <> %-4.1f %-10s SP < Min: %-8ld SP > Max: %ld\n",
		  spc->gd_min, spc->gd_max, units[pd->loc_dp], sr3->sm_sp, sr3->gr_sp);
         fprintf(flog, "MIN DEPTH: %-12.1f MAX DEPTH: %.1f\n",
            sr3->min_dep, sr3->max_dep);
      }
         /* source 4 */
      if (pd->loc_srce > 3) {
         fprintf(flog, "     %s Source Gun Depths\n", pd->loc4);
         fprintf(flog,
            "Spec: %4.1f <> %-4.1f %-10s SP < Min: %-8ld SP > Max: %ld\n",
		  spc->gd_min, spc->gd_max, units[pd->loc_dp], sr4->sm_sp, sr4->gr_sp);
         fprintf(flog, "MIN DEPTH %-12.1f MAX DEPTH: %.1f\n",
            sr4->min_dep, sr4->max_dep);
      }
      fwrite("\n", sizeof (char), 1, flog);
      /* individual source depth units */
		gdstat = stgundp;
      fprintf(flog, "     Individual Source Depth Units\n");
		fprintf(flog,
   "--------------------------------------------------------------\n");
		fprintf(flog,
   "UNIT          UNIT      SOURCE    SP OUTSIDE     DEPTH  RANGE\n");
		fprintf(flog,
   "NAME          NUMBER    NUMBER     WINDOW       MIN   <>   MAX\n");
      for(i=0;i<ext->numberdp;i++) {
         fprintf(flog, "%-10s      %-3u       %-2u",
			gdstat->gundp_name, gdstat->gundp_num, gdstat->srce);
         fprintf(flog, "         %-5u", gdstat->tot_sp);
         fprintf(flog, "    %5.1f   <>   %-5.1f", gdstat->min_dep,
							  gdstat->max_dep);
			fwrite("\n", sizeof (char), 1, flog);
			gdstat = gdstat->next;
      }
		fprintf(flog,
   "--------------------------------------------------------------\n\n");
   }
      /* clock timing */
   if( (hd->contr != 2) && andef->clk_anl && hd->clk) {  /* NOT Western */
      if( (hd->clk==18)||(hd->clk==19)||(hd->clk==21) )   /* GECO */
         fprintf(flog, "     Intervessel Timing\n");
      if(hd->clk == 71)   /* DIGICON TRACE0 TB ECHO */
         fprintf(flog, "     TB Echo Timing\n");
      if(hd->clk == 91)   /* PRAKLA ADDED FTB TIMES (1+2+3) */
         fprintf(flog, "     FTB Timing\n");
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
      fprintf(flog, "Min: %-10.1f Mean: %-10.1f Max: %-8.1f mS\n",
          t_clk->min, t_clk->mean, t_clk->max);
      fprintf(flog, "Corrupt: %-6u Valid: %u\n",
         t_clk->corrupt, t_clk->tot_anl);
      fprintf(flog, "Corrupt: %.1f %%", t_clk->per_corr);
      fprintf(flog, "   Spec: %.1f mS +/- %.1f mS\n",
         spc->clk, spc->clk_ra);
      fprintf(flog, "SP < Spec: %-4u SP > Spec: %u\n",
         t_clk->sm_spec, t_clk->gr_spec);
		fprintf(flog,
            "Valid Times Outside Spec: %-4.1f %%\n\n", t_clk->per_spec);
   }
      /* water depths */
   if(andef->wdp_anl && hd->wa) {
      fprintf(flog, "    Water Depths\n");
		fprintf(flog,
            "Min: %-10.1f Max: %-8.1f %s\n", wat->min,
					  wat->max, units[pd->loc_dp]);
		fprintf(flog,
            "Spec: %5.1f <> %-5.1f %-10s Outside Window:  %u\n\n",
                 spc->wd_min, spc->wd_max, units[pd->loc_dp], wat->err);
   }
      /* SP time intervals */
   if(andef->tim_anl && hd->tm) {
      fprintf(flog, "    SP Time Intervals\n");
      if(!t_int->tot_anl)
         t_int->mean = 0.0F;
      else
         t_int->mean = t_int->total/ (float)t_int->tot_anl;
      fprintf(flog,
            "Min:  %-10.2f Mean: %-10.2f Max: %-8.2f SEC\n",
                t_int->tmin, t_int->mean, t_int->tmax);
      fprintf(flog,
            "Corrupt: %-7u Valid: %u\n",
             t_int->corrupt, t_int->tot_anl);
      fprintf(flog,
            "%u SP < %.2f SEC     %u SP > %.2f SEC\n\n",
             t_int->sm_spec, spc->t_min, t_int->gr_spec, spc->t_max);
   }
         /* speed - GECO & DIGICON TRACE0 only */
   if( (!hd->contr || hd->contr == 5) && andef->gsp_anl && hd->gs) {
      fprintf(flog, "     Speed Along Track\n");
      fprintf(flog,
            "Min: %-10.2f Mean: %-10.2f Max: %-6.2f %s\n",
                gsp.min, gsp.tot/(float)gsp.read, gsp.max, units[pd->sp_un]);
      fprintf(flog, "Corrupt: %-6u Analysed: %-6u DMean: %-6.2f %s\n",
                gsp.corr, gsp.read,
                   gsp.change/(float)gsp.read, units[pd->sp_un]);
      fprintf(flog,
            "SPs > %-.2f %s: %-.1f %%      SOL: %-.1f   EOL: %.1f   %s\n\n",
         spc->gsp, units[pd->sp_un], 100.0F*(float)gsp.spec/(float)gsp.read,
               gsp.sol, gsp.eol, units[pd->sp_un]);
	}
			/* water speed - GECO only */
	if( !hd->contr && andef->ws_anl && hd->ws) {
		fprintf(flog, "     Water Speed\n");
		fprintf(flog,
				"Min: %-10.2f Mean: %-10.2f Max: %-6.2f %s\n",
					 ws.min, ws.tot/(float)ws.read, ws.max, units[pd->sp_un]);
		fprintf(flog, "Corrupt: %-6u Analysed: %-6u DMean: %-6.2f %s\n",
					 ws.corr, ws.read,
						 ws.change/(float)ws.read, units[pd->sp_un]);
		fprintf(flog,
				"SP > %-.2f %s: %-.1f %%      SOL: %-.1f   EOL: %.1f   %s\n\n",
			spc->gsp, units[pd->sp_un], 100.0F*(float)ws.spec/(float)ws.read,
					ws.sol, ws.eol, units[pd->sp_un]);
	}
			/* current - GECO only */
	if( !hd->contr && andef->ws_anl
						&& hd->ws && hd->gs && andef->gsp_anl) {
		fprintf(flog, "     Current\n");
		fprintf(flog,
		 "Min: %+-10.2f Max: %+-10.2f SOL: %+-6.2f EOL: %+-8.2f %s\n",
					cu.min, cu.max, cu.sol, cu.eol, units[pd->sp_un]);
		fprintf(flog,
				"Corrupt: %-6u Valid: %u\n\n", cu.corr, cu.read);
	}
	fprintf(flog,
   "------------------------------------------------------------------\n");
   fprintf(flog, "%s %s %s ANALYSIS COMPLETE\n",
                       ext->pname, sum->comment, ver);
   fprintf(flog,
   "==================================================================\n");
   if(flog) fclose (flog);  /* close the logging file if opened */
	return;
}


/***************************************************************
*   cable & individual controller depths & comms
****************************************************************/
void cab_log (FILE *flog)
{
   register int i;
   struct CAB_DEF *cstat=NULL;      /* temp struct for RUs */
   if ( (andef->dep_anl1 || andef->dep_anl2) && ext->numberru
                                          && hd->ca && pd->loc_strm) {
         /* cable 1 depth */
      fprintf(flog, "     %s Cable Depths\n", pd->locab1);
      if(andef->dep_anl1)
         fprintf(flog,
   "Spec1: %4.1f <> %-4.1f %-8s SP < Min: %-6ld SP > Max: %ld\n",
      spc->d1min, spc->d1max, units[pd->loc_dp], dp1->sm_sp1, dp1->gr_sp1);
      if(andef->dep_anl2)
         fprintf(flog,
   "Spec2: %4.1f <> %-4.1f %-8s SP < Min: %-6ld SP > Max: %ld\n",
      spc->d2min, spc->d2max, units[pd->loc_dp], dp1->sm_sp2, dp1->gr_sp2);
      if(andef->dep_com)
         fprintf(flog,"SP with Comms Error: %-7ld Bad Readings: %ld\n",
            dp1->flag_err, dp1->tot_err);
      fprintf(flog, "MIN DEPTH: %-17.1f MAX DEPTH: %.1f\n\n",
         dp1->min_dep, dp1->max_dep);
         /* cable 2 */
      if (pd->loc_strm > 1) {
         fprintf(flog, "     %s Cable Depths\n", pd->locab2);
         if(andef->dep_anl1)
            fprintf(flog,
            "Spec1: %4.1f <> %-4.1f %-8s SP < Min: %-6ld SP > Max: %ld\n",
      spc->d1min, spc->d1max, units[pd->loc_dp], dp2->sm_sp1, dp2->gr_sp1);
         if(andef->dep_anl2)
            fprintf(flog,
            "Spec2: %4.1f <> %-4.1f %-8s SP < Min: %-6ld SP > Max: %ld\n",
      spc->d2min, spc->d2max, units[pd->loc_dp], dp2->sm_sp2, dp2->gr_sp2);
         if(andef->dep_com)
            fprintf(flog, "SP with Comms Error: %-7ld Bad Readings: %ld\n",
               dp2->flag_err, dp2->tot_err);
         fprintf(flog, "MIN DEPTH: %-17.1f MAX DEPTH: %.1f\n\n",
            dp2->min_dep, dp2->max_dep);
      }
         /* cable 3 */
      if (pd->loc_strm > 2) {
         fprintf(flog, "     %s Cable Depths\n", pd->locab3);
         if(andef->dep_anl1)
            fprintf(flog,
            "Spec1: %4.1f <> %-4.1f %-8s SP < Min: %-6ld SP > Max: %ld\n",
      spc->d1min, spc->d1max, units[pd->loc_dp], dp3->sm_sp1, dp3->gr_sp1);
         if(andef->dep_anl2)
            fprintf(flog,
            "Spec2: %4.1f <> %-4.1f %-8s SP < Min: %-6ld SP > Max: %ld\n",
      spc->d2min, spc->d2max, units[pd->loc_dp], dp3->sm_sp2, dp3->gr_sp2);
         if(andef->dep_com)
            fprintf(flog, "SP with Comms Error: %-7ld Bad Readings: %ld\n",
               dp3->flag_err, dp3->tot_err);
         fprintf(flog, "MIN DEPTH: %-17.1f MAX DEPTH: %.1f\n\n",
            dp3->min_dep, dp3->max_dep);
      }
         /* cable 4 */
      if (pd->loc_strm > 3) {
         fprintf(flog, "     %s Cable Depths\n", pd->locab4);
         if(andef->dep_anl1)
            fprintf(flog,
            "Spec1: %4.1f <> %-4.1f %-8s SP < Min: %-6ld SP > Max: %ld\n",
      spc->d1min, spc->d1max, units[pd->loc_dp], dp4->sm_sp1, dp4->gr_sp1);
         if(andef->dep_anl2)
            fprintf(flog,
            "Spec2: %4.1f <> %-4.1f %-8s SP < Min: %-6ld SP > Max: %ld\n",
       spc->d2min, spc->d2max, units[pd->loc_dp], dp4->sm_sp2, dp4->gr_sp2);
         if(andef->dep_com)
            fprintf(flog, "SP with Comms Error: %-7ld Bad Readings: %ld\n",
               dp4->flag_err, dp4->tot_err);
         fprintf(flog, "MIN DEPTH: %-17.1f MAX DEPTH: %.1f\n\n",
            dp4->min_dep, dp4->max_dep);
      }
         /* individual cable depth units */
      cstat = stcab;
      fprintf(flog, "     Individual Cable Depths\n");
      fprintf(flog,
   "------------------------------------------------------------------\n");
      fprintf(flog,
    "UNIT       UNIT    COMMUNICATIONS   OUT-OF-SPEC      DEPTH  RANGE\n");
      fprintf(flog,
    "NAME       NUMBER   BAD  %% BAD      SPEC1 SPEC2     MIN   <>   MAX\n");
      fprintf(flog,
   "------------------------------------------------------------------\n");
      for(i=0;i<ext->numberru;i++) {
         fprintf(flog, "%-10s  %-3u", cstat->cab_name, cstat->cab_num);
         if(andef->dep_com) {
            if(cstat->tot_err) cstat->per_c =
                  100.0F*(float)cstat->tot_err/(float)sum->procno;
            else cstat->per_c = 0.0F;
            fprintf(flog, "     %-5u", cstat->tot_err);
            fprintf(flog, " %-4.1f", cstat->per_c);
         } else fprintf(flog, "               ");
         if(andef->dep_anl1) fprintf(flog, "      %-5u", cstat->tot_sp1);
         else fprintf(flog, "           ");
         if(andef->dep_anl2) fprintf(flog, "  %-5u", cstat->tot_sp2);
         else fprintf(flog, "       ");
         fprintf(flog, "   %5.1f  <>  %-5.1f", cstat->min_dep, cstat->max_dep);
         fwrite("\n", sizeof (char), 1, flog);
         cstat = cstat->next;
      }
      fprintf(flog,
   "------------------------------------------------------------------\n\n");
   }
}


/***************************************************************
*   source timing & volumes
****************************************************************/
void srce_log (FILE *flog)
{
   struct SRCE_DEF *gstat=NULL;     /* ditto for guns */
   register int i;
   float gun_perc;                  /* gun misfire percents */
   if( (andef->gun_anl1 || andef->gun_anl2) && ext->numbergun
                                   && hd->gu && pd->loc_srce) {
         /* overall source */
      fprintf(flog, "     Overall Source\n");
      fprintf(flog, "SP No Local Source Fired: %-7u", xsr->no_fire);
      fprintf(flog, "SP > 1 Local Source Fired: %u\n", xsr->x_fire);
      fprintf(flog,
      "SPs Included in Misfire %% (at least 1 gun fired): %u\n",
         xsr->tot_sp);
      if(andef->gun_anl1) {
         fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun1);
         fprintf(flog,
          "    SP Outside Window: %-6u",
             xsr->spec1);
         fprintf(flog, "Misfires:");
         if(xsr->tot_sp)
            xsr->p_spec1 = ((float)xsr->spec1)*100.0F/((float)xsr->tot_sp);
         else xsr->p_spec1 = 0.0F;
         fprintf(flog, " %.1f %%\n", xsr->p_spec1);
      }
      if(andef->gun_anl2) {
         fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun2);
         fprintf(flog,
          "    SP Outside Window: %-6u",
             xsr->spec2);
         fprintf(flog, "Misfires:");
         if(xsr->tot_sp)
            xsr->p_spec2 = ((float)xsr->spec2)*100.0F/((float)xsr->tot_sp);
         else xsr->p_spec2 = 0.0F;
         fprintf(flog, " %.1f %%\n", xsr->p_spec2);
      }
      fwrite("\n", sizeof (char), 1, flog);
         /* source 1 */
      fprintf(flog, "     %s Source\n", pd->loc1);
      fprintf(flog, "Source Fired: %-9u", sr1->en_num);
      fprintf(flog, "NOFIRE/AUTOFIRE: %u\n", sr1->tot_mf);
      if(andef->gun_anl1) {
         fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun1);
         fprintf(flog,
          "    SP Outside Window: %-6u",
             sr1->sp1);
         fprintf(flog, "Misfires:");
         if(sr1->en_num)
             sr1->p_spec1 = ((float)sr1->sp1)*100.0F/((float)sr1->en_num);
         else sr1->p_spec1 = 0.0F;
         fprintf(flog, " %.1f %%\n", sr1->p_spec1);
         if (andef->mf1_anl) {
            fprintf(flog,
          "SP With More Than %u Consecutive Misfires > %-.2f mS: %u\n",
                  spc->mf1, spc->gun1, msf->s1ct1);
         }
         if (andef->mf3_anl) {
            fprintf(flog,
          "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u\n",
                  spc->mf3, spc->mf3_ra, spc->gun1, msf->s1mt1);
         }
      }
      if(andef->gun_anl2) {
         fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun2);
         fprintf(flog,
          "    SP Outside Window: %-6u",
             sr1->sp2);
         fprintf(flog, "Misfires:");
         if(sr1->en_num)
             sr1->p_spec2 = ((float)sr1->sp2)*100.0F/((float)sr1->en_num);
         else sr1->p_spec2 = 0.0F;
         fprintf(flog, " %.1f %%\n", sr1->p_spec2);
         if (andef->mf1_anl) {
            fprintf(flog,
          "SP With More Than %u Consecutive Misfires > %-.2f mS: %u\n",
                  spc->mf1, spc->gun2, msf->s1ct2);
         }
         if (andef->mf3_anl) {
            fprintf(flog,
          "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u\n",
                  spc->mf3, spc->mf3_ra, spc->gun2, msf->s1mt2);
         }
      }
      if(andef->vol_anl1 || andef->vol_anl2) {
         fprintf(flog, "Volume SOL: %-9u", sr1->sol_vol);
         fprintf(flog, "EOL: %u %s\n" , sr1->sp_vol,
                                            units[pd->vol_un]);
         if(andef->vol_anl1) {
            fprintf(flog,
               "SP < %u %s: %-10u",
                  spc->gvol1, units[pd->vol_un], sr1->vol1);
         }
         if(andef->vol_anl2) {
            fprintf(flog,
               "SP < %u %s: %u",
                  spc->gvol2, units[pd->vol_un], sr1->vol2);
         }
         fwrite("\n", sizeof (char), 1, flog);
      }
      fwrite("\n", sizeof (char), 1, flog);
         /* source 2 */
      if (pd->loc_srce > 1) {
         fprintf(flog, "     %s Source\n", pd->loc2);
         fprintf(flog, "Source Fired: %-9u", sr2->en_num);
         fprintf(flog, "NOFIRE/AUTOFIRE: %u\n", sr2->tot_mf);
         if(andef->gun_anl1) {
            fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun1);
            fprintf(flog,
             "    SP Outside Window: %-6u",
                sr2->sp1);
            fprintf(flog, "Misfires:");
            if(sr2->en_num)
               sr2->p_spec1 = ((float)sr2->sp1)*100.0F/((float)sr2->en_num);
            else sr2->p_spec1 = 0.0F;
            fprintf(flog, " %.1f %%\n", sr2->p_spec1);
            if (andef->mf1_anl) {
               fprintf(flog,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u\n",
                     spc->mf1, spc->gun1, msf->s2ct1);
            }
            if (andef->mf3_anl) {
               fprintf(flog,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u\n",
                  spc->mf3, spc->mf3_ra, spc->gun1, msf->s2mt1);
            }
         }
         if(andef->gun_anl2) {
            fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun2);
            fprintf(flog,
             "    SP Outside Window: %-6u",
                sr2->sp2);
            fprintf(flog, "Misfires:");
            if(sr2->en_num)
               sr2->p_spec2 = ((float)sr2->sp2)*100.0F/((float)sr2->en_num);
            else sr2->p_spec2 = 0.0F;
            fprintf(flog, " %.1f %%\n", sr2->p_spec2);
            if (andef->mf1_anl) {
               fprintf(flog,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u\n",
                     spc->mf1, spc->gun2, msf->s2ct2);
            }
            if (andef->mf3_anl) {
               fprintf(flog,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u\n",
                  spc->mf3, spc->mf3_ra, spc->gun2, msf->s2mt2);
            }
         }
         if(andef->vol_anl1 || andef->vol_anl2) {
            fprintf(flog, "Volume SOL: %-9u", sr2->sol_vol);
            fprintf(flog, "EOL: %u %s\n" , sr2->sp_vol,
                                               units[pd->vol_un]);
            if(andef->vol_anl1) {
               fprintf(flog,
                  "SP < %-5u %s: %-10u",
                     spc->gvol1, units[pd->vol_un], sr2->vol1);
            }
            if(andef->vol_anl2) {
               fprintf(flog,
                  "SP < %u %s: %u",
                     spc->gvol2, units[pd->vol_un], sr2->vol2);
            }
            fwrite("\n", sizeof (char), 1, flog);
         }
         fwrite("\n", sizeof (char), 1, flog);
      }
            /* source 3 */
      if (pd->loc_srce > 2) {
         fprintf(flog, "     %s Source\n", pd->loc3);
         fprintf(flog, "Source Fired: %-9u", sr3->en_num);
         fprintf(flog, "NOFIRE/AUTOFIRE: %u\n", sr3->tot_mf);
         if(andef->gun_anl1) {
            fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun1);
            fprintf(flog,
               "    SP Outside Window: %-6u",
                  sr3->sp1);
            fprintf(flog, "Misfires:");
            if(sr3->en_num)
               sr3->p_spec1 = ((float)sr3->sp1)*100.0F/((float)sr3->en_num);
            else sr3->p_spec1 = 0.0F;
            fprintf(flog, " %.1f %%\n", sr3->p_spec1);
            if (andef->mf1_anl) {
               fprintf(flog,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u\n",
                     spc->mf1, spc->gun1, msf->s3ct1);
            }
            if (andef->mf3_anl) {
               fprintf(flog,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u\n",
                  spc->mf3, spc->mf3_ra, spc->gun1, msf->s3mt1);
            }
         }
         if(andef->gun_anl2) {
            fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun2);
            fprintf(flog,
               "    SP Outside Window: %-6u",
                  sr3->sp2);
            fprintf(flog, "Misfires:");
            if(sr3->en_num)
               sr3->p_spec2 = ((float)sr3->sp2)*100.0F/((float)sr3->en_num);
            else sr3->p_spec2 = 0.0F;
            fprintf(flog, " %.1f %%\n", sr3->p_spec2);
            if (andef->mf1_anl) {
               fprintf(flog,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u\n",
                     spc->mf1, spc->gun2, msf->s3ct2);
            }
            if (andef->mf3_anl) {
               fprintf(flog,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u\n",
                  spc->mf3, spc->mf3_ra, spc->gun2, msf->s3mt2);
            }
         }
         if(andef->vol_anl1 || andef->vol_anl2) {
            fprintf(flog, "Volume SOL: %-9u", sr3->sol_vol);
            fprintf(flog, "EOL: %u %s\n" , sr3->sp_vol,
                                            units[pd->vol_un]);
            if(andef->vol_anl1) {
               fprintf(flog,
                  "SP < %u %s: %-10u",
                     spc->gvol1, units[pd->vol_un], sr3->vol1);
            }
            if(andef->vol_anl2) {
               fprintf(flog,
                  "SP < %u %s: %u",
                     spc->gvol2, units[pd->vol_un], sr3->vol2);
            }
            fwrite("\n", sizeof (char), 1, flog);
         }
         fwrite("\n", sizeof (char), 1, flog);
      }
         /* source 4 */
      if (pd->loc_srce > 3) {
         fprintf(flog, "     %s Source\n", pd->loc4);
         fprintf(flog, "Source Fired: %-10u", sr4->en_num);
         fprintf(flog, "NOFIRE/AUTOFIRE: %u\n", sr4->tot_mf);
         if(andef->gun_anl1) {
            fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun1);
            fprintf(flog,
               "    SP Outside Window: %-6u",
                  sr4->sp1);
            fprintf(flog, "Misfires:");
            if(sr4->en_num)
               sr4->p_spec1 = ((float)sr4->sp1)*100.0F/((float)sr4->en_num);
            else sr4->p_spec1 = 0.0F;
            fprintf(flog, " %.1f %%\n", sr4->p_spec1);
            if (andef->mf1_anl) {
               fprintf(flog,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u\n",
                     spc->mf1, spc->gun1, msf->s4ct1);
            }
            if (andef->mf3_anl) {
               fprintf(flog,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u\n",
                  spc->mf3, spc->mf3_ra, spc->gun1, msf->s4mt1);
            }
         }
         if(andef->gun_anl2) {
            fprintf(flog, "Spec: +/- %-4.2f mS", spc->gun2);
            fprintf(flog,
               "    SP Outside Window: %-6u",
                  sr4->sp2);
            fprintf(flog, "Misfires:");
            if(sr4->en_num)
               sr4->p_spec2 = ((float)sr4->sp2)*100.0F/((float)sr4->en_num);
            else sr4->p_spec2 = 0.0F;
            fprintf(flog, " %.1f %%\n", sr4->p_spec2);
            if (andef->mf1_anl) {
               fprintf(flog,
               "SP With More Than %u Consecutive Misfires > %-.2f mS: %u\n",
                     spc->mf1, spc->gun2, msf->s4ct2);
            }
            if (andef->mf3_anl) {
               fprintf(flog,
            "SP With More Than %u Misfires in Previous %u > %-.2f mS: %u\n",
                  spc->mf3, spc->mf3_ra, spc->gun2, msf->s4mt2);
            }
         }
         if(andef->vol_anl1 || andef->vol_anl2) {
            fprintf(flog, "Volume SOL: %-9u", sr4->sol_vol);
            fprintf(flog, "EOL: %u %s\n" , sr4->sp_vol,
                                            units[pd->vol_un]);
            if(andef->vol_anl1) {
               fprintf(flog,
                  "SP < %u %s: %-10u",
                     spc->gvol1, units[pd->vol_un], sr4->vol1);
            }
            if(andef->vol_anl2) {
               fprintf(flog,
                  "SP < %u %s: %u",
                     spc->gvol2, units[pd->vol_un], sr4->vol2);
            }
            fwrite("\n", sizeof (char), 1, flog);
         }
         fwrite("\n", sizeof (char), 1, flog);
      }
         /* individual gun timing */
      gstat = stsrce;
      fprintf(flog, "     Individual Guns\n");
      fprintf(flog,
   "------------------------------------------------------------------\n");
      fprintf(flog,
 "GUN        GUN  SRCE GUN     SHOTPOINTS      SPEC 1       SPEC 2\n");
      fprintf(flog,
 "NAME       NO.  NO.  VOL.  ENABLE NOF/AUT   BAD   MF%%    BAD   MF%%\n");
      fprintf(flog,
   "------------------------------------------------------------------\n");
      for(i=0;i<ext->numbergun;i++) {
         fprintf(flog, "%-10s %-3d  %-3d  %-4d  %-5u  %-5u",
            gstat->gun_name, gstat->gun_num, gstat->srce,
            gstat->gun_vol, gstat->en_num, gstat->mf_num);
         if(andef->gun_anl1) {
            fprintf(flog, "     %-5u", gstat->tot_sp1);
            if(gstat->en_num)
               gun_perc = ((float)gstat->tot_sp1)*100.0F/((float)gstat->en_num);
            else gun_perc = 0.0F;
            fprintf(flog, " %-4.1f", gun_perc);
         } else fprintf(flog, "               ");
         if(andef->gun_anl2) {
            fprintf(flog, "   %-5u", gstat->tot_sp2);
            if(gstat->en_num)
               gun_perc = ((float)gstat->tot_sp2)*100.0F/((float)gstat->en_num);
            else gun_perc = 0.0F;
            fprintf(flog, " %-4.1f", gun_perc);
         }
         fwrite("\n", sizeof (char), 1, flog);
         gstat = gstat->next;
      }
      fprintf(flog,
   "------------------------------------------------------------------\n\n");
   }
   return;
}
