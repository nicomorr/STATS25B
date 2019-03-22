/***************************************************************
*    STATINIT.C  initialise a LOT of variables
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

#include <stats.h>         /* wcs */
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct ST_SPFN *sps;      /* SP stats */
extern struct ST_SPFN *fns;      /* FN stats */
extern struct ST_DCOM *dco;      /* RU comms stats */
extern struct ST_CDEP *dp1;      /* depth specs cable 1 stats */
extern struct ST_CDEP *dp2;      /* depth specs cable 2 stats */
extern struct ST_CDEP *dp3;      /* depth specs cable 3 stats */
extern struct ST_CDEP *dp4;      /* depth specs cable 4 stats */
extern struct SRCE_ERR *gun;     /* pointer to integer gunspecs */
extern struct ST_SRCE *sr1;      /* source 1 stats */
extern struct ST_SRCE *sr2;      /* source 2 stats */
extern struct ST_SRCE *sr3;      /* source 3 stats */
extern struct ST_SRCE *sr4;      /* source 4 stats */
extern struct ST_XSRC *xsr;      /* source crossfire stats */
extern struct ST_MISF *msf;      /* consecutive & series misfire stats */
extern unsigned *s1m1,   /* source 1 spec1 array for 'x in Y' misfires */
                *s2m1,           /* ditto source 2 */
                *s3m1,           /* ditto 3 */
                *s4m1,           /* ditto 4 */
                *s1m2,   /* source 1 spec2 array for 'x in Y' misfires */
                *s2m2,           /* ditto source 2 */
                *s3m2,           /* ditto 3 */
                *s4m2;           /* ditto 4 */
extern struct ST_MAN *man;       /* manifold pressure stats */
extern struct ST_CLK *t_clk;     /* timing stats structure */
extern struct ST_GMSK *gmsk;     /* gun mask stats */
extern struct ST_SUMM *sum;      /* summary structure pointer */
extern struct HDR_DEF *hd;       /* header  list pointer */
extern struct PRO_DEF *pd;       /* project list pointer */
extern struct OPT_DEF  *opt;     /* pointer to options list */
extern struct ANAL_DEF *andef;   /* pointer to set/analysis list */
extern struct SPEC_DEF *spc;     /* pointer to specifications list */
extern struct GRAF_DEF *graf;    /* pointer to graphics list */
extern struct CAB_DEF *stcab;    /* pointer to start of cable list */
extern struct WING_DEF *stwi;    /* pointer to start of cable list */
extern struct SRCE_DEF *stsrce;	/* pointer to start of source list */
extern struct GUNDP_DEF *stgundp;   /* pointer to start of gundep list */
extern struct FIR_SEQ *fire;     /* first struct for loop fire sequence */
extern struct FIR_SEQ *last;     /* ditto last */
extern struct EXT_DEF *ext;		/* various standard variables */
extern struct ST_WAT *wat;       /* water depth analysis */
extern struct ST_SPTIM *t_int;   /* SP timing analysis */
extern char *sp_err;             /* SP integer graphics array */
extern char *fn_err;             /* FN integer graphics array */
extern char *tm_err;             /* SP timing graphics array */
extern float *c_graf1_min;       /* cable depth error arrays pointers */
extern float *c_graf1_max;
extern float *c_graf2_min;
extern float *c_graf2_max;
extern float *c_graf3_min;
extern float *c_graf3_max;
extern float *c_graf4_min;
extern float *c_graf4_max;
extern float *srce_graf;          /* source timing error array pointer */
extern char *gmsk_graf;           /* gun mask array pointer */
extern char *seq_graf;            /* firing sequence array pointer */
extern struct FIRE_BUF *fire_buf; /* initial fire sequence text */
extern unsigned *man_graf;        /* air pressure error array */
extern unsigned *vol_graf;        /* source volume error array */
extern float *tim_graf;           /* system timing error array */
extern float *gundep_graf_min;    /* gun depth min error array */
extern float *gundep_graf_max;    /* gun depth max error array */
extern float *gs_graf;				 /* ground speed error array */
extern float *gs_graf_temp;
extern float *ws_graf;				 /* water speed error array */
extern float *ws_graf_temp;
extern float *sy_graf;				 /* system heading error array */
extern float *sy_graf_temp;
extern float *gy_graf;				 /* gyro heading error array */
extern float *gy_graf_temp;
extern long *shot;					 /* shotpoint number array */
extern long *shot_temp;


