/***************************************************************
*    RUNFUN3.C  some runtime stats functions
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
#include <stdlib.h>
#include <math.h>

#include <dw.h>            /* greenleaf */
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>
#include <stxconst.h>

         /* local type definition */

typedef unsigned long GTLI;        /* greenleaf */

         /* global declaration */
struct FIR_SEQ *f_temp = NULL;     /* struct for loop fire sequence */

			/* external global declarations */
extern struct ST_SUMM *sum;	  /* summary structure pointer */
extern struct HDR_DEF *hd; 	  /* header	list pointer */
extern struct OPT_DEF  *opt;	  /* pointer to options list */
extern struct SPEC_DEF *spc;	  /* pointer to specifications list */
extern struct ANAL_DEF *andef;  /* pointer to set/analysis list */
extern struct PRO_DEF *pd; 	  /* pointer to project list */
extern struct ST_MAN *man;      /* manifold pressure stats */
extern struct ST_GMSK *gmsk;    /* gun mask stats */
extern struct WING_DEF *stwi;   /* wing angles first */
extern struct GUNDP_DEF *stgundp;     /* pointer to start of gundep list */
extern struct ST_SRCE *sr1;     /* source 1 depth stats */
extern struct ST_SRCE *sr2;     /* source 2 depth stats */
extern struct ST_SRCE *sr3;     /* source 3 depth stats */
extern struct ST_SRCE *sr4;     /* source 4 depth stats */
extern struct ST_WAT *wat;      /* water depth analysis */
extern struct EXT_DEF *ext;     /* various external variables */
extern struct ST_SPTIM *t_int;  /* SP time interval stats */
extern struct FIR_SEQ *fire;    /* pointer to fire sequence loop structure */
extern struct GSP_DEF gsp; 	  /* hold ground speed stats */
extern struct GSP_DEF ws;		  /* hold water speed stats */
extern struct GSP_DEF cu;		  /* hold current stats */
extern struct SEQ_DEF st_seq;   /* hold sequencing stats */
extern struct FIR_SEQ *last;    /* last of ditto */
extern struct GRAF_DEF *graf;   /* pointer to graphics list */
extern struct ERR_GRAF grerr;   /* no-error graph flags */
extern *units[];                /* UNITS list */
extern char *data;              /* pointer to data buffer */
extern char *tm_tmp;            /* SP TIMING errors graphics array */
extern char *gmsk_graf_tmp;     /* gun mask graphics errors */
extern char *seq_graf_tmp;      /* fire seq graphics errors */
extern struct FIRE_BUF *fire_buf_tmp; /* hold initial fire sequence text */
extern unsigned *man_graf_tmp;  /* air pressure errors */
extern float *gundep_graf_min_tmp;    /* gun depths min array */
extern float *gundep_graf_max_tmp;    /* gun depths max array */
extern float *gs_graf;				 /* ground speed error array */
extern float *gs_graf_temp;
extern float *ws_graf;				 /* water speed error array */
extern float *ws_graf_temp;
extern float *sy_graf;				 /* system heading error array */
extern float *sy_graf_temp;
extern float *gy_graf;				 /* gyro heading error array */
extern float *gy_graf_temp;


