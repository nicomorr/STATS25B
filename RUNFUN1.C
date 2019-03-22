/***************************************************************
*    RUNFUN1.C  some runtime functions
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
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>

         /* external global declarations */
extern struct ST_SUMM *sum;	  /* summary structure pointer */
extern struct ST_SPFN *sps;	  /* SP ditto */
extern struct ST_SPFN *fns;	  /* FN ditto */
extern struct ST_DCOM *dco;	  /* RU comms ditto */
extern struct ST_CDEP *dp1;	  /* depth specs cable 1 stats */
extern struct ST_CDEP *dp2;	  /* depth specs cable 2 stats */
extern struct ST_CDEP *dp3;	  /* depth specs cable 3 stats */
extern struct ST_CDEP *dp4;	  /* depth specs cable 4 stats */
extern struct HDR_DEF *hd; 	  /* header	list pointer */
extern struct OPT_DEF  *opt;	  /* pointer to options list */
extern struct SPEC_DEF *spc;	  /* pointer to specifications list */
extern struct CAB_DEF *stcab;   /* pointer to start of cable list */
extern struct CAB_DEF *lacab;   /* pointer to last of cable list */
extern struct ANAL_DEF *andef;  /* pointer to set/analysis list */
extern struct PRO_DEF *pd; 	  /* pointer to project list */
extern struct ST_CLK *t_clk;    /* pointer to timing stats */
extern struct GRAF_DEF *graf;   /* pointer to graphics list */
extern struct ERR_GRAF grerr;   /* no-error graph flags */
extern struct EXT_DEF *ext;     /* various external variables */
extern *units[];
extern char *data;				  /* pointer to data buffer */

         /* external graphics error array pointers */
extern char *sp_tmp;            /* SP graphics errors */
extern char *fn_tmp;            /* FN graphics errors */
extern float *c_graf1_min_tmp;  /* cable depth graphics errors */
extern float *c_graf1_max_tmp;  /* cable 1 */
extern float *c_graf2_min_tmp;
extern float *c_graf2_max_tmp;
extern float *c_graf3_min_tmp;
extern float *c_graf3_max_tmp;
extern float *c_graf4_min_tmp;
extern float *c_graf4_max_tmp;  /* ditto cable 4 */
extern float *tim_graf_tmp;     /* system timing graphics errors */


/****************************************************************
*   process SP numbering errors
*****************************************************************/
void proc_sp (HWND Werroll, long sp, long prev_sp,
                       char *errbuf, long fn, FILE *flog)
{
	int pflag = 0;                     /* set if problem to report */
   if (sum->sp_change == 8) return;     /* in case of funny flag */
	if (sp == prev_sp) pflag = 1;      /* SP did not change */
	else if (sum->sp_change == 6) {         /* decrementing */
		if ((prev_sp - sp) != (long)spc->spint)
			pflag = 2;                   /* SP jump */
	}
	else if (sum->sp_change == 7) {
		if ((sp - prev_sp) != (long)spc->spint)
			pflag = 2;                   /* SP jump */
	}
	if (pflag) {
		if(pflag == 1) {
         if(andef->lsp_anl)
            scr_error("SP FREEZE", errbuf, sp, fn, Werroll, flog);
         sps->no_ch = sps->no_ch+1;
         if(graf->sp_anl) {
            *sp_tmp = '1';
            grerr.sp = 1;
         }
		} else {
         if(andef->lsp_anl)
            scr_error("SP NUMBER JUMP", errbuf, sp, fn, Werroll, flog);
         sps->ju_mp = sps->ju_mp+1;
         if(graf->sp_anl) {
            *sp_tmp = '2';
            grerr.sp = 1;
         }
		}
   }
   if(graf->sp_anl) sp_tmp++;
}


/****************************************************************
*   process FN numbering errors
*****************************************************************/
void proc_fn (HWND Werroll, long fn, long prev_fn,
                       char *errbuf, long sp, FILE *flog)
{
	int pflag = 0;                     /* set if problem to report */
   if (sum->fn_change == 8) return;     /* in case of funny flag */
	if (fn == prev_fn) pflag = 1;      /* FN did not change */
	else if (sum->fn_change == 6) {         /* decrementing */
		if ((prev_fn - fn) != (long)spc->fnint)
			pflag = 2;                   /* FN jump */
	}
	else if (sum->fn_change == 7) {         /* incrementing */
		if ((fn - prev_fn) != (long)spc->fnint)
			pflag = 2;                   /* FN jump */
	}
	if (pflag) {
		if(pflag == 1) {
         if(andef->lfn_anl)
            scr_error("FN FREEZE", errbuf, sp, fn, Werroll, flog);
         fns->no_ch = fns->no_ch+1;
         if(graf->fn_anl) {
            *fn_tmp = '1';
            grerr.fn = 1;
         }
		} else {
         if(andef->lfn_anl)
            scr_error("FN NUMBER JUMP", errbuf, sp, fn, Werroll, flog);
         fns->ju_mp = fns->ju_mp+1;
         if(graf->fn_anl) {
            *fn_tmp = '2';
            grerr.fn = 1;
         }
		}
   }
   if(graf->fn_anl) fn_tmp++;
}


