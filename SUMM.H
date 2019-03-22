/**************************************************************
*    SUMM.H  statistics structure definitions
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

         /* shotpoint & file numbering */
typedef struct ST_SPFN {
	long ju_mp,               /* jump */
		  no_ch;               /* freeze */
};

         /* depth controller remote unit comms */
typedef struct ST_DCOM {
	long flag_err,           /* at least one comm error in SP */
		  tot_err;            /* total bad readings */
};

         /* singular cable depths */
typedef struct ST_CDEP {
	float max_dep,            /* max depth encountered for that cable */
			min_dep;            /* min depth ditto */
	long gr_sp1,              /* no of SPs with  depth > spec1 */
		  sm_sp1,              /* ditto < spec1 */
		  gr_sp2,              /* no of SPs with  depth > spec2 */
		  sm_sp2,              /* ditto < spec2 */
		  flag_err,            /* at least 1 comm error in SP */
		  tot_err;             /* total bad readings that cable */
   float max_wing,
         min_wing;     
};

         /* integer gun timings */
typedef struct SRCE_ERR {
	unsigned sp1,              /* will hold integer gun spec1 */
				sp2;              /* ditto gun spec 2 */
};

         /* singular source timings, misfires, volumes & depths */
typedef struct ST_SRCE {
	unsigned en_num,           /* number of SPs source fired */
				sp1,              /* number of SPs outside timespec 1 */
				sp2,              /* number of SPs outside timespec 2 */
				mf,               /* number of SPs with a NOFIRE/AUTOFIRE */
				tot_mf,           /* total num ber of NOFIRES/AUTOFIRES */
				vol1,             /* number of SPs below volspec 1 */
				vol2,             /* ditto volspec 2 */
				sp_vol,           /* volume first/previous/last SP */
				vol1_fl,          /* whether source below vol spec 1 last SP */
				vol2_fl,          /* whether source below vol spec 2 last SP */
				sol_vol;          /* volume first enabled SP */
	float    p_spec1,          /* percent misfires spec 1 */
				p_spec2,          /* ditto spec 2 */
            min_dep,          /* minimum gun depth of source */
            max_dep;          /* maximum ditto */
	long     gr_sp,            /* no of SPs with a gun depth > spec */
				sm_sp;            /* ditto < spec */
};

         /* air pressure */
typedef struct ST_MAN {
	unsigned man1,             /* number of SPs below manifold spec 1 */
				man2,             /* ditto manifold spec 2 */
				sol,              /* pressure at SOL */
				eol,              /* pressure at EOL */
				max,              /* max pressure */
				min,              /* min pressure */
				mean,             /* mean pressure */
				man1_fl,          /* below spec1 last shot? */
				man2_fl;          /* below spec2 last shot? */
	unsigned long total;       /* running total for mean */
	float    p_spec1,          /* percent below pressure spec 1 */
				p_spec2;          /* ditto spec 2 */
};

         /* overall source controller totals & crossfiring */
typedef struct ST_XSRC {
	unsigned tot_sp,          /* number of SPs any source fired */
				spec1,           /* number of SPs with a Spec1 misfire */
				spec2,           /* number of SPs with a Spec2 misfire */
				no_fire,         /* number of SPs no source fired */
				x_fire;          /* number of SPs two or more sources
											fired on top of each other */
	float    p_spec1,         /* percent misfires spec 1 */
				p_spec2;         /* ditto spec 2 */
};

         /* gun masks */
typedef struct ST_GMSK {  /* GECO GUNMASK ERROR CHECKING */
	unsigned loc1,            /* number of SPs local source 1 fired but
										  was less than minimum bits set */
				ovr_loc1,        /* number of SPs local source 1 overfired */
				loc2,            /* number of SPs local source 2 fired but
										  was less than minimum bits set */
				ovr_loc2,        /* number of SPs local source 2 overfired */
				rem1,            /* number of SPs remote source 1 fired but
										  was less than minimum bits set */
				ovr_rem1,        /* number of SPs remote source 1 overfired */
				rem2,            /* number of SPs remote source 2 fired but
										  was less than minimum bits set */
				ovr_rem2,        /* number of SPs remote source 2 overfired */
				tot_ovr,         /* total number of SPs with overfire */
				tot_set,         /* total number of SPs under minimum bits */
				tot_none,        /* total number SPs no source fired */
            loc3,
            ovr_loc3,
            loc4,
            ovr_loc4,
            rem3,
            ovr_rem3,
            rem4,
            ovr_rem4;
};

         /* system or intervessel timing */
typedef struct ST_CLK {
	unsigned corrupt,          /* number of corrupt times */
				tot_anl,          /* total number good & analysed */
				sm_spec,          /* number less than min time */
				gr_spec;          /* number greater than max time */
	float    min,              /* minimum time */
				max,              /* maximum time */
				total,            /* add up times for mean time */
				mean,             /* mean time of non-corrupt times */
				per_corr,         /* percent corrupt */
				per_spec;         /* percent non-corrupt out of spec */
};

         /* water depth */
typedef struct ST_WAT {
   float    min,
            max;
   unsigned err,
            corr;
};

         /* shotpoint time interval */
typedef struct ST_SPTIM {
   float    tmin,
            tmax,
				total,            /* add up times for mean time */
            mean;             /* mean time of non-corrupt times */
	unsigned corrupt,          /* number of corrupt times */
				tot_anl,          /* total number good & analysed */
				sm_spec,          /* number less than min time */
            gr_spec;          /* number greater than max time */
   unsigned long t_prev;      /* previous SP time (GTLI) */
};

         /* singular source consecutive & series misfires */
typedef struct ST_MISF {
      /* consecutive misfires spec1 */
   unsigned s1c1,     /* source 1 spec1 running misfire last x SP */
            s2c1,     /* ditto source 2 */
            s3c1,     /* ditto 3 */
            s4c1,     /* ditto 4 */
            s1ct1,    /* source 1 spec1 no SP last x exceeded */
            s2ct1,    /* ditto source 2 */
            s3ct1,    /* ditto 3 */
            s4ct1;    /* ditto 4 */
      /* consecutive misfires spec2 */
   unsigned s1c2,     /* source 1 spec2 running misfire last x SP */
            s2c2,     /* ditto source 2 */
            s3c2,     /* ditto 3 */
            s4c2,     /* ditto 4 */
            s1ct2,    /* source 1 spec2 no SP last x exceeded */
            s2ct2,    /* ditto source 2 */
            s3ct2,    /* ditto 3 */
            s4ct2;    /* ditto 4 */
      /* series misfires spec1 */
   unsigned s1mt1,    /* source 1 spec1 no SP 'x in Y' exceeded */
            s2mt1,    /* source 2 ditto */
            s3mt1,    /* source 3 */
            s4mt1;    /* source 4 */
      /* series misfires spec2 */
   unsigned s1mt2,    /* source 1 spec2 no SP 'x in Y' exceeded */
            s2mt2,    /* source 2 ditto */
            s3mt2,    /* source 3 */
            s4mt2;    /* source 4 */
      /* running misfire % spec1 */
   unsigned s1pt1,
            s2pt1,
            s3pt1,
            s4pt1;
      /* running misfire % spec2 */
   unsigned s1pt2,
            s2pt2,
            s3pt2,
            s4pt2;
};
