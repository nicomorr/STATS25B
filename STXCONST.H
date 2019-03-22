/***************************************************************
*    STXCONST.H    Holds various constants
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

			/* floating point constants & conversions */
#define GRAD		57.29577951F		  /* radians to degrees */
#define DRAD		0.0174532925F		  /* degrees to radians */
#define PI			3.14159265F			  /* Pi */
#define MS_KNOTS	1.944F				  /* metres/second to knots */
#define MT_FEET	3.2808399F			  /* metres to feet */
#define FT_METRES 0.3048F				  /* feet to metres */
#define BIN_SEMI	0.0054931F			  /* binary semicircles to degrees */
#define PS_BARS							  /* Psi to Bars */
#define BR_PSI 							  /* Bars to Psi */
#define CI_CCS 							  /* CuIns to CC */
#define CC_CUI 							  /* CC to CuIns */
#define LPT1		0						  /* printer port */
#define ONE_PAGE	16383L				  /* EMM page */
