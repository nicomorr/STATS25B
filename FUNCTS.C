/***********************************************************
*    FUNCTS.C   simpler run-time functions
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
#include <string.h>
#include <conio.h>

#include <stats.h>         /* wcs */
#include <extern.h>
#include <stxfunct.h>
#include <stxconst.h>      /* constant definitions */

				/* external global declarations */
extern struct OPT_DEF  *opt;      /* pointer to options list */
extern struct ANAL_DEF *andef;    /* pointer to analysis list */
extern struct EXT_DEF *ext;		 /* pointer to general variables */


/****************************************************************
* return an integer julian date
* uses conv_bin() to convert a two byte binary integer; NOR 2 & 3
* & DIGICON TRACE0
* uses conv_bcd() to convert a two byte bcd integer; WEST 3 & 4
* note WEST FMT 3 & 4 have last digit of year first
* st is byte offset in header
* bp is pointer to data buffer
* last integer in conv_bin() is (1=LSByte first, 0=MSByte first)
*****************************************************************/
find_jd (int st, unsigned fmt, char *bp)
{
	long p, q;
   if ( (fmt == 6)||(fmt == 30)||(fmt == 71) )  /* NOR 2 & 3 & TRACE0 */
		return ((int)conv_bin (st, 2, bp, 0));  /* two bytes, MSByte first */
   if ( (fmt == 63)||(fmt == 64) ) {            /* WESTERN 3 & 4 */
		p = (conv_bcd(st, 2, bp));
		if(p>1000L) {       /* 1st digit is year, next 3 are julian day */
			q = p/1000L;
			p = p - 1000L*q;
			return((int)p);
		} else return((int)p);
	}
}


/****************************************************************
*  time as HH:MM:SS directly updating global buffers ext->tim[9]
*  & ext->l_tim[12]
*  uses conv_bin() to convert 2 byte binary integers GECO/WESTERN
*  & DIGICON TRACE0
*  ditto conv_bcd() for PRAKLA SYN V2.3
*	st is byte offset in header
*  bp is pointer to data buffer
*****************************************************************/
void find_tm (int st, unsigned fmt, char *bp)
{
	long p;
	register int n;
	char temp[3];
	strset (ext->tim, '\0');
	strset (temp, '\0');
   for (n=0; n<3; n++) {   /* deal with HR:MN:SS first */
      if( (fmt == 6)||(fmt == 30)||(fmt == 63)||(fmt == 64)||(fmt == 71) )
         p = conv_bin(st + 2*n, 2, bp, 0);
      if(fmt == 91) p = conv_bcd(st + n, 1, bp);
      if (p > 9) {
         itoa ((int) p, temp, 10);
         if (n != 2) strcat (temp, ":");
         strcat (ext->tim, temp);
      } else {
         strcat (ext->tim, "0");
         itoa ((int) p, temp, 10);
         strcat (ext->tim, temp);
         if (n != 2) strcat (ext->tim, ":");
      }
   }
   strcpy(ext->l_tim, ext->tim);
   if( (fmt == 6)||(fmt == 30)||(fmt == 91) ) { /* add 1/00 secs if exist */
      if((fmt == 6)||(fmt == 30))
         p = conv_bin(st + 2*n, 2, bp, 0);
      if(fmt == 91) p = conv_bcd(st + n, 1, bp);
      strcat (ext->l_tim, ".");
      if (p > 9) {
         itoa ((int) p, temp, 10);
         strcat (ext->l_tim, temp);
      } else {
         strcat (ext->l_tim, "0");
         itoa ((int) p, temp, 10);
         strcat (ext->l_tim, temp);
      }
   }
   if( (fmt == 63)||(fmt == 64) || (fmt == 71) )  /* pad if no 1/00 sec */
      strcat(ext->l_tim, ".00");
}


/****************************************************************
* return a long file number
* uses conv_bcd() to convert a 2 byte bcd integer; GECO DFSFILNO
* & DIGICON TRACE0
* st is byte offset in header
* bp is pointer to data buffer
*****************************************************************/
long find_fn (int st, unsigned fmt, char *bp)
{
   if ( (fmt == 5)||(fmt == 71) )    /* GECO DFSFILNO & DIGICON TRACE0 */
      return (conv_bcd(st, 2, bp));
}


