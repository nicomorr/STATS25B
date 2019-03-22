/*************************************************************
*   GUNDPCFG.C   gun depth configuration file for stats
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

#include <dw.h>            /* greenleaf */
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct HDR_DEF *hd;          /* pointer to header list */
extern struct PRO_DEF *pd;          /* pointer to project list */
extern struct EXT_DEF *ext;         /* ptr to external variables */
extern struct GUNDP_DEF *stgundp;   /* pointer to start of gundep list */
extern struct GUNDP_DEF *lagundp;   /* pointer to last of gundep list */
extern char *formats[];             /* formats list */


/**************************************************************
*   view gundepth configuration
**************************************************************/
void view_gundp(int n, unsigned fmt, unsigned gundp_max)
{
	HWND W1, W2;
	unsigned r;
	register int i = 0, p;
	struct GUNDP_DEF *info;
	info = stgundp;
	if (!n) p = 1;
		else p = n;
	W1 = vcreat (p, 68, NORML, NO);
	W2 = vcreat (1, 65, NORML, NO);
	if (n > 18) p = 18;
		else p = n;
	vwind (W1, p, 67, 0, 0);
	vlocate (W1, 3, 6);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " Gun Depths ");
	vwind (W2, 1, 60, NORML, NO);
	vlocate (W2, 4+p, 7);
	vatputf (W2, 0, 0, "DUs: %-3d", n);
	if (hd->contr == 0)	 /* if GECO */
		vatputf (W2, 0, 13, "Fmt: %-15s", formats[fmt]);
	if (hd->contr == 1)	 /* if PRAKLA */
		vatputs (W2, 0, 13, "Fmt: SYN V2.00");
	if (hd->contr == 2)	 /* if WESTERN */
		vatputf (W2, 0, 13, "Fmt: WESTERN %u", hd->west + 1);
   if (hd->contr == 5)   /* if DIGICON TRACE0 */
      vatputs (W2, 0, 13, "Fmt: TRACE0");
	vatputf (W2, 0, 36, "Max: %-3u", gundp_max);
	if (n > 18) vratputs(W2, 0, 51, HIGHNORML, "Arrows Up/Down");
	if (!info) {
		vratputs (W1, 0, 22, REVERR, "NO GUNDEPTHS DEFINED YET");
		vbeep();
	}
	while (info) {
		vatputf(W1, i, 1,
			"    DU Number: %-2d        DU Name: %-10s       Source: %-1d",
				 info->gundp_num, info->gundp_name, info->srce);
		info = info->next;
		i++;
	}
	visible (W1, YES, YES);
	if (n) visible (W2, YES, YES);
	i=0;
	do {
		r = vgetkey(UpdateClockWindow);
		switch (r) {
			case CURDN:
				if (i >= n - p ) vbeep();
				if (i < n - p)
					vloc (W1, ++i, 0);
				break;
			case CURUP:
				if (i == 0) vbeep();
				if (i > 0)
					vloc (W1, --i, 0);
					break;
		}
	} while ((r != ESC) && (r != CR));
	vdelete (W2, NONE);
	vdelete (W1, NONE);
}


