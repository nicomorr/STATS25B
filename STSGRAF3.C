/***************************************************************
*    STSGRAF3.C  graphics printer functions file
*
****************************************************************
*    Copyright (c) West Country Systems, England, 1991.
****************************************************************/

#include <stdio.h>         /* microsoft */
#include <string.h>
#include <graph.h>
#include <bios.h>
#include <stdlib.h>
#include <math.h>

#include <stats.h>         /* wcs */
#include <stxfunct.h>

#define ESCape  '\x1b'     /* local convenient definition */

        /* global printer variables */
int XWidth, YWidth;
unsigned PrntErr;

         /* printer initialisation codes */
char ToshResArray[4][2] = {";","\x1d",";",";"};
char EMXResArray[4][2] = {"K", "L", "Y", "Z"};
char ELQResArray[5][2] = {"\x20", "\x21", "\x26", "\x27", "\x28"};
char HPLResStringsY[4][4]  = {"75","100","150","300"};
char HPJetResStringsY[2][5]  = {"640","1280"};

         /* special comparison string def */
int HVBitMask[8] = {128,64,32,16,8,4,2,1};

         /* external global declaration */
extern struct PRN_DEF *prn;


/****************************************************************
*   main high-resolution screen dump routine
****************************************************************/
void ScreenDump (int printer, int res, float xm,float ym,
                 int rv, int orient, int ff, int minX, int minY,
                 int maxX, int maxY)
{
   PrntErr = 0;
   XWidth = maxX - minX;
   YWidth = maxY - minY;
   _setviewport(minX, minY, maxX, maxY);
   switch ( printer) {
      case 0:
         EpsonMXScreenDump (res, xm, ym, rv, orient, ff);
         break;
      case 1:
         EpsonLQScreenDump (res, xm, ym, rv, orient, ff);
         break;
      case 2:
         ToshibaPScreenDump (res, xm, ym, rv, orient, ff);
         break;
      case 3:
         HPLaserPlusScreenDump (res, xm, ym, rv, orient, ff);
         break;
      case 4:
         HPThinkJetScreenDump (res, xm, ym, rv, orient, ff);
         break;
      case 5:
         EpsonFXScreenDump (res, xm, ym, rv, orient, ff);
         break;
    }
}


/****************************************************************
*    HP LaserJetPlus routine
****************************************************************/
void HPLaserPlusScreenDump (int res, float xm, float ym,
                       int rv, int orient, int ff)
{
   int  maxX, maxY, x, y, l, ycalc;
   char lch[5], ch;
   float xinc;
   if (res>3)  res = 0;  /* max res of 3 */
   OutPrnCh(ESCape);
   OutPrnCh('*');
   OutPrnCh('t');
   OutPrnStr(HPLResStringsY[res]);
   OutPrnCh('R');
   OutPrnCh(ESCape);
   OutPrnCh('*');
   OutPrnCh('r');
   OutPrnCh('0');
   OutPrnCh('A');
   maxX = (int) floor((GetPrnMaxX(orient) + 1) * xm);
   maxY = (int) floor((GetPrnMaxY(orient) + 1) * ym);
   l = (maxX+7) / 8;
   itoa( l,lch,10);
   xinc = (float) (1.0/xm);
   for (y = 0; y < maxY; y++){
      OutPrnCh(ESCape);
      OutPrnCh('*');
      OutPrnCh('b');
      OutPrnStr(lch);
      OutPrnCh('W');
      ycalc = (int) floor(y/ym);
      x = 0;
      while ( x <= maxX -1) {
         ch = GetHorizByte(x,ycalc,xinc,rv, orient);
         OutPrnCh(ch);
         x += 8;
      }
   }
   OutPrnCh(ESCape);
   OutPrnCh('*');
   OutPrnCh('r');
   OutPrnCh('B');
   if (ff == 0)  OutPrnCh(12);
}


