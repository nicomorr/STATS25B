/***************************************************************
*   STS.C   HeadStat MAIN file
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
*  Include:      stats.h   main structure definitions
*                 summ.h   statistics structure definitions
*                strun.h
*               extern.h   important global variables
*             st_graph.h   graphics structure definitions
*             stxfunct.h   function prototypes; also some
*                          global definitions since this file
*                          is included in every source file
*             stxconst.h   constant definitions
****************************************************************
*  (c) West Country Systems Ltd, Cyprus, 1991.
****************************************************************/

#include <stdio.h>         /* Microsoft */
#include <stdlib.h>
#include <direct.h>
#include <graph.h>
#include <dos.h>
#include <process.h>
#include <malloc.h>

#include <dw.h>            /* Greenleaf */
#include <dwdata.h>
#include <color.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* West Country Systems */
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>

			/* global pointer declarations */
struct HDR_DEF *hd = NULL;         /* pointer to header list */
struct PRO_DEF *pd = NULL;         /* pointer to project list */
struct SRCE_DEF *stsrce = NULL;    /* pointer to start of source list */
struct SRCE_DEF *lasrce = NULL;    /* pointer to last  of source list */
struct CAB_DEF *stcab = NULL;      /* pointer to start of cable list */
struct CAB_DEF *lacab = NULL;      /* pointer to last of cable list */
struct WING_DEF *stwi = NULL;      /* wing angles first */
struct WING_DEF *lawi = NULL;      /* wing angles last */
struct GUNDP_DEF *stgundp = NULL;  /* pointer to start of gundep list */
struct GUNDP_DEF *lagundp = NULL;  /* pointer to last of gundep list */
struct ANAL_DEF *andef = NULL;     /* pointer to set/analysis list */
struct SPEC_DEF *spc = NULL;       /* pointer to spec list */
struct OPT_DEF  *opt = NULL;       /* pointer to options list */
struct EXT_DEF *ext = NULL;		  /* pointer for general variables */
struct ST_WAT *wat = NULL;         /* water depth analysis */
struct GRAF_DEF *graf = NULL;      /* pointer to graphics list */
char *data = NULL;					  /* pointer to data buffer */
struct PRN_DEF *prn = NULL;        /* printer definition */

         /* global window handle */
HWND Wwait; 							  /* please wait window */

			/* master YES/NO DIS/EN etc selection */
char *answer[] = {
   "NO ",   /* 0 */
	"YES",
	"DISABLED",
	"ENABLED",
	"CORRUPT",
	"VALID",
	"-",
	"+",
   "?",     /* 8 */
	'\0',
};

/****************************
*    master program version
*****************************/
char ver[15];


