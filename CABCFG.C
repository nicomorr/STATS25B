/*************************************************************
*   CABCFG.C   cable configuration file
****************************************************************
*
*	Compiler:  MSC V6.00  /AL /W4 /Os /Gs /Zp /Zl /G2 /ST:3072
*  Linker:    MSC V5.10  /NOE /SE:256 /NOD /EXEPACK
*  Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWL.LIB
*             Greenleaf Superfunctions (MSC) V1.00 GSFL.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*	(c) West Country Systems Ltd, Cyprus, 1992
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
extern struct HDR_DEF *hd; 		  /* pointer to header list */
extern struct PRO_DEF *pd;         /* pointer to project list */
extern struct CAB_DEF *stcab; 	  /* pointer to start of cable list */
extern struct CAB_DEF *lacab; 	  /* pointer to last	of cable list */
extern struct EXT_DEF *ext;
extern char *formats[]; 			  /* formats list */


/**************************************************************
*   view cable configuration
**************************************************************/
void view_cab(int n, unsigned fmt, unsigned cab_max)
{
	HWND W1, W2;
	unsigned r;
	register int i = 0, p;
	struct CAB_DEF *info;
	info = stcab;
	if (!n) p = 1;
		else p = n;
	W1 = vcreat (p, 68, NORML, NO);
	W2 = vcreat (1, 65, NORML, NO);
	if (n > 18) p = 18;
		else p = n;
	vwind (W1, p, 67, 0, 0);
	vlocate (W1, 3, 6);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " Cable Depths ");
	vlocate (W2, 4+p, 7);
	vatputf (W2, 0, 0, "RUs: %-3d", n);
	if (hd->contr == 0)	 /* if GECO */
		vatputf (W2, 0, 13, "Fmt: %-15s", formats[fmt]);
	if (hd->contr == 1)	 /* if PRAKLA */
      vatputs (W2, 0, 13, "Fmt: SYN V2.3");
	if (hd->contr == 2)	 /* if WESTERN */
		vatputf (W2, 0, 13, "Fmt: WESTERN %u", hd->west + 1);
   if (hd->contr == 5)   /* if DIGICON TRACE0 */
      vatputs (W2, 0, 13, "Fmt: TRACE0");
	vatputf (W2, 0, 36, "Max: %-3u", cab_max);
	if (n > 18) vratputs(W2, 0, 51, HIGHNORML, "Arrows Up/Down");
	if (!info) {
		vratputs (W1, 0, 20, REVERR, "NO CABLE DEPTHS DEFINED YET");
		vbeep();
	}
	while (info) {
		vatputf(W1, i, 1, "    RU Number: %-2d        RU Name: %-10s        Cable: %-1d",
				 info->cab_num, info->cab_name, info->cable);
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
*   add a remote unit to cable configuration
**************************************************************/
add_cab(int n, unsigned cab_max)
{
	struct CAB_DEF *info;
	HWND W1;           /* data entry window */
	TRANSACTION *tp;   /* transaction structure */
	FIELD *fld;        /* field structure */
	info = (struct CAB_DEF *) malloc(sizeof(struct CAB_DEF));
	if (!info) {
		StatsErr("20014");
		return(n);
	}
	if (n >= (int) cab_max) {
		StatsErr("20001");
		free(info);
		return (n);
	}
   W1 = vcreat (6, 35, NORML, NO);
   vlocate (W1, 3, 23);
	vframe (W1, NORML, FRDOUBLE);
   vtitle (W1, NORML, " Add RU ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);	 /* add 3D effect */
      /*# defldval_cab() cab_help() detranval_glob() EscapeKey() */
   tp = vpreload (3, 2, NULLF, defldval_cab, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   tp->helpfn = cab_help;
	fld = *(tp->def);
	vdefstr (tp, 0, 1, 1, "RU Name", 18, 10, NULL, ext->b_str, YES);
	vdefuint (tp, 1, 2, 1, "RU Number", 18, 4, "99", &ext->a_num, YES);
	vdefuint (tp, 2, 3, 1, "Cable Number", 18, 4, "9", &ext->b_num, YES);
   vdefatsa (tp, 0, 5, 1, NORML, "<ESC> Exit");
   vdefatsa (tp, 1, 5, 15, HIGHNORML, "<Ctrl-Enter> Accept");
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
		strcpy(info->cab_name, ext->b_str);
		info->cab_num = ext->a_num;
		info->cable = ext->b_num;
		info->tot_err = 0;
		info->tot_sp1 = 0;
		info->tot_sp2 = 0;
		info->min_dep = 1000.0F;
		info->max_dep = 0.0F;
		info->per_c = 0.0F;
		cab_store(info, &stcab, &lacab);	 /* double linked list */
		++n;
	}
	vdelete(W1, NONE);
	return(n);
}


/**************************************************************
*   delete a remote unit from cable configuration
**************************************************************/
del_cab(int n)
{
	struct CAB_DEF *info;
	HWND W1;           /* data entry window */
	TRANSACTION *tp;   /* transaction structure */
	FIELD *fld;        /* field structure */
	if (n<1) {
		StatsErr("20002");
		return (n);
	}
   W1 = vcreat (4, 35, NORML, NO);
   vlocate (W1, 3, 23);
	vframe (W1, NORML, FRDOUBLE);
   vtitle (W1, NORML, " Delete RU ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);	 /* add 3D effect */
      /*# defldval_cabdel() detranval_glob() cab_help() EscapeKey() */
   tp = vpreload (1, 2, NULLF, defldval_cabdel, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   tp->helpfn = cab_help;
	fld = *(tp->def);
	vdefstr (tp, 0, 1, 1, "RU Name", 18, 10, NULL, ext->b_str, YES);
   vdefatsa (tp, 0, 3, 3, NORML, "<ESC> Exit");
   vdefatsa (tp, 1, 3, 21, HIGHNORML, "<CR> Accept");
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
		info = stcab;
		while(info) {
			if (!strcmp(ext->b_str, info->cab_name))
				break;	  /* found */
			info = info->next;
		}
		if(info->prior) info->prior->next = info->next;
		else {	/* first new item */
		stcab = info->next;
		if(stcab) stcab->prior = '\0';
		}
		if(info->next) info->next->prior = info->prior;
		else	/* deleting last element */
			lacab = info->prior;
		free(info);
		--n;
	}
	vdelete(W1, NONE);
	return(n);
}


/**************************************************************
*   global cable help window
**************************************************************/
void cab_help()
{
   string_help("Cable Depth");
}


/**************************************************************
*   delete RU field validation function
**************************************************************/
defldval_cabdel ()
{
	if(!findcab(ext->b_str)) {
		StatsErr("20003");
		return(FALSE);
	}
	return(TRUE);
}


/**************************************************************
*   find a remote unit name for deletion
**************************************************************/
struct CAB_DEF *findcab(char *name)
{
	struct CAB_DEF *info;
	info = stcab;
	while(info) {
		if (!strcmp(name, info->cab_name)) {
			return (info);
		}
		info = info->next;
	}
	return (NULL);  /* not found */
}


/**************************************************************
*   save cable configuration to disk
**************************************************************/
void save_cab(int n)
{
	struct CAB_DEF *info;
	FILE *fp;
	if (!n) {
		StatsErr ("20004");
		return;
	}
	if (!KeySure()) return;
	fp = fopen("CABLE.CFG", "wb");
	info = stcab;
	while(info) {
		fwrite(info, sizeof(struct CAB_DEF), 1, fp);
		info = info->next;
	}
	fclose(fp);
   StatsMessage("SAVED CABLE.CFG");
}


/**************************************************************
*   load cable configuration from disk
**************************************************************/
load_cab(int i)
{
	int n = 0;					/* reset the RU counter */
	struct CAB_DEF *info;
   FILE *fp;
   if(i)
      if (!KeySure())
          return(ext->numberru);
	if(stcab) {
		info = stcab;
		while(info) {
			free(info);
			info = info->next;
		}
		stcab = lacab = NULL;
	}
	if((fp = fopen("CABLE.CFG", "rb")) == NULL) {
	  if(i) StatsErr("20005");
	  return(0);
	}
	while(!feof(fp)) {
		info = (struct CAB_DEF *) malloc(sizeof(struct CAB_DEF));
		if(!info) {
			StatsErr("20006");
			fclose(fp);
			return(0);
		}
		if (1 != fread(info, sizeof(struct CAB_DEF), 1, fp))
			break;
		cab_store(info, &stcab, &lacab);
		++n;
	}
   fclose(fp);
   if(i) StatsMessage("LOADED CABLE.CFG");
	return(n);
}


/***************************************************************
*   create a doubly linked list in sorted order (by cab_num)
*   uses integer sorting - not for arrays!
***************************************************************/
void cab_store(
	struct CAB_DEF *i,        /* new element */
	struct CAB_DEF **stcab,   /* first element in list */
	struct CAB_DEF **lacab)   /* last element in list */
{
	struct CAB_DEF *old, *p;
	if (*lacab == NULL) {     /* if entry is first to go in list */
		i->next = NULL;
		i->prior = NULL;
		*lacab = i;
		*stcab = i;
		return;
	}
	p = *stcab;   /* start at top of list */
	old = NULL;
	while(p) {
		if (p->cab_num < i->cab_num) {     /* insert format selection here */
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
			*stcab = i;
			return;
		}
	}
	old->next = i;       /* put on end */
	i->next = NULL;
	i->prior = old;
	*lacab = i;
}



/**************************************************************
*   add RU field validation function
**************************************************************/
defldval_cab (TRANSACTION *tp)
{
	struct CAB_DEF *info;
	info = stcab;
	if(pd->loc_strm==0) {
		StatsErr("20007");	 /* no local streamers in project def */
		tp->tra_abort=TRUE;
		return(TRUE);
	}
	if(tp->cur_fld == 0) {   /* RU name validation */
		if(findcab(ext->b_str)) {
			StatsErr("20008");
			return(FALSE);
		}
		if(!strlen(ext->b_str)) {
			StatsErr("20009");
			return(FALSE);
		}
	}
	if(((tp->cur_fld==1)&&(ext->a_num==0))||((tp->cur_fld==2)&&(ext->b_num==0))||
				  ((tp->cur_fld==3)&&(ext->c_num==0))) {
			StatsErr("20010");
			return(FALSE);
	}
	if(tp->cur_fld == 1) {   /* RU number validation */
		while(info) {
			if(ext->a_num == info->cab_num) {
				StatsErr("20011");
				return(FALSE);
			}
			info = info->next;
		}
		if(ext->a_num > hd->ca_num) {
			StatsErr("20012");
			return(FALSE);
		}
	}
	if(tp->cur_fld == 2) {   /* cable number validation */
		if(ext->b_num > pd->loc_strm) {
			StatsErr("20013");
			return(FALSE);
		}
	}
	return (TRUE);
}