/****************************************************************
*    Toshiba 24 Pin routine
****************************************************************/
void ToshibaPScreenDump (int res, float xm, float ym,
                        int rv, int orient, int ff)
{
   int maxX, maxY, x, y, l, e;
   char ch, numCol[80];
   float yinc, yminc;
   if (res>1)  res = 0;     /* res 0 or 1 only */
   OutPrnCh(  ESCape );
   OutPrnCh(  'L' );
   OutPrnCh(  '0' );
   OutPrnCh(  '7' );
   y = 0;
   maxX = (int) (floor((GetPrnMaxX(orient) + 1) * xm));
   maxY = (int) (floor((GetPrnMaxY(orient) + 1) * ym));
   itoa(maxX, numCol, 10);
   yinc = 1/ym;
   while ((y < maxY) && !prnerr(PrntErr) ) {
      yminc = y*yinc;
      e = y+5;
      OutPrnCh(  ESCape);
      OutPrnStr(  ToshResArray[res] );
      if (strlen(numCol) < 4 )
         OutPrnCh(  '0');
      OutPrnStr(numCol);
      if (!(prnerr(PrntErr))) {
         for ( x = 0; x <= maxX-1; x++) {
            for (l = 0; l <= 3; l++) {
               e = y+l*6;
               ch = GetVertByte(x,e,e+5,xm,yinc,yminc, rv,orient);
               OutPrnCh(ch);
            }
         }
         if (!prnerr(PrntErr)) {
            OutPrnCh( '\x0d');
            OutPrnCh( '\x0a');
            y +=  24;
         }
      }
   }
   if (!prnerr(PrntErr)){
      OutPrnCh(  ESCape );
      OutPrnCh(  '\x1a' );
      OutPrnCh(  'I' );
      if (ff== 0) OutPrnCh(12);
   }
}


/****************************************************************
*   Epson MX routine
****************************************************************/
void EpsonMXScreenDump (int res, float xm, float ym,
                       int rv, int orient, int ff)
{
   int ilow, ihigh, maxX, maxY, x, y, e;
   char ch, low, high;
   float yinc, yminc;
   if (res>3) res = 0;   /* res 3 to 0 only */
   y = 0;
   maxX = (int) floor((GetPrnMaxX(orient) + 1) * xm);
   maxY = (int) floor((GetPrnMaxY(orient) + 1) * ym);
   ilow = maxX % 256;
   low = (char) ilow;
   ihigh = (maxX / 256 );
   high  = (char) ihigh;
   OutPrnCh(  ESCape);
   OutPrnCh(  'A');
   OutPrnCh(  '\x08');
   OutPrnCh( ESCape);
   OutPrnCh( '\x32');
   yinc = 1/ym;
   while ((y < maxY) && (!prnerr(PrntErr))) {
      yminc = y*yinc;
      e = y + 7;
      OutPrnCh(ESCape);
      OutPrnStr( EMXResArray[res]);
      OutPrnCh(low);
      OutPrnCh(high);
      if (!prnerr(PrntErr)) {
         for (x = 0;  x <= maxX-1; x++) {
            ch = GetVertByte(x,y,e,xm,yinc,yminc,rv,orient);
            OutPrnCh(ch);
         }
         if (!prnerr(PrntErr)) {
            OutPrnCh( '\x0d');
            OutPrnCh( '\x0a');
            y +=  8;
         }
      }
   }
   if (!prnerr(PrntErr)){
      if (ff== 0)  OutPrnCh(12);
   }
}