/****************************************************************
*	 process cable (depth) COMMS	errors
*****************************************************************/
void proc_dcom (HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
	register int i=0, n=0;
	int c1=0,c2=0,c3=0,c4=0;   /* 4 cable error-in-SP flags */
	struct CAB_DEF *info;
	char *bp;
	unsigned char h;   /* byte value */
	char t_buf[41];
	info = stcab;
   if( (hd->ca == 16)||(hd->ca == 15) ) {   /* if GECO DC 293/SYNTRAK */
		bp = data + hd->ca_ofst + opt->spn_fld + 4;	/* set to byte */
		for(i;i<ext->numberru;i++) {   /* for NUMBER of RUs in CABDEF */
			h = *(bp + ((info->cab_num -1)*3));
			if(h & 0x10) {   /* comms bit set/bad reading */
            if(andef->ldep_com) {
               sprintf(t_buf, "Depth %-10s No Response", info->cab_name);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
				++n;  /* an error */
				dco->tot_err = dco->tot_err+1;   /* add errors to stats */
				info->tot_err = info->tot_err+1;
				if(info->cable == 1) {
					dp1->tot_err = dp1->tot_err+1;
					c1=1;
				}
				if(info->cable == 2) {
					dp2->tot_err = dp2->tot_err+1;
					c2=1;
				}
				if(info->cable == 3) {
					dp3->tot_err = dp3->tot_err+1;
					c3=1;
				}
				if(info->cable == 4) {
					dp4->tot_err = dp4->tot_err+1;
					c4=1;
				}
			}
			info = info->next;
		}
	}
	if(n) dco->flag_err = dco->flag_err+1;
	if(c1) dp1->flag_err = dp1->flag_err+1;
	if(c2) dp2->flag_err = dp2->flag_err+1;
	if(c3) dp3->flag_err = dp3->flag_err+1;
	if(c4) dp4->flag_err = dp4->flag_err+1;
}


/****************************************************************
*   process cable depth spec1 errors
*****************************************************************/
void proc_dep1(HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
   register int i=0;
	struct CAB_DEF *info;
	unsigned bb;
	char *bp;
	int h1_flag=0, l1_flag=0;   /* cable 1 too high/too low flags */
	int h2_flag=0, l2_flag=0;   /* cable 2 too high/too low flags */
	int h3_flag=0, l3_flag=0;   /* cable 3 too high/too low flags */
	int h4_flag=0, l4_flag=0;   /* cable 4 too high/too low flags */
	int l;             /* MSB value */
	unsigned char h, s;   /* byte value & sign */
	float p;
	char t_buf[41];
   info = stcab;
         /* GECO DC293/SYNTRAK */
   if( (hd->ca == 16)||(hd->ca == 15) )
      bp = data + hd->ca_ofst + opt->spn_fld + 3;     /* set to byte */
   /* WESTERN FMTS 3 & 4 & PRAKLA SYN V2.3 (twin streamer Mintrop 7/91) */
   if((hd->ca == 63)||(hd->ca == 64)||(hd->ca == 91))
		bb = hd->ca_ofst+1+opt->spn_fld;
   if(hd->ca == 71)   /* DIGICON TRACE0 */
      bb = hd->ca_ofst+ opt->spn_fld;
	for(i;i<ext->numberru;i++) {   /* for NUMBER of RUs in CABDEF */
         /* format PRAKLA SYN V2.3 */
		if((hd->ca==91)&&(info->cab_num < 17))  /* Prakla streamer 1 offset */
	p = ((float)conv_bcd(((int)bb)+((info->cab_num-1)*3), 2, data))/10.0F;
		if((hd->ca==91)&&(info->cab_num >16))   /* Prakla streamer 2 offset */
	p = ((float)conv_bcd(((int)bb)+113+((info->cab_num-17)*3), 2, data))/10.0F;
         /* format GECO DC293/SYNTRAK */
      if( (hd->ca == 16)||(hd->ca == 15) ) {
			h =  (unsigned char) (*(bp+1+((info->cab_num-1)*3))&0x07);
			l = 256*((int)h);
			s =  (unsigned char) (*(bp+1+((info->cab_num-1)*3))&0x08);
			h =  *(bp + ((info->cab_num -1)*3));
			l = l + (int)h;
			if(s) l = -l;
			if(l) p = ((float)l)/10;
				else p = 0.0F;
		}
			/* WESTERN FMTS 3 & 4 (to be modified) */
		if((hd->ca == 63)||(hd->ca == 64)) {
			p = (float)conv_bcd(((int)bb)+((info->cab_num-1)*2), 1, data);
      }
         /* DIGICON TRACE0 */
      if(hd->ca == 71) {
         p = (float)conv_bin(((int)bb)+((info->cab_num-1)*2), 2, data, 0);
         if(p < 0.0F) {  /* if less than 0 reading is invalid */
            if(andef->ldep_anl1) {
               sprintf(t_buf, "Depth %-10s INVALID", info->cab_name);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
      }
		p = opt->mul_cabdep * p;	/* apply multiplier */
		if(p > info->max_dep) info->max_dep = p;
		if(p < info->min_dep) info->min_dep = p;
		if(info->cable == 1) {
			if(p > dp1->max_dep) dp1->max_dep = p;
			if(p < dp1->min_dep) dp1->min_dep = p;
			if(p < spc->d1min) l1_flag = 1;
         if(p > spc->d1max) h1_flag = 1;
         if(graf->dep_anl1) {
            if(p < *c_graf1_min_tmp) {
               *c_graf1_min_tmp = p;
               grerr.c1 = 1;
            }
            if(p > *c_graf1_max_tmp) {
               *c_graf1_max_tmp = p;
               grerr.c1 = 1;
            }
         }
		}
		if((info->cable == 2)&&(pd->loc_strm > 1)) {
			if(p > dp2->max_dep) dp2->max_dep = p;
			if(p < dp2->min_dep) dp2->min_dep = p;
			if(p < spc->d1min) l2_flag = 1;
         if(p > spc->d1max) h2_flag = 1;
         if(graf->dep_anl1) {
            if(p < *c_graf2_min_tmp) {
               *c_graf2_min_tmp = p;
               grerr.c2 = 1;
            }
            if(p > *c_graf2_max_tmp) {
               *c_graf2_max_tmp = p;
               grerr.c2 = 1;
            }
         }
		}
		if((info->cable == 3)&&(pd->loc_strm > 2)) {
			if(p > dp3->max_dep) dp3->max_dep = p;
			if(p < dp3->min_dep) dp3->min_dep = p;
			if(p < spc->d1min) l3_flag = 1;
         if(p > spc->d1max) h3_flag = 1;
         if(graf->dep_anl1) {
            if(p < *c_graf3_min_tmp) {
               *c_graf3_min_tmp = p;
               grerr.c3 = 1;
            }
            if(p > *c_graf3_max_tmp) {
               *c_graf3_max_tmp = p;
               grerr.c3 = 1;
            }
         }
		}
		if((info->cable == 4)&&(pd->loc_strm > 3)) {
			if(p > dp4->max_dep) dp4->max_dep = p;
			if(p < dp4->min_dep) dp4->min_dep = p;
			if(p < spc->d1min) l4_flag = 1;
         if(p > spc->d1max) h4_flag = 1;
         if(graf->dep_anl1) {
            if(p < *c_graf4_min_tmp) {
               *c_graf4_min_tmp = p;
               grerr.c4 = 1;
            }
            if(p > *c_graf4_max_tmp) {
               *c_graf4_max_tmp = p;
               grerr.c4 = 1;
            }
         }
		}
		if((p < spc->d1min)||(p > spc->d1max)){   /* depth spec1 exceeded */
         if(andef->ldep_anl1) {
            sprintf(t_buf, "Depth %-10s %-04.1f %s", info->cab_name, p, units[pd->loc_dp]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
			info->tot_sp1 = info->tot_sp1+1;
		}
		info = info->next;
	}
	if(h1_flag) dp1->gr_sp1 = dp1->gr_sp1 +1;
	if(l1_flag) dp1->sm_sp1 = dp1->sm_sp1 +1;
	if(h2_flag) dp2->gr_sp1 = dp2->gr_sp1 +1;
	if(l2_flag) dp2->sm_sp1 = dp2->sm_sp1 +1;
	if(h3_flag) dp3->gr_sp1 = dp3->gr_sp1 +1;
	if(l3_flag) dp3->sm_sp1 = dp3->sm_sp1 +1;
	if(h4_flag) dp4->gr_sp1 = dp4->gr_sp1 +1;
   if(l4_flag) dp4->sm_sp1 = dp4->sm_sp1 +1;
   if(graf->dep_anl1) {
      if(pd->loc_strm) {
         c_graf1_min_tmp++;
         c_graf1_max_tmp++;
      }
      if(pd->loc_strm > 1) {
         c_graf2_min_tmp++;
         c_graf2_max_tmp++;
      }
      if(pd->loc_strm > 2) {
         c_graf3_min_tmp++;
         c_graf3_max_tmp++;
      }
      if(pd->loc_strm > 3) {
         c_graf4_min_tmp++;
         c_graf4_max_tmp++;
      }
   }
}


/****************************************************************
*   process cable depth spec2 errors
*      details as in proc_dep1() as above
*****************************************************************/
void proc_dep2(HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
   register int i=0;
	struct CAB_DEF *info;
	char *bp;
	int h1_flag=0, l1_flag=0;   /* cable 1 too high/too low flags */
	int h2_flag=0, l2_flag=0;   /* cable 2 too high/too low flags */
	int h3_flag=0, l3_flag=0;   /* cable 3 too high/too low flags */
	int h4_flag=0, l4_flag=0;   /* cable 4 too high/too low flags */
	int l;                /* MSB value */
	unsigned char h, s;   /* byte value & sign */
	unsigned bb;
	float p;
	char t_buf[41];
	info = stcab;
   if( (hd->ca == 16)||(hd->ca == 15) )    /* if GECO DC 293/SYNTRAK */
		bp = data + hd->ca_ofst + opt->spn_fld + 3;		/* set to byte */
   if((hd->ca == 63)||(hd->ca == 64)||(hd->ca == 91))  /* WESTERN FMTS 3 & 4 or PRAKLA SYN 2.3 */
		bb = hd->ca_ofst+1+opt->spn_fld;
   if(hd->ca == 71)   /* DIGICON TRACE0 */
      bb = hd->ca_ofst+ opt->spn_fld;
	for(i;i<ext->numberru;i++) {   /* for NUMBER of RUs in CABDEF */
         /* format PRAKLA SYN 2.3 */
		if((hd->ca==91)&&(info->cab_num < 17))  /* Prakla streamer 1 offset */
	p = ((float)conv_bcd(((int)bb)+((info->cab_num-1)*3), 2, data))/10.0F;
		if((hd->ca==91)&&(info->cab_num >16))   /* Prakla streamer 2 offset */
	p = ((float)conv_bcd(((int)bb)+113+((info->cab_num-17)*3), 2, data))/10.0F;
         /* format GECO DC293/SYNTRAK */
      if( (hd->ca == 16)||(hd->ca == 15) ) {
			h =  (unsigned char) (*(bp+1+((info->cab_num-1)*3))&0x07);
			l = 256*((int)h);
			s =  (unsigned char) (*(bp+1+((info->cab_num-1)*3))&0x08);
			h =  *(bp + ((info->cab_num -1)*3));
			l = l + (int)h;
			if(s) l = -l;
			if(l) p = ((float)l)/10;
				else p = 0.0F;
		}
			/* WESTERN FMTS 3 & 4 (to be modified) */
		if((hd->ca == 63)||(hd->ca == 64)) {
			p = (float)conv_bcd(((int)bb)+((info->cab_num-1)*2), 1, data);
		}
         /* DIGICON TRACE0 */
      if(hd->ca == 71) {
         p = (float)conv_bin(((int)bb)+((info->cab_num-1)*2), 2, data, 0);
         if(p < 0.0F) {  /* if less than 0 reading is invalid */
            if(andef->ldep_anl1) {
               sprintf(t_buf, "Depth %-10s INVALID", info->cab_name);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
      }
		p = opt->mul_cabdep * p;	/* apply multiplier */
		if(info->cable == 1) {
			if(p > dp1->max_dep) dp1->max_dep = p;
			if(p < dp1->min_dep) dp1->min_dep = p;
			if(p < spc->d2min) l1_flag = 1;
			if(p > spc->d2max) h1_flag = 1;
		}
		if((info->cable == 2)&&(pd->loc_strm > 1)) {
			if(p > dp2->max_dep) dp2->max_dep = p;
			if(p < dp2->min_dep) dp2->min_dep = p;
			if(p < spc->d2min) l2_flag = 1;
			if(p > spc->d2max) h2_flag = 1;
		}
		if((info->cable == 3)&&(pd->loc_strm > 2)) {
			if(p > dp3->max_dep) dp3->max_dep = p;
			if(p < dp3->min_dep) dp3->min_dep = p;
			if(p < spc->d2min) l3_flag = 1;
			if(p > spc->d2max) h3_flag = 1;
		}
		if((info->cable == 4)&&(pd->loc_strm > 3)) {
			if(p > dp4->max_dep) dp4->max_dep = p;
			if(p < dp4->min_dep) dp4->min_dep = p;
			if(p < spc->d2min) l4_flag = 1;
			if(p > spc->d2max) h4_flag = 1;
		}
		if((p < spc->d2min)||(p > spc->d2max)){   /* depth spec2 exceeded */
         if(andef->ldep_anl2) {
            sprintf(t_buf, "Depth %-10s * %-04.1f %s", info->cab_name, p, units[pd->loc_dp]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
			info->tot_sp2 = info->tot_sp2+1;
		}
		info = info->next;
	}
	if(h1_flag) dp1->gr_sp2 = dp1->gr_sp2 +1;
	if(l1_flag) dp1->sm_sp2 = dp1->sm_sp2 +1;
	if(h2_flag) dp2->gr_sp2 = dp2->gr_sp2 +1;
	if(l2_flag) dp2->sm_sp2 = dp2->sm_sp2 +1;
	if(h3_flag) dp3->gr_sp2 = dp3->gr_sp2 +1;
	if(l3_flag) dp3->sm_sp2 = dp3->sm_sp2 +1;
	if(h4_flag) dp4->gr_sp2 = dp4->gr_sp2 +1;
	if(l4_flag) dp4->sm_sp2 = dp4->sm_sp2 +1;
}


/****************************************************************
*   process clock timing errors
*   GECO GM2B & PULSECLK reads freerun, closure/ftb & link delay
*      clocks & lists them all into a .PRN file & QCs link delay
*      against the specified nominal time & range
*   DIGICON TRACE0 lists TB ECHO TIME into .PRN & QCs it.
*   PRAKLA SYN V2.3 lists FTB1, FTB2 & FTB3 times separately
*      into ,PRN file, then adds them together & QCs the total
*      against the specified nominal time & range
*****************************************************************/
void proc_clk (HWND Werroll, long fn, char *errbuf, long sp,
				 FILE *flog, FILE *tprn)
{
	char sp_buff[11];  /* SP buffer */
	char t_buf[41];    /* temp error buffer */
	char tstamp[8], ftb[8], link[8];  /* time buffers */
	int ftb1, ftb2, ftb3, ftb_tim;
	char *bp;
	register int i;
	float lnk_del;
	ltoa(sp, sp_buff, 10);
   if((hd->clk == 19)||(hd->clk == 21)) {   /* if GM2B 3/4 or PULSECLK */
		bp = data+opt->spn_fld+hd->clk_ofst+1;  /* set to byte for PULSECLK */
		if(hd->clk == 19) bp = bp +8;  /* offset for GM2B */
		for (i=0;i<7;i++) {
			tstamp[i] = *bp;
			bp = bp +1;
		}
		tstamp[7] = '\0';
		bp = bp +2;
		for (i=0;i<7;i++) {
			ftb[i] = *bp;
			bp = bp +1;
		}
		ftb[7] = '\0';
		bp = bp +2;
		for (i=0;i<7;i++) {
			link[i] = *bp;
			bp = bp +1;
		}
		link[7] = '\0';
			/* write the .PRN file record */
		fprintf(tprn, "%s,%s,%s,%s\n", ltoa(sp, sp_buff, 10),
			tstamp, ftb, link);
		lnk_del = (float) atof(link);
		if((!lnk_del )||(lnk_del < 0.0F)) {    /* if corrupt */
			t_clk->corrupt = t_clk->corrupt +1;
         if(andef->lclk_anl)
            scr_error("LINK DELAY TIME CORRUPT", errbuf, sp, fn, Werroll, flog);
		} else {
			t_clk->tot_anl = t_clk->tot_anl +1;  /* total good readings */
			t_clk->total = t_clk->total +lnk_del;   /* total good times */
			if(lnk_del < t_clk->min) t_clk->min = lnk_del;  /* check min val */
			if(lnk_del > t_clk->max) t_clk->max = lnk_del;  /* check max val */
			if((lnk_del < spc->clk - spc->clk_ra)||         /* if an error val */
						 (lnk_del > spc->clk + spc->clk_ra)) {
            if(andef->lclk_anl) {
               sprintf(t_buf, "Link Delay Time  %-3.1f mS.", lnk_del);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
            if(graf->clk_anl) *tim_graf_tmp = lnk_del;
            grerr.tb = 1;
			}
         if(lnk_del < spc->clk - spc->clk_ra)   /* if highside error */
            t_clk->sm_spec = t_clk->sm_spec +1;
         if(lnk_del > spc->clk + spc->clk_ra)   /* if lowside error */
            t_clk->gr_spec = t_clk->gr_spec +1;
		}
   }
   if(hd->clk == 71) {   /* if DIGICON TRACE0 */
      lnk_del = ( (float) conv_bin(opt->spn_fld+hd->clk_ofst, 2, data, 0))/10.0F;
			/* write the .PRN file record */
      fprintf(tprn, "%s,%.1f\n", ltoa(sp, sp_buff, 10), lnk_del);
         /* IF ZERO THEN ANNOUNCE IT AS CORRUPT */
		if((!lnk_del )||(lnk_del < 0.0F)) {    /* if corrupt */
			t_clk->corrupt = t_clk->corrupt +1;
         if(andef->lclk_anl)
            scr_error("TB ECHO TIME CORRUPT", errbuf, sp, fn, Werroll, flog);
		} else {
			t_clk->tot_anl = t_clk->tot_anl +1;  /* total good readings */
			t_clk->total = t_clk->total +lnk_del;   /* total good times */
			if(lnk_del < t_clk->min) t_clk->min = lnk_del;  /* check min val */
			if(lnk_del > t_clk->max) t_clk->max = lnk_del;  /* check max val */
			if((lnk_del < spc->clk - spc->clk_ra)||         /* if an error val */
						 (lnk_del > spc->clk + spc->clk_ra)) {
            if(andef->lclk_anl) {
               sprintf(t_buf, "TB Echo Time  %-3.1f mS.", lnk_del);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
            if(graf->clk_anl) *tim_graf_tmp = lnk_del;
            grerr.tb = 1;
			}
         if(lnk_del < spc->clk - spc->clk_ra)   /* if highside error */
            t_clk->sm_spec = t_clk->sm_spec +1;
         if(lnk_del > spc->clk + spc->clk_ra)   /* if lowside error */
            t_clk->gr_spec = t_clk->gr_spec +1;
		}
   }
	if(hd->clk == 91) {   /* if PRAKLA FTB */
		ftb1 = (int) conv_bcd(opt->spn_fld+hd->clk_ofst, 2, data);
		ftb2 = (int) conv_bcd(opt->spn_fld+hd->clk_ofst+2, 2, data);
		ftb3 = (int) conv_bcd(opt->spn_fld+hd->clk_ofst+4, 2, data);
		ftb_tim = ftb1 + ftb2 + ftb3;
			/* write the .PRN file record */
		fprintf(tprn, "%s,%d,%d,%d,%d\n", ltoa(sp, sp_buff, 10),
			ftb1, ftb2, ftb3, ftb_tim);
		lnk_del = (float) ftb_tim;
		t_clk->tot_anl = t_clk->tot_anl +1;  /* total good readings */
		t_clk->total = t_clk->total +lnk_del;   /* total good times */
		if(lnk_del < t_clk->min) t_clk->min = lnk_del;  /* check min val */
		if(lnk_del > t_clk->max) t_clk->max = lnk_del;  /* check max val */
		if((lnk_del < spc->clk - spc->clk_ra)||         /* if an error val */
					 (lnk_del > spc->clk + spc->clk_ra)) {
         if(andef->lclk_anl) {
            sprintf(t_buf, "FTB TIME %-3.1f mS.", lnk_del);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
            if(graf->clk_anl) *tim_graf_tmp = lnk_del;
            grerr.tb = 1;
		}
		if(lnk_del < spc->clk - spc->clk_ra)   /* if highside error */
			t_clk->sm_spec = t_clk->sm_spec +1;
		if(lnk_del > spc->clk + spc->clk_ra)   /* if lowside error */
			t_clk->gr_spec = t_clk->gr_spec +1;
   }
   if(graf->clk_anl) tim_graf_tmp++;
}
