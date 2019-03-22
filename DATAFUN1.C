/***************************************************************
*    DATAFUN1.C    common data entry or validation functions
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

#include <extern.h>        /* wcs */
#include <stxfunct.h>

			/* external global declarations */
extern struct EXT_DEF *ext;


/**************************************************************
*  escape from data entry (cancel user key - otherwise YES/NO
*  may pop up after ch_anal() or ch_hdr() or ch_spec()
**************************************************************/
int EscapeKey(TRANSACTION *tp)
{
   USR_KEY1=0x00;
	if(tp->thiskey==ESCAPE_KEY || tp->thiskey==REV_ESCAPE)
		tp->tra_abort=TRUE;
	return TRUE;
}


/**************************************************************
*  ketboard simulation for data entry
**************************************************************/
unsigned KeySim(unsigned (*fp)(),TRANSACTION *pt)
{
	fp=fp;
	if(ext->ee) {         /* if ee false then only jump to nextfield */
		if(ext->ff) {
			ext->ff=FALSE;
			return (unsigned)FIELD_ERASE; /* Clear The Field First */
		}
		if(*ext->s_input)
			return (unsigned)*ext->s_input++;
		else {
			ext->s_input=NULL;
			ext->ff=TRUE;
			pt->inp_get=(unsigned (*)())vgetkey;
			return (unsigned)NXT_FLD1; /* force next field on form */
		}
	} else {
		ext->ee = TRUE;
		pt->inp_get=(unsigned (*)())vgetkey;
		return (unsigned)NXT_FLD1;
	}
}


/*************************************************************
*   global transaction validate function
*   just calls keysure() - also clears userkey
*   does not call KeySure() if <ESC> was pressed
**************************************************************/
detranval_glob (TRANSACTION *tp)
{
   USR_KEY1 = 0x00;
   if(tp->thiskey==ESCAPE_KEY || tp->thiskey==REV_ESCAPE)
      return FALSE;
	if (KeySure()) return (TRUE);
	else return (FALSE);
}