/****************************************************************
*   process air pressure - GECO & PRAKLA & DIGICON TRACE0
*****************************************************************/
void proc_man (HWND Werroll, long fn, char *errbuf, long sp,
				 FILE *flog)
{
	unsigned pr;            /* pressure this shot */
   char t_buf[41];         /* temp error buffer */
      /* GECO MANIFOLD, PRAKLA SYN V2 & DIGICON TRACE0 */
   if( (hd->ma == 27)||(hd->ma == 91)||(hd->ma == 71) )
		pr = (unsigned) conv_bcd(hd->ma_ofst + opt->spn_fld, 2, data);
   if(hd->ma == 91) pr = pr/10;        /* divide by 10 for Prakla Syn V2 */
	if(sp == sum->fsp) {
		sprintf(t_buf, "SOL MANIFOLD PRESSURE %4.4u  %s", pr,
					  units[pd->pr_un]);
      scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
		man->sol = pr;
	}
	if(sp == sum->lsp) {
		sprintf(t_buf, "EOL MANIFOLD PRESSURE %4.4u  %s", pr,
					  units[pd->pr_un]);
      scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
		man->eol = pr;
	}
	if((pr < spc->ma1)&&(!man->man1_fl)) {
		man->man1_fl = 1;
		sprintf(t_buf, "Pressure Change < Spec1: %4.4u  %s", pr,
					  units[pd->pr_un]);
      if(andef->lman_anl1)
         scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
	}
	if((pr < spc->ma2)&&(!man->man2_fl)) {
		man->man2_fl = 1;
		sprintf(t_buf, "Pressure Change < Spec2: %4.4u  %s", pr,
					  units[pd->pr_un]);
      if(andef->lman_anl2)
         scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
	}
	if((pr > spc->ma1)&&(man->man1_fl)) {
		man->man1_fl = 0;
		sprintf(t_buf, "Pressure Change > Spec1: %4.4u  %s", pr,
					  units[pd->pr_un]);
      if(andef->lman_anl1)
         scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
	}
	if((pr > spc->ma2)&&(man->man2_fl)) {
		man->man2_fl = 0;
		sprintf(t_buf, "Pressure Change > Spec2: %4.4u  %s", pr,
					  units[pd->pr_un]);
      if(andef->lman_anl2)
         scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
	}
	if(pr > man->max) man->max = pr;
	if(pr < man->min) man->min = pr;
	if(man->man1_fl) man->man1 = man->man1 +1;
	if(man->man2_fl) man->man2 = man->man2 +1;
   man->total = man->total + (long) pr;
   if(graf->man_anl1) {
      if(pr < spc->ma1) {
         *man_graf_tmp = pr;
         grerr.pr = 1;
      }
      man_graf_tmp++;
   }
}


/****************************************************************
*   process gun masks
*   GECO only (Western RDC to be added later)
*   independant of source controller checks.
*   looks for overfiring arrays and if format is GECO GUNDA or
*   LITTON + GM2B it looks for no source fired as well
*   this routine limited to 2 local & 2 remote sources
*   at present (current GECO formats).
*****************************************************************/
void proc_gmsk (HWND Werroll, long fn, char *errbuf, long sp,
				 FILE *flog, int a)
{      /* if gun mask GM2B + controller GECO LITTON 2 or GUNDA 2 */
	register int i;
	unsigned loc1=0, loc2=0, rem1=0, rem2=0;   /* hold number bits set */
	unsigned l1_fl=0, l2_fl=0, r1_fl=0, r2_fl=0;  /* flags if bits set */
	char *bp;           /* byte pointer */
	char t_buf[41];         /* temp error buffer */
	if(pd->seq_len < 2) {     /* check more than 1 source (using seq_len) */
      StatsErr("43001");
      return;
	}
			/* do local sources first - nothing assumed yet about remote */
	if(hd->gu == 9)      /* set bp for GECO LITTON 2/3 masks */
		bp = data+opt->spn_fld+hd->gu_ofst+256; /* set mask start byte */
	if(hd->gu == 11)     /* set bp for GECO GUNDA 2 masks */
		bp = data+opt->spn_fld+hd->gu_ofst
					+14+(hd->gu_num*12); /* set mask start byte */
	for(i=0; i<4; i++) {     /* local source 2 (port) */
		loc2 = loc2 + bit_set(*bp);
		bp = bp +1;
	}
	for(i=0; i<4; i++) {     /* local source 1 (stbd) */
		loc1 = loc1 + bit_set(*bp);
		bp = bp +1;
	}
			/* now do remote source(s) if they exist */
	if((hd->gm == 18)||(hd->gm == 19)) {      /* if a GECO format */
		bp = data+opt->spn_fld+hd->gm_ofst;  /* set to GM2B bytes */
		for(i=0; i<4; i++) {     /* remote source 2 (port) */
			rem2 = rem2 + bit_set(*bp);
			bp = bp +1;
		}
		for(i=0; i<4; i++) {     /* remote source 1 (stbd) */
			rem1 = rem1 + bit_set(*bp);
			bp = bp +1;
		}
	}
	if(loc1) l1_fl = 1;
	if(loc2) l2_fl = 1;
	if(rem1) r1_fl = 1;
	if(rem2) r2_fl = 1;
   if(!a) { /* if !a running stats, otherwise making SP/FN/GMSK list */
		if(!(l1_fl+l2_fl+r1_fl+r2_fl)) {
         if(andef->lgmsk_anl)
            scr_error("GUN MASK - NO GUNS FIRED", errbuf,
                                       sp, fn, Werroll, flog);
         gmsk->tot_none = gmsk->tot_none +1;   /* add to total */
         if(graf->gmsk_anl) {
            *gmsk_graf_tmp = '1';
            grerr.gm = 1;
         }
		}
		if((l1_fl+l2_fl+r1_fl+r2_fl) > 1) {
         if(andef->lgmsk_anl)
            scr_error("GUN MASK > 1 SOURCE FIRED", errbuf,
                                        sp, fn, Werroll, flog);
			gmsk->tot_ovr = gmsk->tot_ovr +1;     /* add to total */
			if(l1_fl) gmsk->ovr_loc1 = gmsk->ovr_loc1 +1;
			if(l2_fl) gmsk->ovr_loc2 = gmsk->ovr_loc2 +1;
			if(r1_fl) gmsk->ovr_rem1 = gmsk->ovr_rem1 +1;
         if(r2_fl) gmsk->ovr_rem2 = gmsk->ovr_rem2 +1;
         if(graf->gmsk_anl) {
            *gmsk_graf_tmp = '2';
            grerr.gm = 1;
         }
		}
		if(((l1_fl+l2_fl+r1_fl+r2_fl) == 1)&&
					 ((loc1+loc2+rem1+rem2) < spc->gmsk)) {
			sprintf(t_buf, "GUN MASK - Only %u Guns Fired",
					  (loc1+loc2+rem1+rem2));
         if(andef->lgmsk_anl)
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			gmsk->tot_set = gmsk->tot_set +1;      /* add to total */
			if(l1_fl) gmsk->loc1 = gmsk->loc1 +1;
			if(l2_fl) gmsk->loc2 = gmsk->loc2 +1;
			if(r1_fl) gmsk->rem1 = gmsk->rem1 +1;
			if(r2_fl) gmsk->rem2 = gmsk->rem2 +1;
		}
   } else {   /* making listing to disk file */
		if(l1_fl) fprintf(flog, "   %s", pd->loc1);
		if(l2_fl) fprintf(flog, "   %s", pd->loc2);
		if(r1_fl) fprintf(flog, "   %s", pd->rem1);
		if(r2_fl) fprintf(flog, "   %s", pd->rem2);
   }
   if(graf->gmsk_anl) gmsk_graf_tmp++;
}


