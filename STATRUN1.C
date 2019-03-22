/***************************************************************
*    STATRUN1.C  master run engine for stats
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
#include <malloc.h>
#include <time.h>

#include <dw.h>            /* greenleaf */
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>

			/* global pointer declarations */
struct ST_SPFN *sps;    /* SP stats */
struct ST_SPFN *fns;    /* FN stats */
struct ST_DCOM *dco;    /* RU comms stats */
struct ST_CDEP *dp1;    /* depth specs cable 1 stats */
struct ST_CDEP *dp2;    /* depth specs cable 2 stats */
struct ST_CDEP *dp3;    /* depth specs cable 3 stats */
struct ST_CDEP *dp4;    /* depth specs cable 4 stats */
struct SRCE_ERR *gun;   /* pointer to integer gunspecs */
struct ST_SRCE *sr1;    /* source 1 stats */
struct ST_SRCE *sr2;    /* source 2 stats */
struct ST_SRCE *sr3;    /* source 3 stats */
struct ST_SRCE *sr4;    /* source 4 stats */
struct ST_XSRC *xsr;    /* crossfire stats from local gun contr */
struct ST_MISF *msf;    /* consecutive & series misfire stats */
unsigned *s1m1,         /* source 1 spec1 array for 'x in Y' misfires */
         *s2m1,         /* ditto source 2 */
         *s3m1,         /* ditto 3 */
         *s4m1,         /* ditto 4 */
         *s1m2,         /* source 1 spec2 array for 'x in Y' misfires */
         *s2m2,         /* ditto source 2 */
         *s3m2,         /* ditto 3 */
         *s4m2;         /* ditto 4 */
struct ST_MAN *man;     /* manifold pressure stats */
struct ST_CLK *t_clk;   /* system timing stats structure */
struct ST_GMSK *gmsk;   /* gun mask stats */
struct ST_SPTIM *t_int; /* SP time interval stats */
struct FIR_SEQ *fire;   /* struct for loop fire sequence */
struct FIR_SEQ *last;   /* struct for loop fire sequence */

         /* global structure declarations */
struct SEQ_DEF st_seq;     /* hold sequencing stats */
struct GSP_DEF gsp;			/* hold speed along track (ground speed) stats */
struct GSP_DEF ws;			/* hold water speed (log speed) stats */
struct GSP_DEF cu;			/* hold current (ground - water speeds) stats */

         /* graphics error array pointers */
char *sp_err;           /* SP (semaphore) graphics */
char *sp_tmp;
char *fn_err;           /* FN (semaphore) graphics */
char *fn_tmp;
char *tm_err;           /* SP timing (semaphore) graphics */
char *tm_tmp;
float *c_graf1_min;     /* Cable 1 (varfill) graphics */
float *c_graf1_min_tmp;
float *c_graf1_max;
float *c_graf1_max_tmp;
float *c_graf2_min;     /* Ditto 2 */
float *c_graf2_min_tmp;
float *c_graf2_max;
float *c_graf2_max_tmp;
float *c_graf3_min;     /* Ditto 3 */
float *c_graf3_min_tmp;
float *c_graf3_max;
float *c_graf3_max_tmp;
float *c_graf4_min;     /* Ditto 4 */
float *c_graf4_min_tmp;
float *c_graf4_max;
float *c_graf4_max_tmp;
float *srce_graf;       /* source timing error array pointer */
float *srce_graf_tmp;
char *gmsk_graf;        /* gun mask error array pointer */
char *gmsk_graf_tmp;
char *seq_graf;         /* firing sequence array pointer */
char *seq_graf_tmp;
struct FIRE_BUF *fire_buf;     /* graphics initial fire sequence text */
struct FIRE_BUF *fire_buf_tmp; /* ditto temp */
unsigned *man_graf;             /* manifold air error array */
unsigned *man_graf_tmp;         /* ditto temp */
unsigned *vol_graf;             /* source volume error array */
unsigned *vol_graf_tmp;         /* ditto temp */
float *tim_graf;                /* system timing error array */
float *tim_graf_tmp;            /* ditto temp */
float *gundep_graf_min;         /* gun depth min error array */
float *gundep_graf_min_tmp;     /* ditto temp */
float *gundep_graf_max;         /* gun depth max error array */
float *gundep_graf_max_tmp;     /* ditto temp */
struct ERR_GRAF grerr;			  /* no-error graph flags */
float *gs_graf;					  /* ground speed error array */
float *gs_graf_temp;
float *ws_graf;					  /* water speed error array */
float *ws_graf_temp;
float *sy_graf;					  /* system heading error array */
float *sy_graf_temp;
float *gy_graf;					  /* gyro heading error array */
float *gy_graf_temp;
long *shot; 						  /* shotpoint number array */
long *shot_temp;

			/* external global declarations */