/****************************************************************
*   Epson FX routine
****************************************************************/
void EpsonFXScreenDump (int res, float xm, float ym,
                       int rv, int orient, int ff)
{
   int ilow, ihigh, maxX, maxY, x, y, e;
   char ch, low, high;
   float yinc, yminc;
   if (res>3) res = 0;  /* res 3 to 0 only */
   y = 0;
   maxX = (int) floor((GetPrnMaxX(orient) + 1) * xm);
   maxY = (int) floor((GetPrnMaxY(orient) + 1) * ym);
   ilow = maxX % 256;
   low = ((char)ilow);
   ihigh = (maxX / 256 );
   high  = ((char)ihigh);
   OutPrnCh(ESCape);
   OutPrnCh('A');
   OutPrnCh('\x08');
   yinc = 1/ym;
   while ((y < maxY) && !prnerr(PrntErr)) {
      yminc = y*yinc;
      e = y + 7;
      OutPrnCh(ESCape);
      OutPrnStr( EMXResArray[res] );
      OutPrnCh(low);
      OutPrnCh(high);
     if (!prnerr(PrntErr)) {
        for (x = 0;  x <= maxX-1; x++) {
           ch = GetVertByte(x,y,e,xm,yinc,yminc,rv,orient);
           OutPrnCh(ch);
        }
        if (!prnerr(PrntErr)) {
           OutPrnCh( '\x0d');
           OutPrnCh( '\x0a');
           y +=  8;
        }
     }
  }
  if (!prnerr(PrntErr)) {
     if (ff== 0)  OutPrnCh(12);
   }
}


/****************************************************************
*   Epson LQ routine
****************************************************************/
void EpsonLQScreenDump (int  res, float  xm, float  ym,
                       int rv, int orient, int ff)
{
   int maxX, maxY, x, y, l, ilow, ihigh;
   char ch, high, low;
   float yinc;
   float xcalc;
   if (res>8)  res = 0;  /* res 8 to 0 only */
   if (res<4)  EpsonFXScreenDump (res, xm, ym, rv, orient, ff);
   else  {
      yinc = 1/ym;
      res -= 4;
      y = 0;
      maxX = (int) floor((GetPrnMaxX(orient) + 1) * xm);
      maxY = (int) floor((GetPrnMaxY(orient) + 1) * ym);
      ilow = maxX % 256;
      low = ((char)ilow);
      ihigh = (maxX / 256 );
      high  = ((char)ihigh);
      OutPrnCh(ESCape);
      OutPrnCh('A');
      OutPrnCh('\x08');
      while ((y < maxY) && !prnerr(PrntErr)) {
         OutPrnCh(ESCape);
         OutPrnCh('*');
         OutPrnStr(ELQResArray[res]);
         OutPrnCh(low);
         OutPrnCh(high);
         if (!prnerr(PrntErr)){
            for (x = 0; x <= maxX-1; x++){
               xcalc = x/xm;
               for (l = 0; l <= 2; l++) {
                  ch = GetLQByte(y+l*8,8,xcalc,yinc,rv,orient);
                  OutPrnCh(ch);
               }
            }
         }
         if (!prnerr(PrntErr)) {
            OutPrnCh( '\x0d');
            OutPrnCh( '\x0a');
            y += 24;
         }
      }
      if(!prnerr(PrntErr)) {
         if (ff== 0) OutPrnCh(12);
      }
   }
}


/****************************************************************
*   HP ThinkJet routine
****************************************************************/
void HPThinkJetScreenDump (int res, float  xm, float  ym,
                           int rv, int orient, int ff)
{
   int  maxX, maxY, x, y, l, ycalc;
   char lch[5], ch;
   float xinc;
   if (res>1)  res = 0;   /* res 1 to 0 only */
   OutPrnCh(ESCape);
   OutPrnCh('*');
   OutPrnCh('r');
   OutPrnStr( HPJetResStringsY[res] );
   OutPrnCh('S');
   OutPrnCh(ESCape);
   OutPrnCh('*');
   OutPrnCh('r');
   OutPrnCh('A');
   maxX = (int) floor((GetPrnMaxX(orient) + 1) * xm);
   maxY = (int) floor((GetPrnMaxY(orient) + 1) * ym);
   l = (maxX+7) / 8;
   itoa( l,lch,10);
   xinc = (float) (1.0/xm);
   for (y = 0; y < maxY; y++) {
      OutPrnCh(ESCape);
      OutPrnCh('*');
      OutPrnCh('b');
      OutPrnStr(lch);
      OutPrnCh('W');
      ycalc = (int) floor(y/ym);
      x = 0;
      while (  x <= maxX-1) {
         ch = GetHorizByte(x,ycalc,xinc,rv,orient);
         OutPrnCh(ch);
         x += 8;
      }
   }
   OutPrnCh(ESCape);
   OutPrnCh('*');
   OutPrnCh('r');
   OutPrnCh('B');
   if (ff== 0)  OutPrnCh(12);
}


