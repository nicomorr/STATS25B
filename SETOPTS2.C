/*************************************************************
*   SETOPTS2.C   set the options - also various utilities
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
#include <dos.h>
#include <direct.h>

#include <dw.h>            /* greenleaf */
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct HDR_DEF *hd;        /* header  list pointer */
extern struct PRO_DEF *pd;        /* project list pointer */
extern struct OPT_DEF  *opt;      /* pointer to options list */
extern struct ANAL_DEF *andef;    /* pointer to set/analysis list */
extern struct ST_SUMM *sum;       /* summary structure pointer */
extern struct EXT_DEF *ext;       /* general useful globals */
extern char *units[];             /* pointer to units list */
extern char *answer[];            /* YES/NO - + ? etc etc */
extern HWND Wwait;                /* wait window (StWait() & EnWait() */
extern char *units[];				 /* FEET/METRES PSI/BARS etc */


/**************************************************************
*	 option configuration
**************************************************************/
void set_dirs (void)
{
	HWND W1, W2;				/* data entry/message windows */
	TRANSACTION *tp;			/* transaction structure */
	FIELD *fld; 				/* field structure */
	register int i;
			/* define (double) field variables */
   double   mul_wadep,     /* multiplier for fathometer depths */
				draft,			/* fathometer draft correction */
            mul_cabdep,    /* cable depth multiplier */
            mul_gundep,    /* gundepth multiplier */
				gundep_ofst,	/* gundepth sensor offset */
				wasp_ofst,		/* water speed offset */
				gyro_ofst;		/* gyro correction */
			/* initialise (double) conversion field variables */
	mul_wadep = (double) opt->mul_wadep;	 /* multiplier for water depths */
	draft = (double) opt->draft;				 /* fatho draft */
	mul_cabdep = (double) opt->mul_cabdep;	 /* multiplier for cable depths */
	mul_gundep = (double) opt->mul_gundep;	 /* multiplier for gun depths */
	gundep_ofst = (double) opt->gundep_ofst;	/* offset for gun depths */
	wasp_ofst = (double) opt->wasp_ofst;	 /* water speed offset */
	gyro_ofst = (double) opt->gyro_ofst;		 /* gyro correction */
	W2 = vcreat (1, 80, REVNORML, NO);
	vlocate (W2, 24, 0);
	vratputs (W2, 0, 1, REVNORML, "<ESC> Exit");
	vratputs (W2, 0, 36, REVERR, "  F1  ");
	vratputs (W2, 0, 60, REVEMPHNORML, "<CTRL-ENTER> Accept");
	W1 = vcreat (18, 68, NORML, NO);
	vlocate (W1, 3, 5);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " View/Change Options ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
      /*# defldval_opt() detranval_glob() defldset_opt()
          opt_help() EscapeKey() */
	tp = vpreload (19, 6, NULLF, defldval_opt, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
   tp->inp_idle = UpdateClockWindow;
   tp->fld_setfn = defldset_opt;
   tp->helpfn = opt_help;   /* ignore param warning */
	fld = *(tp->def);
	for(i=0;i<19;i++)
	fld[i].use_ext = TRUE;   /* use/display the preload values */
	vdefuint (tp, 0, 1, 1,
		 "Work Drive", 16, 1, "9", &opt->work_dr, YES);
	vdefatsa (tp, 0, 0, 23, EMPHNORML, "Drive Numbers 1 to 5 only (Drives A to E)");
	vdefstr (tp, 1, 2, 1, "Data   (work) Directory", 26, 40,
		 "\\!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", opt->dat_dir, NO);
	vdefstr (tp, 2, 3, 1, "Report (work) Directory", 26, 40,
       "\\!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", opt->txt_dir, NO);
	vdefstr (tp, 3, 4, 1, "Config (work) Directory", 26, 40,
       "\\!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", opt->cfg_dir, NO);
	vdefuint (tp, 4, 6, 1,
		 "Archive Drive", 16, 1, "9", &ext->fl_drive, YES);
	vdefstr (tp, 5, 7, 1, "Archive/Bkup  Directory", 26, 40,
		 "\\!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", ext->back_dir, NO);
	vdefuint (tp, 6, 9, 1, "Additional Header Length",
					  26, 0, "9999", &opt->add_hdr, YES);
	vdefuint (tp, 7, 9, 33, "Comment Field Length",
					  57, 0, "99", &opt->com_fld, YES);
	vdefuint (tp, 8, 10, 1, "Comment Field Offset",
					  26, 0, "9999", &opt->com_ofst, YES);
	vdefuint (tp, 9, 10, 33, "Total Preamble Offset",
					  57, 0, "99999", &opt->hdr_fld, YES);
	vdefuint (tp, 10, 11, 1, "Record Prefix Length",
					  26, 0, "99", &opt->spn_fld, YES);
	vdefuint (tp, 11, 11, 33, "Production Calculation",
					  57, 0, "9", &opt->prod_cal, YES);
	vdefdbl (tp, 12, 13, 1, "Water Depth Multiplier",
					  25, 0, "99.99", &mul_wadep, YES);
	vdefdbl (tp, 13, 13, 32, "+ Draft",
					  41, 0, "99.99", &draft, YES);
	vdefdbl (tp, 14, 14, 1, "Gun Depth Multiplier",
					  25, 0, "99.99", &mul_gundep, YES);
	vdefdbl (tp, 15, 14, 32, "+ Offset",
					  41, 0, "99.99", &gundep_ofst, YES);
	vdefdbl (tp, 16, 15, 1, "Cable Depth Multiplier",
					  25, 0, "99.99", &mul_cabdep, YES);
	vdefdbl (tp, 17, 16, 1, "Water Speed Correction",
					  25, 0, "99.99", &wasp_ofst, YES);
	vdefdbl (tp, 18, 17, 1, "Gyro Correction",
					  25, 0, "999.99", &gyro_ofst, YES);
	vdefatsa (tp, 1, 13, 50, NORML, units[pd->loc_dp]);
	vdefatsa (tp, 2, 12, 35, EMPHNORML, "(0 = -1SP, 1 = SP, 2 = +1SP)");
	vdefatsa (tp, 3, 14, 50, NORML, units[pd->loc_dp]);
	vdefatsa (tp, 4, 16, 33, NORML, units[pd->sp_un]);
	vdefatsa (tp, 5, 17, 33, NORML, "DEGREES");
	visible (W2, YES, NO);
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
		opt->mul_wadep = (float) mul_wadep; 	 /* water depth multiplier */
      opt->draft = (float) draft;             /* fatho draft addition */
		opt->mul_cabdep = (float) mul_cabdep;	 /* multiplier for cable depths */
		opt->mul_gundep = (float) mul_gundep;	 /* multiplier for gun depths */
		opt->gundep_ofst = (float) gundep_ofst; /* offset for gun depths */
		opt->wasp_ofst = (float) wasp_ofst; 	 /* water speed correction */
		opt->gyro_ofst = (float) gyro_ofst;	 /* gyro correction */
	}
	vdelete(W1, NONE);
	vdelete (W2, NONE);
}


