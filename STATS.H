/***************************************************************
*   STATS.H   structure definitions (header, source, cable etc)
****************************************************************
*
*	Version:   2.40
*	Compiler:  MSC V6.00  /AL /W4 /Os /Gs /Zp /Zl /G2 /ST:3072
*  Linker:    MSC V5.10  /NOE /SE:256 /NOD /EXEPACK
*  Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWL.LIB
*             Greenleaf Superfunctions (MSC) V1.00 GSFL.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*	(c) West Country Systems Ltd, Cyprus, 1992.
****************************************************************/

         /*  HEADER */
typedef struct HDR_DEF {
	unsigned h_len,         /* header length in bytes */
				sp,            /* shotpoint format */
				sp_ofst,       /* offset */
				fn,            /* file number format */
				fn_ofst,
				jd,            /* julian date format */
				jd_ofst,
				tm,            /* time format */
				tm_ofst,
				ln,            /* line name format */
				ln_ofst,
				ar,            /* area format */
				ar_ofst,
				cl,            /* client format */
				cl_ofst,
				vs,            /* vessel format */
				vs_ofst,
				ca,            /* cable depth controller format */
				ca_num,        /* number of cable controllers in header */
				ca_ofst,
				gu,            /* gun controller format */
				gu_num,        /* number of guns in header */
				gu_ofst,
				gm,            /* gun mask format */
				gm_ofst,
				ma,            /* manifold air pressure format */
				ma_ofst,
				gs,            /* ground speed format */
				gs_ofst,
				wa,            /* water depth format */
				wa_ofst,
				gd,            /* gun depth format */
				gd_ofst,
				gd_num,
				syhd,
				syhd_ofst,     /* system heading format */
				clk,           /* timing checks (GM2B/PULSECLK etc) */
				clk_ofst,
				contr,			/* contractor number */
            geco, west, prak, digi, cgg, hgs, ssl, /* contr fmt number */
            wi,            /* wing angle format */
            wi_ofst,
				wi_num,
				ws,				/* water speed */
				ws_ofst,
				gy,				/* gyro heading */
				gy_ofst;
};

         /* PROJECT */
typedef struct PRO_DEF {
	char pro_name[31],      /* your own project name */
		  cli_name[31],      /* your own client name */
		  con_name[31];      /* your own contract name */
	char qc_com1[31],       /* your own comment to print */
		  qc_com2[31],       /* ditto 2 */
		  qc_com3[31],       /* ditto 3 */
		  qc_com4[31];       /* ditto 4 */
	unsigned vsl_numb,      /* number of vessels */
				loc_strm,      /* number of local cables */
				rem_strm,      /* number of remote cables */
				loc_srce,      /* number of local sources */
				rem_srce,      /* number of remote sources */
				seq_len,       /* firing sequence length */
				loc_dp,        /* 0=FEET 1=METRES */
				pr_un,         /* 2=PSI  3=BARS */
				vol_un,        /* 4=CU/IN 5=LITRES */
				sp_un;         /* 6=KNOTS 7=KMS/HR */
   float    sp_dst;        /* SP interval */
                /* sources*/
   char     loc1[12],      /* local source 1 name */
            loc2[12],      /* ditto local 2 */
            loc3[12],      /* ditto local 3 */
            loc4[12],      /* ditto local 4 */
            rem1[12],      /* ditto remote 1 */
            rem2[12],      /* ditto remote 2 */
            rem3[12],      /* ditto remote 3 */
            rem4[12],      /* ditto remote 4 */
                 /* cables */
            locab1[12],    /* local & remote cable names */
            locab2[12],
            locab3[12],
            locab4[12],
            remcab1[12],
            remcab2[12],
            remcab3[12],
            remcab4[12],
                 /* header logger version */
            log_ver[5];    /* not used yet */
};

         /* GUN UNIT */
typedef struct SRCE_DEF {
	int gun_num;                /* primarily Litton gun numbers */
	int srce;                   /* source number - user defined */
	int gun_vol;                /* gun volume - user defined */
	unsigned gunno  :  4;       /* gunda - gun number 1 to 8 */
	unsigned arrno  :  3;       /* gunda array no 1 to 8 */
	unsigned side   :  1;       /* gunda side Port=1 Stbd=0 */
	char gun_name[11];          /* gun name - user defined */
	unsigned max_err,           /* maximum error of that gun */
				en_num,            /* number of SPs gun was enabled */
				tot_sp1,           /* number of SPs outside spec1 */
				tot_sp2,           /* ditto spec2 */
				mf_num;            /* number of NOFIRE/AUTOFIRE */
	struct SRCE_DEF *next;      /* pointer to next in list */
	struct SRCE_DEF *prior;     /* pointer to prior in list */
};

         /* CABLE DEPTH UNIT (RU) */