/****************************************************************
*   basic error checking
****************************************************************/
int prnerr (int i)
{
 if (((i&0x01)==0x01)||((i&0x08)==0x08)||((i&0x20)==0x20))
   return(1);
 else
   return(0);
}


/****************************************************************
*   Get an Epson LQ byte
****************************************************************/
char GetLQByte (int y, int numPix, float xcalc, float yinc,
                int rv, int orient)
{
   int i,e,b,l, yflr, xflr;
   char  p;
   char GetVertByteResult;
   float yminc;
   e = y + numPix - 1;
   b = 0;
   l = 0;
   xflr = (int) xcalc;
   yminc = y*yinc;
   for (i = y; i <= e; i++) {
      yflr = (int) floor(yminc);
      p = (char)(GetPrnPixel( xflr, yflr, orient) >0); /* check TYPE CONV */
      if (rv==1)  p= (char) !(p);
      if (p)  b = (b | (HVBitMask[l]));
      l += 1;
      yminc += yinc;
   }
   GetVertByteResult = ( (char) b);
   return(GetVertByteResult);
}


/****************************************************************
*   sort out MaxX for portrait or landscape
****************************************************************/
int GetPrnMaxX (int orient)
{
   if (orient==1)
      return(YWidth);
   else
      return(XWidth);
}


/****************************************************************
*   sort out MaxY for portrait or landscape
****************************************************************/
int GetPrnMaxY (int orient)
{
   if (orient==1)
      return(XWidth);
   else
      return(YWidth);
}


/****************************************************************
*   get printer pixel
****************************************************************/
int GetPrnPixel (int x, int y, int orient)
{
   int xval;
   if (orient == 1) {
      xval = XWidth-y;
      if (xval >= 0)
         return(_getpixel(xval,x));
      else
         return(0);
   }
   else
      return(_getpixel(x,y));
}


/****************************************************************
*   send a printer character
****************************************************************/
void OutPrnCh(char ch)
{
   PrntErr = _bios_printer(_PRINTER_WRITE, prn->ioport, ch);
}


/****************************************************************
*   send a printer string
****************************************************************/
void OutPrnStr(char *s)
{
   int i,l;
   char ss[255];
   strcpy( ss,s);
   l = strlen(ss);
   for (i=0; i< l; i++)
      OutPrnCh(ss[i]);
}


/****************************************************************
*   get vertical byte
****************************************************************/
char GetVertByte (int x, int y, int e, float xm, float yinc, float ystart,
		 int rv, int orient)
{
   int i, b=0, l=0, xcalc, yflr;
   char  p, GetVertByteResult;
   float yminc;
   yminc = ystart;
   xcalc = (int) floor(x/xm);
   for (i = y; i <= e; i++) {
      yflr = (int) floor(yminc);
      p = (char)  (GetPrnPixel( xcalc, yflr, orient) >0);
      if (rv==1)  p = (char) !(p);
      if (p)  b = (b | (HVBitMask[l]));
      l += 1;
      yminc += yinc;
   }
   GetVertByteResult = ( (char) b);
   return(GetVertByteResult);
}


/****************************************************************
*   get horizontal byte
****************************************************************/
char GetHorizByte (int x, int ycalc, float xinc, int rv, int orient)
{
   int i, result, xflr;
   char p, GetHorizByteResult;
   float xminc;
   result = 0;
   xminc = x * xinc;
   for ( i = 0; i <= 7; i++)  {
      xflr = (int) floor(xminc);
      p = (char) (GetPrnPixel( xflr, ycalc, orient) > 0);
      if (rv==1)  p = (char) !(p);
      if (p)  result = (result | (HVBitMask[i]));
      xminc += xinc;
   }
   GetHorizByteResult =( (char) result);
   return( GetHorizByteResult);
}