/****************************************************************
*   process source firing sequence - GECO only
*   checks sequence length & order from Gun Masks
*   and writes errors to disk file .SEQ
*****************************************************************/
void proc_fseq (long fn, long sp, FILE *flog)
{
   int i=0, n=0, p=0;
   int lf=0;   /* line feed flag */
   int loc1=0, loc2=0, rem1=0, rem2=0;      /* hold number bits set */
   int l1_fl=0, l2_fl=0, r1_fl=0, r2_fl=0;  /* flags if bits set */
   char *bp;                                     /* byte pointer */
   unsigned a=0, b=0;
   char t_buf[66], buf[2];                      /* temp error buffer */
   struct FIR_SEQ *f_loop=NULL;     /* struct for loop fire sequence */
	if(pd->seq_len < 2) {     /* check more than 1 source (using seq_len) */
      StatsErr("43001");
      return;
	}
			/* do local sources first - nothing assumed yet about remote */
	if(hd->gu == 9)      /* set bp for GECO LITTON 2/3 masks */
		bp = data+opt->spn_fld+hd->gu_ofst+256; /* set mask start byte */
	if(hd->gu == 11)     /* set bp for GECO GUNDA 2 masks */
		bp = data+opt->spn_fld+hd->gu_ofst
					+14+(hd->gu_num*12); /* set mask start byte */
   for(i; i<4; i++) {     /* local source 2 (port) */
		loc2 = loc2 + bit_set(*bp);
		bp = bp +1;
	}
	for(i=0; i<4; i++) {     /* local source 1 (stbd) */
		loc1 = loc1 + bit_set(*bp);
		bp = bp +1;
	}
			/* now do remote source(s) if they exist */
	if((hd->gm == 18)||(hd->gm == 19)) {      /* if a GECO format */
		bp = data+opt->spn_fld+hd->gm_ofst;  /* set to GM2B bytes */
		for(i=0; i<4; i++) {     /* remote source 2 (port) */
			rem2 = rem2 + bit_set(*bp);
			bp = bp +1;
		}
		for(i=0; i<4; i++) {     /* remote source 1 (stbd) */
			rem1 = rem1 + bit_set(*bp);
			bp = bp +1;
		}
   }
   if(loc1) l1_fl = 1;
	if(loc2) l2_fl = 1;
	if(rem1) r1_fl = 1;
   if(rem2) r2_fl = 1;
            /* sources analysed - now do sequence analyses */
   if(sp==sum->fsp) f_temp = fire;
   i = l1_fl+l2_fl+r1_fl+r2_fl;  /* determine number of sources that fired */
   if(labs(sp - sum->fsp) < (int)pd->seq_len) n = 1; /* if first 'seq_len' SPs */
   sprintf(t_buf, "%-10s%-6lu %-4lu  %s", f_temp->nom_sname, sp, fn,  ext->tim);
   if(!i) {
      strcat(t_buf, "  No Source Fired");
      lf=1;
      strcpy(f_temp->act_sname, f_temp->nom_sname);
      f_temp->act_srce = 0;
   }
   if(i > 1) {
      strcat(t_buf, "  ");
      strcat(t_buf, itoa(i, buf, 10));
      strcat(t_buf, " Fired:");
      lf=1;
      f_temp->act_srce = 5;  /* too many sources  */
   }
   if(l1_fl) {
      strcat(t_buf, "  ");
      strcat(t_buf, pd->loc1);
      strcpy(f_temp->act_sname, pd->loc1);
      f_temp->act_srce = 1;
   }
   if(l2_fl) {
      strcat(t_buf, "  ");
      strcat(t_buf, pd->loc2);
      strcpy(f_temp->act_sname, pd->loc2);
      f_temp->act_srce = 2;
   }
   if(r1_fl) {
      strcat(t_buf, "  ");
      strcat(t_buf, pd->rem1);
      strcpy(f_temp->act_sname, pd->rem1);
      f_temp->act_srce = 3;
   }
   if(r2_fl) {
      strcat(t_buf, "  ");
      strcat(t_buf, pd->rem2);
      strcpy(f_temp->act_sname, pd->rem2);
      f_temp->act_srce = 4;
   }
         /* text buffers of sources fired first seq_len SPs */
   if( labs(sp - sum->fsp) < (int)pd->seq_len) {
      sprintf (fire_buf_tmp->fire_info, "%-7ld%-7ld%-8s", sp, fn, ext->tim);
      if(l1_fl) {
         strcat(fire_buf_tmp->fire_info, "  ");
         strcat(fire_buf_tmp->fire_info, pd->loc1);
      }
      if(l2_fl) {
         strcat(fire_buf_tmp->fire_info, "  ");
         strcat(fire_buf_tmp->fire_info, pd->loc2);
      }
      if(r1_fl) {
         strcat(fire_buf_tmp->fire_info, "  ");
         strcat(fire_buf_tmp->fire_info, pd->rem1);
      }
      if(r2_fl) {
         strcat(fire_buf_tmp->fire_info, "  ");
         strcat(fire_buf_tmp->fire_info, pd->rem2);
      }
      fire_buf_tmp++;
   }
         /* put a line in disk file after first 'pd->seq_len' SPs */
   if(labs(sp - sum->fsp) == (int)pd->seq_len)
      fprintf(flog,
    "--------- Initial Sequence Complete ------------------------------\n");
   if(strcmp(f_temp->nom_sname, f_temp->act_sname)) p=1;
   a = f_temp->srce_ok;
   if((n)||(p)||(i != 1)||(!a)) {
      lf=1;
      if ((p)&&(i == 1)) {
         strcat(t_buf, "  Change Seq");
         if(graf->seq_anl) {
            *seq_graf_tmp = '1';
            grerr.fr = 1;
         }
         n = 1;    /* stop "new sequence" flag */
         strcpy(f_temp->nom_sname, f_temp->act_sname);
         f_temp->nom_srce = f_temp->act_srce;
         st_seq.change = st_seq.change + 1;
      } else n = 0;
   }
   if(!p) {
      f_temp->srce_ok = 1;
      f_temp->all_ok = 1;
   } else {
      f_temp->srce_ok = 0;
      f_temp->all_ok = 0;
   }
   a=b=0;
   if(labs(sp - sum->fsp) >= (int)pd->seq_len) {
      f_loop = fire;
      while (f_loop) {
         a = f_loop->act_srce + a;
         b = f_loop->srce_ok + b;
         f_loop = f_loop->next;
      }
      if(b != pd->seq_len) {
         lf = 1;
         if(!n) {
            if(i==1) strcat(t_buf, "  New Seq");
            if(graf->seq_anl) {
               *seq_graf_tmp = '2';
               grerr.fr = 1;
            }
            st_seq.reset = st_seq.reset +1;
         }
      }
   }
   if(!(f_temp = f_temp->next)) f_temp = fire;
   if (lf) {
      fprintf(flog, t_buf);
      fwrite("\n", sizeof(char), 1, flog);
   }
   if(graf->seq_anl) seq_graf_tmp++;
}


