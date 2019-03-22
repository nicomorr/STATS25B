/**************************************************************
*   SRCECFG.C   Source configuration file for stats
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
#include <ctype.h>

#include <dw.h>            /* greenleaf */
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct SRCE_DEF *stsrce;   /* pointer to start of source list */
extern struct SRCE_DEF *lasrce;   /* pointer to last  of source list */
extern struct HDR_DEF *hd;        /* pointer to header list */
extern struct PRO_DEF *pd;        /* pointer to project list */
extern struct EXT_DEF *ext;
extern char *formats[];           /* format definitions */


/**************************************************************
*   view source configuration
**************************************************************/
void view_gun(int n, unsigned fmt, unsigned gun_max)
{
	HWND W1, W2;
	unsigned r;
	register int i = 0, p;
	struct SRCE_DEF *info;
	info = stsrce;
	if (!n) p = 1;
		else p = n;
	W1 = vcreat (p, 68, NORML, NO);
	W2 = vcreat (1, 65, NORML, NO);
	if (n > 18) p = 18;
		else p = n;
	vwind (W1, p, 67, 0, 0);
	vlocate (W1, 3, 6);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " Sources ");
	vwind (W2, 1, 60, NORML, NO);
	vlocate (W2, 4+p, 7);
	vatputf (W2, 0, 0, "Guns: %-3d", n);
	if (hd->contr == 0)	 /* if GECO */
		vatputf (W2, 0, 13, "Fmt: %-15s", formats[fmt]);
	if (hd->contr == 1)	 /* if PRAKLA */
      vatputs (W2, 0, 13, "Fmt: SYN V2.3");
	if (hd->contr == 2)	 /* if WESTERN */
		vatputf (W2, 0, 13, "Fmt: WESTERN %u", hd->west + 1);
   if (hd->contr == 5)   /* if DIGICON TRACE0 */
      vatputs (W2, 0, 13, "Fmt: TRACE0");
	vatputf (W2, 0, 36, "Max: %-3u", gun_max);
	if (n > 18) vratputs(W2, 0, 51, HIGHNORML, "Arrows Up/Down");
	if (!info) {
		vratputs (W1, 0, 24, REVERR, "NO GUNS DEFINED YET");
		vbeep();
	}
	while (info) {
		vatputf(W1, i, 1, "Gun Number: %-3d   Gun Name: %-10s   Source: %-1d   Volume: %-5d",
				 info->gun_num, info->gun_name, info->srce, info->gun_vol);
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
			default:
				break;
		}
	} while ((r != ESC) && (r != CR));
	vdelete (W2, NONE);
	vdelete (W1, NONE);
	return;
}