extern struct ST_SUMM *sum;		/* summary structure pointer */
extern struct HDR_DEF *hd;       /* header  list pointer */
extern struct PRO_DEF *pd;       /* project list pointer */
extern struct OPT_DEF  *opt;     /* pointer to options list */
extern struct ANAL_DEF *andef;   /* pointer to set/analysis list */
extern struct SPEC_DEF *spc;     /* pointer to specifications list */
extern struct CAB_DEF *stcab; 	/* pointer to start of cable list */
extern struct SRCE_DEF *stsrce;	/* pointer to start of source list */
extern struct EXT_DEF *ext;		/* external general variables */
extern struct ST_WAT *wat;       /* water depth analysis */
extern struct GRAF_DEF *graf;    /* pointer to graphics list */
extern char *units[];				/* pointer to units list */
extern char *data;               /* pointer to data buffer */
extern char *answer[];           /* YES/NO - + ? etc etc */
extern char ln_name[17], cl_name[17],    /* line & client names */
     vs_name[17], ar_name[17];   /* vessel & area names */


/*************************************************************
* main RUN engine
**************************************************************/
statrun (FILE *fi)
{
	HWND Wproc;               /* processing totals window */
	HWND Werroll;             /* scrolling error window */
   HWND Wmsg;                /* function key window */
#ifdef DEBUG
	HWND W2; 					  /* test window only - delete after */
#endif
   register int i = 0;
   time_t tstart, tend;      /* for processing time */
	int rr;                   /* switch to stat_make() */
   int c = 0;                /* escape during run */
   FILE *flog = NULL;        /* log (.LOG) file */
	FILE *tprn = NULL;        /* timing (.PRN) file */
   FILE *firs = NULL;        /* firing sequence (.SEQ) file */
	FILE *grif = NULL;		  /* independant graphics (.GRF) file */
	char errbuf[71];			  /* buffer for Werroll errors */
	char t_buf[41];           /* temporary error buffer */
	long sp;                  /* SP being processed */
	long prev_sp;             /* previous SP being processed */
	long fn, prev_fn;         /* ditto FN */
	int currfl = 0;           /* flags a corrupt SP if set */
	sum->procno = 0;          /* init number processed */
   sum->currno = 0;          /* init number corrupt */
            /* check drives & directories & open disk files */
   if(check_drive(opt->work_dr, opt->txt_dir))  /* check txt dr/dir */
      return(1);
   if(!(flog = log_file())) return(1);  /* open & init .LOG file */
   if ((andef->clk_anl)&&(hd->clk)) {   /* open & init .PRN file */
      if(!(tprn = prn_file())) {
         fclose(flog);
         return(1);
      }
   }
   if(andef->seq_anl && hd->gm) {     /* open & init .SEQ file */
      if(!(firs = seq_file())) {
         fclose(flog);
         if(tprn) fclose(tprn);
         return(1);
      }
	}
		/* NULL all relevant pointers before malloc */
   sps = NULL;     /* SP stats */
   fns = NULL;     /* FN stats */
   dco = NULL;     /* RU comms stats */
   dp1 = NULL;     /* depth specs cable 1 stats */
   dp2 = NULL;     /* depth specs cable 2 stats */
   dp3 = NULL;     /* depth specs cable 3 stats */
   dp4 = NULL;     /* depth specs cable 4 stats */
   gun = NULL;     /* pointer to integer gunspecs */
   sr1 = NULL;     /* source 1 stats */
   sr2 = NULL;     /* source 2 stats */
   sr3 = NULL;     /* source 3 stats */
   sr4 = NULL;     /* source 4 stats */
   xsr = NULL;     /* source crossfire stats */
   msf = NULL;     /* consecutive & series misfire stats */
   s1m1 = NULL;    /* source 1 spec1 array for 'x in Y' misfires */
   s2m1 = NULL;    /* ditto source 2 */
   s3m1 = NULL;    /* ditto 3 */
   s4m1 = NULL;    /* ditto 4 */
   s1m2 = NULL;    /* source 1 spec2 array for 'x in Y' misfires */
   s2m2 = NULL;    /* ditto source 2 */
   s3m2 = NULL;    /* ditto 3 */
   s4m2 = NULL;    /* ditto 4 */
   man = NULL;     /* manifold pressure stats */
   t_clk = NULL;   /* timing stats structure */
   gmsk = NULL;    /* gun mask stats */
   wat = NULL;     /* water depth stats */
   t_int = NULL;   /* SP interval stats */
   fire = NULL;    /* first of firing sequence */
   last = NULL;    /* ditto last */
   sp_err=NULL;       /* SP (semaphore) graphics */
   fn_err=NULL;       /* FN (semaphore) graphics */
   tm_err=NULL;       /* SP timing (semaphore) graphics */
   c_graf1_min=NULL;  /* Cable 1 (varfill) graphics */
   c_graf1_max=NULL;
   c_graf2_min=NULL;  /* Ditto 2 */
   c_graf2_max=NULL;
   c_graf3_min=NULL;  /* Ditto 3 */
   c_graf3_max=NULL;
   c_graf4_min=NULL;  /* Ditto 4 */
   c_graf4_max=NULL;
   srce_graf = NULL;  /* source timing error array pointer */
   gmsk_graf = NULL;  /* gun mask error array pointer */
   seq_graf=NULL;     /* firing sequence array pointer */
   fire_buf=NULL;     /* graphics initial fire sequence text */
   man_graf=NULL;     /* air pressure array pointer */
   vol_graf=NULL;     /* source volume error array */
   tim_graf=NULL;     /* system timing error array */
   gundep_graf_min=NULL;   /* gun depth min error array */
   gundep_graf_max=NULL;   /* gun depth max error array */
	gs_graf = NULL;					  /* ground speed error array */
	gs_graf_temp = NULL;
	ws_graf = NULL;					  /* water speed error array */
	ws_graf_temp = NULL;
	sy_graf = NULL;					  /* system heading error array */
	sy_graf_temp = NULL;
	gy_graf = NULL;					  /* gyro heading error array */
	gy_graf_temp = NULL;
	shot = NULL;
	shot_temp = NULL;
				/* reset sequencing stats */
   st_seq.change = 0;
   st_seq.reset = 0;
				/* reset ground speed stats */
   gsp.min = 1000.0F;
	gsp.tot = 0.0F;
	gsp.max = -1000.0F;
   gsp.last = 0.0F;
   gsp.change = 0.0F;
   gsp.spec = 0;
   gsp.read = 0;
   gsp.sol = 0.0F;
   gsp.eol = 0.0F;
	gsp.corr = 0;
				/* reset water speed stats */
	ws.min = 1000.0F;
	ws.tot = 0.0F;
	ws.max = -1000.0F;
	ws.last = 0.0F;
	ws.change = 0.0F;
	ws.spec = 0;
	ws.read = 0;
	ws.sol = 0.0F;
	ws.eol = 0.0F;
	ws.corr = 0;
				/* reset current stats */
	cu.min = 1000.0F;
	cu.tot = 0.0F;
	cu.max = -1000.0F;
	cu.last = 0.0F;
	cu.change = 0.0F;
	cu.spec = 0;
	cu.read = 0;
	cu.sol = 0.0F;
	cu.eol = 0.0F;
	cu.corr = 0;
            /* reset no-error box graphic bool flags */
   grerr.sp = 0;             /* sp numbering */
   grerr.fn = 0;             /* fn numbering */
   grerr.tm = 0;             /* sp time intervals */
   grerr.fr = 0;             /* fire sequence */
   grerr.gm = 0;             /* gun masks */
   grerr.gu = 0;             /* srce timing */
   grerr.pr = 0;             /* air pressure */
   grerr.vl = 0;             /* srce volumes */
   grerr.tb = 0;             /* system timing */
   grerr.c1 = 0;             /* cable depth 1 */
   grerr.c2 = 0;             /* cable depth 2 */
   grerr.c3 = 0;             /* cable depth 3 */
   grerr.c4 = 0;             /* cable depth 4 */
   grerr.gd = 0;             /* gun depths */
   grerr.s1 = 0;             /* spare 1 */
   grerr.s2 = 0;             /* spare 2 */
            /* unfragment & check heap & set locations to 0 */
   _heapmin();
   i = _heapset(48);
      switch (i) {
         case _HEAPOK:
            break;
         case _HEAPEMPTY:
            StatsErr("11006");   /* OK empty heap */
            break;
         case _HEAPEND:
            StatsErr("11007");   /* OK - heap end */
            break;
         case _HEAPBADPTR:
            StatsErr("11008");   /* bad pointer */
            break;
         case _HEAPBADBEGIN:
            StatsErr("11009");   /* bad start */
            break;
         case _HEAPBADNODE:
            StatsErr("11010");   /* bad node */
            break;
      }
            /* main memory allocation routines */
   if(init_stat()) {
      free_stat_init();   /* if not enough memory available */
      StatsErr("11003");  /* suggest less analyses or TSRs */
      StatsErr("11004");
      StatsErr("11005");
      fclose(flog);       /* close open report files */
      fclose(tprn);
      fclose(firs);
      return(1);          /* return to main menu */
   }
            /* bottom line window */
	Wmsg = vcreat(1, 80, REVNORML, NO);
	vlocate(Wmsg, 24, 0);
   vratputs(Wmsg, 0, 29, REVNORML, "<ESC> Interrupt Run");
   visible(Wmsg, YES, YES);
            /* processing status window */
	Wproc = vcreat(4, 58, NORML, NO);
   vlocate(Wproc, 1, 1);
	vframe (Wproc, EMPHNORML, FRDOUBLE);
   vtitle (Wproc, EMPHNORML, " Status ");
            /* scrolling error log window */
	Werroll = vcreat(15, 70, NORML, NO);
	vlocate (Werroll, 8, 5);
	vframe (Werroll, EMPHNORML, FRDOUBLE);
   vtitle (Werroll, EMPHNORML, " Errors ");
            /* write initial processing status */
   vatputf(Wproc, 0, 2, "File %-13s Headstat ID: %s", ext->pname, sum->comment);
	vatputf(Wproc, 1, 2, "SP Range %6ld - %-6ld", sum->fsp, sum->lsp);
	if(hd->fn)
	vatputf(Wproc, 1, 26, "  FN Range %6ld - %-6ld", sum->ffn, sum->lfn);
	else vatputs(Wproc, 1, 26, "                         ");
   vatputf(Wproc, 2, 2, "To Process: %ld", sum->act_sp);
   vatputs(Wproc, 2, 27, "SP:");
   vatputs(Wproc, 2, 40, "Time:     Secs");
   vatputs(Wproc, 3, 3, "Processed:");
   vatputs(Wproc, 3, 22,"Corrupt:");
   vatputs(Wproc, 3, 36 ,"Waypoint:");
   if(sum->way_fl)
      vatputf(Wproc, 3, 46, "%-6ld", sum->way_sp);
   else
      vatputs(Wproc, 3, 46, "NONE  ");
            /* make windows visible */
	visible (Werroll, YES, YES);
   visible(Wproc, YES, YES);
            /* set first record file position */
   fseek (fi, sum->fsp_pos, SEEK_SET);
   time(&tstart);   /* get the start time */
            /* START READ LOOP */
   do {
      sum->procno++;   /* add to processed total */
      if(gfkbhit()) (c = key_int(Werroll, sp, fn, errbuf, flog)); /* if ESC */
			/* master read function */
      if(feof(fi) || ((opt->spn_fld+hd->h_len) != fread (data, sizeof (char),
                                  opt->spn_fld + hd->h_len, fi)) ) {
         if(!feof(fi)) StatsErr("11001");   /* read error */
         else StatsErr("11002");            /* EOF error */
			vdelete(Wmsg, NONE);
			vdelete(Werroll, NONE);
			vdelete(Wproc, NONE);
         free_stat_init();
         fclose(flog);
         if(tprn) fclose(tprn);
         if(firs) fclose(firs);
			return(1);
      }
         /* if 'Additional Header Length' - set next start byte to
            have the added padded bytes added (F5 Options) */
      if(opt->add_hdr) {
         if(fseek (fi, (long) opt->add_hdr, SEEK_CUR) ) {
            StatsErr("11011");   /* fseek return error */
            vdelete(Wmsg, NONE);
            vdelete(Werroll, NONE);
            vdelete(Wproc, NONE);
            free_stat_init();
            fclose(flog);
            if(tprn) fclose(tprn);
            if(firs) fclose(firs);
            return(1);
         }
      }
         /* write waypoint SP if flag is set */
      if(sum->way_fl)
         vratputf(Wproc, 3, 46, REVERR, "%-6ld", sum->way_sp);
      else
         vatputs(Wproc, 3, 46, "NONE  ");
         /* write the elapsed processing time */
      time(&tend);
      vatputf(Wproc, 2, 46, "%d",  (int) difftime( tend, tstart ));
		currfl = 0;   /* reset the corrupt flag */
		if(hd->sp)
         sp = find_sp (hd->sp_ofst + opt->spn_fld, hd->sp, data);
      if((sp == sum->way_sp)&&(sum->way_fl)) {  /* check for a waypoint */
         scr_error("WAYPOINT", errbuf, sp, fn, Werroll, flog);
         key_int(Werroll, sp, fn, errbuf, flog);
      }
		if (hd->fn)
			fn = find_fn (hd->fn_ofst + opt->spn_fld, hd->fn, data);
		if(hd->tm)
			find_tm ((hd->tm_ofst + opt->spn_fld), hd->tm, data);
      if (sp == sum->fsp) {        /* FSP OF RUN */
         scr_error("START RUN", errbuf, sp, fn, Werroll, flog);
            /* assign temporary graphics error array pointers */
         sp_tmp = sp_err;            /* SP */
         fn_tmp = fn_err;            /* FN */
         tm_tmp = tm_err;            /* SP timing */
         srce_graf_tmp = srce_graf;  /* source timing */
         gmsk_graf_tmp = gmsk_graf;  /* gun mask errors */
         seq_graf_tmp = seq_graf;    /* sequence errors */
         fire_buf_tmp = fire_buf;    /* graphics fire seq text */
         man_graf_tmp = man_graf;    /* air pressure errors */
         vol_graf_tmp = vol_graf;    /* srce volume errors */
         tim_graf_tmp = tim_graf;    /* system timing errors */
			gs_graf_temp = gs_graf;		 /* ground speed error array */
			ws_graf_temp = ws_graf;		 /* water speed error array */
			shot_temp = shot; 			 /* SP number array */
			sy_graf_temp = sy_graf;		 /* system heading error array */
			gy_graf_temp = gy_graf;		 /* gyro heading error array */
				/* initialise gun depths graphics error array */
         if(hd->gd && ext->numberdp && andef->gd_anl && graf->gd_anl) {
            gundep_graf_min_tmp = gundep_graf_min; /* assign temp start */
            gundep_graf_max_tmp = gundep_graf_max;
            for(i=0; i<(int)sum->act_sp; i++) {    /* set array values */
               gundep_graf_min_tmp[i] = spc->gd_min;
               gundep_graf_max_tmp[i] = spc->gd_max;
            }
         }
            /* initialise cable depths graphics error array */
         if(hd->ca && ext->numberru && pd->loc_strm
                   && andef->dep_anl1 && graf->dep_anl1) {
            c_graf1_min_tmp = c_graf1_min;         /* assign temp start */
            c_graf1_max_tmp = c_graf1_max;
            c_graf2_min_tmp = c_graf2_min;
            c_graf2_max_tmp = c_graf2_max;
            c_graf3_min_tmp = c_graf3_min;
            c_graf3_max_tmp = c_graf3_max;
            c_graf4_min_tmp = c_graf4_min;
            c_graf4_max_tmp = c_graf4_max;
            for(i=0; i<(int)sum->act_sp; i++) {    /* set array values */
               c_graf1_min_tmp[i] = spc->d1min;    /* always 1 cable */
               c_graf1_max_tmp[i] = spc->d1max;
               if(pd->loc_strm > 1) {
                  c_graf2_min_tmp[i] = spc->d1min;
                  c_graf2_max_tmp[i] = spc->d1max;
               }
               if(pd->loc_strm > 2) {
                  c_graf3_min_tmp[i] = spc->d1min;
                  c_graf3_max_tmp[i] = spc->d1max;
               }
               if(pd->loc_strm > 3) {
                  c_graf4_min_tmp[i] = spc->d1min;
                  c_graf4_max_tmp[i] = spc->d1max;
               }
            }
         }
      }
         /* determine if SP outside proc range & therefore corrupt */
      if ( (sp < sum->fsp && sp < sum->lsp) ||
           (sp > sum->fsp && sp > sum->lsp) ) {
         currfl = 1;         /* set corrupt flag if outside range */
         sum->currno++;
         sp = 0L;
		}
			/* write SP number to array for graphing */
		if( ( hd->gs && hd->ws && andef->cu_anl && graf->spd_anl ) ||
			 (	hd->syhd && hd->gy && andef->cr_anl && graf->hed_anl ) ) {
			*shot_temp = sp;
			shot_temp++;
		}
         /* DO FOLLOWING ROUTINES WHETHER SP CORRUPT OR NOT */
            /* file number numbering - GECO & DIGICON TRACE0 */
      if ( (!hd->contr || hd->contr == 5) && andef->fn_anl
                 && hd->fn && (sp != sum->fsp) )
         proc_fn (Werroll, fn, prev_fn, errbuf, sp, flog);

            /* cable depth remote unit comms - GECO only */
      if (!hd->contr && ext->numberru && andef->dep_com && hd->ca)
         proc_dcom (Werroll, fn, errbuf, sp, flog);

            /* cable depths - spec 1 */
      if (andef->dep_anl1 && ext->numberru && hd->ca)
         proc_dep1 (Werroll, fn, errbuf, sp, flog);

            /* cable depths spec 2 */
      if (andef->dep_anl2 && ext->numberru && hd->ca)
         proc_dep2 (Werroll, fn, errbuf, sp, flog);

            /* cable depth controller wing angles - GECO only */
      if (!hd->contr && ext->numberwing && andef->wi_anl && hd->wi)
         proc_wing (Werroll, fn, errbuf, sp, flog);

            /* source gun timing & volume analysis */
      if(hd->gu && ext->numbergun && (andef->gun_anl1 || andef->gun_anl2 ||
            andef->vol_anl1 || andef->vol_anl2) )
         proc_srce(Werroll, fn, errbuf, sp, flog);

            /* system (TB) or intervessel timing - NOT Western */
      if ( (hd->contr != 2) && andef->clk_anl && hd->clk)
         proc_clk(Werroll, fn, errbuf, sp, flog, tprn);

            /* air pressure - NOT Western */
      if ( (hd->contr != 2) && (andef->man_anl1 || andef->man_anl2) && hd->ma)
         proc_man(Werroll, fn, errbuf, sp, flog);

            /* gun masks - GECO only */
      if(!hd->contr && andef->gmsk_anl && hd->gm)
         proc_gmsk(Werroll, fn, errbuf, sp, flog, 0);

            /* source firing sequence - GECO only */
      if(!hd->contr && andef->seq_anl && hd->gm)
         proc_fseq(fn, sp, firs);

            /* source depths */
      if(andef->gd_anl && ext->numberdp && hd->gd)
         proc_gundp(Werroll, fn, errbuf, sp, flog);

            /* water depths */
      if(andef->wdp_anl && hd->wa)
         proc_wadep(Werroll, fn, errbuf, sp, flog);

			/* IF SP NOT CORRUPT DO FOLLOWING PROCESSING ROUTINNES */
      if(!currfl) {
               /* shotpoint numbering */
         if ( andef->sp_anl && hd->sp && (sp != sum->fsp) )
            proc_sp (Werroll, sp, prev_sp, errbuf, fn, flog);

               /* time between SPs */
			if(andef->tim_anl && hd->tm)
				proc_sptim(Werroll, fn, errbuf, sp, flog);

					/* speed along track - GECO & DIGICON TRACE0 */
			if( (!hd->contr || hd->contr == 5) && andef->gsp_anl && hd->gs)
				proc_sal(Werroll, fn, errbuf, sp, flog);

					/* water speed - GECO */
			if( !hd->contr && andef->ws_anl && hd->ws)
				proc_ws(Werroll, fn, errbuf, sp, flog);

					/* current - GECO */
			if( !hd->contr && andef->ws_anl
								&& hd->ws && hd->gs && andef->gsp_anl)
				proc_current( sp );

					/* system heading - GECO  */
			if( !hd->contr && andef->cr_anl && hd->syhd)
				proc_syhd(Werroll, fn, errbuf, sp, flog);

					/* gyro heading - GECO */
			if( !hd->contr && andef->cr_anl && hd->gy)
				proc_gyro(Werroll, fn, errbuf, sp, flog);

					/* crab angle - GECO */
			if( !hd->contr && andef->cr_anl && hd->syhd && hd->gy )
				proc_heading( sp );

		}
               /* EOL gun volumes if enabled */
      if( (sp == sum->lsp) && (andef->vol_anl1 || andef->vol_anl2) ) {
         if(sr1->en_num && pd->loc_srce) {
            sprintf(t_buf, "EOL VOLUME: %s %u %s", pd->loc1, sr1->sp_vol, units[pd->vol_un]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
         if(sr2->en_num && (pd->loc_srce > 1) ) {
            sprintf(t_buf, "EOL VOLUME: %s %u %s", pd->loc2, sr2->sp_vol, units[pd->vol_un]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
         if(sr3->en_num && (pd->loc_srce > 2) ) {
            sprintf(t_buf, "EOL VOLUME: %s %u %s", pd->loc3, sr3->sp_vol, units[pd->vol_un]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
         if(sr4->en_num && (pd->loc_srce > 3) ) {
            sprintf(t_buf, "EOL VOLUME: %s %u %s", pd->loc4, sr4->sp_vol, units[pd->vol_un]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
      }
		prev_fn = fn;
		if (!currfl) {
			prev_sp = sp;
         vratputf(Wproc, 2, 31, REVERR, "%-6ld", sp);
		} else {
         vratputf(Wproc, 2, 31, HIGHERR, "%-6ld", prev_sp);
         scr_error("SP NUMBER CORRUPT", errbuf, sp, fn, Werroll, flog);
		}
      vratputf(Wproc, 3, 14, REVERR, "%-6ld", sum->procno);
      vratputf(Wproc, 3, 31, REVERR, "%ld", sum->currno);
		currfl = 0;
		if (sp == sum->lsp) {
         scr_error("END RUN", errbuf, sp, fn, Werroll, flog);
		}
   } while ((ftell(fi) <= sum->lsp_pos) && !c);  /* END READ LOOP */

	if(sp != sum->lsp) {
      scr_error("ABORTED RUN", errbuf, sp, fn, Werroll, flog);
	}
	StWait();	/* set the WORKIN! sign going */
   if(flog) {   /* if a .LOG file, write end & close it */
      fprintf(flog,
   "------------------------------------------------------------------\n");
      fprintf(flog, "Run complete on %s @ %-24s INTERA ECL\n",
                     GetFDate(ext->a_str, 26), GetFTime(ext->b_str, 6) );
      fprintf(flog,
   "==================================================================\n");
      fclose (flog);   /* close .LOG file if opened */
   }
   if(tprn) fclose (tprn);   /* close .PRN file if opened */
   if(firs) {   /* if a .SEQ firing sequence file, write end & close it */
      fprintf(firs,
   "------------------------------------------------------------------\n");
      fprintf(firs, "Run complete on %s @ %-24s INTERA ECL\n",
                     GetFDate(ext->a_str, 26), GetFTime(ext->b_str, 6) );
      fprintf(firs,
   "==================================================================\n");
      fclose (firs);   /* close .SEQ file if opened */
	}
			/* write the values to the .GRF file */
	if( ( hd->gs && hd->ws && andef->cu_anl && graf->spd_anl ) ||
		 (	hd->syhd && hd->gy && andef->cr_anl && graf->hed_anl ) ) {
		shot_temp = shot; 			 /* shotpoint number array */
		gs_graf_temp = gs_graf;		 /* ground speed error array */
		ws_graf_temp = ws_graf;		 /* water speed error array */
		sy_graf_temp = sy_graf;		 /* system heading error array */
		gy_graf_temp = gy_graf;		 /* gyro heading error array */
		if(!(grif = grf_file())) {  /* open .GRF file */
         fclose(flog);
         if(tprn) fclose(tprn);
			if(firs) fclose(firs);
         return(1);
      }
				/* 1 if speed done */
		rr = 0;
		if( graf->spd_anl ) rr = 1;  /* if speed done */
		fwrite( &rr, sizeof( int ), 1, grif );
				/* 1 if heading done */
		rr = 0;
		if( graf->hed_anl ) rr = 1;	/* if heading done */
		fwrite( &rr, sizeof( int ), 1, grif );
				/* write number of array items to .GRF */
		rr = ( (int) sum->act_sp ) - 1;	 /* convert to integer */
		fwrite( &rr, sizeof( int ), 1, grif );
				/* write filename, client & area */
		strcpy( errbuf, ext->pname ); 			 /* 12 */
		if( hd->cl ) {
			strcat( errbuf, " " );               /* 1 */
			strcat( errbuf, cl_name ); 			 /* 16 */
		}
		if( hd->ar ) {
		strcat( errbuf, " " );						 /* 1 */
		strcat( errbuf, ar_name ); 				 /* 16 */
		}
		if( hd->vs ) {
		strcat( errbuf, " " );						 /* 1 */
		strcat( errbuf, vs_name ); 				 /* 16 */
		}
		fprintf( grif, "%-70s\0", errbuf );
				/* write line name, date, start/end times & SP range */
		if( hd->ln )
			strcpy( errbuf, ln_name ); 			 /* 16 */
		else if( strlen( sum->comment ) )
			strcpy( errbuf, sum->comment );		 /* or 21 */
		else strcpy( errbuf, ext->pname);		 /* or 12 */
		if( hd->jd ) {
			strcat( errbuf, "  JD:" );          /* 5 */
			strcat( errbuf, itoa( sum->fsp_date, t_buf, 10 ) );  /* 3 */
		}
		if( hd->tm ) {
			strcat( errbuf, " " );              /* 1 */
			strcat( errbuf, sum->fsp_tim );		 /* 8 */
			strcat( errbuf, " - " );				 /* 3 */
			strcat( errbuf, sum->lsp_tim );		 /* 8 */
		}
		strcat( errbuf, "  SP: " );				 /* 6 */
		strcat( errbuf, ltoa( sum->fsp, t_buf, 10 ) );	  /* 6 */
		strcat( errbuf, "/" );									  /* 1 */
		strcat( errbuf, ltoa( sum->lsp, t_buf, 10 ) );	  /* 6 */
		fprintf( grif, "%-70s\0", errbuf );
				/* write SP numbers to .GRF */
		for( i = 0; i < rr; i++ )
			fprintf( grif, "%06ld", shot_temp[i] );
		if( graf->spd_anl ) {		/* if speed done */
				/* write ground speed (along track) to .GRF */
			for( i = 0; i < rr; i++ )
				fprintf( grif, "%09.4f", gs_graf_temp[i] );
				/* write water speed to .GRF */
			for( i = 0; i < rr; i++ )
				fprintf( grif, "%09.4f", ws_graf_temp[i] );
		}
		if( graf->hed_anl ) {		/* if heading done */
				/* write system heading to .GRF */
			for( i = 0; i < rr; i++ )
				fprintf( grif, "%09.4f", sy_graf_temp[i] );
				/* write gyro heading speed to .GRF */
			for( i = 0; i < rr; i++ )
				fprintf( grif, "%09.4f", gy_graf_temp[i] );
		}
		fclose( grif );
	}
	stat_log(); 				  /* write the stats disk log (.TXT) */
	vclear(Wmsg);
	vratputs(Wmsg, 0, 5, REVNORML, "F7 View Stats");
	vratputs(Wmsg, 0, 31, REVNORML, "F8 Run Graphics");
   vratputs(Wmsg, 0, 52, REVNORML, "<ESC> Return Main Menu");
   i=0;
   vbeep();      /* double beep to signal end of run */
   Delay18(3);
	vbeep();
	EnWait();	/* turn working sign off */
   do {                      /* switch to stats or graphics or not */
		rr = vgetkey(UpdateClockWindow);
		switch (rr) {
         case F7:            /* screen stats */
				_heapmin();
				stat_make();
            _heapmin();
				break;
						/* DEBUG code for no-error blocks */
#ifdef DEBUG
         case F2:
   W2 = vcreat (16, 30, NORML, NO);
   vlocate (W2, 4, 20);
   vatputf(W2, 0, 0, "sp numbering  :  %d", grerr.sp);
   vatputf(W2, 1, 0, "fn numbering  :  %d", grerr.fn);
   vatputf(W2, 2, 0, "sp time ints  :  %d", grerr.tm);
   vatputf(W2, 3, 0, "fire sequence :  %d", grerr.fr);
   vatputf(W2, 4, 0, "gun masks     :  %d", grerr.gm);
   vatputf(W2, 5, 0, "srce timingg  :  %d", grerr.gu);
   vatputf(W2, 6, 0, "air pressure  :  %d", grerr.pr);
   vatputf(W2, 7, 0, "srce volumes  :  %d", grerr.vl);
   vatputf(W2, 8, 0, "system timing :  %d", grerr.tb);
   vatputf(W2, 9, 0, "cable 1 depth :  %d", grerr.c1);
   vatputf(W2, 10, 0, "cable 2 depth :  %d", grerr.c2);
   vatputf(W2, 11, 0, "cable 3 depth :  %d", grerr.c3);
   vatputf(W2, 12, 0, "cable 4 depth :  %d", grerr.c4);
   vatputf(W2, 13, 0, "gun depth     :  %d", grerr.gd);
   vatputf(W2, 14, 0, "spare 1       :  %d", grerr.s1);
   vatputf(W2, 15, 0, "spare 2       :  %d", grerr.s2);
   visible (W2, YES, YES);
   vgetkey(NULLF);
   vdelete (W2, NONE);
				break;
# endif
						/* end debug code */

         case F8:            /* graphics */
            _heapmin();
            visible(Wmsg, NO, YES);
            visible (Werroll, NO, YES);
            visible(Wproc, NO, YES);
            graf_init();     /* run graphics */
            pcuroff();
            visible(Wmsg, YES, YES);
            visible (Werroll, YES, YES);
            visible(Wproc, YES, YES);
            _heapmin();
            break;
         case ESC:           /* return to main menu */
				if(KeySure()) i=1;
				break;
#ifdef DEBUG
         case ALTM:
            heapstat();
				break;
#endif
		}
	} while(!i);
	vdelete (Werroll, NONE);
   vdelete (Wproc, NONE);
	vdelete (Wmsg, NONE);
            /* free allocated memory */
   free_stat_init();
	return (0);
}


/*************************************************************
* free allocated memory - reverse order to allocation !!
**************************************************************/
void free_stat_init()
{
   register int i = 0;
	_heapmin();
	if( gy_graf) free( gy_graf );
	if( sy_graf) free( sy_graf );
	if( ws_graf) free( ws_graf );
	if( gs_graf) free( gs_graf );
   if(tm_err) free (tm_err);
   if(t_int) free (t_int);
   if(wat) free (wat);
   if (gundep_graf_max) free (gundep_graf_max);  /* gundepths max */
   if (gundep_graf_min) free (gundep_graf_min);  /* gundepths min */
   if (gmsk_graf) free (gmsk_graf);         /* gun mask error array */
	if(gmsk) free(gmsk);
   if (tim_graf) free (tim_graf);           /* system timing */
	if(t_clk) free(t_clk);
   if (man_graf) free (man_graf);           /* air pressure */
	if(man) free(man);
   if (srce_graf) free (srce_graf);         /* source error array */
   if (vol_graf) free (vol_graf);           /* source volumes */
   if (s4m2) free (s4m2);  /* source 4 spec2 array for 'x in Y' misfires */
   if (s3m2) free (s3m2);  /* ditto 3 */
   if (s2m2) free (s2m2);  /* ditto 2 */
   if (s1m2) free (s1m2);  /* ditto 1 */
   if (s4m1) free (s4m1);  /* source 1 spec1 array for 'x in Y' misfires */
   if (s3m1) free (s3m1);  /* ditto 3 */
   if (s2m1) free (s2m1);  /* ditto 2 */
   if (s1m1) free (s1m1);  /* ditto 1 */
   if(msf) free (msf);
	if(sr4) free(sr4);
	if(sr3) free(sr3);
	if(sr2) free(sr2);
	if(sr1) free(sr1);
   if(xsr) free (xsr);
	if(gun) free(gun);
   if (c_graf4_max) free (c_graf4_max);
   if (c_graf4_min) free (c_graf4_min);     /* Ditto 4 */
	if(dp4) free(dp4);
   if (c_graf3_max) free (c_graf3_max);
   if (c_graf3_min) free (c_graf3_min);     /* Ditto 3 */
	if(dp3) free(dp3);
   if (c_graf2_max) free (c_graf2_max);
   if (c_graf2_min) free (c_graf2_min);     /* Ditto 2 */
	if(dp2) free(dp2);
   if (c_graf1_max) free (c_graf1_max);
   if (c_graf1_min) free (c_graf1_min);     /* Cable 1 graphics array */
	if(dp1) free(dp1);
	if(dco) free(dco);
   if(fn_err) free (fn_err);
	if(fns) free(fns);
   if(sp_err) free (sp_err);
	if(sps) free(sps);
	if( shot ) free( shot );
   if (seq_graf) free (seq_graf);           /* fire seq error array */
   if (fire_buf) free (fire_buf);           /* fire seq text buffers */
   if (fire) {                              /* fire sequence linked list */
      for(i=0;i<(int)pd->seq_len;i++) {
         free (fire);
         fire = fire->next;
      }
   }
   _heapmin();
	return;
}


/*************************************************************
* check memory & heap	- DEBUG CODE only!
**************************************************************/
#ifdef DEBUG

void heapstat()              /*& MEMORY/HEAP */
{                            /*$ MEMORY/HEAP */
   HWND W1;
   struct _heapinfo entry;   /* allocate a heap structure for checking */
   register int i=0, c, n=1;
   unsigned long total_size=0, use=0, n_use=0;
      /* make the window*/
   W1 = vcreat (22, 70, NORML, NO);
   vlocate (W1, 2, 5);
   vframe (W1, NORML, FRSINGLE);
   visible(W1, YES, YES);
      /* walk the heap to see how many blocks & if in use */
   entry._pentry = NULL;
   while(c = _heapwalk(&entry)) {
      sprintf(ext->a_str, "%-7d Addr: %-13p %-8s Size: %-9lu",
               n, entry._pentry,
               entry._useflag == _USEDENTRY ? "USED" : "FREE",
               (unsigned long) entry._size);
      total_size += (unsigned long) entry._size;
      if(entry._useflag)
         use += (unsigned long) entry._size;
      else
         n_use += (unsigned long) entry._size;
      if(c == _HEAPOK)
         strcat(ext->a_str, "OK");
      else if(c == _HEAPBADBEGIN) {
         strcat(ext->a_str, "HEADER DAMAGED");
         break;
      }
      else if(c == _HEAPBADPTR) {
         strcat(ext->a_str, "INVALID _PENTRY");
         break;
      }
      else if(c == _HEAPBADNODE) {
         strcat(ext->a_str, "CORRUPTED NODE");
         break;
      }
      else if (c == _HEAPEND) {
         strcpy(ext->a_str, "LAST ENTRY");
         total_size -= (unsigned long) entry._size;
         if(entry._useflag)
            use -= (unsigned long) entry._size;
         else
            n_use -= (unsigned long) entry._size;
         break;
      }
      vatputs(W1, i, 2, ext->a_str);
      ++i;
      ++n;
      if(i>21) {
         vgetkey(UpdateClockWindow);
         vclear(W1);
         i=0;
      }
   }
   if(i>16) vscrolup(W1, i-16);
   vatputs(W1, 17, 2, ext->a_str);
   vatputf(W1, 18, 2, "Blocks: %-6d Total: %-9lu Used: %-9lu Free: %lu",
                 n, total_size, use, n_use);
            /* memory available */
   vatputf (W1, 19, 2, "Dos Free: %ld", 16L*(long)dos_mem());

   vatputf (W1, 20, 2, "Near Heap Free: %u", _memavl());
   vatputf (W1, 21, 2,
       "Largest Free Block: %-35u <ESC> Exit", _memmax());
   while(vgetkey(UpdateClockWindow) != ESC);
   vdelete(W1, NONE);
}
#endif