/**************************************************************
*	 START MAIN
**************************************************************/
void main (int argc, char **argv)
{
	HWND W0, W1, W00;
	unsigned c; 						  /* keyboard code */
	register int i;					  /* general variable */
   _heapmin();                     /* defragment heap */
   _harderr (hhandler);            /* install critical error handler */
         /* allocate memory to external variables pointer */
	if (!(ext = (struct EXT_DEF *)malloc(sizeof(struct EXT_DEF)))) {
      StatsErr("80001");
      goto FREEM;
   }
    /* force use of \ECLSTAT as program directory */
	if(FORCEDIR) {				/* defined in STXFUNCT.H */
      getcwd(ext->a_str, 40);
      if( (strcmp(ext->a_str, "C:\\ECLSTAT")!=0) &&
               (strcmp(ext->a_str, "D:\\ECLSTAT")!=0) ) {
         _clearscreen(_GCLEARSCREEN);
         _settextposition(3, 0);
         printf("PROGRAM FILES MUST BE IN C:\\ECLSTAT OR D:\\ECLSTAT\n");
         printf("C:\\ECLSTAT OR D:\\ECLSTAT MUST BE THE CURRENT DIRECTORY\n\n");
         printf("The current directory is %s\n\n", ext->a_str);
         printf("\t\tSee ECLSTAT.DOC for Details\n\n");
         if(ext) free (ext);
         exit(1);
      }
   }
         /* ECL MicroPhar Dongle Security */
   if (SECURITY && (strcmp(argv[1], "hypothesis") != 0) ) {
		i = spawnl(P_WAIT, "dongle.exe", NULL);
		switch (i) {
         case -1:         /* cannot find 'DONGLE.EXE' */
            StatsErr("80009");
            exit(1);
         case 0:
            break;
         case 3:          /* cannot find dongle */
            StatsErr("80010");  /* no dongle */
            StatsErr("80011");  /* if prn piggy-backed, is switched on? */
            exit(1);
         default:         /* faulty dongle */
            StatsErr("80012");  /* contact ECL */
            exit(1);
      }
   }
         /* set global version number */
	strcpy(ver, "HEADSTAT V2.40");	/* version 2 extended */
         /*   strcat(ver, "\xe1");   beta test character */
			/* continue allocating memory to global pointers */
	if (!(hd = (struct HDR_DEF *)malloc(sizeof(struct HDR_DEF)))) {
		StatsErr("80002");
      goto FREEM;
	}
	if (!(pd = (struct PRO_DEF *)malloc(sizeof(struct PRO_DEF)))) {
		StatsErr("80003");
		goto FREEM;
	}
	if (!(andef = (struct ANAL_DEF *)malloc(sizeof(struct ANAL_DEF)))) {
		StatsErr("80004");
		goto FREEM;
	}
	if (!(spc = (struct SPEC_DEF *)malloc(sizeof(struct SPEC_DEF)))) {
		StatsErr("80005");
		goto FREEM;
	}
	if (!(opt = (struct OPT_DEF *)malloc(sizeof(struct OPT_DEF)))) {
		StatsErr("80006");
		goto FREEM;
   }
   if (!(graf = (struct GRAF_DEF *)malloc(sizeof(struct GRAF_DEF)))) {
      StatsErr("80007");
		goto FREEM;
	}
   if (!(prn = (struct PRN_DEF *)malloc(sizeof(struct PRN_DEF)))) {
      StatsErr("80008");
		goto FREEM;
	}
			/* initialise general variables */
	ext->ff = TRUE;					  /* couple of bools for keysim */
	ext->ee = TRUE;					  /* and other uses */
	ext->s_input = NULL; 			  /* pointer to str for keysim */
	ext->numbergun = 0;				  /* number of guns */
   ext->numberru = 0;              /* number of RUs */
   ext->numberwing = 0;            /* number of wing angles */
	ext->numberdp = 0;				  /* number of GUNDEPTHS */
	ext->fl_drive=1;					  /* drive number for archive/backup */
	ext->a_num=0;
	ext->b_num=0;
	ext->c_num=0;						  /* general unsigned ints */
   strcpy(ext->back_dir, "\\");    /* default archive directory */
          /* default header settings */
   hd->west = 4;                   /* default western FMT 4 */
   hd->prak = 0;
   hd->digi = 0;
   hd->cgg = 0;
   hd->hgs = 0;
   hd->ssl = 0;
   geco_init();
			/* hardcode in default project definition */
	strcpy(pd->pro_name, " ");
	strcpy(pd->cli_name, " ");
   strcpy(pd->con_name, " ");
   strcpy(pd->log_ver, "ECLH");
	pd->vsl_numb = 1;
	pd->loc_strm = 2;
	pd->rem_strm = 0;
	pd->loc_srce = 2;
	pd->rem_srce = 0;
	pd->seq_len = 2;
	pd->loc_dp = 1;
	pd->pr_un = 2;
	pd->vol_un = 4;
   pd->sp_un =  6;                 /* 6=KNOTS  7=M/S  */
	pd->sp_dst = 25.0F;
	strcpy(pd->qc_com1, " ");
	strcpy(pd->qc_com2, " ");
	strcpy(pd->qc_com3, " ");
   strcpy(pd->qc_com4, " ");
   strcpy(pd->loc1, "LOC SRCE 1");    /* local source 1 name */
   strcpy(pd->loc2, "LOC SRCE 2");    /* ditto local 2 */
   strcpy(pd->loc3, "LOC SRCE 3");    /* ditto local 3 */
   strcpy(pd->loc4, "LOC SRCE 4");    /* ditto local 4 */
   strcpy(pd->rem1, "REM SRCE 1");    /* ditto remote 1 */
   strcpy(pd->rem2, "REM SRCE 2");    /* ditto remote 2 */
   strcpy(pd->rem3, "REM SRCE 3");    /* ditto remote 3 */
   strcpy(pd->rem4, "REM SRCE 4");    /* ditto remote 4 */
   strcpy(pd->locab1, "LOC CAB 1");   /* local cable 1 */
   strcpy(pd->locab2, "LOC CAB 2");
   strcpy(pd->locab3, "LOC CAB 3");
   strcpy(pd->locab4, "LOC CAB 4");
   strcpy(pd->remcab1, "REM CAB 1");  /* remote cable 1 */
   strcpy(pd->remcab2, "REM CAB 2");
   strcpy(pd->remcab3, "REM CAB 3");
   strcpy(pd->remcab4, "REM CAB 4");
        /* default specs */
   spc->d1min = 7.0F;              /* cable depths */
   spc->d1max = 9.0F;              /* two spec levels */
   spc->d2min = 6.0F;
   spc->d2max = 10.0F;
   spc->dcom = 5;                  /* cable RU comms % bad allowed */
   spc->gun1 = 1.0F;               /* gun timing */
   spc->gun2 = 2.0F;               /* two spec levels */
   spc->gvol1 = 2000;              /* source volumes */
   spc->gvol2 = 1800;              /* two spec levels */
   spc->ma1 = 1900;                /* manifold pressure */
   spc->ma2 = 1700;                /* two spec levels */
   spc->mf1 = 8;                   /* consecutive misfires allowed */
   spc->clk = 128.0F;               /* Clock timing nominal */
   spc->clk_ra = 0.5F;             /* range allowed */
   spc->mf3 = 12;                  /* Y misfires allowed */
   spc->mf3_ra = 20;               /* in y shots */
   spc->mf4 = 5;                   /* total misfire % allowed */
   spc->spint = 1;                 /* SP integer */
   spc->fnint = 1;                 /* FN integer */
   spc->gsp = 5.5F;                /* max ground speed */
   spc->gmsk = 15;                 /* minimum gun mask bits required */
   spc->wd_min = 0.0F;             /* min & max water depths */
   spc->wd_max = 100.0F;
   spc->gd_min = 5.0F;             /* min and max gun depths */
   spc->gd_max = 7.0F;
   spc->wing = 5.0F;               /* max (+/-) wing angle */
   spc->t_min = 7.0F;              /* min & max SP time interval */
   spc->t_max = 13.0F;
   strcpy(spc->f_seq, "1234");     /* gun fire sequence */
        /* default option settings */
   opt->work_dr = 3;               /* drive C (3) */
   opt->add_hdr = 0;               /* 0 bytes to add to header length */
   opt->com_fld = 20;              /* length of comment field */
   opt->com_ofst = 0;              /* offset to start comment field */
   opt->hdr_fld = 4096;            /* total jump to first data */
   opt->spn_fld = 6;               /* andys CLIPPER data format RECNO */
   opt->prod_cal = 2;              /* method of production calculation */
   opt->mul_wadep = 1.0F;          /* water depth multiplier */
   opt->mul_cabdep = 1.0F;
   opt->mul_gundep = 1.0F;
   opt->gundep_ofst = 0.0F;
   opt->draft = 0.0F;
	opt->wasp_ofst = 0.0F;				/* water speed correction */
	strcpy(opt->dat_dir, "\\ECLHEAD\\DATA");
	strcpy(opt->txt_dir, "\\ECLSTAT\\TEXT");
   strcpy(opt->cfg_dir, "\\ECLSTAT\\CONFIG");
         /* default analysis settings */
   andef->dep_anl1  = 0;           /* analyse cable depths */
   andef->dep_anl2  = 0;
   andef->dep_com   = 0;           /* analyse RU comms % bad */
   andef->gun_anl1  = 0;           /* analyse gun timing */
   andef->gun_anl2  = 0;
   andef->vol_anl1  = 0;           /* analyse source volumes */
   andef->vol_anl2  = 0;
   andef->man_anl1  = 0;           /* analyse manifold pressure */
   andef->man_anl2  = 0;
   andef->mf1_anl   = 0;           /* analyse consecutive misfires */
   andef->clk_anl   = 0;           /* analyse timing clock */
   andef->mf3_anl   = 0;           /* analyse Y in y misfires */
   andef->mf4_anl   = 0;           /* analyse total misfire % */
   andef->sp_anl    = 0;           /* analyse SP interval */
   andef->fn_anl    = 0;           /* analyse FN interval */
   andef->gsp_anl   = 0;           /* analyse ground speed */
   andef->gmsk_anl  = 0;           /* analyse gun mask errors */
   andef->seq_anl   = 0;           /* analyse sequence errors */
   andef->wdp_anl   = 0;           /* analyse water depth errors */
   andef->gd_anl    = 0;           /* analyse gun depth errors */
   andef->tim_anl   = 0;           /* sp time interval */
	andef->wi_anl	  = 0;			  /* analyse wing angles */
	andef->ws_anl	  = 0;			  /* water speed */
	andef->cu_anl	  = 0;			  /* current analysis */
	andef->cr_anl	  = 0;			  /* crab angle analysis */
   andef->ldep_anl1  = 0;          /* log cable depth errors */
   andef->ldep_anl2  = 0;
   andef->ldep_com   = 0;          /* log bad RU comms */
   andef->lgun_anl1  = 0;          /* log gun timing errors */
   andef->lgun_anl2  = 0;
   andef->lvol_anl1  = 0;          /* log source volume errors */
   andef->lvol_anl2  = 0;
   andef->lman_anl1  = 0;          /* log manifold pressure errors */
   andef->lman_anl2  = 0;
   andef->lmf1_anl   = 0;          /* log consecutive misfire errors */
   andef->lclk_anl   = 0;          /* log clock errors */
   andef->lmf3_anl   = 0;          /* log Y in y misfire errors */
   andef->lmf4_anl   = 0;          /* log total misfire % exceeded */
   andef->lsp_anl    = 0;          /* log SP interval errors */
   andef->lfn_anl    = 0;          /* log FN interval errors */
   andef->lgsp_anl   = 0;          /* log ground speed errors */
   andef->lgmsk_anl  = 0;          /* log gun mask errors */
   andef->lseq_anl   = 0;          /* log sequence errors */
   andef->lwdp_anl   = 0;          /* log water depth errors */
   andef->lgd_anl    = 0;          /* log gun depth errors */
   andef->lwi_anl    = 0;          /* log wing angle errors */
	andef->ltim_anl	= 0;			  /* log sp time interval errors */
	andef->lws_anl 	= 0;			  /* log water speed errors */
         /* default graphics settings */
   graf->dep_anl1  = 0;            /* graph cable depth errors */
   graf->gun_anl1  = 0;            /* graph gun timing errors */
   graf->vol_anl1  = 0;            /* graph source volumes */
   graf->man_anl1  = 0;            /* graph manifold pressure */
   graf->clk_anl   = 0;            /* graph timing clock */
   graf->sp_anl    = 0;            /* graph SP interval */
   graf->fn_anl    = 0;            /* graph FN interval */
   graf->gmsk_anl  = 0;            /* graph gun mask errors */
   graf->seq_anl   = 0;            /* graph sequence errors */
   graf->gd_anl    = 0;            /* graph gun depth errors */
   graf->tim_anl   = 0;            /* sp time interval */
   graf->print     = 0;
   graf->log       = 0;
   graf->cab_scale = 2.0F;
   graf->cab_line  = 1.0F;
   graf->gun_scale = 5.0F;
   graf->tim_scale = 1.0F;
   graf->gud_scale = 2.0F;
   graf->gud_line  = 1.0F;
   graf->vol_scale = 500;
   graf->air_scale = 200;
         /* default printer settings */
   prn->prndr = 0;
   prn->ioport = 0;
   prn->orient = 0;  /* MUST LEAVE IN - NOT MENU SELECTABLE */
   prn->res = 0;
   prn->xm = 1.0F;
   prn->ym = 1.0F;
   prn->page  = 6;
   prn->lines = 66;
   prn->top = 4;
   prn->left = 10;
   prn->l_feed = 6;
         /* <Ctrl_Brk> check OFF */
	breakchk(OFF);
         /* create windows */
	W0 = vcreat (1, 80, REVNORML, NO);
	W00 = vcreat (1, 80, REVNORML, NO);
	Wwait = vcreat (1, 11, NORML, NO);
         /* set 3B shading attribute for menus - defined in STXFUNCT.H */
   tblattr(BACK3D, bldattr(GREEN, LTGREY));
			/* set a YELLOW/GREEN colour combo - defined in STXFUNCT.H */
	tblattr(YELGRN, bldattr(YELLOW, GREEN));
			/* set a blinking colour combo - defined in STXFUNCT.H */
	tblattr(BLINKBLUEWH, bldattr(BLUE, WHITE));
         /* locate windows & continue */
	vlocate (Wwait, 12, 34);
	vratputs (Wwait, 0, 0, BLINKBLUEWH,"  WORKIN!  ");
	vframe (Wwait, REVERR, FRDOUBLE);
	vlocate (W00, 24, 0);
	vratputs (W0, 0, 1, REVNORML, ver);
	BuildClockWindow (0, -1, REVNORML, 0, 10, 5, 2);
	W1 = vcreat (1, 6, REVEMPHNORML, NO);
	vlocate (W1, 0, 36);
	vratputs (W1, 0, 0, REVERR, "  F1  ");
	visible (W0, YES, NO);
	visible (W00, YES, NO);
	visible (W1, YES, NO);
   ControlClockWindow (YES, NO);
	vratputs (W00, 0, 1, REVNORML,
"F2 Run  F3 Cnfg  F4 Anls  F5 Opts  F6 Spec  F7 Fils  F8 Gfx  F9 Grf F10 Quit");
			/* after install - run with /X option for "NEW Additions" */
	if( !strcmp( argv[1], "/X" ) )
		string_help( "NEW Additions" );
         /* load configuration from disk */
	loaddef(1, atoi( argv[1] ) );   /* suppress banner if argv[1] = 100 */
			/* return arguments from VGR_STS.EXE for .GFR graphics */
	if( atoi( argv[2] ) == 16001 )
		StatsErr( "16001");			  /* .GFR file data corrupt */
	if( atoi( argv[2] ) == 16002 )
		StatsErr( "16002");			  /* .GFR could not open file */
	if( atoi( argv[2] ) == 16003 )
		StatsErr( "16003");			  /* .GFR not enough memory */
			/* main menu loop */
	while( ( c = vgetkey( UpdateClockWindow ) ) != F10 ) {
		switch (c) {
			case F1:					  /* calls "STATMAIN.HLP" window */
				visible (W00, NO, NO);
				help_main();
				visible (W00, YES, NO);
				break;
			case F2: 					/* run stats */
				visible (W00, NO, NO);
				visible (W1, NO, NO);
				_heapmin();			  /* defragment heap */
				runfind(1);
				_heapmin();			  /* defragment heap */
				visible (W00, YES, NO);
				visible (W1, YES, NO);
				break;
			case F3: 					/* configuration */
				visible (W00, NO, NO);
				visible (W1, NO, NO);
				maincfg();
				visible (W00, YES, NO);
				visible (W1, YES, NO);
				break;
			case F4: 					/* select analyses */
				visible (W00, NO, NO);
				visible (W1, NO, NO);
				setanal();
				visible (W00, YES, NO);
				visible (W1, YES, NO);
				break;
			case F5: 					/* select options	*/
				visible (W00, NO, NO);
				visible (W1, NO, NO);
				set_opts();
				visible (W00, YES, NO);
				visible (W1, YES, NO);
				break;
			case F6: 					/* set specs	*/
				visible (W00, NO, NO);
				visible (W1, NO, NO);
				setspec();
				visible (W00, YES, NO);
				visible (W1, YES, NO);
				break;
			case F7: 					/* archive routines */
				visible (W00, NO, NO);
				visible (W1, NO, NO);
				if(arc_menu()) runfind(0);
				visible (W00, YES, NO);
				visible (W1, YES, NO);
				break;
			case F8:						/* configure graphics */
				visible (W00, NO, NO);
				visible (W1, NO, NO);
				setgraf();
				visible (W00, YES, NO);
				visible (W1, YES, NO);
				break;
			case F9: 					/* run .GRF graphics */
				VGAMenu();
				break;
			default:
				break;
		}
	}
	pcuron();
	_settextposition(9,0);
   FREEM:                           /* jump point for mem alloc fail */
	if (ext) free (ext);
	if (hd) free (hd);
	if (pd) free (pd);
	if (andef) free (andef);
	if (spc) free (spc);
   if (opt) free (opt);
   if (graf) free (graf);
   if (prn) free (prn);
   breakchk(ON);
	vexit(0);
}


/***************************************************************
*   declare two datawindows stubs to reduce .EXE size
*   this can only be done if Memo & Subform type fields are
*   not used in the program
*   these two stubs replace low level service routines
*	 they save about 15K of code - goody!!
****************************************************************/
unsigned _memo(TRANSACTION *t1, HWND t2) {}
unsigned _subform(TRANSACTION *t1, HWND t2) {}