/**************************************************************
*   add a gun to source configuration
**************************************************************/
add_gun(int n, unsigned fmt, unsigned gun_max)
{
	struct SRCE_DEF *info;
	HWND W1;           /* data entry window */
	TRANSACTION *tp;   /* transaction structure */
	FIELD *fld;        /* field structure */
	info = (struct SRCE_DEF *) malloc(sizeof(struct SRCE_DEF));
	if (!info) {
		StatsErr("32001");
		return(n);
	}
	if (n >= (int) gun_max) {
		StatsErr("32002");
		free(info);
		return (n);
	}
   W1 = vcreat (7, 35, NORML, NO);
   vlocate (W1, 3, 23);
	vframe (W1, NORML, FRDOUBLE);
   vtitle (W1, NORML, " Add Gun ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
      /*# defldval_srce() srce_help() EscapeKey() */
   tp = vpreload (4, 2, NULLF, defldval_srce, NULLF, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   tp->helpfn = srce_help;
	fld = *(tp->def);
	if((fmt==10)||(fmt==11)) {  /* Gunda 1 & 2 */
		fld[2].selectenable = FALSE;
		fld[2].noecho = TRUE;
		tp->noechochar = '*';
	}       /* if GECO/WESTERN LITTON or PRAKLA VZAD (Syn V2.0) */
	if((fmt==8)||(fmt==9)||(fmt==63)||(fmt==64)||(fmt==91))
		vdefstr (tp, 0, 1, 1, "Gun Name", 18, 10, NULL, ext->b_str, YES);
	if((fmt==10)||(fmt==11))    /* Gunda 1 & 2 */
		vdefstr (tp, 0, 1, 1, "Gun Name", 18, 5, "!-9-9", ext->b_str, NO);
	vdefuint (tp, 1, 2, 1, "Gun Number", 18, 4, "999", &ext->a_num, YES);
	vdefuint (tp, 2, 3, 1, "Source Number", 18, 4, "9", &ext->b_num, YES);
	vdefuint (tp, 3, 4, 1, "Gun Volume", 18, 4, "99999", &ext->c_num, YES);
   vdefatsa (tp, 0, 6, 1, NORML, "<ESC> Exit");
   vdefatsa (tp, 1, 6, 15, HIGHNORML, "<Ctrl-Enter> Accept");
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
		if(KeySure()) {
			strcpy(info->gun_name, ext->b_str);
			info->gun_num = ext->a_num;
			info->srce = ext->b_num;
			info->gun_vol = ext->c_num;
			info->max_err=0;   /* initialise the errors to 0 */
			info->en_num=0;
			info->tot_sp1=0;
			info->tot_sp2=0;
			info->mf_num=0;
			if((fmt==10)||(fmt==11)) {   /* Gunda 1 & 2 */
				info->gunno = toascii(ext->b_str[4]) - 49;  /* gun number 0 to 7 */
				info->arrno = toascii(ext->b_str[2]) - 49;  /* array no 0 to 7 */
				if(ext->b_str[0]=='S') info->side = 0;  /* gunda side Port=1 Stbd=0 */
				if(ext->b_str[0]=='P') info->side = 1;
			}
			srce_store(info, &stsrce, &lasrce);	/* double linked list */
			++n;
		}
	}
	vdelete(W1, NONE);
	return(n);
}


/**************************************************************
*   add gun field validation function
**************************************************************/
defldval_srce (TRANSACTION *tp)
{
	struct SRCE_DEF *info;
	info = stsrce;
	if(pd->loc_srce==0) {
		StatsErr("32003");
		tp->tra_abort=TRUE;
		return(TRUE);
	}
	if(tp->cur_fld == 0) {   /* gun name validation */
		if(findsrce(ext->b_str)) {
			StatsErr("32004");
			return(FALSE);
		}
		if(!strlen(ext->b_str)) {
			StatsErr("32005");
			return(FALSE);
		}
		if((ext->d_num==10)||(ext->d_num==11)) {   /* special for Gunda */
			if(pd->loc_srce==2) {
				if((ext->b_str[0]!='P')&&(ext->b_str[0]!='S')) {
					StatsErr("32006");
					return(FALSE);
				}
			}
			if(pd->loc_srce==1) {
				if(ext->b_str[0]!='S') {
					StatsErr("32007");
					return(FALSE);
				}
			}
			if(pd->loc_srce>2) {
				StatsErr("32008");
				return(FALSE);
			}
			if((ext->b_str[2]<'1')||(ext->b_str[2]>'8')||(ext->b_str[4]<'1')||
															 (ext->b_str[4]>'8')) {
				StatsErr("32009");
				return(FALSE);
			}
		}
	}
	if((ext->d_num==10)||(ext->d_num==11)) {   /* special for Gunda */
		if(ext->b_str[0]=='S') ext->b_num = 1;
		if(ext->b_str[0]=='P') ext->b_num = 2;
	}
	if(((tp->cur_fld==1)&&(ext->a_num==0))||((tp->cur_fld==2)&&(ext->b_num==0))||
				  ((tp->cur_fld==3)&&(ext->c_num==0))) {
			StatsErr("32010");
			return(FALSE);
	}
	if(tp->cur_fld == 1) {   /* gun number validation */
		while(info) {
			if(ext->a_num == (unsigned) info->gun_num) {
				StatsErr("32011");
				return(FALSE);
			}
			info = info->next;
		}
		if(ext->a_num > hd->gu_num) {
			StatsErr("32012");
			return(FALSE);
		}
	}
	if(tp->cur_fld == 2) {   /* source number validation */
		if(ext->b_num > pd->loc_srce) {
			StatsErr("32013");
			return(FALSE);
		}
	}
	return (TRUE);
}


/**************************************************************
*   delete a gun from source configuration
**************************************************************/
del_gun(int n)
{
	struct SRCE_DEF *info;
	HWND W1;           /* data entry window */
	TRANSACTION *tp;   /* transaction structure */
	FIELD *fld;        /* field structure */
	if (n<1) {
		StatsErr("32014");
		return (n);
	}
   W1 = vcreat (4, 35, NORML, NO);
   vlocate (W1, 3, 23);
	vframe (W1, NORML, FRDOUBLE);
   vtitle (W1, NORML, " Delete Gun ");
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
      /*# defldval_srcedel() detranval_glob() srce_help() EscapeKey() */
   tp = vpreload (1, 2, NULLF, defldval_srcedel, detranval_glob, NULLF);
	tp->cancfn=EscapeKey;
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
   tp->helpfn = srce_help;
	fld = *(tp->def);
	vdefstr (tp, 0, 1, 1, "Gun Name", 18, 10, NULL, ext->b_str, YES);
   vdefatsa (tp, 0, 3, 3, NORML, "<ESC> Exit");
   vdefatsa (tp, 1, 3, 21, HIGHNORML, "<CR> Accept");
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) {  /* only if DWSUCCESS (0) */
      info = stsrce;
      while(info) {
         if (!strcmp(ext->b_str, info->gun_name))
            break;     /* found */
         info = info->next;
      }
      if(info->prior)
         info->prior->next = info->next;
      else {   /* first new item */
         stsrce = info->next;
         if(stsrce) stsrce->prior = '\0';
      }
      if(info->next)
         info->next->prior = info->prior;
      else  /* deleting last element */
         lasrce = info->prior;
      free(info);
      --n;
	}
	vdelete(W1, NONE);
	return(n);
}