/**************************************************************
*   add a DU to configuration
**************************************************************/
add_gundp(int n, unsigned gundp_max)
{
	struct GUNDP_DEF *info;
	HWND W1;           /* data entry window */
	TRANSACTION *tp;   /* transaction structure */
	FIELD *fld;        /* field structure */
	info = (struct GUNDP_DEF *) malloc(sizeof(struct GUNDP_DEF));
	if (!info) {
		StatsErr("21001");
		return(n);
	}
	if (n >= (int) gundp_max) {
		StatsErr("21002");
		free(info);
		return (n);
	}
   W1 = vcreat (6, 35, NORML, NO);
   vlocate (W1, 3, 23);
	vframe (W1, NORML, FRDOUBLE);
   vtitle (W1, NORML, " Add DU ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
      /*# defldval_gundp() detranval_glob() EscapeKey() gundp_help() */
   tp = vpreload (3, 2, NULLF, defldval_gundp, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   tp->helpfn = gundp_help;
	fld = *(tp->def);
	vdefstr (tp, 0, 1, 1, "DU Name", 18, 10, NULL, ext->b_str, YES);
	vdefuint (tp, 1, 2, 1, "DU Number", 18, 4, "99", &ext->a_num, YES);
	vdefuint (tp, 2, 3, 1, "Source Number", 18, 4, "9", &ext->b_num, YES);
   vdefatsa (tp, 0, 5, 1, NORML, "<ESC> Exit");
   vdefatsa (tp, 1, 5, 15, HIGHNORML, "<Ctrl-Enter> Accept");
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
		strcpy(info->gundp_name, ext->b_str);
		info->gundp_num = ext->a_num;
		info->srce = ext->b_num;
		info->tot_sp = 0;
		info->min_dep = 1000.0F;
		info->max_dep = 0.0F;
		gundp_store(info, &stgundp, &lagundp);	 /* double linked list */
		++n;
	}
	vdelete(W1, NONE);
	return(n);
}


/**************************************************************
*   delete a DU from configuration
**************************************************************/
del_gundp(int n)
{
	struct GUNDP_DEF *info;
	HWND W1;           /* data entry window */
	TRANSACTION *tp;   /* transaction structure */
	FIELD *fld;        /* field structure */
	if (n<1) {
		StatsErr("21003");
		return (n);
	}
   W1 = vcreat (4, 35, NORML, NO);
   vlocate (W1, 3, 23);
	vframe (W1, NORML, FRDOUBLE);
   vtitle (W1, NORML, " Delete DU ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
      /*# defldval_gundpdel() detranval_glob() EscapeKey() gundp_help() */
   tp = vpreload (1, 2, NULLF, defldval_gundpdel, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   tp->helpfn = gundp_help;
	fld = *(tp->def);
	vdefstr (tp, 0, 1, 1, "DU Name", 18, 10, NULL, ext->b_str, YES);
   vdefatsa (tp, 0, 3, 3, NORML, "<ESC> Exit");
   vdefatsa (tp, 1, 3, 21, HIGHNORML, "<CR> Accept");
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
		info = stgundp;
		while(info) {
			if (!strcmp(ext->b_str, info->gundp_name))
				break;	  /* found */
			info = info->next;
		}
		if(info->prior) info->prior->next = info->next;
		else {	/* first new item */
		stgundp = info->next;
		if(stgundp) stgundp->prior = '\0';
		}
		if(info->next) info->next->prior = info->prior;
		else	/* deleting last element */
			lagundp = info->prior;
		free(info);
		--n;
	}
	vdelete(W1, NONE);
	return(n);
}

/**************************************************************
*   global gun depth help window
**************************************************************/
void gundp_help()
{
   string_help("Gun Depth");
}



/**************************************************************
*   delete DU field validation function
**************************************************************/
defldval_gundpdel ()
{
	if(!findgundp(ext->b_str)) {
		StatsErr("21004");
		return(FALSE);
	}
	return(TRUE);
}


/**************************************************************
*   find a DU name for deletion
**************************************************************/
struct GUNDP_DEF *findgundp(char *name)
{
	struct GUNDP_DEF *info;
	info = stgundp;
	while(info) {
		if (!strcmp(name, info->gundp_name)) {
			return (info);
		}
		info = info->next;
	}
	return (NULL);  /* not found */
}


/**************************************************************
*   save gun depth configuration to disk
**************************************************************/
void save_gundp(int n)
{
	struct GUNDP_DEF *info;
	FILE *fp;
	if (!n) {
		StatsErr("21005");
		return;
	}
	if (!KeySure()) return;
	fp = fopen("GUNDEPTH.CFG", "wb");
	info = stgundp;
	while(info) {
		fwrite(info, sizeof(struct GUNDP_DEF), 1, fp);
		info = info->next;
	}
	fclose(fp);
   StatsMessage("SAVED GUNDEPTH.CFG");
}


/**************************************************************
*   load gun depth configuration from disk
**************************************************************/
load_gundp(int i)
{
	int n = 0;					/* reset the DU counter */
	struct GUNDP_DEF *info;
   FILE *fp;
   if(i)
      if (!KeySure())
         return(ext->numberdp);
	if(stgundp) {
		info = stgundp;
		while(info) {
			free(info);
			info = info->next;
		}
		stgundp = lagundp = NULL;
	}
	if((fp = fopen("GUNDEPTH.CFG", "rb")) == NULL) {
	  if(i) StatsErr("21006");
	  return(0);
	}
	while(!feof(fp)) {
		info = (struct GUNDP_DEF *) malloc(sizeof(struct GUNDP_DEF));
		if(!info) {
			StatsErr("21007");
			fclose(fp);
			return(0);
		}
		if (1 != fread(info, sizeof(struct GUNDP_DEF), 1, fp))
			break;
		gundp_store(info, &stgundp, &lagundp);
		++n;
	}
   fclose(fp);
   if(i) StatsMessage("LOADED GUNDEPTH.CFG");
	return(n);
}


/***************************************************************
*   create a doubly linked list in sorted order (by gundp_num)
*   uses integer sorting - not for arrays!
***************************************************************/
void gundp_store(
	struct GUNDP_DEF *i,         /* new element */
	struct GUNDP_DEF **stgundp,   /* first element in list */
	struct GUNDP_DEF **lagundp)	 /* last element in list */
{
	struct GUNDP_DEF *old, *p;
	if (*lagundp == NULL) {   /* if entry is first to go in list */
		i->next = NULL;
		i->prior = NULL;
		*lagundp = i;
		*stgundp = i;
		return;
	}
	p = *stgundp;   /* start at top of list */
	old = NULL;
	while(p) {
		if (p->gundp_num < i->gundp_num) {  /* insert format selection here */
			old = p;
			p = p->next;
		}
		else {
			if (p->prior) {
				p->prior->next = i;
				i->next = p;
				i->prior = p->prior;
				p->prior = i;
				return;
			}
			i->next = p;   /* first new element */
			i->prior = NULL;
			p->prior = i;
			*stgundp = i;
			return;
		}
	}
	old->next = i;       /* put on end */
	i->next = NULL;
	i->prior = old;
	*lagundp = i;
}


/**************************************************************
*   add DU field validation function
**************************************************************/
defldval_gundp (TRANSACTION *tp)
{
	struct GUNDP_DEF *info;
	info = stgundp;
	if(pd->loc_srce==0) {
		StatsErr("21008");
		tp->tra_abort=TRUE;
		return(TRUE);
	}
	if(tp->cur_fld == 0) {   /* DU name validation */
		if(findgundp(ext->b_str)) {
			StatsErr("21009");
			return(FALSE);
		}
		if(!strlen(ext->b_str)) {
			StatsErr("21010");
			return(FALSE);
		}
	}
	if(((tp->cur_fld==1)&&(ext->a_num==0))||((tp->cur_fld==2)&&(ext->b_num==0))||
				  ((tp->cur_fld==3)&&(ext->c_num==0))) {
			StatsErr("21011");
			return(FALSE);
	}
	if(tp->cur_fld == 1) {   /* DU number validation */
		while(info) {
			if(ext->a_num == info->gundp_num) {
				StatsErr("21012");
				return(FALSE);
			}
			info = info->next;
		}
		if(ext->a_num > hd->gd_num) {
			StatsErr("21013");
			return(FALSE);
		}
	}
	if(tp->cur_fld == 2) {   /* source number validation */
		if(ext->b_num > pd->loc_srce) {
			StatsErr("21014");
			return(FALSE);
		}
	}
	return (TRUE);
}