/*************************************************************
*   options general help function
**************************************************************/
void opt_help()
{
   string_help("Options");
}


/*************************************************************
*   options directories help function
**************************************************************/
void dir_help()
{
   string_help("Drives/Directories");
}


/*************************************************************
*	 file viewer help function
**************************************************************/
void view_help()
{
	string_help("Report Files");
}


/*************************************************************
*   options production help function
**************************************************************/
void prod_help()
{
   string_help("Other Units");
}


/*************************************************************
*   options depth units help function
**************************************************************/
void depun_help()
{
   string_help("Depth Units");
}


/**************************************************************
*   field set function - just set helpfn for the field
*   ignore param warnings
**************************************************************/
int defldset_opt (TRANSACTION *tp)
{        /* ignore help function param warnings */
      /*# dir_help() opt_help() prod_help() depun_help() */
   if (tp->cur_fld < 6) tp->helpfn = dir_help;     /* dr/dirs */
   if ( (tp->cur_fld > 5) && (tp->cur_fld < 11) )  /* general */
      tp->helpfn = opt_help;
   if (tp->cur_fld == 11) tp->helpfn = prod_help;  /* production */
   if (tp->cur_fld > 11) tp->helpfn = depun_help;  /* depth units */
   return (TRUE);
}


/**************************************************************
*   field validation for drive numbers & directories
**************************************************************/
defldval_opt (TRANSACTION *tp) /* field validation */
{
	unsigned drive, num_drives;
   if(tp->cur_fld == 0) {      /* work drive field */
		_dos_getdrive(&drive);   /* get current disk drive */
		_dos_setdrive(drive, &num_drives);  /* get number of logical drives */
      if(opt->work_dr<3) {
         StatsErr("30001");   /* must be a hard drive */
			return(FALSE);
      }
      if(opt->work_dr>num_drives) {
         StatsErr("30002");   /* work drive does not exist */
			return(FALSE);
      }
   }
   if(tp->cur_fld == 1) {      /* data directory */
      if(check_drive(opt->work_dr, opt->dat_dir))
         return(FALSE);
   }
   if(tp->cur_fld == 2) {      /* reporting directory */
      if(check_drive(opt->work_dr, opt->txt_dir))
         return(FALSE);
   }
   if(tp->cur_fld == 3) {      /* configuration directory */
      if(check_drive(opt->work_dr, opt->cfg_dir))
         return(FALSE);
   }
   if(tp->cur_fld == 4) {      /* archive drive field */
		_dos_getdrive(&drive);   /* get current disk drive */
		_dos_setdrive(drive, &num_drives);  /* get number of logical drives */
      if(ext->fl_drive<1) {
         StatsErr("30007");    /* drive 0 does not exist */
			return(FALSE);
      }
      if(ext->fl_drive>num_drives) {
         StatsErr("30008");    /* archive drive does not exist */
			return(FALSE);
      }
   }
   if(tp->cur_fld == 5) {      /* archive directory */
      if(ext->fl_drive>2) {
         if(check_drive(ext->fl_drive, ext->back_dir))
            return(FALSE);
      }
   }
	return(TRUE);
}