/****************************************************************
*   process gun depths - GECO, WESTERN, PRAKLA & DIGICON TRACE0
*      tested DIGICON but no data for others yet
*****************************************************************/
void proc_gundp (HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
	register int i=0;
   struct GUNDP_DEF *info;     /* temp gundepth structure pointer */
   unsigned bb;                /* offset bytes in data block */
	int h1_flag=0, l1_flag=0;   /* source 1 too high/too low flags */
	int h2_flag=0, l2_flag=0;   /* source 2 too high/too low flags */
	int h3_flag=0, l3_flag=0;   /* source 3 too high/too low flags */
	int h4_flag=0, l4_flag=0;   /* source 4 too high/too low flags */
	float p;
	char t_buf[41];
	char b_buf;
   info = stgundp;             /* set to first gundepth in list */
         /* set start byte in datablock */
      /* GECO GUNDEPTH & DIGICON TRACE0 */
   if( (hd->gd == 23) || (hd->gd == 71) )
      bb = hd->gd_ofst + opt->spn_fld;
      /* WESTERN FMTS 3 & 4 & PRAKLA SYN V2.3 */
   if( (hd->gd == 63) || (hd->gd == 64) || (hd->gd == 91) )
      bb += 1;   /* do lower byte first */
         /* run through configured gun depths */
   for(i;i<ext->numberdp;i++) {
      if(hd->gd == 23) {  /* GECO GUNDEPTH */
            /* convert 2 bytes BCD */
         p = (float)conv_bcd(((int)bb)+((info->gundp_num-1)*2), 2, data);
         p = p/100.0F;   /* convert from cms to metres */
      }
         /* WESTERN 3 & 4 & PRAKLA SYN V2.3 */
      if( (hd->gd == 63)||(hd->gd == 64)||(hd->gd == 91) )
            /* convert 1 byte BCD */
			p = (float)conv_bcd(((int)bb)+((info->gundp_num-1)*2), 1, data);
      if(hd->gd == 91) {   /* PRAKLA SYN V2.3 */
            /* AND out the most significant nibble (sign etc) */
         b_buf = (char) (*(data+bb-1+((info->gundp_num-1)*2)) & 0x0f);
            /* add lower byte/10 & higher byte least signif nibble x10 */
			p = p/10.0F + ((float)conv_bcd(0, 1, &b_buf))*10.0F;
      }
      if(hd->gd == 71)   /* digicon trace0 */
         p = (float)conv_bin(((int)bb)+info->gundp_num-1, 1, data, 0);
         /* multiplier then add offset */
      p = (opt->mul_gundep * p) + opt->gundep_ofst;
		if(p > info->max_dep) info->max_dep = p;
		if(p < info->min_dep) info->min_dep = p;
		if(info->srce == 1) {
			if(p > sr1->max_dep) sr1->max_dep = p;
			if(p < sr1->min_dep) sr1->min_dep = p;
			if(p < spc->gd_min) l1_flag = 1;
			if(p > spc->gd_max) h1_flag = 1;
		}
		if((info->srce == 2)&&(pd->loc_srce > 1)) {
			if(p > sr2->max_dep) sr2->max_dep = p;
			if(p < sr2->min_dep) sr2->min_dep = p;
			if(p < spc->gd_min) l2_flag = 1;
			if(p > spc->gd_max) h2_flag = 1;
		}
		if((info->srce == 3)&&(pd->loc_srce > 2)) {
			if(p > sr3->max_dep) sr3->max_dep = p;
			if(p < sr3->min_dep) sr3->min_dep = p;
			if(p < spc->gd_min) l3_flag = 1;
			if(p > spc->gd_max) h3_flag = 1;
		}
		if((info->srce == 4)&&(pd->loc_srce > 3)) {
			if(p > sr4->max_dep) sr4->max_dep = p;
			if(p < sr4->min_dep) sr4->min_dep = p;
			if(p < spc->gd_min) l4_flag = 1;
			if(p > spc->gd_max) h4_flag = 1;
		}
		if((p < spc->gd_min)||(p > spc->gd_max)){   /* depth spec exceeded */
         sprintf(t_buf, "Gun Depth %-10s %-04.1f %s",
                   info->gundp_name, p, units[pd->loc_dp]);
         if(andef->lgd_anl)
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         info->tot_sp = info->tot_sp+1;
         if(graf->gd_anl) {      /* do graphics error array */
            if(p < *gundep_graf_min_tmp) {
               *gundep_graf_min_tmp = p;
               grerr.gd = 1;
            }
            if(p > *gundep_graf_max_tmp) {
               *gundep_graf_max_tmp = p;
               grerr.gd = 1;
            }
         }
		}
		info = info->next;
	}
	if(h1_flag) sr1->gr_sp = sr1->gr_sp +1;
	if(l1_flag) sr1->sm_sp = sr1->sm_sp +1;
	if(h2_flag) sr2->gr_sp = sr2->gr_sp +1;
	if(l2_flag) sr2->sm_sp = sr2->sm_sp +1;
	if(h3_flag) sr3->gr_sp = sr3->gr_sp +1;
	if(l3_flag) sr3->sm_sp = sr3->sm_sp +1;
	if(h4_flag) sr4->gr_sp = sr4->gr_sp +1;
   if(l4_flag) sr4->sm_sp = sr4->sm_sp +1;
   if(graf->gd_anl) {
      gundep_graf_min_tmp++;
      gundep_graf_max_tmp++;
   }
}


