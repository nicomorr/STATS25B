/**************************************************************
*    STRUN.H  run-time global variables structure definition
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

         /* run_time summary */
typedef struct ST_SUMM {
   long fsp,                  /* fsp to process */
		  lsp,				  /* lsp ditto */
		  fsp_pos,			  /* file position fsp */
		  lsp_pos,			  /* file position lsp */
		  f_fsp,				  /* file fsp */
		  f_lsp,				  /* file lsp */
		  sp,					  /* sp being processed */
		  way_sp,			  /* waypoint SP */
		  way_fl,			  /* waypoint flag - stop at waypoint if set */
		  file_sp,			  /* no of SPs on the file */
		  sel_sp,			  /* no of SPs by (abs(fsp-lsp)+prod cal-1) */
		  act_sp,			  /* no of SPs from file positions (for malloc) */
		  procno,			  /* no of SPs processed (counting corrupt) */
		  currno,			  /* no of corrupt SPs encountered */
		  ffn,				  /* first fn to process */
		  lfn,				  /* last fn to process */
		  f_ffn,				  /* file ffn */
		  f_lfn;				  /* file lfn */
   char comment[22];          /* comment line (ID) */
   long ffsp_pos, flsp_pos;   /* file positions of above */
   int ln_tim, selln_tim ;    /*line time in minutes */
   float ln_len, selln_len;   /* line length */
   float av_spd, selav_spd;   /* average speed */
   char ffsp_tim[9], flsp_tim[9]; /* times of first/last SP on file */
   char fsp_tim[9], lsp_tim[9];   /* times of selected SP */
	int ffsp_date, flsp_date, fsp_date, lsp_date;  /* dates as above */
	char ffsp_bf[7], flsp_bf[7], fsp_bf[7], lsp_bf[7];  /* rev-vid buffers */
	unsigned sp_change, fn_change;  /* 6=decr, 7=incr, 8=no change */
   float ffsp_wat, flsp_wat;  /* water depths */
   float fsp_wat, lsp_wat;    /* ditto */
   float ffsp_hed, flsp_hed;  /* system headings */
   float fsp_hed, lsp_hed;    /* headings */
};
