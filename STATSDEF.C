/***************************************************************
*    STATSDEF.C    handles loading of configuration files
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
#include <process.h>

#include <dw.h>            /* greenleaf */
#include <dwmenu.h>
#include <sftd.h>
#include <ibmkeys.h>
#include <color.h>

#include <stats.h>         /* wcs */
#include <st_graph.h>
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct HDR_DEF *hd;        /* header list pointer */
extern struct PRO_DEF *pd;        /* project list pointer */
extern struct ANAL_DEF *andef;    /* pointer to set/analysis list */
extern struct SPEC_DEF *spc;      /* pointer to specifications list */
extern struct OPT_DEF  *opt;      /* pointer to options list */
extern struct GRAF_DEF *graf;     /* pointer to graphics list */
extern struct EXT_DEF *ext;       /* external variables */
extern struct PRN_DEF *prn;       /* printer definitions file */
extern char *units[];              /* FEET/METRES PSI/BARS etc */


/*************************************************************
* start the load sequence
**************************************************************/
void loaddef(int i, int b )
{
	HWND Wdef;
	register int n = 0;   /* variable size window line counter */
	Wdef = vcreat (12, 17, NORML, NO);
   if(!load_hdr(0))
	vratputs (Wdef, n++, 1, REVERR,	 "NO HEADER.CFG");
   if(!load_pro(0))
	vratputs (Wdef, n++, 1, REVERR,	 "NO PROJECT.CFG");
	if(!(ext->numbergun = load_gun(0)))
	vratputs (Wdef, n++, 1, REVERR,	 "NO SOURCE.CFG");
	if(!(ext->numberru = load_cab(0)))
	vratputs (Wdef, n++, 1, REVERR,	 "NO CABLE.CFG");
   if(!(ext->numberwing = load_wing(0)))
	vratputs (Wdef, n++, 1, REVERR,	 "NO WINGS.CFG");
	if(!(ext->numberdp = load_gundp(0)))
	vratputs (Wdef, n++, 1, REVERR,	 "NO GUNDEPTH.CFG");
	if(!load_anl(0))
	vratputs (Wdef, n++, 1, REVERR,	 "NO ANALYSIS.CFG");
	if(!load_spec(0))
	vratputs (Wdef, n++, 1, REVERR,	 "NO SPECSET.CFG");
	if(!load_opt(0))
	vratputs (Wdef, n++, 1, REVERR,	 "NO OPTIONS.CFG");
   if(!load_graf(0))
	vratputs (Wdef, n++, 1, REVERR,	 "NO GRAPHICS.CFG");
   if(!load_prn(0))
	vratputs (Wdef, n++, 1, REVERR,	 "NO PRINTER.SET");
      /* load message */
	vwind (Wdef, n, 17, 0, 0);
	vlocate (Wdef, 8, 31);
	vframe (Wdef, EMPHNORML, FRDOUBLE);
	switch(hd->contr) {
		case 0:				 /* GECO */
			break;
		case 1:				 /* PRAKLA */
			sel_prakla(0);
			break;
		case 2:				 /* WESTERN */
			switch(hd->west) {
				case 2:			 /* Fmt 3 */
					sel_west3(0);
					break;
				case 3:		 /* Fmt 4 */
					sel_west4(0);
					break;
				default:
               StatsErr("17001");  /* Invalid Western Format IN HEADER.CFG */
					break;
			}
			break;
      case 5:            /* DIGICON */
         sel_digicon(0);
			break;
		default:
         StatsErr("17002");  /* Invalid Contractor IN HEADER.CFG */
			break;
	}
	if( b != 100 ) {	 /* suppress if returning from VGR.EXE */
		visible (Wdef, YES, NO);
		if(i) PressKey(11+n, 32);
		else Delay18 (27);
	}
	vdelete (Wdef, NONE);
	return;
}

/*************************************************************
*  GRF graphics
**************************************************************/
void VGAMenu()
{
	HWND W1;
	int a, b, c, d, e, f, g, h, j;
	char a_buff[8], b_buff[8], c_buff[8], d_buff[8];
	char e_buff[8], f_buff[8], g_buff[8], h_buff[8];
	char i_buff[8], j_buff[8], k_buff[8], l_buff[8];
	char m_buff[8], n_buff[8];
	char prog_buf[_MAX_PATH];		 /* .GRF path/file buffer */
	W1 = vcreat(2, 70, NORML, NO);
	vlocate(W1, 3, 4);
	vframe (W1, EMPHNORML, FRDOUBLE);
	vatputs(W1, 0, 2, "Select .GRF File for viewing &/or printing of speed comparison plots" );
	vatputs(W1, 1, 4, "for tidal current analysis.  GRF Graphics Settings via F8 Setup");
	visible (W1, YES, NO);
	if( find_file(0, "*.GRF", opt->work_dr, opt->txt_dir ) == 1 ) {
		pcuron();
		breakchk(ON);
		sprintf(prog_buf, "%c:%s", 'A' + opt->work_dr -1, opt->txt_dir);
		if(strlen(opt->txt_dir)>1) strcat(prog_buf, "\\");
		strcat(prog_buf, ext->pname);
		sprintf( f_buff, "%f", graf->xm );
		sprintf( g_buff, "%f", graf->ym );
		sprintf( m_buff, "%f", graf->spwin );
		sprintf( n_buff, "%f", graf->hdwin );
		a = ( int ) graf->polyorder;
		b = prn->prndr;
		c = prn->ioport;
		d = ( int ) graf->res;
		e = ( int ) graf->or;
		f = ( int ) graf->ff;
		g = ( int ) graf->tmar;
		h = ( int ) graf->lmar;
		j = prn->lines;
		if (ext) free (ext);
		if (hd) free (hd);
		if (pd) free (pd);
		if (andef) free (andef);
		if (spc) free (spc);
		if (opt) free (opt);
		if (graf) free (graf);
		if (prn) free (prn);
		vdelete(W1, NONE);
		if( spawnlp( P_OVERLAY, "STS_VGR.EXE", "STS_VGR.EXE",
					/***********	ARGUMENTS  ********/
			prog_buf,							/* argv[1] - full path .GRF file */
			units[pd->sp_un],						/* argv[2] - speed units */
			itoa( 0, a_buff, 10 ),				/* argv[3] - NOT USED */
			itoa( a , b_buff, 10 ),	/* argv[4] - polyorder */
			itoa( b,	c_buff, 10 ),		/* argv[5] - printer type */
			itoa( c, d_buff, 10 ),		/* argv[6] - com port */
			itoa( d, e_buff, 10 ),		/* argv[7] - resolution */
			f_buff,								/* argv[8] - x mult */
			g_buff,								/* argv[9] - y mult */
			itoa( e, h_buff, 10 ),	/* argv[10] - orientation */
			itoa( f, i_buff, 10 ),		/* argv[11] - FF after print */
			itoa( g,	j_buff, 10 ),	/* argv[12] - top margin */
			itoa( h,	k_buff, 10 ),	/* argv[13] - left margin */
			itoa( j,	l_buff, 10 ),	/* argv[14] - page length */
			m_buff,								/* argv[15] -	speed clip window */
			n_buff,								/* argv[16] -	head clip window */
																NULL ) == -1) {
			a= errno;		 /* IF A DOS ERROR */
			StatsDosError("STS_VGR.EXE", a);
		}
	}
	vdelete(W1, NONE);
	return;
}
