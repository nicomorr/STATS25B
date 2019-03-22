/***************************************************************
*   CONVERT.C  Mathematical/Format Conversion functions
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

#include <stdlib.h>          /* Microsoft C PDS V6.00 */
#include <dos.h>
#include <math.h>
#include <string.h>

#include <stxfunct.h>        /* wcs */


/***************************************************************
*  convert n bytes of binary integers to return a Long
*  positive integer
*  bp points to the array of data and st is the byte offset
*  n is number of bytes to convert
*  d is direction (1 = LSByte first, 0 = MSByte first)
***************************************************************/
long conv_bin (int st, int n, char *bp, int d)
{
   long r = 0;   /* return value */
	unsigned char h;   /* byte value */
	register int i;        /* loop counter */
	int p = 0;    /* increment byte offset */
	if(!d) {      /* MSByte first */
		for (i=n; i>=1; i--) {
			h = *(bp + st + p);
			if (h != 0)
				r = (h*(long)pow((double)256, ((double)i-1))) + r;
			p++;
		}
		return (r);
	}
	else {        /* LSByte first */
		for (i=n; i>=1; i--) {
			h = *(bp + st + n + p);
			if (h != 0)
				r = (h*(long)pow((double)256, ((double)i-1))) + r;
			p--;
		}
		return (r);
	}
}


/***************************************************************
*  convert n bytes of BCD to return a Long positive integer
*  bp points to the array of data and st is the byte offset
*  n is the number of bytes to convert
***************************************************************/
long conv_bcd (int st, int n, char *bp)
{
   long r = 0;   /* return value */
	unsigned char h;   /* byte value */
	register int i;        /* loop counter */
	unsigned char j;   /* intermediate value */
   int p = 0;    /* increment byte offset */
	for (i=n; i>=1; i--) {
      h = *(bp + st + p);
		if ( (j = (h & 0x0f) + 10*(h >> 4) ) != 0)
			r = r +(long)((double)j*pow((double)10, 2*((double)i-1)));
	p++;
   }
   return (r);
}


/***************************************************************
*   convert HP floating point format REAL*4
*      ------------------------------------------------
*      | s |    23 bit fraction    |  7 bit exp   | s |
*      ------------------------------------------------
*      exponent & fraction in two's complement
****************************************************************/
double conv_real4 (int st, char *bp)
{
	register int i = 0;
	double signif, exp, rr;
	unsigned char b[5];
	for (i; i<4; i++)
		b[i] = *(bp + st + i);
	if (b[0] < 128)
		signif = (double)(b[0]*65536 + b[1]*256 + b[2]);
	else
		signif = (double) -(16777216 - (b[0]*65536 + b[1]*256 + b[2]));
	signif = signif*pow((double)2, (double)-23);
	if (b[3] %2)
		exp = -(double)(128 - (b[3]/2));
	else
		exp = (double) b[3]/2;
	rr = signif*pow((double)2, exp);
	return (rr);
}


/***************************************************************
*   convert HP floating point format REAL*8
*      ------------------------------------------------
*      | s |    55 bit fraction    |  7 bit exp   | s |
*      ------------------------------------------------
*      exponent & fraction in two's complement
****************************************************************
double conv_real8 (int st, char *bp)
{
	double r = 0.0F;
                         still to write (not required)
	return(r);
}
*********************/


/***************************************************************
*  convert 2 bytes of 2s complement binary integer
*     used in GUNDA & VZAD gun timing errors
***************************************************************/
conv_2comp2(char *bp)       /* convert 2 bytes 2s compl */
{
	long error;
	error = (long) (256*(*bp))+(*(bp+1));
	if(error > 32768L) error = -1L*(65536L-error);
	return((int)error);
}


/***************************************************************
*   convert 1 byte of 2s complement binary integer
*   used in VZAD gun timing errors
****************************************************************/
conv_2comp1(char *bp)
{
	long error;
	error = (long) *bp;
	if(error > 127L) error = -1L*(256L-error);
	return((int)error);
}


/****************************************************************
*   return number of bits set in a byte (for gun masks)
*****************************************************************/
unsigned bit_set(char p)
{
	register int i;
	unsigned n=0;
	for(i=0;i<8;i++) {
		if(p & 1) n++;
		p = (char) (p >> 1);
	}
	return(n);
}


/****************************************************************
*  read ASCII bytes from header into string (asc_buff)
*  bp points to the array of data and st is the byte offset
*  n is the number of bytes to convert
*****************************************************************/
void conv_ascii(int st, char *bp, char *asc_buff, int n)
{
   register int i;
   strset (asc_buff, '\0');
      for(i=0;i<n;i++, st++)
         asc_buff[i] = bp[st];
}
