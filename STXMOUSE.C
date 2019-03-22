/***************************************************************
*    STXMOUSE.C   mouse functions for GSF & DATAWINDOWS
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

#include <stdio.h>      /* microsoft */

#include <dw.h>         /* greenleaf */
#include <sftd.h>
#include <ibmkeys.h>

#include <stxfunct.h>   /* wcs function prototypes */


/**************************************************************
*	 check mouse present & if so turn mouse cursor on
*	 return 1 if mouse/otherwise 0
*	 input (0) leaves mouse cursor wherever it was last
*	 input (1) puts mouse cursor in the middle of the screen
**************************************************************/
MouseOn (register int i)
{
	if(mousinst()) {
		if(i) setmcpos(12*8,40*8);   /* set to middle screen */
		ctlmscur(ON);		 /* display mouse cursor */
		return (1);
	}
	return(1);
}


/**************************************************************
*   like getkey() but responds to mouse button press also
*   if i=0 just initialises mouse cursor
*   if n=1, updates clock (not wanted in graphics modes)
**************************************************************/
unsigned MouseKey (int i, int n)
{
	GKEY k;
	unsigned key, a=0;
			/* initialise UserIn() */
	inikbd( BUFFERSCAN | MONXMOUSE | MONYMOUSE | MONMOUSEBUTTN
			| MOUSEMAPPING | WAITFORKEY, &k);
   if(n) k.UserHook = UpdateClockWindow;   /* the clock idle function */
	MouseOn(0);
	if (i) {
		while(key = UserIn(&k)) {
			switch(k.Status) {
				case KEYACTIVITY:
					return(key);
				case MOUSEACTIVITY:
					if(k.MouseBtns) {
						a=1;
						break;
					}
					if(a) return(key);
			}
		}
	}
}
