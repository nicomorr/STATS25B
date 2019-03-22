/***************************************************************
*   STATSERR.C   various error handling functions
****************************************************************
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
#include <string.h>

#include <dw.h>            /* greenleaf */
#include <sftd.h>
#include <ibmkeys.h>

#include <stxfunct.h>      /* wcs */

			/* external global declarations */
extern HWND Wwait;


/***************************************************************
*   StatsDosError()  displays dos error in window
*   calculates window width based on strlen()
*   takes a string argument and DOS error number
*   MUST BE CALLED IMMEDIATELY AFTER A DOS ERROR otherwise
*   strerror() may return a changed value.
****************************************************************/
void StatsDosError (char *arg, int num)
{
	HWND Werr;
	Werr = vcreat (1, 70, HIGHERR, NO);
	vlocate (Werr, 20, 5);
	vframe (Werr, REVERR, FRSINGLE);
	vtitle (Werr, REVERR, " Error ");
	vshadow(Werr, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
	vatputf (Werr, 0, 0, "%s %s", strupr(arg), strerror(num));
	vwind (Werr, 1, strlen(arg) +strlen(strerror(num))+2, 0, 0);
	visible (Werr, YES, NO);
	vgetkey( UpdateClockWindow );
	vdelete (Werr, NONE);
}


/***************************************************************
*   StatsErr()  displays error message in window
*   finds error message from STATERR.ERR & displays
*   takes a string argument
****************************************************************/
void StatsErr (char *arg)
{
	HWND Werr;
	char buff[75];
	char *buf;
	Werr = vcreat (1, 75, HIGHERR, NO);
	vlocate (Werr, 20, 2);
	vframe (Werr, REVERR, FRSINGLE);
	vtitle (Werr, REVERR, arg);
	if((buf = ExtractFromHelpFile("STATERR.ERR", arg)) == NULL) {
		strcpy(buff, arg);
		strcat(buff, ": UNABLE TO LOCATE ERROR TABLE FILE - STATERR.ERR");
		StatsError(buff);
		return;
	}
	vatputf (Werr, 0, 0, "%s", buf);
	vwind (Werr, 1, strlen(buf), 0, 0);
	visible (Werr, YES, NO);
	vgetkey( UpdateClockWindow );
	vdelete (Werr, NONE);
}


/***************************************************************
*   StatsError()  displays error message in window
*   calculates window width based on strlen()
*   takes a string argument
****************************************************************/
void StatsError (char *arg)
{
	HWND Werr;
	Werr = vcreat (1, 70, HIGHERR, NO);
	vlocate (Werr, 20, 5);
	vframe (Werr, REVERR, FRSINGLE);
	vtitle (Werr, REVERR, " Error ");
	vatputf (Werr, 0, 0, "%s", arg);
	vwind (Werr, 1, strlen(arg), 0, 0);
	visible (Werr, YES, NO);
	vgetkey( UpdateClockWindow );
	vdelete (Werr, NONE);
}

/***************************************************************
*   not implemented yet function
****************************************************************/
void NonImpl()
{
	StatsError("Not Implemented Yet");
}



/***************************************************************
*   StatsMessage()  displays  timed message in window
*   calculates window width based on strlen()
*   takes a string argument  -  no title frame
****************************************************************/
void StatsMessage (char *arg)
{
	HWND Wmsg;
	Wmsg = vcreat (1, 59, HIGHNORML, NO);
	vlocate (Wmsg, 21, 20);
	vframe (Wmsg, HIGHNORML, FRSINGLE);
	vatputf (Wmsg, 0, 0, "%s", arg);
	vwind (Wmsg, 1, strlen(arg), 0, 0);
	visible (Wmsg, YES, NO);
	Delay18(18);
	vdelete (Wmsg, NONE);
}


/***************************************************************
*	 PressKey()  displays  "Press Any Key"
*   arguments are X, Y location only
****************************************************************/
void PressKey(int x, int y)
{
	HWND Wkey;
	Wkey = vcreat (1, 13, NORML, NO);
	vlocate (Wkey, x, y);
	vframe (Wkey, NORML, FRSINGLE);
	vratputs (Wkey, 0, 0, EMPHNORML, "Press Any Key");
	vwind (Wkey, 1, 13, 0, 0);
	visible (Wkey, YES, NO);
	vgetkey( UpdateClockWindow );
	vdelete (Wkey, NONE);
}


/***************************************************************
*   KeySure()  displays  "Are You Sure?  .........."
*   returns 0 if NO - 1 if YES
****************************************************************/
KeySure()
{
	unsigned c;
   HWND Wkey;
	Wkey = vcreat (1, 26, NORML, NO);
	vlocate (Wkey, 12, 26);
	vframe (Wkey, EMPHNORML, FRDOUBLE);
	vatputs (Wkey, 0, 0, "Are You Sure!  ........  ");
	vratputc (Wkey, 0, 25, REVEMPHNORML, 'N');
	vwind (Wkey, 1, 27, 0, 0);
   visible (Wkey, YES, NO);
	while( c = vgetkey( UpdateClockWindow ) ) {
		if ((c != 89) && (c != 121)) {
			vdelete (Wkey, NONE);
			return (0);
		} else {
			vratputc (Wkey, 0, 25, REVEMPHNORML, (char)toupper(c));
			if( ( c = vgetkey( UpdateClockWindow ) ) != 13 ) {
				vdelete (Wkey, NONE);
				return (0);
			} else {
				vdelete (Wkey, NONE);
				return (1);
			}
		}
   }
}


/***************************************************************
*   StWait   makes the wait window visible
****************************************************************/
void StWait()
{
	visible (Wwait, YES, NO);
	return;
}


/***************************************************************
*   EnWait   make the wait window invisible
****************************************************************/
void EnWait ()
{
	visible (Wwait, NO, NO);
	return;
}
