/*************************************************************
*   PROCFG.C   project configuration
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

#include <dw.h>            /* greenleaf */
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct PRO_DEF *pd;         /* pointer to project list */
extern struct EXT_DEF *ext;
extern char *units[];              /* FEET/METRES PSI/BARS etc */
extern ITEM pro_mnu[];


/**************************************************************
*   save project definition to disk
**************************************************************/
void save_pro ()
{
	FILE *fp;
	if (!KeySure()) return;
	fp = fopen("PROJECT.CFG", "wb");
	fwrite(pd, sizeof(struct PRO_DEF), 1, fp);
	fclose(fp);
   StatsMessage ("SAVED PROJECT.CFG");
}


/**************************************************************
*   load project definition from disk
**************************************************************/
load_pro (int i)
{
   FILE *fp;
   if(i)
      if (!KeySure())
         return(0);
	if((fp = fopen("PROJECT.CFG", "rb")) == NULL) {
	  if(i) StatsErr("26001");
	  return (0);
	}
	if (1 != fread(pd, sizeof(struct PRO_DEF), 1, fp))
		StatsErr("26002");
   fclose(fp);
   if(i) StatsMessage("LOADED PROJECT.CFG");
	return (1);
}


/**************************************************************
*   set project definition
**************************************************************/
void set_pro ()
{
	register int i=0, n=1;
	HWND W1, Wmsg;				/* data entry/message windows */
	TRANSACTION *tp;			/* transaction structure */
	FIELD *fld; 				/* field structure */
		/* define (double conversion necessary) field variables */
	double   sp_dst;        /* SP interval */
		/* initialise the UNITS field variables */
	ext->a_num = pd->loc_dp;
	strcpy(ext->b_str, units[pd->loc_dp]);
	ext->b_num = pd->pr_un;
	strcpy(ext->c_str, units[pd->pr_un]);
	ext->c_num = pd->vol_un;
	strcpy(ext->d_str, units[pd->vol_un]);
	ext->d_num = pd->sp_un;        /* 6=KNOTS  7=M/S  */
	strcpy(ext->e_str, units[pd->sp_un]);
		/* initialise conversion variables */
	sp_dst = (double)pd->sp_dst;  /* coerce float to double */
			/* start doing the set up */
   W1 = vcreat (19, 74, NORML, NO);
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
	Wmsg = vcreat (1, 80, REVNORML, NO);
	vlocate (Wmsg, 24, 0);
	vratputs (Wmsg, 0, 1, REVNORML, "<ESC> Exit");
	vratputs (Wmsg, 0, 36, REVERR, "  F1  ");
	vratputs (Wmsg, 0, 60, REVEMPHNORML, "<CTRL-ENTER> Accept");
	vlocate (W1, 2, 3);
	vframe (W1, EMPHNORML, FRDOUBLE);
	vtitle (W1, EMPHNORML, " Project Definition ");
      /*# defldval_pro() detranval_pro() EscapeKey() pro_help() */
   tp = vpreload (34, 3, NULLF, defldval_pro, detranval_pro, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
   tp->helpfn = pro_help;
	tp->inp_idle = UpdateClockWindow;
	fld = *(tp->def);
   for (i; i<34 ; i++)
		fld[i].use_ext = TRUE;   /* use/display the preload values */
			/* string fields */
   vdefstr (tp, 0, 1, 1, "Company", 17, 30, NULL, pd->cli_name, YES);
   vdefstr (tp, 1, 2, 1, "Project", 17, 30, NULL, pd->pro_name, YES);
   vdefstr (tp, 2, 3, 1, "Contractor", 17, 30, NULL, pd->con_name, YES);
   vdefstr (tp, 3, 4, 1, "Vessel", 17, 30, NULL, pd->qc_com1, YES);
   vdefstr (tp, 4, 5, 1, "Client Rep", 17, 30, NULL, pd->qc_com2, YES);
   vdefstr (tp, 5, 6, 1, "Seismic", 17, 30, NULL, pd->qc_com3, YES);
   vdefstr (tp, 6, 7, 1, "Navigation", 17, 30, NULL, pd->qc_com4, YES);
         /* unsigned integer fields */
	vdefuint (tp, 7, 8, 1, "Vessels", 17, 0, "9", &pd->vsl_numb, YES);
	vdefuint (tp, 8, 9, 1, "Local Cables", 17, 0, "9", &pd->loc_strm, YES);
	vdefuint (tp, 9, 10, 1, "Remote Cables", 17, 0, "9", &pd->rem_strm, YES);
	vdefuint (tp, 10, 11, 1, "Local Sources", 17, 0, "9", &pd->loc_srce, YES);
	vdefuint (tp, 11, 12, 1, "Remote Sources", 17, 0, "9", &pd->rem_srce, YES);
   vdefuint (tp, 12, 13, 1, "Gun Seq Length", 17, 0, "9", &pd->seq_len, YES);
         /* string fields for popup items */
   vdefstr (tp, 13, 14, 1, "Depth Units", 17, 10, NULL, ext->b_str, YES);
   vdefstr (tp, 14, 15, 1, "Pressure Units", 17, 10, NULL, ext->c_str, YES);
   vdefstr (tp, 15, 16, 1, "Volume Units", 17, 10, NULL, ext->d_str, YES);
   vdefstr (tp, 16, 17, 1, "Speed Units", 17, 10, NULL, ext->e_str, YES);
         /* double field */
   vdefdbl (tp, 17, 18, 1, "Shot Interval", 17, 0, "999.99", &sp_dst, YES);
         /* source string fields */
   vdefstr (tp, 18, 1, 53, "Loc 1", 60, 10, NULL, pd->loc1, YES);
   vdefstr (tp, 19, 2, 53, "Loc 2", 60, 10, NULL, pd->loc2, YES);
   vdefstr (tp, 20, 3, 53, "Loc 3", 60, 10, NULL, pd->loc3, YES);
   vdefstr (tp, 21, 4, 53, "Loc 4", 60, 10, NULL, pd->loc4, YES);
   vdefstr (tp, 22, 5, 53, "Rem 1", 60, 10, NULL, pd->rem1, YES);
   vdefstr (tp, 23, 6, 53, "Rem 2", 60, 10, NULL, pd->rem2, YES);
   vdefstr (tp, 24, 7, 53, "Rem 3", 60, 10, NULL, pd->rem3, YES);
   vdefstr (tp, 25, 8, 53, "Rem 4", 60, 10, NULL, pd->rem4, YES);
         /* cable string fields */
   vdefstr (tp, 26, 11, 53, "Loc 1", 60, 10, NULL, pd->locab1, YES);
   vdefstr (tp, 27, 12, 53, "Loc 2", 60, 10, NULL, pd->locab2, YES);
   vdefstr (tp, 28, 13, 53, "Loc 3", 60, 10, NULL, pd->locab3, YES);
   vdefstr (tp, 29, 14, 53, "Loc 4", 60, 10, NULL, pd->locab4, YES);
   vdefstr (tp, 30, 15, 53, "Rem 1", 60, 10, NULL, pd->remcab1, YES);
   vdefstr (tp, 31, 16, 53, "Rem 2", 60, 10, NULL, pd->remcab2, YES);
   vdefstr (tp, 32, 17, 53, "Rem 3", 60, 10, NULL, pd->remcab3, YES);
   vdefstr (tp, 33, 18, 53, "Rem 4", 60, 10, NULL, pd->remcab4, YES);
			/* comment fields */
   vdefatsa (tp, 0, 18, 25, NORML, "Metres");
   vdefatsa (tp, 1, 0, 54, NORML, "Source Names");
   vdefatsa (tp, 2, 10, 54, NORML, "Cable Names");
         /* setting fields */
      /*# defldset_pro() */
	deSetFieldSetfn(tp, defldset_pro);
	visible (Wmsg, YES, NO);
	visible (W1, YES, NO);
			/* DO IT */
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
			/* if all OK - make the changes */
		pd->sp_dst = (float) sp_dst;  /* coerce double to float */
		pd->loc_dp = ext->a_num;
		pd->pr_un = ext->b_num;
		pd->vol_un = ext->c_num;
      pd->sp_un = ext->d_num;        /* 6=KNOTS  7=M/S  */
	}
	vdelete (W1, NONE);
	vdelete (Wmsg, NONE);
	return;
}