/**************************************************************
*   save option configuration
**************************************************************/
void save_opt (void)   /* save option cofiguration */
{
	FILE *fp, *fi;
	if (!KeySure()) return;
	fp = fopen("OPTIONS.CFG", "wb");
	fwrite(opt, sizeof(struct OPT_DEF), 1, fp);
	fclose(fp);
	fi = fopen("BKDRIVE.SET", "wb");
	fwrite(&ext->fl_drive, sizeof(ext->fl_drive), 1, fi);
	fwrite(ext->back_dir, sizeof(ext->back_dir), 1, fi);
	fclose(fi);
   StatsMessage ("SAVED OPTIONS.CFG/BKDRIVE.SET");
	return;
}


/**************************************************************
*   load option configuration
**************************************************************/
load_opt (int i)	 /* load option configuration */
{
   FILE *fp, *fi;
   if(i)
      if (!KeySure())
         return(0);
	if((fi = fopen("BKDRIVE.SET", "rb")) == NULL) {
	  if(i) StatsErr("30003");
	} else {
		if ((1 != fread(&ext->fl_drive, sizeof(ext->fl_drive), 1, fi))||
					(1 != fread(ext->back_dir, sizeof(ext->back_dir), 1, fi)))
			StatsErr("30004");
		fclose(fi);
	}
	if((fp = fopen("OPTIONS.CFG", "rb")) == NULL) {
	  if(i) StatsErr("30005");
	  return(0);
	}
	if (1 != fread(opt, sizeof(struct OPT_DEF), 1, fp))
		StatsErr("30006");
   fclose(fp);
   if(i) StatsMessage("LOADED OPTIONS.CFG/BKDRIVE.SET");
	return(1);
}
