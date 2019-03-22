/*************************************************************
*   PRINTCFG.C   printer configuration
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
#include <string.h>
#include <malloc.h>

#include <dw.h>            /* greenleaf */
#include <dwdata.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>

			/* external global declarations */
extern struct PRN_DEF *prn;
extern struct EXT_DEF *ext;

         /* global printer multipliers - must be global for field val */
double xm,                /* x mult */
       ym;                /* y mult */

         /* printer types */
char *printers[] = {
   "Epson MX",
   "Epson LQ",
   "Toshiba 24 pin",
   "HP LaserJet Plus",
   "HP ThinkJet",
   "Epson FX",
   '\0',
};
         /* output ports */
char *comports[] = {
   "LPT 1",
   "LPT 2",
   "COM 1",
   "COM 2",
   '\0',
};


/**************************************************************
*   select/change  printer configuration
**************************************************************/
void ch_prn()
{
	HWND W1, W2;				  /* data entry/message windows */
	TRANSACTION *tp;			  /* transaction structure */
	FIELD *fld; 				  /* field structure */
	register int i;
   char printer[18], ioport[6];
   _heapmin();               /* defragment the heap */
   xm = (double) prn->xm;
   ym = (double) prn->ym;
   ext->a_num = prn->prndr;
   ext->b_num = prn->ioport;
   strcpy(printer, printers[prn->prndr]);
   strcpy(ioport, comports[prn->ioport]);
	W2 = vcreat (1, 80, REVNORML, NO);
	vlocate (W2, 24, 0);
	vratputs (W2, 0, 1, REVNORML, "<ESC> Exit");
	vratputs (W2, 0, 36, REVERR, "  F1  ");
	vratputs (W2, 0, 60, REVEMPHNORML, "<CTRL-ENTER> Accept");
   W1 = vcreat (11, 45, NORML, NO);
	vshadow(W1, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
   vlocate (W1, 10, 5);
	vframe (W1, EMPHNORML, FRDOUBLE);
   vtitle (W1, EMPHNORML, " Printer ");
      /*# defldval_prn() detranval_glob() EscapeKey() print_help() */
   tp = vpreload (9, 0, NULLF, defldval_prn, detranval_glob, NULLF);
   tp->cancfn=EscapeKey;
   tp->helpfn = print_help;   /* ignore param warning */
	tp->paint_fx = TRUE;
	tp->paint_fl = TRUE;
	tp->inp_idle = UpdateClockWindow;
	fld = *(tp->def);
   for(i=0;i<9;i++)
		fld[i].use_ext = TRUE;	 /* use/display the preload value */
   vdefstr  (tp, 0, 1, 1, "Select Printer", 17, 18, NULL, printer, YES);
   vdefstr  (tp, 1, 2, 1, "Select ComPort", 17, 6, NULL, ioport, YES);
   vdefint (tp, 2, 3, 1, "Resolution", 17, 1, "9", &prn->res, YES);
   vdefdbl (tp, 3, 4, 1, "X Multiplier", 17, 0, "9999.99", &xm, YES);
   vdefdbl (tp, 4, 5, 1, "Y Multiplier", 17, 0, "9999.99", &ym, YES);
   vdefint (tp, 5, 6, 1, "Graphs Per Page", 17, 2, "99", &prn->page, YES);
   vdefint (tp, 6, 7, 1, "Lines Per Page", 17, 2, "99", &prn->lines, YES);
   vdefint (tp, 7, 8, 1, "Top Margin", 17, 2, "99", &prn->top, YES);
   vdefint (tp, 8, 9, 1, "Left Margin", 17, 2, "99", &prn->left, YES);
      /*# defldset_prn() res_help() mul_help() oth_help() */
   deSetFieldSetfn(tp, defldset_prn);
	visible (W2, YES, NO);
	visible (W1, YES, NO);
	if(!vread (tp, W1, YES)) { /* only if DWSUCCESS (0) */
      prn->xm = (float) xm;
      prn->ym = (float) ym;
      prn->prndr = ext->a_num;
      prn->ioport = ext->b_num;
   }
   vdelete (W1, NONE);
   vdelete (W2, NONE);
   _heapmin();               /* defragment the heap */
   return;
}


/*************************************************************
*   printer help function
**************************************************************/
void print_help()
{
   string_help("Printer");
}


/**************************************************************
*   set up fixed menu selections for printer & comports
**************************************************************/
defldset_prn (TRANSACTION *tp)
{
	register int sel;							 /* format number */
	unsigned i;
	ext->ee = ext->ff = TRUE;            /* set the bool flags */
   if ((tp->cur_fld == 0)&&(tp->thiskey!=ESC)) {
      i = ext->a_num;
      if((sel = select_prn(i)) != -1) {   /* get a format */
         ext->a_num = sel; /* temporary format number */
			tp->inp_get = KeySim;						/* do the actual keysim */
         ext->s_input = printers[sel];
		} else { 											/* do only nextfield jump */
			ext->ee = FALSE;								/* see KeySim() function */
			tp->inp_get = KeySim;
		}
   }
   if ((tp->cur_fld == 1)&&(tp->thiskey!=ESC)) {
      i = ext->b_num;
      if((sel = select_port(i)) != -1) {   /* get a format */
         ext->b_num = sel; /* temporary format number */
			tp->inp_get = KeySim;						/* do the actual keysim */
         ext->s_input = comports[sel];
		} else { 											/* do only nextfield jump */
			ext->ee = FALSE;								/* see KeySim() function */
			tp->inp_get = KeySim;
		}
   }
   if(tp->cur_fld == 2 ) tp->helpfn = res_help; /* ignore param warn */
   if( (tp->cur_fld == 3) || (tp->cur_fld == 4) )  /* X & Y mults */
      tp->helpfn = mul_help; /* ignore param warn */
   if(tp->cur_fld > 4) tp->helpfn = oth_help;   /* ignore param warn */
	return (TRUE);
}


/*************************************************************
*   printer resolutions help function
**************************************************************/
void res_help()
{
   string_help("Printer Resolutions");
}


/*************************************************************
*   printer multipliers help function
**************************************************************/
void mul_help()
{
   string_help("Printer Multipliers");
}


/*************************************************************
*   printer graphs/page, lines/page & margins help
**************************************************************/
void oth_help()
{
   string_help("Other Printer Settings");
}


/**************************************************************
*   field validation function
**************************************************************/
defldval_prn (TRANSACTION  *tp)
{
   if(tp->cur_fld == 1) {   /* serial ports not implemented */
      if(ext->b_num >1) {
         StatsErr("15001");
         return(FALSE);
      }
   }
   if(tp->cur_fld == 2) {  /* resolution limits MX FX Laser */
      if((ext->a_num == 0)||(ext->a_num == 3)||(ext->a_num == 5)) {
         if(prn->res > 3) {  /* max 3 */
            StatsErr("15002");
            return(FALSE);
         }
      }
      if((ext->a_num == 2)||(ext->a_num == 4)) {  /* Tosh & HP ThinkJet */
         if(prn->res > 1) {  /* max 1 */
            StatsErr("15003");
            return(FALSE);
         }
      }
      if(ext->a_num == 1) {  /* Epson LQ */
         if(prn->res > 8) {  /* max 8 */
            StatsErr("15004");
            return(FALSE);
         }
      }
   }
   if(tp->cur_fld == 3) {     /* X  multiplier */
      if(xm <= 0.0) {
         StatsErr("15009");   /* 0 or less not permitted */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 4) {     /* Y  multiplier */
      if(ym <= 0.0) {
         StatsErr("15009");   /* 0 or less not permitted */
         return(FALSE);
      }
   }
   if(tp->cur_fld == 5) {  /* graphs per printed page */
      if(prn->page%2) {    /* must be an even number */
         StatsErr("15007");
         return(FALSE);
      }
   }
   if(tp->cur_fld >= 6) {  /* lines/top/left */
      if( (prn->prndr>=2)&&(prn->prndr<=4) ) { /* only work for Epson */
         StatsErr("15008");
         return(TRUE);
      }
   }

	return (TRUE);
}


/**************************************************************
*   offer up a printer list to select
**************************************************************/
select_prn(unsigned n)
{
	LISTITEM *l;
	LBOXSTA *b;
	register int i = 0;
	unsigned key;
	char *s;
	l = initlist();
   while (printers[i])
      adtolist (l, printers[i++]);
   b = WCS_lboxinit (11, 57, 6, NORML, REVNORML, NULL, EMPHNORML,
						 FRDOUBLE, EMPHNORML, l);	 /* WCS clone of DW function */
	for(i=0; i<(int)n; i++) lboxctrl(b, CURDN);
	visible(b->lb, YES, NO);
	while((key=vgetkey(UpdateClockWindow)) != ESC && key != 0x0d)
		lboxctrl(b, key);
	if(key == ESC) {
		lboxdel(b);
		return(-1);
	}
	s = lboxsel(b);
	lboxdel(b);
	i = 0;
   while (printers[i]) {
     if(!strcmp(printers[i], s)) return (i);
	  i++;
	}
	return (-1);
}


/**************************************************************
*   offer up a port list to select
**************************************************************/
select_port(unsigned n)
{
	LISTITEM *l;
	LBOXSTA *b;
	register int i = 0;
	unsigned key;
	char *s;
	l = initlist();
   while (comports[i])
      adtolist (l, comports[i++]);
   b = WCS_lboxinit (12, 62, 4, NORML, REVNORML, NULL, EMPHNORML,
						 FRDOUBLE, EMPHNORML, l);	 /* WCS clone of DW function */
	for(i=0; i<(int)n; i++) lboxctrl(b, CURDN);
	visible(b->lb, YES, NO);
	while((key=vgetkey(UpdateClockWindow)) != ESC && key != 0x0d)
		lboxctrl(b, key);
	if(key == ESC) {
		lboxdel(b);
		return(-1);
	}
	s = lboxsel(b);
	lboxdel(b);
	i = 0;
   while (comports[i]) {
     if(!strcmp(comports[i], s)) return (i);
	  i++;
	}
	return (-1);
}


/**************************************************************
*   save printer definition to disk
**************************************************************/
void save_prn ()
{
	FILE *fp;
	if (!KeySure()) return;
   fp = fopen("PRINTER.SET", "wb");
   fwrite(prn, sizeof(struct PRN_DEF), 1, fp);
	fclose(fp);
   StatsMessage ("SAVED PRINTER.SET");
}


/**************************************************************
*   load printer definition from disk
**************************************************************/
load_prn (int i)
{
   FILE *fp;
   if(i)
   if (!KeySure())
      return(0);
   if((fp = fopen("PRINTER.SET", "rb")) == NULL) {
     if(i) StatsErr("15005");            /* not found */
	  return(0);
	}
   if (1 != fread(prn, sizeof(struct PRN_DEF), 1, fp))
      StatsErr("15006");
   fclose(fp);
   if(i) StatsMessage("LOADED PRINTER.SET");
	return(1);
}