/*************************************************************
*   project help function
**************************************************************/
void pro_help()
{
   string_help("Project");
}


/**************************************************************
*   set up the fixed menu selection for the unit fields
**************************************************************/
defldset_pro (TRANSACTION *tp)
{
	int sel;                   /* format number */
	ext->ee = ext->ff = TRUE;            /* set the bool flags */
   if (((tp->cur_fld > 12) && (tp->cur_fld < 17)) && (tp->thiskey !=ESC)) {
      if(tp->cur_fld == 13) ext->e_num = 0;
      if(tp->cur_fld == 14) ext->e_num = 1;
      if(tp->cur_fld == 15) ext->e_num = 2;
      if(tp->cur_fld == 16) ext->e_num = 3;
		if((sel = select_unit()) != -1) {   /* get a unit */
         if(tp->cur_fld == 13) ext->a_num = sel;  /* temporary unit number */
         if(tp->cur_fld == 14) ext->b_num = sel;
         if(tp->cur_fld == 15) ext->c_num = sel;
         if(tp->cur_fld == 16) ext->d_num = sel;
         ext->s_input = units[sel];   /* keysim() has to use ext->s_input */
            /*# KeySim() */
			tp->inp_get = KeySim;    /* do the actual keysim */
		} else {                    /* do only nextfield jump */
			ext->ee = FALSE;              /* see KeySim() function */
			tp->inp_get = KeySim;
		}
	}
	return (TRUE);
}


