/***************************************************************
*   ST_GRAPH.H   graphics structure definitions
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

			/* graphics analysis enable */
typedef struct GRAF_DEF {
   unsigned dep_anl1  :  1;    /* graph cable depth errors */
   unsigned gun_anl1  :  1;    /* graph gun timing errors */
   unsigned vol_anl1  :  1;    /* graph source volumes */
   unsigned man_anl1  :  1;    /* graph manifold pressure */
   unsigned clk_anl   :  1;    /* graph timing clock */
   unsigned sp_anl    :  1;    /* graph SP interval */
   unsigned fn_anl    :  1;    /* graph FN interval */
   unsigned gmsk_anl  :  1;    /* graph gun mask errors */
   unsigned seq_anl   :  1;    /* graph sequence errors */
   unsigned gd_anl    :  1;    /* graph gun depth errors */
	unsigned tim_anl	 :  1;	 /* sp time interval */
	unsigned spd_anl	 :  1;	 /* speed comparison */
	unsigned hed_anl	 :  1;	 /* heading comparison */

         /* whether to print graphs */
   unsigned print;
         /* whether to log graphs to disk - not used */
   unsigned log;
            /* various graphics specs */
   double   cab_scale,
            cab_line,
            gun_scale,
            tim_scale,
            gud_scale,
            gud_line;
   unsigned vol_scale,
				air_scale;
	double   spwin,            /* speed clip window */
				hdwin;            /* ditto heading */
	unsigned polyorder,			/* curve fit polynomial order */
				res;					/* resolution (mode) */
	double	xm,					/* X multiplier */
				ym;					/* Y multiplier */
	unsigned or,					/* portrait = 0, landscape = 1; */
				ff,					/* formfeed = 0; */
				tmar,             /* top margin */
				lmar;             /* left margin */
};

			/* not used yet */
typedef struct GRFPRN_DEF {
	double   spwin,            /* speed clip window */
				hdwin;            /* ditto heading */
	unsigned polyorder,			/* curve fit polynomial order */
				res;					/* resolution (mode) */
	double	xm,					/* X multiplier */
				ym;					/* Y multiplier */
	unsigned or,					/* portrait = 0, landscape = 1; */
				ff,					/* formfeed = 0; */
				tmar,             /* top margin */
				lmar;             /* left margin */
};


         /* window coordinates */
typedef struct L_BORD {
      double wu;    /* upper window coord */
      double wl;    /* lower window coord */
      double ru;    /* upper rectangle limit */
      double rl;    /* lower rectangle limit */
      double sx;    /* spec line x coord */
};


         /* increment/decrement & shotpoint & filenumbers */
typedef struct IN_CREM {
   long tsp,
        fsp,
        lsp,
        ffn,
        lfn;
    unsigned sp_inc,
             fn_inc;
};


         /* initial firing sequence */
typedef struct FIRE_BUF {
   char fire_info[50];
};


         /* no-error graph flags (16 bit bools) */
typedef struct ERR_GRAF {
   unsigned sp    :    1;             /* sp numbering */
   unsigned fn    :    1;             /* fn numbering */
   unsigned tm    :    1;             /* sp time intervals */
   unsigned fr    :    1;             /* fire sequence */
   unsigned gm    :    1;             /* gun masks */
   unsigned gu    :    1;             /* srce timing */
   unsigned pr    :    1;             /* air pressure */
   unsigned vl    :    1;             /* srce volumes */
   unsigned tb    :    1;             /* system timing */
   unsigned c1    :    1;             /* cable depth 1 */
   unsigned c2    :    1;             /* cable depth 2 */
   unsigned c3    :    1;             /* cable depth 3 */
   unsigned c4    :    1;             /* cable depth 4 */
   unsigned gd    :    1;             /* gun depths */
   unsigned s1    :    1;             /* spare 1 */
   unsigned s2    :    1;             /* spare 2 */
};


         /* make the graphs or not flags (16 bit bools) */
typedef struct MAKE_GRAF {
   unsigned sp    :    1;             /* sp numbering */
   unsigned fn    :    1;             /* fn numbering */
   unsigned tm    :    1;             /* sp time intervals */
   unsigned fr    :    1;             /* fire sequence */
   unsigned gm    :    1;             /* gun masks */
   unsigned gu    :    1;             /* srce timing */
   unsigned pr    :    1;             /* air pressure */
   unsigned vl    :    1;             /* srce volumes */
   unsigned tb    :    1;             /* system timing */
   unsigned c1    :    1;             /* cable depth 1 */
   unsigned c2    :    1;             /* cable depth 2 */
   unsigned c3    :    1;             /* cable depth 3 */
   unsigned c4    :    1;             /* cable depth 4 */
   unsigned gd    :    1;             /* gun depths */
   unsigned s1    :    1;             /* spare 1 */
   unsigned s2    :    1;             /* spare 2 */
};


         /* flags if no-error box report already made */
typedef struct REP_GRAF {
   unsigned sp    :    1;             /* sp numbering */
   unsigned fn    :    1;             /* fn numbering */
   unsigned tm    :    1;             /* sp time intervals */
   unsigned fr    :    1;             /* fire sequence */
   unsigned gm    :    1;             /* gun masks */
   unsigned gu    :    1;             /* srce timing */
   unsigned pr    :    1;             /* air pressure */
   unsigned vl    :    1;             /* srce volumes */
   unsigned tb    :    1;             /* system timing */
   unsigned c1    :    1;             /* cable depth 1 */
   unsigned c2    :    1;             /* cable depth 2 */
   unsigned c3    :    1;             /* cable depth 3 */
   unsigned c4    :    1;             /* cable depth 4 */
   unsigned gd    :    1;             /* gun depths */
   unsigned s1    :    1;             /* spare 1 */
   unsigned s2    :    1;             /* spare 2 */
};