/****************************************************************
*   process water depths - uses find_wat() GECO, WESTERN, PRAKLA
*****************************************************************/
void proc_wadep (HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
	char t_buf[41];
	float p;
   p = find_wat (hd->wa_ofst+opt->spn_fld, hd->wa, data);
   if(!p) {
      wat->corr = wat->corr + 1;
      return;
   }
   p = p + opt->draft;
	if(p > wat->max) wat->max = p;
	if(p < wat->min) wat->min = p;
	if((p < spc->wd_min)||(p > spc->wd_max)) {
		sprintf(t_buf, "Water Depth %-04.1f %s", p, units[pd->loc_dp]);
      if(andef->lwdp_anl)
         scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
		wat->err = wat->err +1;
	}
}


/****************************************************************
*   process SP time interval - uses result of find_tm()
*   GECO & PRAKLA -  analysis to 0.01 sec
*   WESTERN only has seconds - but results meaned to 0.01 sec
*****************************************************************/
void proc_sptim (HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
   GTLI curr_time;   /* greenleaf data type */
   long t_dif;
   float tim_dif;
   char t_buf[41];
   curr_time = FTimeToGtli (ext->l_tim, 7);   /* get the current SP time */
   if(!curr_time) {        /* assume time string corrupt if 0 */
      t_int->corrupt = t_int->corrupt +1;
      return;
   }
   if(sp == sum->fsp) {    /* set the comparison time if first SP of run */
      t_int->t_prev = (unsigned long) curr_time;
      return;
   }
   t_dif = HSecsBetweenGtli ( ((GTLI)t_int->t_prev), curr_time);
   t_int->t_prev = (unsigned long) curr_time;
      /* FORGOT WHY THESE 2 LINES WORK ???  BUT THEY DO !!  */
   if(t_dif<0L)  t_dif = 65536L + t_dif;
   if(t_dif>300000L) t_dif = t_dif -327680L;
      /* arrive at by 8640000 - 65536 - 327680 ??? */
   if(t_dif<0L)  t_dif = 8246784L + t_dif;  /* correct for midnight */
   tim_dif = ((float) t_dif) /100.0F;
   t_int->total = t_int->total + tim_dif; /* running interval total time */
   t_int->tot_anl = t_int->tot_anl + 1;   /*   running total analysed */
   if(tim_dif < t_int->tmin) t_int->tmin = tim_dif;  /* get the min */
   if(tim_dif > t_int->tmax) t_int->tmax = tim_dif;  /* get the max */
   if(tim_dif < spc->t_min) {
      sprintf(t_buf, "SHORT SP %-4.2f Secs.", tim_dif);
      if(andef->ltim_anl)
         scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
      t_int->sm_spec = t_int->sm_spec +1;
      if(graf->tim_anl) {
         *tm_tmp = '2';
         grerr.tm = 1;
      }
	}
   if(tim_dif > spc->t_max) {
      sprintf(t_buf, "LONG SP %-4.2f Secs.", tim_dif);
      if(andef->ltim_anl)
         scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
      t_int->gr_spec = t_int->gr_spec +1;
      if(graf->tim_anl) {
         *tm_tmp = '1';
         grerr.tm = 1;
      }
   }
   if(graf->tim_anl) tm_tmp++;
}