/*************************************************************
*   main initialisation function
*   allocate memory & initialise various items
**************************************************************/
init_stat()
{
   struct CAB_DEF *cabcom = NULL;    /* for initialising comms & depths */
   struct WING_DEF *wing = NULL;     /* ditto wing angles */
   struct SRCE_DEF *srcini = NULL;   /* ditto guns */
   struct GUNDP_DEF *gundp = NULL;   /* ditto gun depths */
   struct FIR_SEQ *info;             /* pointer for loop fire sequence */
   register int i=0, p;
      /* firing sequence - this is first allocation because it is a
         variable nuumber (seq_len) & therefore should be the last
         to be freed by free_stat_init() */
   if((andef->seq_anl)&&(hd->gm)) {
         /* make fire seq rotating struct */
      for(i=0;i<(int)pd->seq_len; i++) {
         info = (struct FIR_SEQ *) malloc(sizeof(struct FIR_SEQ));
         if(!info) {
            StatsErr("35039");   /* called it '*fire' */
            return(1);
         }
         if(!i) fire = info;     /* initialise it from project def */
         p = (int) spc->f_seq[i] - 48;
         info->seq_ok = 0;
         info->all_ok = 0;
         info->act_srce = 0;
         info->srce_ok = 0;
         if(p <= (int)pd->loc_srce) {
            if(p==1) {
               strcpy(info->nom_sname, pd->loc1);
               info->nom_srce = p;
            }
            if(p==2) {
               strcpy(info->nom_sname, pd->loc2);
               info->nom_srce = p;
            }
            if(p==3) {
               strcpy(info->nom_sname, pd->loc3);
               info->nom_srce = p;
            }
            if(p==4) {
               strcpy(info->nom_sname, pd->loc4);
               info->nom_srce = p;
            }
         } else {
            p = p - pd->loc_srce;
            if(p==1) {
               strcpy(info->nom_sname, pd->rem1);
               info->nom_srce = p + pd->loc_srce;
            }
            if(p==2) {
               strcpy(info->nom_sname, pd->rem2);
               info->nom_srce = p + pd->loc_srce;
            }
            if(p==3) {
               strcpy(info->nom_sname, pd->rem3);
               info->nom_srce = p + pd->loc_srce;
            }
            if(p==4) {
               strcpy(info->nom_sname, pd->rem4);
               info->nom_srce = p + pd->loc_srce;
            }
         }
         if (!last) last = info;      /* first item on list */
         else last->next = info;
         info->next = NULL;
         last = info;
      }
      info->next = NULL;
         /* initial firing seq text */
      if (!(fire_buf = (struct FIRE_BUF *)malloc(sizeof(struct FIRE_BUF) *
                             (int)(pd->seq_len + 1) ))) {
            StatsErr("35031");
				return(1);
      }
      if((seq_graf = (char *)calloc( (int) sum->act_sp,
                                sizeof(char) )) == NULL ) {
         StatsErr("35032"); /* sequence graphics error array */
         return(1);
      }
	}
			/* array for shotpoint numbers */
	if( hd->sp && (graf->spd_anl || graf->hed_anl) ) {
		if( (shot = (long *)calloc( (int) sum->act_sp,
									sizeof( long ) )) == NULL ) {
			StatsErr("35041");
			return(1);
		}
	}
         /* SP numbering */
	if (andef->sp_anl && hd->sp) {
      if (!(sps = (struct ST_SPFN *)malloc(sizeof(struct ST_SPFN)))) {
      StatsErr("35001");
         return(1);
      }
		sps->ju_mp = 0;
      sps->no_ch = 0;
      if(graf->sp_anl) {
         if( (sp_err = (char *)calloc( (int) sum->act_sp,
                               sizeof( char ) )) == NULL ) {
            StatsErr("35019");
            return(1);
         }
      }
   }
         /* FN numbering */
   if (andef->fn_anl && hd->fn) {
      if (!(fns = (struct ST_SPFN *)malloc(sizeof(struct ST_SPFN)))) {
         StatsErr("35002");
         return(1);
		}
		fns->ju_mp = 0;
      fns->no_ch = 0;
      if(graf->fn_anl) {
         if( (fn_err = (char *)calloc( (int) sum->act_sp,
                               sizeof( char ) )) == NULL ) {
            StatsErr("35020");
            return(1);
         }
      }
   }
         /* cable depth controller RU comms stats */
   if (andef->dep_com && pd->loc_strm && ext->numberru && hd->ca) {
		if (!(dco = (struct ST_DCOM *)malloc(sizeof(struct ST_DCOM)))) {
         StatsErr("35003");
         return(1);
		}
		dco->flag_err = 0;
		dco->tot_err = 0;
   }
         /* cable depths & comms stats */
   if ( (andef->dep_anl1 || andef->dep_anl2) && hd->ca
                      && ext->numberru && pd->loc_strm) {
         /* cable 1 */
		if (!(dp1 = (struct ST_CDEP *)malloc(sizeof(struct ST_CDEP)))) {
         StatsErr("35004");
			return(1);
		}
		dp1->max_dep = 0.0F;/* max depth encountered for that cable */
		dp1->min_dep = 1000.0F;   /* min depth ditto */
		dp1->gr_sp1 = 0;       /* no of SPs with  depth > spec1 */
		dp1->sm_sp1 = 0;       /* ditto < spec1 */
		dp1->gr_sp2 = 0;       /* no of SPs with  depth > spec2 */
		dp1->sm_sp2 = 0;       /* ditto < spec2 */
		dp1->flag_err = 0;
      dp1->tot_err = 0;
      if((graf->dep_anl1)&&(andef->dep_anl1)) {
         if( (c_graf1_min = (float *)calloc( (int) sum->act_sp,
                                     sizeof( float ) )) == NULL ) {
            StatsErr("35021");
            return(1);
         }
         if( (c_graf1_max = (float *)calloc( (int) sum->act_sp,
                                     sizeof( float ) )) == NULL ) {
            StatsErr("35022");
            return(1);
         }
      }
         /* cable 2 */
      if (pd->loc_strm > 1) {
         if (!(dp2 = (struct ST_CDEP *)malloc(sizeof(struct ST_CDEP)))) {
            StatsErr("35005");
            return(1);
         }
         dp2->max_dep = 0.0F;/* max depth encountered for that cable */
         dp2->min_dep = 1000.0F;   /* min depth ditto */
         dp2->gr_sp1 = 0;       /* no of SPs with  depth > spec1 */
         dp2->sm_sp1 = 0;       /* ditto < spec1 */
         dp2->gr_sp2 = 0;       /* no of SPs with  depth > spec2 */
         dp2->sm_sp2 = 0;       /* ditto < spec2 */
         dp2->flag_err = 0;
         dp2->tot_err = 0;
         if((graf->dep_anl1)&&(andef->dep_anl1)) {
            if( (c_graf2_min = (float *)calloc( (int) sum->act_sp,
                                        sizeof( float ) )) == NULL ) {
               StatsErr("35023");
               return(1);
            }
            if( (c_graf2_max = (float *)calloc( (int) sum->act_sp,
                                        sizeof( float ) )) == NULL ) {
               StatsErr("35024");
               return(1);
            }
         }
      }
         /* cable 3 */
      if (pd->loc_strm > 2) {
         if (!(dp3 = (struct ST_CDEP *)malloc(sizeof(struct ST_CDEP)))) {
            StatsErr("35006");
            return(1);
         }
         dp3->max_dep = 0.0F;/* max depth encountered for that cable */
         dp3->min_dep = 1000.0F;   /* min depth ditto */
         dp3->gr_sp1 = 0;       /* no of SPs with  depth > spec1 */
         dp3->sm_sp1 = 0;       /* ditto < spec1 */
         dp3->gr_sp2 = 0;       /* no of SPs with  depth > spec2 */
         dp3->sm_sp2 = 0;       /* ditto < spec2 */
         dp3->flag_err = 0;
         dp3->tot_err = 0;
         if((graf->dep_anl1)&&(andef->dep_anl1)) {
            if( (c_graf3_min = (float *)calloc( (int) sum->act_sp,
                                        sizeof( float ) )) == NULL ) {
               StatsErr("35025");
               return(1);
            }
            if( (c_graf3_max = (float *)calloc( (int) sum->act_sp,
                                        sizeof( float ) )) == NULL ) {
               StatsErr("35026");
               return(1);
            }
         }
      }
         /* cable 4 */
      if (pd->loc_strm > 3) {
         if (!(dp4 = (struct ST_CDEP *)malloc(sizeof(struct ST_CDEP)))) {
            StatsErr("35007");
            return(1);
         }
         dp4->max_dep = 0.0F;/* max depth encountered for that cable */
         dp4->min_dep = 1000.0F;   /* min depth ditto */
         dp4->gr_sp1 = 0;       /* no of SPs with  depth > spec1 */
         dp4->sm_sp1 = 0;       /* ditto < spec1 */
         dp4->gr_sp2 = 0;       /* no of SPs with  depth > spec2 */
         dp4->sm_sp2 = 0;       /* ditto < spec2 */
         dp4->flag_err = 0;
         dp4->tot_err = 0;
         if((graf->dep_anl1)&&(andef->dep_anl1)) {
            if( (c_graf4_min = (float *)calloc( (int) sum->act_sp,
                                        sizeof( float ) )) == NULL ) {
               StatsErr("35027");
               return(1);
            }
            if( (c_graf4_max = (float *)calloc( (int) sum->act_sp,
                                        sizeof( float ) )) == NULL ) {
               StatsErr("35028");
               return(1);
            }
         }
      }
         /* individual cable RUs */
		cabcom = stcab;
      for (i=0; i<ext->numberru; i++) {
		  cabcom->tot_err = 0;
		  cabcom->tot_sp1 = 0;
		  cabcom->tot_sp2 = 0;
		  cabcom->min_dep = 1000.0F;
		  cabcom->max_dep = 0.0F;
		  cabcom->per_c = 0.0F;
		  cabcom = cabcom->next;
		}
   }
         /* cable depth controller wing angles */
   if(andef->wi_anl) {
      wing = stwi;
      for (i=0; i<ext->numberwing; i++) {
        wing->spec = 0;
        wing->on = 0;
        wing->last = 0.0F;
        wing->min = 20.0F;
        wing->max = -20.0F;
        wing->mean = 0.0F;
        wing->change = 0.0F;
        wing = wing->next;
		}
   }
         /* source timing, volume, & depth */
   if( ((andef->gun_anl1)||(andef->gun_anl2) ||
        (andef->vol_anl1)||(andef->vol_anl2) ||
        (andef->man_anl1)||(andef->man_anl2))
                  && pd->loc_srce && ext->numbergun) {
            /* source integer timing specs */
      if (!(gun = (struct SRCE_ERR *)malloc(sizeof(struct SRCE_ERR)))) {
         StatsErr("35008");
			return(1);
      }
         /* initialise for GECO 1 & 2 & WESTERN LITTON 3 & 4 controllers */
      if((hd->gu==9)||(hd->gu==8)||(hd->gu==63)||(hd->gu==64)) {
         gun->sp1 = (int) (4.0F*spc->gun1);
			gun->sp2 = (int) (4.0F*spc->gun2);
      }
         /* initialise for GECO GUNDA 1 & 2, PRAKLA VZAD & DIGICON TRACE0 */
      if( (hd->gu==10)||(hd->gu==11)||(hd->gu==91)||(hd->gu==71) ) {
         gun->sp1 = (int) (10.0F*spc->gun1);
			gun->sp2 = (int) (10.0F*spc->gun2);
      }
         /* crossfire stats */
      if (!(xsr = (struct ST_XSRC *)malloc(sizeof(struct ST_XSRC)))) {
         StatsErr("35009");
			return(1);
		} else {
			xsr->tot_sp = 0;
			xsr->spec1 = 0;
			xsr->spec2 = 0;
			xsr->no_fire = 0;
			xsr->x_fire = 0;
      }
         /* source 1 */
      if (!(sr1 = (struct ST_SRCE *)malloc(sizeof(struct ST_SRCE)))) {
            StatsErr("35010");
				return(1);
			}
      sr1->en_num = 0;    /* number of SPs source fired */
      sr1->sp1 = 0;       /* number of SPs outside timespec 1 */
      sr1->sp2 = 0;       /* number of SPs outside timespec 2 */
      sr1->mf = 0;        /* number of SPs with a NOFIRE/AUTOFIRE */
      sr1->tot_mf = 0;    /* total num ber of NOFIRES/AUTOFIRES */
      sr1->vol1 = 0;      /* number of SPs below volspec 1 */
      sr1->vol2 = 0;      /* ditto volspec 2 */
      sr1->sp_vol = 0;    /* volume 1st/previous/last SP */
      sr1->vol1_fl = 0;   /* whether source in vol spec 1 last SP */
      sr1->vol2_fl = 0;   /* whether source in vol spec 2 last SP */
      sr1->sol_vol = 0;   /* volume first enabled SP */
      sr1->p_spec1 = 0.0F;   /* percent misfires spec 1 */
      sr1->p_spec2 = 0.0F;   /* percent misfires spec 2 */
      sr1->min_dep = 1000.0F;   /* minimum gun depth of source */
      sr1->max_dep = 0.0F;     /* maximum ditto */
      sr1->gr_sp = 0L;       /* no of SPs with a gun depth > spec */
      sr1->sm_sp = 0L;       /* ditto < spec */
         /* source 2 */
      if (pd->loc_srce > 1) {
			if (!(sr2 = (struct ST_SRCE *)malloc(sizeof(struct ST_SRCE)))) {
            StatsErr("35011");
				return(1);
			}
			sr2->en_num = 0;    /* number of SPs source fired */
			sr2->sp1 = 0;       /* number of SPs outside timespec 1 */
			sr2->sp2 = 0;       /* number of SPs outside timespec 2 */
			sr2->mf = 0;        /* number of SPs with a NOFIRE/AUTOFIRE */
			sr2->tot_mf = 0;    /* total num ber of NOFIRES/AUTOFIRES */
			sr2->vol1 = 0;      /* number of SPs below volspec 1 */
			sr2->vol2 = 0;      /* ditto volspec 2 */
			sr2->sp_vol = 0;    /* 1st etc */
			sr2->vol1_fl = 0;   /* whether source in vol spec 1 last SP */
			sr2->vol2_fl = 0;   /* whether source in vol spec 2 last SP */
			sr2->sol_vol = 0;   /* volume first enabled SP */
			sr2->p_spec1 = 0.0F;   /* percent misfires spec 1 */
			sr2->p_spec2 = 0.0F;   /* percent misfires spec 2 */
			sr2->min_dep = 1000.0F;   /* minimum gun depth of source */
			sr2->max_dep = 0.0F;     /* maximum ditto */
			sr2->gr_sp = 0L;       /* no of SPs with a gun depth > spec */
			sr2->sm_sp = 0L;       /* ditto < spec */
      }
         /* source 3 */
      if (pd->loc_srce > 2) {
			if (!(sr3 = (struct ST_SRCE *)malloc(sizeof(struct ST_SRCE)))) {
            StatsErr("35012");
				return(1);
			}
			sr3->en_num = 0;    /* number of SPs source fired */
			sr3->sp1 = 0;       /* number of SPs outside timespec 1 */
			sr3->sp2 = 0;       /* number of SPs outside timespec 2 */
			sr3->mf = 0;        /* number of SPs with a NOFIRE/AUTOFIRE */
			sr3->tot_mf = 0;    /* total num ber of NOFIRES/AUTOFIRES */
			sr3->vol1 = 0;      /* number of SPs below volspec 1 */
			sr3->vol2 = 0;      /* ditto volspec 2 */
			sr3->sp_vol = 0;    /* 1st etc */
			sr3->vol1_fl = 0;   /* whether source in vol spec 1 last SP */
			sr3->vol2_fl = 0;   /* whether source in vol spec 2 last SP */
			sr3->sol_vol = 0;   /* volume first enabled SP */
			sr3->p_spec1 = 0.0F;   /* percent misfires spec 1 */
			sr3->p_spec2 = 0.0F;   /* percent misfires spec 2 */
			sr3->min_dep = 1000.0F;   /* minimum gun depth of source */
			sr3->max_dep = 0.0F;     /* maximum ditto */
			sr3->gr_sp = 0L;       /* no of SPs with a gun depth > spec */
			sr3->sm_sp = 0L;       /* ditto < spec */
      }
         /* source 4 */
      if (pd->loc_srce > 3) {
			if (!(sr4 = (struct ST_SRCE *)malloc(sizeof(struct ST_SRCE)))) {
            StatsErr("35013");
				return(1);
			}
			sr4->en_num = 0;    /* number of SPs source fired */
			sr4->sp1 = 0;       /* number of SPs outside timespec 1 */
			sr4->sp2 = 0;       /* number of SPs outside timespec 2 */
			sr4->mf = 0;        /* number of SPs with a NOFIRE/AUTOFIRE */
			sr4->tot_mf = 0;    /* total num ber of NOFIRES/AUTOFIRES */
			sr4->vol1 = 0;      /* number of SPs below volspec 1 */
			sr4->vol2 = 0;      /* ditto volspec 2 */
			sr4->sp_vol = 0;    /* 1st etc */
			sr4->vol1_fl = 0;   /* whether source in vol spec 1 last SP */
			sr4->vol2_fl = 0;   /* whether source in vol spec 2 last SP */
			sr4->sol_vol = 0;   /* volume first enabled SP */
			sr4->p_spec1 = 0.0F;   /* percent misfires spec 1 */
			sr4->p_spec2 = 0.0F;   /* percent misfires spec 2 */
			sr4->min_dep = 1000.0F;   /* minimum gun depth of source */
			sr4->max_dep = 0.0F;     /* maximum ditto */
			sr4->gr_sp = 0L;       /* no of SPs with a gun depth > spec */
			sr4->sm_sp = 0L;       /* ditto < spec */
		}
         /* initialise individual gun stats */
		srcini = stsrce;
		for(i=0;i<ext->numbergun;i++) {
			srcini->max_err = 0;
			srcini->en_num=0;
			srcini->tot_sp1=0;
			srcini->tot_sp2=0;
			srcini->mf_num=0;
			srcini = srcini->next;
      }
         /* consecutive & series misfies */
      if( (andef->gun_anl1 || andef->gun_anl2) &&
          (andef->mf1_anl || andef->mf3_anl) ) {
         if (!(msf = (struct ST_MISF *)malloc(sizeof(struct ST_MISF)))) {
            StatsErr("350  ");
				return(1);
			}
         msf->s1c1 = 0;     /* source 1 spec1 running misfire last x SP */
         msf->s2c1 = 0;     /* ditto source 2 */
         msf->s3c1 = 0;     /* ditto 3 */
         msf->s4c1 = 0;     /* ditto 4 */
         msf->s1ct1 = 0;    /* source 1 spec1 no SP last x exceeded */
         msf->s2ct1 = 0;    /* ditto source 2 */
         msf->s3ct1 = 0;    /* ditto 3 */
         msf->s4ct1 = 0;    /* ditto 4 */
         msf->s1c2 = 0;     /* source 1 spec2 running misfire last x SP */
         msf->s2c2 = 0;     /* ditto source 2 */
         msf->s3c2 = 0;     /* ditto 3 */
         msf->s4c2 = 0;     /* ditto 4 */
         msf->s1ct2 = 0;    /* source 1 spec2 no SP last x exceeded */
         msf->s2ct2 = 0;    /* ditto source 2 */
         msf->s3ct2 = 0;    /* ditto 3 */
         msf->s4ct2 = 0;    /* ditto 4 */
         msf->s1mt1 = 0;    /* source 1 spec1 no SP 'x in Y' exceeded */
         msf->s2mt1 = 0;    /* source 2 ditto */
         msf->s3mt1 = 0;    /* source 3 */
         msf->s4mt1 = 0;    /* source 4 */
         msf->s1mt2 = 0;    /* source 1 spec2 no SP 'x in Y' exceeded */
         msf->s2mt2 = 0;    /* source 2 ditto */
         msf->s3mt2 = 0;    /* source 3 */
         msf->s4mt2 = 0;    /* source 4 */
         msf->s1pt1 = 0;    /* source 1 spec1 misfire % exceeded */
         msf->s2pt1 = 0;    /* source 2 ditto */
         msf->s3pt1 = 0;    /* source 3 */
         msf->s4pt1 = 0;    /* source 4 */
         msf->s1pt2 = 0;    /* source 1 spec2 misfire % exceeded */
         msf->s2pt2 = 0;    /* source 2 ditto */
         msf->s3pt2 = 0;    /* source 3 */
         msf->s4pt2 = 0;    /* source 4 */
         if( (s1m1 = (unsigned *)calloc( spc->mf3_ra,
                                 sizeof( unsigned ) )) == NULL ) {
            StatsErr("35040"); /* source1 spec1 misfire series array */
            return(1);
         }
         if( (s2m1 = (unsigned *)calloc( spc->mf3_ra,
                                 sizeof( unsigned ) )) == NULL ) {
            StatsErr("35040"); /* source2 spec1 misfire series array */
            return(1);
         }
         if( (s3m1 = (unsigned *)calloc( spc->mf3_ra,
                                 sizeof( unsigned ) )) == NULL ) {
            StatsErr("35040"); /* source3 spec1 misfire series array */
            return(1);
         }
         if( (s4m1 = (unsigned *)calloc( spc->mf3_ra,
                                 sizeof( unsigned ) )) == NULL ) {
            StatsErr("35040"); /* source4 spec1 misfire series array */
            return(1);
         }
         if( (s1m2 = (unsigned *)calloc( spc->mf3_ra,
                                 sizeof( unsigned ) )) == NULL ) {
            StatsErr("35040"); /* source1 spec2 misfire series array */
            return(1);
         }
         if( (s2m2 = (unsigned *)calloc( spc->mf3_ra,
                                 sizeof( unsigned ) )) == NULL ) {
            StatsErr("35040"); /* source2 spec2 misfire series array */
            return(1);
         }
         if( (s3m2 = (unsigned *)calloc( spc->mf3_ra,
                                 sizeof( unsigned ) )) == NULL ) {
            StatsErr("35040"); /* source3 spec2 misfire series array */
            return(1);
         }
         if( (s4m2 = (unsigned *)calloc( spc->mf3_ra,
                                 sizeof( unsigned ) )) == NULL ) {
            StatsErr("35040"); /* source4 spec2 misfire series array */
            return(1);
         }
      }
   }
         /* source volume graphics */
   if(hd->gu && ext->numbergun && andef->vol_anl1 && graf->vol_anl1) {
      if( (vol_graf = (unsigned *)calloc( (int) sum->act_sp,
                                  sizeof( unsigned ) )) == NULL ) {
         StatsErr("35038"); /* srce volume graphics error array */
         return(1);
         }
   }
         /* source timing graphics */
   if(hd->gu && ext->numbergun && andef->gun_anl1 && graf->gun_anl1) {
      if( (srce_graf = (float *)calloc( (int) sum->act_sp,
                                sizeof( float ) )) == NULL ) {
         StatsErr("35029"); /* srce timing graphics error array */
         return(1);
         }
   }
         /* air pressure stats */
   if ( (andef->man_anl1 || andef->man_anl2) && hd->ma) {
		if (!(man = (struct ST_MAN *)malloc(sizeof(struct ST_MAN)))) {
         StatsErr("35014");
			return(1);
      }
      if(andef->man_anl1 && graf->man_anl1) {   /* air pressure graphics */
         if( (man_graf = (unsigned *)calloc( (int) sum->act_sp,
                                        sizeof( unsigned ) )) == NULL ) {
            StatsErr("35034"); /* air pressure graphics error array */
            return(1);
         }
      }
		man->man1 = 0;             /* number of SPs below manifold spec 1 */
		man->man2 = 0;             /* ditto manifold spec 2 */
      man->man1_fl = 0;          /* init last shot low press spec1 flag */
      man->man2_fl = 0;          /* init last shot low press spec2 flag */
		man->sol = 0;              /* pressure at SOL */
		man->eol = 0;              /* pressure at EOL */
		man->max = 0;              /* max pressure */
      man->min = 9999;           /* min pressure */
		man->mean = 0;             /* mean pressure */
		man->total = 0L;           /* running total for mean */
		man->p_spec1 = 0.0F;       /* percent below pressure spec 1 */
		man->p_spec2 = 0.0F;       /* ditto spec 2 */
   }
         /* system or intervessel timing stats */
   if (andef->clk_anl && hd->clk) {
		if (!(t_clk = (struct ST_CLK *)malloc(sizeof(struct ST_CLK)))) {
            StatsErr("35015");
				return(1);
		}
		t_clk->corrupt=0;          /* number of corrupt times */
		t_clk->tot_anl=0;          /* total number good & analysed */
		t_clk->sm_spec=0;          /* number less than min time */
		t_clk->gr_spec=0;          /* number greater than max time */
		t_clk->min = 999.9F;              /* minimum time */
		t_clk->max = 0.0F;              /* maximum time */
		t_clk->total = 0.0F;            /* add up times for mean time */
		t_clk->mean = 0.0F;             /* mean time of non-corrupt times */
		t_clk->per_corr = 0.0F;         /* percent corrupt */
      t_clk->per_spec = 0.0F;         /* percent non-corrupt out of spec */
      if (graf->clk_anl) {
         if( (tim_graf = (float *)calloc( (int) sum->act_sp,
                                        sizeof( float ) )) == NULL ) {
            StatsErr("35035"); /* system timing graphics error array */
            return(1);
         }
      }
   }
         /* gun mask analysis */
	if(andef->gmsk_anl) {
		if (!(gmsk = (struct ST_GMSK *)malloc(sizeof(struct ST_GMSK)))) {
            StatsErr("35016");
				return(1);
      }
		gmsk->loc1=0;       /* number of SPs local source 1 fired but
									  was less than minimum bits set */
		gmsk->ovr_loc1=0;   /* number of SPs local source 1 overfired */
		gmsk->loc2=0;       /* number of SPs local source 2 fired but
									  was less than minimum bits set */
		gmsk->ovr_loc2=0;   /* number of SPs local source 2 overfired */
		gmsk->rem1=0;       /* number of SPs remote source 1 fired but
									  was less than minimum bits set */
		gmsk->ovr_rem1=0;   /* number of SPs remote source 1 overfired */
		gmsk->rem2=0;       /* number of SPs remote source 2 fired but
									  was less than minimum bits set */
		gmsk->ovr_rem2=0;   /* number of SPs remote source 2 overfired */
		gmsk->tot_ovr=0;    /* total number of SPs with overfire */
		gmsk->tot_set=0;    /* total number of SPs under minimum bits */
      gmsk->tot_none=0;    /* total number SPs no source fired */
      if(graf->gmsk_anl) {
         if((gmsk_graf = (char *)calloc( (int) sum->act_sp, sizeof(char) )) == NULL ) {
            StatsErr("35030"); /* gun mask graphics error array */
            return(1);
         }
      }
   }
			/* initialise individual gun depths */
	if((andef->gd_anl)&&(hd->gd)) {
		gundp = stgundp;
      for (i=0; i<ext->numberdp; i++) {
			gundp->tot_sp = 0;
			gundp->min_dep = 1000.0F;
			gundp->max_dep = 0.0F;
			gundp = gundp->next;
      }
      if(graf->gd_anl) {  /* allocate gundepth error arrays */
         if((gundep_graf_min = (float *)calloc( (int) sum->act_sp,
                                        sizeof(float) )) == NULL ) {
            StatsErr("35036"); /* gun depth min graphics array */
            return(1);
         }
         if((gundep_graf_max = (float *)calloc( (int) sum->act_sp,
                                        sizeof(float) )) == NULL ) {
            StatsErr("35037"); /* gun depth max graphics array */
            return(1);
         }
      }
	}
			/* initialise water depth stats */
	if((andef->wdp_anl)&&(hd->wa)) {	  /* do water depth analysis */
		if (!(wat = (struct ST_WAT *)malloc(sizeof(struct ST_WAT)))) {
            StatsErr("35017");
				return(1);
		}
      wat->err = 0;             /* total errors */
      wat->corr = 0;            /* total corrupt (0 or less) */
      wat->min = 1000.0F;       /* min */
      wat->max = 0.0F;          /* max */
   }
         /* initialise SP timing stats */
   if((andef->tim_anl)&&(hd->tm)) {   /* do SP timing analysis */
      if (!(t_int = (struct ST_SPTIM *)malloc(sizeof(struct ST_SPTIM)))) {
				return(1);
      }
      t_int->tmin = 1000.0F;    /* min */
      t_int->tmax = 0.0F;       /* max */
      t_int->t_prev = 0L;       /* previous value */
      t_int->total = 0.0F;      /* total times (for mean) */
      t_int->mean = 0.0F;       /* mean time */
      t_int->corrupt = 0;       /* number of corrupt times */
      t_int->tot_anl = 0;       /* total number good & analysed */
      t_int->sm_spec = 0;       /* number less than min time */
      t_int->gr_spec = 0;       /* number greater than max time */
         /* time interval graphic error array */
      if(graf->tim_anl) {
         if( (tm_err = (char *)calloc( (int) sum->act_sp,
                               sizeof( char ) )) == NULL ) {
            StatsErr("35033");  /* SP timing error array */
            return(1);
         }
      }
	}
			/* speed along track - GECO & DIGICON TRACE0 */
	if( (!hd->contr || hd->contr == 5) && andef->gsp_anl &&
											  hd->gs && graf->spd_anl) {
		if((gs_graf = (float *)calloc( (int) sum->act_sp,
                                        sizeof(float) )) == NULL ) {
			StatsErr("35042"); /* ground speed graphics array */
			return(1);
		}
	}
			/* water speed - GECO */
	if( !hd->contr && andef->ws_anl && hd->ws && graf->spd_anl) {
		if((ws_graf = (float *)calloc( (int) sum->act_sp,
                                        sizeof(float) )) == NULL ) {
			StatsErr("35043"); /* water speed graphics array */
			return(1);
		}
	}
			/* system heading - GECO only */
	if( !hd->contr && hd->syhd && andef->cr_anl && graf->hed_anl ) {
		if((sy_graf = (float *)calloc( (int) sum->act_sp,
                                        sizeof(float) )) == NULL ) {
			StatsErr("35045"); /* system heading graphics array */
			return(1);
		}
	}
			/* gyro heading - GECO */
	if( !hd->contr && hd->gy && andef->cr_anl && graf->hed_anl ) {
		if((gy_graf = (float *)calloc( (int) sum->act_sp,
                                        sizeof(float) )) == NULL ) {
			StatsErr("35046"); /* gyro heading graphics array */
			return(1);
		}
	}
	return(0);
}
