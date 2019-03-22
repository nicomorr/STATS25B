/*************************************************************
*   WINGCFG.C   wing angles configuration file
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
extern struct HDR_DEF *hd; 		  /* pointer to header list */
extern struct PRO_DEF *pd;         /* pointer to project list */
extern struct WING_DEF *stwi;      /* wing angles first */
extern struct WING_DEF *lawi;      /* wing angles last */
extern struct EXT_DEF *ext;        /* external variables */
extern char *formats[]; 			  /* formats list */


/**************************************************************
*   view wing angle configuration
**************************************************************/
void view_wing(int n, unsigned fmt, unsigned wing_max)
{
	HWND W1, W2;
	unsigned r;
	register int i = 0, p;
   struct WING_DEF *info;
   if(hd->contr) {
      StatsErr("25015");   /* only GECO logs wing angles */
      return;
   }
   info = stwi;
	if (!n) p = 1;
		else p = n;
	W1 = vcreat (p, 68, NORML, NO);
	W2 = vcreat (1, 65, NORML, NO);
	if (n > 18) p = 18;
		else p = n;
	vwind (W1, p, 67, 0, 0);
	vlocate (W1, 3, 6);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " Wing Angles ");
	vlocate (W2, 4+p, 7);
   vatputf (W2, 0, 0, "WUs: %-3d", n);
      /* only GECO logs wing angles at present */
   vatputf (W2, 0, 13, "Fmt: %-15s", formats[fmt]);
   vatputf (W2, 0, 36, "Max: %-3u", wing_max);
	if (n > 18) vratputs(W2, 0, 51, HIGHNORML, "Arrows Up/Down");
	if (!info) {
      vratputs (W1, 0, 20, REVERR, "NO WING ANGLES DEFINED YET");
		vbeep();
	}
	while (info) {
      vatputf(W1, i, 1, "    WU Number: %-2d        WU Name: %-10s        Cable: %-1d",
             info->num, info->name, info->cable);
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
add_wing(int n, unsigned wing_max)
{
   struct WING_DEF *info;
	HWND W1;           /* data entry window */
	TRANSACTION *tp;   /* transaction structure */
   FIELD *fld;        /* field structure */
   if(hd->contr) {
      StatsErr("25015");   /* only GECO logs wing angles */
      return(0);
   }
   info = (struct WING_DEF *) malloc(sizeof(struct WING_DEF));
	if (!info) {
      StatsErr("25014");
		return(n);
	}
   if (n >= (int) wing_max) {
      StatsErr("25001");
		free(info);
		return (n);
	}
   W1 = vcreat (6, 35, NORML, NO);
   vlocate (W1, 3, 23);
	vframe (W1, NORML, FRDOUBLE);
   vtitle (W1, NORML, " Add WU ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
      /*# defldval_wing() detranval_glob() EscapeKey() wing_help() */
   tp = vpreload (3, 2, NULLF, defldval_wing, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   tp->helpfn = wing_help;      /* ignore param warning */
	fld = *(tp->def);
   vdefstr (tp, 0, 1, 1, "WU Name", 18, 10, NULL, ext->b_str, YES);
   vdefuint (tp, 1, 2, 1, "WU Number", 18, 4, "99", &ext->a_num, YES);
	vdefuint (tp, 2, 3, 1, "Cable Number", 18, 4, "9", &ext->b_num, YES);
   vdefatsa (tp, 0, 5, 1, NORML, "<ESC> Exit");
   vdefatsa (tp, 1, 5, 15, HIGHNORML, "<Ctrl-Enter> Accept");
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
      strcpy(info->name, ext->b_str);
      info->num = ext->a_num;
		info->cable = ext->b_num;
      info->spec = 0;
      info->on = 0;
      info->mean  = 0.0F;
      info->min = 20.0F;
      info->max = -20.0F;
      info->change = 0.0F;
      info->last = 0.0F;
      wing_store(info, &stwi, &lawi);  /* double linked list */
		++n;
	}
	vdelete(W1, NONE);
	return(n);
}


/**************************************************************
*   delete a remote unit from cable configuration
**************************************************************/
del_wing(int n)
{
   struct WING_DEF *info;
	HWND W1;           /* data entry window */
	TRANSACTION *tp;   /* transaction structure */
	FIELD *fld;        /* field structure */
	if (n<1) {
      StatsErr("25002");
		return (n);
	}
   W1 = vcreat (4, 35, NORML, NO);
   vlocate (W1, 3, 23);
	vframe (W1, NORML, FRDOUBLE);
   vtitle (W1, NORML, " Delete WU ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
      /*# defldval_wingdel() detranval_glob() EscapeKey() wing_help() */
   tp = vpreload (1, 2, NULLF, defldval_wingdel, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   tp->helpfn = wing_help;      /* ignore param warning */
	fld = *(tp->def);
   vdefstr (tp, 0, 1, 1, "WU Name", 18, 10, NULL, ext->b_str, YES);
   vdefatsa (tp, 0, 3, 3, NORML, "<ESC> Exit");
   vdefatsa (tp, 1, 3, 21, HIGHNORML, "<CR> Accept");
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
      info = stwi;
		while(info) {
         if (!strcmp(ext->b_str, info->name))
				break;	  /* found */
			info = info->next;
		}
		if(info->prior) info->prior->next = info->next;
		else {	/* first new item */
      stwi = info->next;
      if(stwi) stwi->prior = '\0';
		}
		if(info->next) info->next->prior = info->prior;
		else	/* deleting last element */
         lawi = info->prior;
		free(info);
		--n;
	}
	vdelete(W1, NONE);
	return(n);
}


/**************************************************************
*   global wing angle help window
**************************************************************/
void wing_help()
{
   string_help("Wing Angle");
}


/**************************************************************
*   delete WU field validation function
**************************************************************/
defldval_wingdel ()
{
   if(!findwing(ext->b_str)) {
      StatsErr("25003");
		return(FALSE);
	}
	return(TRUE);
}


/**************************************************************
*   find a remote unit name for deletion
**************************************************************/
struct WING_DEF *findwing(char *name)
{
   struct WING_DEF *info;
   info = stwi;
	while(info) {
      if (!strcmp(name, info->name)) {
			return (info);
		}
		info = info->next;
	}
	return (NULL);  /* not found */
}


/**************************************************************
*   save wing configuration to disk
**************************************************************/
void save_wing(int n)
{
   struct WING_DEF *info;
	FILE *fp;
   if(hd->contr) {
      StatsErr("25015");   /* only GECO logs wing angles */
      return;
   }
	if (!n) {
      StatsErr ("25004");
		return;
	}
	if (!KeySure()) return;
   fp = fopen("WINGS.CFG", "wb");
   info = stwi;
	while(info) {
      fwrite(info, sizeof(struct WING_DEF), 1, fp);
		info = info->next;
	}
	fclose(fp);
   StatsMessage("Saved WINGS.CFG - OK");
}


/**************************************************************
*   load wing configuration from disk
*      parameter i stops error messages & keysure on autoload
*      at program start
**************************************************************/
load_wing(int i)
{
   int n = 0;              /* reset the WU counter */
   struct WING_DEF *info;
   FILE *fp;
   if(hd->contr && i) {
      StatsErr("25015");   /* only GECO logs wing angles */
      return(n);
   }
   if(i)
      if (!KeySure())
        return(ext->numberwing);
   if(stwi) {
      info = stwi;
		while(info) {
			free(info);
			info = info->next;
		}
      stwi = lawi = NULL;
	}
   if((fp = fopen("WINGS.CFG", "rb")) == NULL) {
     if(i) StatsErr("25005");
	  return(0);
	}
	while(!feof(fp)) {
      info = (struct WING_DEF *) malloc(sizeof(struct WING_DEF));
		if(!info) {
         StatsErr("25006");
			fclose(fp);
			return(0);
		}
      if (1 != fread(info, sizeof(struct WING_DEF), 1, fp))
			break;
      wing_store(info, &stwi, &lawi);
		++n;
	}
   fclose(fp);
   if(i) StatsMessage("LOADED WINGS.CFG");
	return(n);
}


/***************************************************************
*   create a doubly linked list in sorted order (by cab_num)
*   uses integer sorting - not for arrays!
***************************************************************/
void wing_store(
   struct WING_DEF *i,        /* new element */
   struct WING_DEF **stwi,   /* first element in list */
   struct WING_DEF **lawi)   /* last element in list */
{
   struct WING_DEF *old, *p;
   if (*lawi == NULL) {     /* if entry is first to go in list */
		i->next = NULL;
		i->prior = NULL;
      *lawi = i;
      *stwi = i;
		return;
	}
   p = *stwi;   /* start at top of list */
	old = NULL;
	while(p) {
      if (p->num < i->num) {     /* insert format selection here */
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
         *stwi = i;
			return;
		}
	}
	old->next = i;       /* put on end */
	i->next = NULL;
	i->prior = old;
   *lawi = i;
}



/**************************************************************
*   add WU field validation function
**************************************************************/
defldval_wing (TRANSACTION *tp)
{
   struct WING_DEF *info;
   info = stwi;
	if(pd->loc_strm==0) {
      StatsErr("25007");    /* no local streamers in project def */
		tp->tra_abort=TRUE;
		return(TRUE);
	}
   if(tp->cur_fld == 0) {   /* WU name validation */
      if(findwing(ext->b_str)) {
         StatsErr("25008");
			return(FALSE);
		}
		if(!strlen(ext->b_str)) {
         StatsErr("25009");
			return(FALSE);
		}
	}
 if(((tp->cur_fld==1)&&(ext->a_num==0))||((tp->cur_fld==2)&&(ext->b_num==0))||
				  ((tp->cur_fld==3)&&(ext->c_num==0))) {
         StatsErr("25010");
			return(FALSE);
	}
   if(tp->cur_fld == 1) {   /* WU number validation */
		while(info) {
         if(ext->a_num == info->num) {
            StatsErr("25011");
				return(FALSE);
			}
			info = info->next;
		}
      if(ext->a_num > hd->wi_num) {
         StatsErr("25012");
			return(FALSE);
		}
	}
	if(tp->cur_fld == 2) {   /* cable number validation */
		if(ext->b_num > pd->loc_strm) {
         StatsErr("25013");
			return(FALSE);
		}
	}
	return (TRUE);
}
