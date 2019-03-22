/***************************************************************
*    WCS_GNLF.C in here any modified Greenleaf functions
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

#include <dwsystem.h>      /* greenleaf */

#include <stxfunct.h>      /* wcs */


/**************************************************************
 * ARGUMENT
 *  (int)		row	 -	row for listbox window.
 *  (int)		col	 -	column for listbox window.
 *  (int)		rows	 -	number of rows in listbox.
 *  (int)		uattr	 -	logical attribute for unselected items
 *  (int)		sattr	 -	logical attribute for selected items
 *  (char *)		name	 -	title string.
 *  (int)		tattr	 -	logical attribute for title.
 *  (char *)		frstring -	frame string.
 *  (int)		fattr	 -	attribute to use for frame.
 *  (LISTITEM *)	listhead -	points to head of list
 *
 * DESCRIPTION
 *  Initializes list box, creates window, allocates memory etc.
 */

/***************************************************************
 * modified pop-up box with cursor on pre-selected item
****************************************************************/
LBOXSTA *WCS_lboxinit(row,col,rows,uattr,sattr,name,tattr,frstring,fattr,listhead)
int row,col,rows,uattr,sattr,tattr,fattr;
char *name,*frstring;
LISTITEM *listhead;
{
	LBOXSTA *lbs;
	LISTITEM *p;
	int scratch;
	if((lbs=(LBOXSTA *)calloc(sizeof(LBOXSTA),1))==(LBOXSTA *)0) {
		_dwerror=DWNOMEMORY;
		return((LBOXSTA *)0);
	}
	_dwerror=DWSUCCESS;
	if((lbs->itemcount=listcnt(listhead))==0) {
		free(lbs);
		_dwerror=DWINVPAR;
		return((LBOXSTA *)0);
	}
	if(rows>lbs->itemcount)
		rows=lbs->itemcount;
	if(lbs->itemcount>rows) {
		lbs->leftmar=2;
		lbs->hasbars=1;
	}
      /* determine length of longest item in list */
	for(lbs->lmaxlen=0,p=listhead->lnext;p;p=p->lnext)
		if(p->listitem)
			if((scratch=strlen(p->listitem))>lbs->lmaxlen)
				lbs->lmaxlen=scratch;
	if((lbs->lb=vcreat(rows,lbs->lmaxlen+lbs->leftmar,
			   uattr,NO))==(HWND)-1) {
		_dwerror=(int)lbs->lb;
		free(lbs);
		return((LBOXSTA *)0);
	}
	if((scratch=vlocate(lbs->lb,row,col))!=DWSUCCESS) {
		_dwerror=scratch;
		vdelete(lbs->lb,NONE);
		free(lbs);
		return((LBOXSTA *)0);
	}
	vframe(lbs->lb,fattr,frstring);
	vtitle(lbs->lb,tattr,name);
	lbs->selectedlink=1;
	lbs->selectitem=listhead->lnext;
	_dlist(lbs->lb,lbs->selectitem,rows,0,uattr,lbs->leftmar);
	modattr(lbs->lb,0,lbs->leftmar,lbs->lmaxlen,sattr);
	if(lbs->hasbars) {
		for(scratch=0;scratch<rows;++scratch)
			vratputc(lbs->lb,scratch,1,uattr,(char)0xb3);
		vratputc(lbs->lb,rows-1,0,uattr,'\037');
	}
	lbs->lrows=rows;
	lbs->lhead=listhead;
	lbs->luattr=uattr;
	lbs->lsattr=sattr;
	return(lbs);
}