/**************************************************************
*   offer up a units list to select
**************************************************************/
select_unit()
{
	LISTITEM *l;
	register int i;
	static char *unlist[] = {"DEPTH", "PRESS", "VOL", "SPEED",""};
	char *s;
   l = initlist();
	if(!ext->e_num) i=0;
		else if(ext->e_num == 1) i=2;
			else if(ext->e_num == 2) i=4;
			  else i=6;
	adtolist (l, units[i++]);
	adtolist (l, units[i]);
   s = listsel (14, 35, 15, NORML, REVNORML, unlist[ext->e_num], HIGHNORML,
						 FRDOUBLE, NORML, l, UpdateClockWindow);
	freelist (l, 0);
	i = 0;
	while (units[i]) {
	  if(!strcmp(units[i], s)) return (i);
	  i++;
	}
	return (-1);
}


/*************************************************************
* field validation
**************************************************************/
defldval_pro (TRANSACTION *tp)    /* field validation */
{
	register int sum;
	if(tp->cur_fld == 7) {
		sum = (int) *(*(tp->def))[7].dataptr;
		if((sum < 1)||(sum > 2)) {
			StatsErr("26003");
			return(FALSE);
		}
	}

	return(TRUE);
}

/*************************************************************
* transaction validation
**************************************************************/
detranval_pro (TRANSACTION *tp)
{
	register int i, sum;
	int loc, rem, seq;
   for(i=8;i<12;i++) { /* no more than 4 local or remote cables or sources each */
		sum = (int) *(*(tp->def))[i].dataptr;
		if(sum > 4) {
			StatsErr("26004");
			tp->cur_fld = 8;
			return(FALSE);
		}
	}
	loc = (int) *(*(tp->def))[10].dataptr;
	rem = (int) *(*(tp->def))[11].dataptr;
	seq = (int) *(*(tp->def))[12].dataptr;
   if(seq != (loc + rem)) {  /* fire seq must equal local + remote sources */
		StatsErr("26005");
		return(FALSE);
	}
   if(1 == ((int) *(*(tp->def))[7].dataptr)) { /* if 1 bt no remotes !! */
		if(( ((int) *(*(tp->def))[9].dataptr) != 0) || ( ((int) *(*(tp->def))[11].dataptr) != 0)) {
			StatsErr("26006");
			return(FALSE);
		}
	}
	if(KeySure()) return(TRUE);
	else return(FALSE);
}