/****************************************************************
*   process wing angles - GECO SYNTRAK & DC293
*****************************************************************/
void proc_wing(HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
   register int i=0;
   struct WING_DEF *info;
	char *bp;
	int l;             /* MSB value */
	unsigned char h, s;   /* byte value & sign */
	float p;
	char t_buf[41];
   info = stwi;
   if(hd->wi == 16)    /* if GECO DC 293 */
      bp = data + hd->wi_ofst + opt->spn_fld + 3;     /* set to byte */
   for(i;i<ext->numberwing;i++) {   /* for NUMBER of RUs in WINGDEF */
      h =  (unsigned char) (*(bp+1+((info->num-1)*3))&0x07);
      l = 256*((int)h);
      s =  (unsigned char) (*(bp+1+((info->num-1)*3))&0x08);
      h =  *(bp + ((info->num -1)*3));
      l = l + (int)h;
      if(s) l = -l;
      if(l) p = ((float)l)/10;
         else p = 0.0F;
      info->mean = info->mean + p;       /* add total for later division */
      if(p > info->max) info->max = p;   /* set max */
      if(p < info->min) info->min = p;   /* set min */
      if(p != info->last) {
         info->on = info->on + 1;
   info->change = info->change + (float) fabs((double)(info->last - p));
         info->last = p;
      }
      if((p < (-spc->wing))||(p > spc->wing)){   /* angle spec exceeded */
         if(andef->lwi_anl) {
            sprintf(t_buf, "Angle %-10s %-04.1f DEG", info->name, p);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
         info->spec = info->spec + 1;
		}
		info = info->next;
	}
}


/****************************************************************
*	 process speed along track - GECO NOR* 2/3 or DIGICON TRACE0
*****************************************************************/
void proc_sal(HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
   float x;
   char t_buf[41];
   static int h_flag;
   if ((hd->gs == 6)||(hd->gs == 30)) {         /* GECO NORSTAR 2 & 3 */
      x = ((float) conv_real4(hd->gs_ofst+opt->spn_fld, data));
      if(pd->sp_un==6) x = x * MS_KNOTS;     /* convert to KNOTS */
   }
   if(hd->gs == 71)      /* DIGICON TRACE0 */
      x = ( (float) conv_bcd(hd->gs_ofst+opt->spn_fld, 2, data) )/10.0F;
   if(sp == sum->fsp) {   /* if FSP of run */
      gsp.sol = x;        /* SOL speed */
      gsp.last = x;       /* set last read */
      h_flag=0;           /* set spec flag */
		sprintf(t_buf, "SOL SPEED ALONG TRACK %-1.1f %s", x, units[pd->sp_un]);
      scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
   }
   if(!x) {
      gsp.corr = gsp.corr +1;
      return;
   }
   if(sp == sum->lsp) {   /* if EOL */
      gsp.eol = x;        /* EOL speed */
		sprintf(t_buf, "EOL SPEED ALONG TRACK %-1.1f %s", x, units[pd->sp_un]);
      scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
   }
   if(x > gsp.max) gsp.max = x;   /* get MAX */
   if(x < gsp.min) gsp.min = x;   /* get MIN */
   gsp.tot = gsp.tot + x;         /* for MEAN */
   gsp.change = (float)fabs((double)(gsp.last-x))+gsp.change; /* DMEAN */
   if(x > spc->gsp) {
      gsp.spec = gsp.spec + 1;
      if(!h_flag) {
			sprintf(t_buf, "Track Speed > Spec: %-.1f %s", x, units[pd->sp_un]);
         h_flag=1;
         if(andef->lgsp_anl)
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
      }
   } else {
      if(h_flag) {
	sprintf(t_buf, "Track Speed < Spec: %-.1f %s", x, units[pd->sp_un]);
         h_flag=0;
         if(andef->lgsp_anl)
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
      }
   }
   gsp.last = x;
	gsp.read = gsp.read +1;
	if( graf->spd_anl ) {
		*gs_graf_temp = x;
		gs_graf_temp++;
	}
}


/****************************************************************
*	 process water speed - GECO NOR* 2/3
*****************************************************************/
void proc_ws(HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
   float x;
   char t_buf[41];
   static int h_flag;
	if ((hd->ws == 6)||(hd->ws == 30)) {			 /* GECO NORSTAR 2 & 3 */
		x = ((float) conv_real4(hd->ws_ofst+opt->spn_fld, data));
      if(pd->sp_un==6) x = x * MS_KNOTS;     /* convert to KNOTS */
	}
	x += opt->wasp_ofst;   /* add water speed offset correction */
   if(sp == sum->fsp) {   /* if FSP of run */
		ws.sol = x; 		  /* SOL speed */
		ws.last = x;		  /* set last read */
      h_flag=0;           /* set spec flag */
		sprintf(t_buf, "SOL WATER SPEED %-1.1f %s", x, units[pd->sp_un]);
      scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
   }
   if(!x) {
		ws.corr = ws.corr +1;
      return;
   }
   if(sp == sum->lsp) {   /* if EOL */
		ws.eol = x; 		 /* EOL speed */
		sprintf(t_buf, "EOL WATER SPEED %-1.1f %s", x, units[pd->sp_un]);
      scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
   }
	if(x > ws.max) ws.max = x;   /* get MAX */
	if(x < ws.min) ws.min = x;   /* get MIN */
	ws.tot = ws.tot + x; 		  /* for MEAN */
	ws.change = (float)fabs((double)(ws.last-x))+ws.change; /* DMEAN */
	if(x > spc->gsp) {	/* same spec for water & track speeds */
		ws.spec = ws.spec + 1;
      if(!h_flag) {
			sprintf(t_buf, "Water Speed > Spec: %-.1f %s", x, units[pd->sp_un]);
         h_flag=1;
			if(andef->lws_anl)
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
      }
   } else {
      if(h_flag) {
	sprintf(t_buf, "Water Speed < Spec: %-.1f %s", x, units[pd->sp_un]);
         h_flag=0;
			if(andef->lws_anl)
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
      }
   }
	ws.last = x;
	ws.read = ws.read +1;
	if( graf->spd_anl ) {
		*ws_graf_temp = x;
		ws_graf_temp++;
	}
}


/****************************************************************
*	 do current statistics - GECO NOR* 2/3 only
*****************************************************************/
void proc_current( long sp )
{
	float x;
	if( !gsp.last || !ws.last) {
		if(sp == sum->fsp)  cu.last = 0.0F;	/* if FSP of run */
		x = cu.last;
		cu.corr = cu.corr +1;
	} else {
	/* convention is that current is positive if ground speed is larger */
		/* i.e. a following current is positive */
		x = gsp.last - ws.last;
		if( x > cu.max ) cu.max = x;	  /* get MAX */
		if( x < cu.min ) cu.min = x;	  /* get MIN */
		cu.read = cu.read + 1;			  /* total analysed */
		if( sp == sum->fsp ) cu.sol = x;
		if( sp == sum->lsp ) cu.eol = x;
	}
	return;
}


/****************************************************************
*	 system heading (GECO, WESTERN, PRAKLA & DIGICON)
*****************************************************************/
void proc_syhd(HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
	float x;
	x = find_hed( hd->syhd_ofst + opt->spn_fld, hd->syhd, data );
	if( graf->hed_anl ) {	/* if graphics enabled */
		*sy_graf_temp = x;
		sy_graf_temp++;
	}
}


/****************************************************************
*	 process gyro heading - GECO NOR* 2/3 ONLY
*****************************************************************/
void proc_gyro(HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
   float x;
	x = find_hed( hd->gy_ofst + opt->spn_fld, hd->gy, data );
	x += opt->gyro_ofst;	/* add water speed offset correction */
	if( graf->hed_anl ) {
		*gy_graf_temp = x;
		gy_graf_temp++;
	}
}


/****************************************************************
*	do crab angle stats
*****************************************************************/
void proc_heading( long sp )
{
	return;
}