typedef struct CAB_DEF {
	unsigned  cab_num,         /* remote unit number */
				 cable,           /* remote unit cable */
				 tot_err,         /* number of bad readings */
				 tot_sp1,         /* number out-of-spec1 */
				 tot_sp2;         /* number out-of-spec2 */
	char cab_name[11];         /* remote unit name */
	float min_dep,             /* minimum depth */
			max_dep,             /* maximum depth */
			per_c;               /* percent bad readings */
	struct CAB_DEF *next;      /* pointer to next in list */
	struct CAB_DEF *prior;     /* pointer to prior in list */
};

         /* WING ANGLE UNIT (WU) */
typedef struct WING_DEF {
   unsigned  num,             /* unit number */
             cable,           /* cable */
             spec,            /* number out-of-spec */
             on;              /* activity */
   char      name[11];        /* unit name */
   float     min,             /* minimum angle */
             max,             /* maximum angle */
             mean,            /* mean angle (actually total readings) */
             change,          /* total change in wing angles */
             last;            /* last reading for activity comparison */
   struct WING_DEF *next;      /* pointer to next in list */
   struct WING_DEF *prior;     /* pointer to prior in list */
};

         /* GUN DEPTH UNIT (DU) */
typedef struct GUNDP_DEF {
	unsigned  gundp_num,       /* gun depth unit number */
				 srce,            /* gun depth unit source */
				 tot_sp;          /* number out-of-spec */
	char gundp_name[11];       /* remote unit name */
	float min_dep,             /* minimum depth */
			max_dep;             /* maximum depth */
	struct GUNDP_DEF *next;    /* pointer to next in list */
	struct GUNDP_DEF *prior;   /* pointer to prior in list */
};

         /* SPECIFICATIONS */
typedef struct SPEC_DEF {
   float    d1min,            /* cable depths */
            d1max,               /* two spec levels */
            d2min,
            d2max;
	unsigned dcom;          /* cable RU comms % bad allowed */
	float    gun1,          /* gun timing */
				gun2;             /* two spec levels */
	unsigned gvol1,         /* source volumes */
				gvol2,            /* two spec levels */
				ma1,           /* manifold pressure */
				ma2,              /* two spec levels */
				mf1;           /* consecutive misfires allowed */
	float    clk,           /* Clock timing nominal */
				clk_ra;           /* range allowed */
	unsigned mf3,           /* Y misfires allowed */
				mf3_ra,           /* in y shots */
				mf4,           /* total misfire % allowed */
				spint,			/* SP integer */
				fnint;			/* FN integer */
	float 	gsp;				/* max ground speed */
	unsigned gmsk;          /* minimum gun mask bits required */
   char f_seq[9];          /* firing sequence */
	float    wd_min,        /* min & max water depths - checks fatho */
				wd_max,
				gd_min,        /* min and max gun depths */
				gd_max,
            wing,          /* max (+/-) wing angle */
				t_min,			/* min & max SP time interval */
            t_max;
};

         /* ANALYSIS */