/**************************************************************
*   delete gun field validation function
**************************************************************/
defldval_srcedel ()
{
	if(!findsrce(ext->b_str)) {
		StatsErr("32015");
		return(FALSE);
	}
	return(TRUE);
}


/**************************************************************
*   global source help window
**************************************************************/
void srce_help()
{
   string_help("Source");
}


/**************************************************************
*   find a gun name for deletion - also used to check
*   for duplicates when adding a gun
**************************************************************/
struct SRCE_DEF *findsrce(char *name)
{
	struct SRCE_DEF *info;
	info = stsrce;
	while(info) {
		if (!strcmp(name, info->gun_name)) {
		return(info);     /* found */
		}
		info = info->next;
	}
	return (NULL);  /* not found */
}


/**************************************************************
*   save source configuration to disk
**************************************************************/
void save_gun(int n)
{
	struct SRCE_DEF *info;
	FILE *fp;
	if (!n) {
		StatsErr("32016");
		return;
	}
	if (!KeySure()) return;
	fp = fopen("SOURCE.CFG", "wb");
	info = stsrce;
	while(info) {
		fwrite(info, sizeof(struct SRCE_DEF), 1, fp);
		info = info->next;
	}
	fclose(fp);
   StatsMessage("SAVED SOURCE.CFG");
}


/**************************************************************
*   load source configuration from disk
**************************************************************/
load_gun(int i)
{
	int n = 0;					/* reset the gun counter */
	struct SRCE_DEF *info;
   FILE *fp;
   if(i)
      if (!KeySure())
         return(ext->numbergun);
	if(stsrce) {
		info = stsrce;
		while(info) {
			free(info);
			info = info->next;
		}
		stsrce = lasrce = NULL;
	}
	if((fp = fopen("SOURCE.CFG", "rb")) == NULL) {
	  if(i) StatsErr("32017");
     return(0);
	}
	while(!feof(fp)) {
		info = (struct SRCE_DEF *) malloc(sizeof(struct SRCE_DEF));
		if(!info) {
			StatsErr("32018");
			fclose(fp);
			return(0);
		}
		if (1 != fread(info, sizeof(struct SRCE_DEF), 1, fp))
			break;
		srce_store(info, &stsrce, &lasrce);
		++n;
	}
   fclose(fp);
   if(i) StatsMessage("LOADED SOURCE.CFG");
	return(n);
}


/***************************************************************
*   create a doubly linked list in sorted order (by gun_num)
*   uses integer sorting - not for arrays!
***************************************************************/
void srce_store(
	struct SRCE_DEF *i,         /* new element */
	struct SRCE_DEF **stsrce,   /* first element in list */
	struct SRCE_DEF **lasrce)	/* last element in list */
{
	struct SRCE_DEF *old, *p;
	if (*lasrce == NULL) {   /* if entry is first to go in list */
		i->next = NULL;
		i->prior = NULL;
		*lasrce = i;
		*stsrce = i;
		return;
	}
	p = *stsrce;   /* start at top of list */
	old = NULL;
	while(p) {
		if (p->gun_num < i->gun_num) {     /* insert format selection here */
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
			*stsrce = i;
			return;
		}
	}
	old->next = i;       /* put on end */
	i->next = NULL;
	i->prior = old;
	*lasrce = i;
}