/****************************************************************
* return a long shotpoint number
* uses conv_bin() to convert a 4 byte binary integer; NOR 2 & 3
* uses conv_bcd() to convert a 4 byte bcd integer; WEST 3 & 4
* & DIGICON TRACE0 & PRAKLA SYN V2.3
* st is byte offset in header
* bp is pointer to data buffer
*****************************************************************/
long find_sp (int st, unsigned fmt, char *bp)
{
	if ((fmt == 6)||(fmt == 30))			  /* GECO NORSTAR 2 & 3	*/
		return (conv_bin(st, 4, bp, 0));
   if ((fmt == 63)||(fmt == 64)||(fmt == 71)||(fmt == 91)) /* WESTERN FMT 3 & 4 */
      return (conv_bcd(st, 4, bp));  /* or PRAKLA SYN V2.3 & TRACE0 */
}


/****************************************************************
* return a floating point water depth
* uses conv_real4() to convert a *REAL4 (NORSTAR 2 & 3)
* uses conv_bin() to convert 4 bytes binary integer (WEST FMT 3 & 4)
* uses con_bcd() to convert 3 bytes BCD (PRAKLA NAVDATA 3000)
* uses con_bcd() to convert 2 bytes BCD (DIGICON TRACE0)
* st is byte offset in header
* bp is pointer to data buffer
*****************************************************************/
float find_wat (int st, unsigned fmt, char *bp)
{
	if ((fmt == 6)||(fmt == 30))			  /* GECO NORSTAR 2 & 3 */
		return (opt->mul_wadep*(float) conv_real4(st, bp));
	if((fmt == 63)||(fmt == 64))			  /* WESTERN FORMATS 3 & 4 */
		return (opt->mul_wadep*(((float)conv_bin(st, 4, bp, 0))/10.0F));
	if(fmt == 91)								  /* PRAKLA NAVDATA 3000 */
		return(opt->mul_wadep*(((float)conv_bcd(st, 3, bp))/10.0F));
   if(fmt == 71)                         /* DIGICON TRACE0 */
      return(opt->mul_wadep*( (float) conv_bcd(st, 2, bp) ) );
}


/****************************************************************
* return a floating point system heading
* uses conv_real4() to convert a *REAL4 (NORSTAR 2 & 3)
* uses conv_bin() to convert 2 bytes binary integer (WEST FMT 3 & 4)
* ditto conv_bcd() for PRAKLA & DIGICON TRACE0
* st is byte offset in header
* bp is pointer to data buffer
*****************************************************************/
float find_hed (int st, unsigned fmt, char *bp)
{
	if ((fmt == 6)||(fmt == 30))		/* GECO NORSTAR 2 & 3 */
		return (GRAD*(float)conv_real4(st, bp));  /* RADS to Degrees */
	if((fmt == 63)||(fmt == 64))		/* WESTERN FORMATS 3 & 4 */
		return(((float) conv_bin(st, 2, bp, 0))*BIN_SEMI);
	if(fmt == 91)							/* PRAKLA NAVDATA 3000 */
		return(((float)conv_bcd(st, 2, bp))/10.0F);
   if(fmt == 71)                    /* DIGICON TRACE0 */
      return( (float) conv_bcd(st, 2, bp) );
}


/****************************************************************
* gets the line name
* st is byte offset in header
* bp is pointer to data buffer
*****************************************************************/
void find_ln (int st, unsigned fmt, char *bp, char *asc_buff)
{
   if( (fmt == 6)||(fmt == 71) )    /* norstar 2 & digicon trace0 */
      conv_ascii(st, bp, asc_buff, 8);
   if(fmt == 30)                    /* norstar 3 */
      conv_ascii(st, bp, asc_buff, 16);
}


/****************************************************************
* gets the client name
* st is byte offset in header
* bp is pointer to data buffer
*****************************************************************/
void find_cl (int st, unsigned fmt, char *bp, char *asc_buff)
{
   if((fmt == 6)||(fmt == 30))
      conv_ascii(st, bp, asc_buff, 16);
}


/****************************************************************
* gets the vessel name
* st is byte offset in header
* bp is pointer to data buffer
*****************************************************************/
void find_vs (int st, unsigned fmt, char *bp, char *asc_buff)
{
   if((fmt == 6)||(fmt == 30))   /* NORSTAR 2 & 3 */
      conv_ascii(st, bp, asc_buff, 16);
   if(fmt == 91)                 /* PRAKLA SYN V2.3 'MINTROP' */
      conv_ascii(st, bp, asc_buff, 9);   /* 9 bytes long */
}


/****************************************************************
* gets the area name
* st is byte offset in header
* bp is pointer to data buffer
*****************************************************************/
void find_ar (int st, unsigned fmt, char *bp, char *asc_buff)
{
   if((fmt == 6)||(fmt == 30))
      conv_ascii(st, bp, asc_buff, 16);
}