typedef struct ANAL_DEF {
      /* do analysis */
	unsigned dep_anl1  :  1;    /* analyse cable depths */
	unsigned dep_anl2  :  1;
	unsigned dep_com   :  1;    /* analyse RU comms % bad */
	unsigned gun_anl1  :  1;    /* analyse gun timing */
	unsigned gun_anl2  :  1;
	unsigned vol_anl1  :  1;    /* analyse source volumes */
	unsigned vol_anl2  :  1;
	unsigned man_anl1  :  1;    /* analyse manifold pressure */
	unsigned man_anl2  :  1;
	unsigned mf1_anl   :  1;    /* analyse consecutive misfires */
	unsigned clk_anl   :  1;    /* analyse timing clock */
	unsigned mf3_anl   :  1;    /* analyse Y in y misfires */
	unsigned mf4_anl   :  1;    /* analyse total misfire % */
	unsigned sp_anl    :  1;    /* analyse SP interval */
	unsigned fn_anl    :  1;    /* analyse FN interval */
	unsigned gsp_anl   :  1;    /* analyse ground speed */
	unsigned gmsk_anl  :  1;    /* analyse gun mask errors */
	unsigned seq_anl   :  1;    /* analyse sequence errors */
	unsigned wdp_anl   :  1;    /* analyse water depth errors */
   unsigned gd_anl    :  1;    /* analyse gun depth errors */
   unsigned tim_anl   :  1;   /* sp time interval */
   unsigned wi_anl    :  1;    /* analyse wing angles */
      /* log to screen & disk */
	unsigned ldep_anl1  :  1;   /* log cable depth errors */
	unsigned ldep_anl2  :  1;
	unsigned ldep_com   :  1;   /* log bad RU comms */
	unsigned lgun_anl1  :  1;   /* log gun timing errors */
	unsigned lgun_anl2  :  1;
	unsigned lvol_anl1  :  1;   /* log source volume errors */
	unsigned lvol_anl2  :  1;
	unsigned lman_anl1  :  1;   /* log manifold pressure errors */
	unsigned lman_anl2  :  1;
	unsigned lmf1_anl   :  1;   /* log consecutive misfire errors */
	unsigned lclk_anl   :  1;   /* log clock errors */
	unsigned lmf3_anl   :  1;   /* log Y in y misfire errors */
	unsigned lmf4_anl   :  1;   /* log total misfire % exceeded */
	unsigned lsp_anl    :  1;   /* log SP interval errors */
	unsigned lfn_anl    :  1;   /* log FN interval errors */
	unsigned lgsp_anl   :  1;   /* log ground speed errors */
	unsigned lgmsk_anl  :  1;   /* log gun mask errors */
	unsigned lseq_anl   :  1;   /* log sequence errors */
	unsigned lwdp_anl   :  1;   /* log water depth errors */
   unsigned lgd_anl    :  1;   /* log gun depth errors */
   unsigned lwi_anl    :  1;   /* log wing angle errors */
	unsigned ltim_anl   :  1;	 /* log sp time interval errors */
		/* analyse water speed & log water speed to disk */
	unsigned ws_anl	  :  1;	 /* analyse water speed */
	unsigned lws_anl	  :  1;	 /* log water speed */
		/* analyse current  */
	unsigned cu_anl	  :  1;	 /* analyse current */
		/* analyse crab angle */
	unsigned cr_anl	  :  1;	 /* analyse crab angle */
};

         /* OPTIONS */
typedef struct OPT_DEF {
	unsigned work_dr,        /* working drive number */
				add_hdr,        /* additional header length (old ECLH) */
				com_fld,        /* length of comment field (20) */
				com_ofst,       /* offset to start comment field */
				hdr_fld,        /* total jump to first field (4096) */
				spn_fld,        /* length of .DAT record prefix (6) */
				prod_cal;       /* method of production calc */
	float    mul_wadep,      /* multiplier for water depths */
				draft;          /* fatho draft correction */
	char     dat_dir[41],    /* data file directory */
				txt_dir[41],    /* reporting directory */
				cfg_dir[41];	 /* configuration directory */
	float 	mul_cabdep, 	 /* cable depth multiplier */
				mul_gundep, 	 /* gundepth multiplier */
				gundep_ofst,	 /* gundepth offset */
				wasp_ofst,		 /* water speed offset correction */
				gyro_ofst;		 /* gyro correction */
};

         /* FIRING SEQUENCE ANALYSIS */
typedef struct FIR_SEQ {
   unsigned       seq_ok,  /* 1 if last 'seq_len' included all specified */
                           /* sources even if order wrong, else 0 */
                  all_ok,  /* 1 if  last 'seq_len' order correct, else 0 */
                  act_srce,    /* srce number from 'l_flags' */
                  nom_srce, /* initialised srce number */
                  srce_ok;     /* 1 if correct source fired, else 0 */
       char       nom_sname[11],
                  act_sname[11];
   struct FIR_SEQ *next;
   struct FIR_SEQ *last;
};

         /* FIRING SEQUENCE STATISTICS */
typedef struct SEQ_DEF {
   unsigned change,
            reset;
};

			/* SPEED STRUCTURES (along track, water & current) */
typedef struct GSP_DEF {
   unsigned spec,         /* number out of spec */
            corr,         /* total corrupt */
            read;         /* total analysed */
      float min,          /* min */
            tot,          /* total readings (for MEAN) */
            max,          /* max */
            last,         /* last reading (for DMEAN) */
            change,       /* total changes (for DMEAN) */
            sol,          /* sol speed */
            eol;          /* eol speed */
};

         /* PRINTER */
typedef struct PRN_DEF {
   int prndr,             /* printer number */
       ioport,            /* printer port */
       res,               /* resolution (mode) */
       orient;            /* portrait or landscape */
   float xm,              /* X multiplier */
         ym;              /* Y multiplier */
   int page,              /* graphs per printed page */
       lines,             /* lines per page (form length */
       top,               /* top margin (lines ) */
       left,              /* left margin ( 1/80 ) */
       l_feed;            /* lines per inch */
};
