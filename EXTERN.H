/*************************************************************
*    EXTERN.H  global variables structure definition
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

typedef struct EXT_DEF {
   char tim[9];               /* short time (to 1 sec) */
   char l_tim[12];            /* long time (to 0.01 sec) */
	int ff;                    /* couple of bools for keysim */
	int ee;                    /* and other uses */
	char *s_input;             /* pointer to str for keysim */
   char a_str[71];            /* string for directories etc */
	char b_str[11], c_str[11], d_str[11], e_str[11]; /* available */
	int numbergun;             /* number of guns */
   int numberru;              /* number of cable depths (RUs) */
   int numberdp;              /* number of gun depths */
   int numberwing;            /* number of wing angles */
   unsigned fl_drive;         /* backup drive number */
	char pname[13];				/* file name */
	char back_dir[41];			/* archive/backup directory */
	unsigned a_num, b_num,		/* general unsigned ints */
				c_num, d_num,
				e_num, f_num,
				g_num, h_num,
				i_num, j_num,
				k_num, l_num,
				m_num, n_num,
				o_num, p_num,
				q_num, r_num,
				s_num, t_num,
				u_num, v_num,
				w_num, x_num,
				y_num, z_num;
   int      a_int,
            b_int,
            c_int,
            d_int;    /* signed ints */
};
