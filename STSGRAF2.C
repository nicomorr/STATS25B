/***************************************************************
*    STSGRAF2.C  graphics screen functions file
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

#include <stdio.h>          /* microsoft */
#include <stdlib.h>
#include <bios.h>
#include <math.h>
#include <graph.h>

#include <dw.h>             /* greenleaf */
#include <ibmkeys.h>

#include <stats.h>          /* WCS */
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>

            /* external global definitions */
extern struct HDR_DEF *hd;       /* header */
extern struct ST_SUMM *sum;      /* summary */
extern struct PRO_DEF *pd;       /* project */
extern struct EXT_DEF *ext;      /* external variables */
extern struct GRAF_DEF *graf;    /* graphics */
extern struct SPEC_DEF *spc;     /* specs */
extern struct PRN_DEF *prn;      /* printer */
extern char ver[];               /* HEADSTAT version */
extern char *units[];            /* FEET/METRES  PSI/BARS  etc */
extern unsigned PrntErr;         /* Printer (ScreenDump) error return */
extern float *tim_graf;          /* system timing error array */
extern float *tim_graf_tmp;      /* ditto temp */

            /* external global graphics structures (ST_GRAPH.H) */
extern struct L_BORD limits;            /* graph border limits */
extern struct IN_CREM inc;              /* incr/decr flags */
extern struct REP_GRAF rep;             /* report made flag */
extern struct ERR_GRAF grerr;           /* no-error graph flags */
extern struct MAKE_GRAF mak;            /* whether graphics required */


/****************************************************************
*   make a border for semaphore type graph
****************************************************************/
SemaphoreBorder(short a)
{
   _setviewport(34, a, 635, a+90);
   _setwindow(TRUE, (double) inc.fsp, -3.0, (double) inc.lsp, 3.0);
   _rectangle_w(_GBORDER, (double) inc.fsp, -2.0,
                          (double) inc.lsp, 2.0);
   _moveto_w((double)inc.fsp, 0.0);
   _lineto_w((double)inc.lsp, 0.0);
   return(1);
}


/****************************************************************
*   do a semaphore flag
*   int i is the linestyle
****************************************************************/
void SemaphoreMove(int i, double y, double sp, double mult)
{
    if(i == 1) _setlinestyle(0xffff);   /* solid line */
   _moveto_w (sp, 0.0);
   _lineto_w (sp, y);
   if(mult) {    /* if not the zero flag (0.0) */
      _moveto_w (sp+mult, 0.0);   /* make it 2 pixels wide */
      _lineto_w (sp+mult, y);
   }
}


/****************************************************************
*   make a border for a fill or linetype graph
****************************************************************/
LimitBorder(short a, double scale, double line)
{
   _setviewport(34, a, 635, a+90);
   _setwindow(TRUE, (double) inc.fsp, limits.wl = 0.0,
                    (double) inc.lsp, limits.wu = scale*6.0);
   _rectangle_w(_GBORDER, (double) inc.fsp, limits.rl = scale,
                          (double) inc.lsp, limits.ru = scale*5.0);
   _moveto_w((double)inc.fsp, limits.sx = line*4.0 +limits.rl);
   _lineto_w((double)inc.lsp, limits.sx);
   return(1);
}


/****************************************************************
*   make a fill/line for a fill type graph
****************************************************************/
void LimitMove(double y, double sp)
{
    y = y*4.0;
   _moveto_w (sp, limits.sx);
    if (y + limits.sx < limits.rl) _lineto_w (sp, limits.rl);
    else if(y + limits.sx > limits.ru) _lineto_w(sp, limits.ru);
    else  _lineto_w (sp, y + limits.sx);
}


/****************************************************************
*   do cable depths graph
****************************************************************/
cab_graf(short a, short b, float *min_tmp1, float *min1,
              float *max_tmp1, float *max1, double h_mult,
              int sp_div, int tic_div)
{
   register int i;
   char t_buf[50], buf[12];
   inc.tsp = 0;
   if(b==1) strcpy(buf, pd->locab1);
   if(b==2) strcpy(buf, pd->locab2);
   if(b==3) strcpy(buf, pd->locab3);
   if(b==4) strcpy(buf, pd->locab4);
   _setfont("t'Modern'h8w8");
   _setviewport(0, a, 635, a+90);
   _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
   _moveto_w(5.0, (double) a + 15.0);
   sprintf(t_buf, "%d", -(int)(graf->cab_scale - graf->cab_line));
   _outgtext(t_buf);
   sprintf(t_buf, "+%d", (int)graf->cab_line);
   _moveto_w(5.0, (double) a + 70.0);
   _outgtext(t_buf);
   _setfont("t'Modern'h10w10");
    LimitBorder(a, graf->cab_scale, graf->cab_line);
   _moveto_w((double)inc.fsp, 5.6*graf->cab_scale);
   sprintf(t_buf, "%s CABLE DEPTH ERRORS", buf);
   _outgtext(t_buf);
   sprintf(t_buf, "SPEC: %.1f <> %.1f %s", spc->d1min, spc->d1max,
                     units[pd->loc_dp]);
   _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)),5.6*graf->cab_scale);
   _outgtext(t_buf);
   MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,  /* X axis */
            sum->lsp, inc.fsp, inc.lsp, 1.0*graf->cab_scale,
            0.85*graf->cab_scale, 0.7*graf->cab_scale, 0.6*graf->cab_scale);
      /* make the graph */
   min_tmp1 = min1;
   max_tmp1 = max1;
   for(i=0;i<(int)sum->act_sp;i++) {
      if(*min_tmp1 < spc->d1min)
         LimitMove((double) (spc->d1min - *min_tmp1), inc.fsp +inc.tsp);
      if(*max_tmp1 > spc->d1max)
         LimitMove((double) (spc->d1max - *max_tmp1), inc.fsp + inc.tsp);
      min_tmp1++;
      max_tmp1++;
      inc.tsp++;
   }
   return(a+=(95));
}


/****************************************************************
*   do source depths graph
****************************************************************/
gundp_graf(short a, float *min_tmp1, float *min1,
              float *max_tmp1, float *max1, double h_mult,
              int sp_div, int tic_div)
{
   register int i;
   char t_buf[50];
   inc.tsp = 0;
   _setfont("t'Modern'h8w8");
   _setviewport(0, a, 635, a+90);
   _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
   _moveto_w(5.0, (double) a + 15.0);
   sprintf(t_buf, "%d", -(int)(graf->gud_scale - graf->gud_line));
   _outgtext(t_buf);
   sprintf(t_buf, "+%d", (int)graf->gud_line);
   _moveto_w(5.0, (double) a + 70.0);
   _outgtext(t_buf);
   _setfont("t'Modern'h10w10");
    LimitBorder(a, graf->gud_scale, graf->gud_line);
   _moveto_w((double)inc.fsp, 5.6*graf->gud_scale);
   sprintf(t_buf, "SOURCE DEPTH ERRORS");
   _outgtext(t_buf);
   sprintf(t_buf, "SPEC: %.1f <> %.1f %s", spc->gd_min, spc->gd_max,
                     units[pd->loc_dp]);
   _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)),5.6*graf->gud_scale);
   _outgtext(t_buf);
   MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,  /* X axis */
            sum->lsp, inc.fsp, inc.lsp, 1.0*graf->gud_scale,
            0.85*graf->gud_scale, 0.7*graf->gud_scale, 0.6*graf->gud_scale);
      /* make the graph */
   min_tmp1 = min1;
   max_tmp1 = max1;
   for(i=0;i<(int)sum->act_sp;i++) {
      if(*min_tmp1 < spc->gd_min)
         LimitMove((double) (spc->gd_min - *min_tmp1), inc.fsp +inc.tsp);
      if(*max_tmp1 > spc->gd_max)
         LimitMove((double) (spc->gd_max - *max_tmp1), inc.fsp + inc.tsp);
      min_tmp1++;
      max_tmp1++;
      inc.tsp++;
   }
   return(a+=(95));
}


/****************************************************************
*   do system timing graph
****************************************************************/
time_graf(short a, double h_mult, int sp_div, int tic_div)
{
   register int i;
   char t_buf[50];
   inc.tsp = 0;
   _setfont("t'Modern'h8w8");
   _setviewport(0, a, 635, a+90);
   _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
   _moveto_w(2.0, (double) a + 15.0);
   sprintf(t_buf, "-%.1f", 0.5*graf->tim_scale );
   _outgtext(t_buf);
   sprintf(t_buf, "+%.1f", 0.5*graf->tim_scale);
   _moveto_w(2.0, (double) a + 70.0);
   _outgtext(t_buf);
   _setfont("t'Modern'h10w10");
    LimitBorder(a, graf->tim_scale, 0.5*graf->tim_scale);
   _moveto_w((double)inc.fsp, 5.6*graf->tim_scale);
   _outgtext("SYSTEM TIMING ERRORS");
   sprintf(t_buf, "SPEC: %.1f +/- %.1f MSEC", spc->clk, spc->clk_ra);
   _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)),5.6*graf->tim_scale);
   _outgtext(t_buf);
   MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,  /* X axis */
            sum->lsp, inc.fsp, inc.lsp, 1.0*graf->tim_scale,
            0.85*graf->tim_scale, 0.7*graf->tim_scale, 0.6*graf->tim_scale);
      /* make the graph */
   tim_graf_tmp = tim_graf;
   for(i=0;i<(int)sum->act_sp;i++) {
      if(*tim_graf_tmp) {
         if(*tim_graf_tmp < (spc->clk - spc->clk_ra))
            LimitMove((double) (spc->clk - spc->clk_ra  - *tim_graf_tmp), inc.fsp +inc.tsp);
         if(*tim_graf_tmp > spc->clk + spc->clk_ra)
            LimitMove((double) (spc->clk + spc->clk_ra - *tim_graf_tmp), inc.fsp + inc.tsp);
      }
      tim_graf_tmp++;
      inc.tsp++;
   }
   return(a+=(95));
}


/****************************************************************
*   make an interval tick for semaphore graphs
****************************************************************/
void Int_Tick (double sp, double tik, double down)
{
   _moveto_w (sp, tik);
   _lineto_w (sp, down);
}


/****************************************************************
*   Screen error messages
*   in the case of printer messages, PrntErr is used as a flag
*   from screendump routines, so this function asks for
*   printer status & checks it
****************************************************************/
ScreenErr(int i)
{
   char view_buf[61];     /* viewing buffer */
   int a = 1;             /* flag for adding "<ESC>ABORTS" etc */
   long image_size;       /* size of image to save */
   char *image_buffer = NULL;   /* pointer to saved image */
   unsigned key;          /* keystroke */
   strcpy(view_buf, "PRINTER ");
   switch (i) {
      case 1:             /* PRINTER NOT READY */
         PrntErr = _bios_printer( _PRINTER_STATUS, prn->ioport, 0 );
         if((PrntErr & 0x29) || !(PrntErr & 0x80) || !(PrntErr & 0x10)) {
            strcat(view_buf, "NOT READY?");
            break;
         }
      case 2:             /* VARIOUS PRINTER ON-LINE ERRORS */
         if(PrntErr&0x01) {
            strcat(view_buf, "TIMEOUT");
            break;
         }
         else if(PrntErr&0x08) {
            strcat(view_buf, "I/O");
            break;
         }
         else if(PrntErr&0x20) {
            strcat(view_buf, "OUT OF PAPER");
            break;
         }
         else return(0);
      case 4:             /* do not run if no graphs were requested */
         strcpy(view_buf,
            "NO GRAPHS REQUESTED (F8 MAIN MENU) - <ESC> RETURNS");
         a=0;
         break;
   }
   if(a) strcat(view_buf, "  <ESC> ABORT/<CR> CONTINUES");
   image_size = _imagesize(8, 175, 8*(short)strlen(view_buf) +16, 191);
   image_buffer = (char *) malloc((int)image_size);
   _getimage(8, 175, 8*(short)strlen(view_buf) +16, 191, image_buffer);
   _settextposition(23, 2);
   _outtext(view_buf);
   do {
      key = vgetkey(NULLF);
   } while ((key != ESC)&&(key != CR));
   _putimage(8, 175, image_buffer, _GPSET);
   if(image_buffer) free (image_buffer);
   if(key == CR) return(0);
   else return (1);
}


/****************************************************************
*   print it (high-res screen dump) & handle pagination etc
****************************************************************/
short PrintIt(short a, int num, int num_graf,
                 int minX, int minY, int maxX, int maxY, int *n_page)
{
   register int i;
   if(!(num%2)) {
      if(!graf->print) {
         if(num < num_graf) {
            MouseKey(1, 0);
         } else {
           return(0);
         }
      } else {
         ScreenDump(prn->prndr, prn->res, prn->xm,
                    prn->ym, 0, prn->orient, num%prn->page,
                    minX, minY, maxX, maxY);
         if(!(num%prn->page)) {  /* if at page end (after FF) */
            *n_page = 1;
            if( (prn->prndr<2)||(prn->prndr>4) ) { /* if EPSON FX/MX/LQ */
               OutPrnCh('\x1b');  /* set 1/6" line spacing */
               OutPrnCh('\x32');
               for(i=0;i<prn->top;i++) { /* do CR/LF top margin */
                  OutPrnCh( '\x0d' );
                  OutPrnCh( '\x0a');
               }
            }
         } else *n_page = 0;
         if(prnerr(PrntErr)) {
            if(ScreenErr(2)) return(0);
         }
      }
      _clearscreen(_GCLEARSCREEN);
      a = 10;
   }
   if(mousinst())
      ctlmscur(OFF);      /* turn mouse cursor OFF if installed */
   return(a);
}


/***************************************************************
*   make the scale with numbers & big & small ticks
*   fxn is either FSP or FFN and lxn is LSP or LFN
*   fxp is either FSP or FFN pos and lxp is LSP or LFN pos
*   inx is 1 if incrementing & 0 if decrementing
****************************************************************/
void MakeX(int sp_div, int tic_div, int inx, double h_mult,
               long fxn, long lxn, long fxp, long lxp,
               double base, double small, double large, double number)
{
   register int i, n = 0;
   int count = 0;
   short c, l_ext = 0, r_ext = 0;
   long x, sp;
   double x_pos;
   char t_buf[7];
   sp = fxn;  /* set the first SP (or FN) number */
   for(i=0;i<(int)sum->act_sp;i++) {  /* get number of SPs to be labelled */
      if(!((int)sp%sp_div)) count++;
      if(inx) sp++;
      else sp--;
   }
   l_ext = _getgtextextent(ltoa((long)fxn, t_buf, 10));  /* label 1st */
   _moveto_w((double)fxp, number);
   _outgtext(ltoa((long)fxn, t_buf, 10));
   r_ext = _getgtextextent(ltoa((long)lxn, t_buf, 10));  /* label last */
   _moveto_w((double)lxp - h_mult * (double) r_ext, number);
   _outgtext(ltoa((long)lxn, t_buf, 10));
   sp = fxn;   /* reset the first SP number */
   for(i=0, x=0; i<(int)sum->act_sp; i++, x++) {
      if(!((int)sp%tic_div))            /* make a small tick at tic_div */
         Int_Tick((double)(fxp + x), base, small);
      if(!((int)sp%sp_div)) {           /* make a big tick at sp_div */
         Int_Tick((double)(fxp + x), base, large);
         c = _getgtextextent(ltoa(sp, t_buf, 10));
         x_pos = (double)(fxp + x) - h_mult*(double)(c/2);
         if( (n>1) && (n < (count-1)) ) {   /* if NOT first 2 or last 2 */
            _moveto_w(x_pos, number);
            _outgtext(ltoa(sp, t_buf, 10));
            n++;
         }
            /* calculate at first two intervals whether space enough */
         if(n<2) {
            if( ( (double)fxp + h_mult*((double)l_ext + 5.0) ) < x_pos) {
               _moveto_w(x_pos, number);
               _outgtext(ltoa(sp, t_buf, 10));
            }
            n++;
         }
            /* calculate at last two intervals whether space enough */
         if ( (n==count) || (n==(count-1)) ) {
            if( (x_pos + h_mult*(double)c) <
                       ( (double)lxp - h_mult*((double)r_ext + 5.0) ) ) {
               _moveto_w(x_pos, number);
               _outgtext(ltoa(sp, t_buf, 10));
            }
            n++;
         }
      }
      if(inx) sp++;
      else sp--;
   }
}


/****************************************************************
*   write a little header if it is a new page after FF
****************************************************************/
int new_page()
{
   char t_buf[60];
   _setviewport(34, 0, 635, 21);
   _setwindow(FALSE, 0.0, 0.0, 600.0, 20.0);
   _setfont("t'Modern'h10w10");
   _setgtextvector(1, 0);
   _rectangle_w(_GBORDER, 0.0, 0.0, 600.0, 11.5);
      /* line ID */
   _moveto_w(10.0, 2.0);
   sprintf(t_buf, "ID:  %s", sum->comment);
   _outgtext(t_buf);
      /* file name */
   sprintf(t_buf, "DATA FILE:  %s", ext->pname);
   _moveto_w(590.0 - (double) _getgtextextent(t_buf), 2.0);
   _outgtext(t_buf);
   return (0);
}


/****************************************************************
*    work out number of big ticks (SPs) to  show on axis
****************************************************************/
make_sp_div(int sp_div)
{
   if(sp_div <= 10) sp_div = 2;
   else if(sp_div <= 40) sp_div = 5;
   else if(sp_div <= 80) sp_div = 10;
   else if(sp_div <= 160) sp_div = 20;
   else if(sp_div <= 320) sp_div = 40;
   else if(sp_div <= 400) sp_div = 50;
   else if(sp_div <= 800) sp_div = 100;
   else if(sp_div <= 1200) sp_div = 150;
   else if(sp_div <= 1600) sp_div = 200;
   else if(sp_div <= 2000) sp_div = 250;
   else if(sp_div <= 2400) sp_div = 300;
   else if(sp_div <= 2800) sp_div = 350;
   else if(sp_div <= 3200) sp_div = 400;
   else if(sp_div <= 3600) sp_div = 450;
   else if(sp_div <= 4000) sp_div = 500;
   else if(sp_div <= 4400) sp_div = 550;
   else if(sp_div <= 4800) sp_div = 600;
   else if(sp_div <= 5200) sp_div = 650;
   else if(sp_div <= 5600) sp_div = 700;
   else if(sp_div <= 6000) sp_div = 750;
   else sp_div = 2000;
   return(sp_div);
}

/****************************************************************
*   work out small ticks on horizontal axis
****************************************************************/
make_tic_div(int sp_div)
{
   int tic_div;
   if(sp_div <= 5) tic_div = 1;
   else if(sp_div <= 20) tic_div = 5;
   else if(sp_div <= 50) tic_div = 10;
   else if(sp_div <= 100) tic_div = 20;
   else if(sp_div <= 150) tic_div = 25;
   else if(sp_div <= 200) tic_div = 50;
   else if(sp_div <= 500) tic_div = 100;
   else if(sp_div <= 750) tic_div = 200;
   else tic_div = 1000;
   return (tic_div);
}


/****************************************************************
*   make the header top
****************************************************************/
short head_top (short a)
{
   char t_buf[60];
   _setviewport(34, a+10, 635, a+90);
   _setwindow(FALSE, 0.0, 0.0, 600.0, 80.0);
   _setfont("t'Modern'h10w10");
   _rectangle_w(_GBORDER, 0.0, 0.0, 600.0, 80.0);
   _moveto_w(0.0, 14.0);
   _lineto_w(600.0, 14.0);
      /* line ID */
   _moveto_w(10.0, 3.0);
   sprintf(t_buf, "ID:  %s", sum->comment);
   _outgtext(t_buf);
      /* file name */
   sprintf(t_buf, "DATA FILE:  %s", ext->pname);
   _moveto_w(590.0 - (double) _getgtextextent(t_buf), 3.0);
   _outgtext(t_buf);
      /* client */
   _moveto_w(10.0, 17.0);
   strcpy(t_buf, pd->cli_name);
   _outgtext(t_buf);
      /* project */
   strcpy(t_buf, pd->pro_name);
   _moveto_w(590.0 - (double) _getgtextextent(t_buf), 17.0);
   _outgtext(t_buf);
      /* contractor */
   _moveto_w(10.0, 29.0);
   strcpy(t_buf, pd->con_name);
   _outgtext(t_buf);
      /* vessel */
   strcpy(t_buf, pd->qc_com1);
   _moveto_w(590.0 - (double) _getgtextextent(t_buf), 29.0);
   _outgtext(t_buf);
      /*  SP range */
  _moveto_w(10.0, 41.0);
   sprintf(t_buf, "SP RANGE: %ld - %-ld", sum->fsp, sum->lsp);
   _outgtext(t_buf);
      /* FN range */
   if(hd->fn) {
      sprintf(t_buf, "FN RANGE: %ld - %-ld", sum->ffn, sum->lfn);
      _moveto_w(590.0 - (double) _getgtextextent(t_buf), 41.0);
      _outgtext(t_buf);
   }
      /* processed */
  _moveto_w(10.0, 53.0);
   sprintf(t_buf, "TOTAL PROCESSED: %ld", sum->procno);
   _outgtext(t_buf);
      /* corrupt */
   sprintf(t_buf, "TOTAL CORRUPT: %ld", sum->currno);
   _moveto_w(590.0 - (double) _getgtextextent(t_buf), 53.0);
   _outgtext(t_buf);
      /* ECL Headstat */
   sprintf(t_buf, "INTERA ECL PETROLEUM TECHNOLOGIES: %s", ver);
   _moveto_w(300.0 - (double) (_getgtextextent(t_buf)/2), 67.0);
   _outgtext(t_buf);
   a += 95; /* END HEADER */
   return (a);
}


/****************************************************************
*   make a no-error box if graph requested but no errors found
****************************************************************/
void no_errbox (short a)
{
   double y = 13.0;   /* vertical text position */
   int num_rep = 0;    /* 6 maximum per box */
   int x_pos = 0;      /* 0 for left & 1 for right */
   char t_buf[50];
   _setviewport(34, a, 635, a+90);
   _setwindow(FALSE, 0.0, 0.0, 601.0, 60.0);
   _rectangle_w(_GBORDER, 0.0, 10.0, 601.0, 60.0 );
   _moveto_w(0.0, 3.5);
   _outgtext("VALUES CHECKED:  NO ERRORS FOUND");
   if(mak.sp && !grerr.sp && !rep.sp) {
      _moveto_w(10.0, y);
      _outgtext("SP NUMBERING");
      x_pos = !x_pos;
      rep.sp = 1;
      num_rep++;
   }
   if(mak.fn && !grerr.fn && !rep.fn) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      _outgtext("FN NUMBERING");
      x_pos = !x_pos;
      rep.fn = 1;
      num_rep++;
   }
   if(mak.tm && !grerr.tm && !rep.tm) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "SP TIMES %.1f/%.1f SECS", spc->t_min, spc->t_max);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.tm = 1;
      num_rep++;
   }
   if(mak.fr && !grerr.fr && !rep.fr) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      _outgtext("FIRING SEQUENCE");
      x_pos = !x_pos;
      rep.fr = 1;
      num_rep++;
   }
   if(mak.gm && !grerr.gm && !rep.gm) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      _outgtext("GUN MASKS");
      x_pos = !x_pos;
      rep.gm = 1;
      num_rep++;
   }
   if(mak.gu && !grerr.gu && !rep.gu) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "SOURCE TIMING %.2f MSEC", spc->gun1);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.gu = 1;
      num_rep++;
   }
   if(mak.pr && !grerr.pr && !rep.pr) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "AIR PRESSURE %u %s", spc->ma1, units[pd->pr_un]);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.pr = 1;
      num_rep++;
   }
   if(mak.vl && !grerr.vl && !rep.vl) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "SOURCE VOLUME %u %s", spc->gvol1, units[pd->vol_un]);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.vl = 1;
      num_rep++;
   }
   if(mak.tb && !grerr.tb && !rep.tb) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "SYS TIMING %.1f +/- %.1f MS",
                 spc->clk, spc->clk_ra);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.tb = 1;
      num_rep++;
   }
   if(mak.c1 && !grerr.c1 && !rep.c1) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "%s CABDEP %.1f/%.1f",
               pd->locab1, spc->d1min, spc->d1max);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.c1 = 1;
      num_rep++;
   }
   if(mak.c2 && !grerr.c2 && !rep.c2) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "%s CABDEP %.1f/%.1f",
               pd->locab2, spc->d1min, spc->d1max);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.c2 = 1;
      num_rep++;
   }
   if(mak.c3 && !grerr.c3 && !rep.c3) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "%s CABDEP %.1f/%.1f",
               pd->locab3, spc->d1min, spc->d1max);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.c3 = 1;
      num_rep++;
   }
   if(num_rep >= 12) return;
   if(mak.c4 && !grerr.c4 && !rep.c4) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "%s CABDEP %.1f/%.1f",
               pd->locab4, spc->d1min, spc->d1max);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.c4 = 1;
      num_rep++;
   }
   if(num_rep >= 12) return;
   if(mak.gd && !grerr.gd && !rep.gd) {
      if(x_pos) _moveto_w(310.0, y);
      else _moveto_w(10.0, y = y + 7.3);
      sprintf(t_buf, "SOURCE DEPTH %.1f/%.1f", spc->gd_min , spc->gd_max);
      _outgtext(t_buf);
      x_pos = !x_pos;
      rep.gd = 1;
      num_rep++;
   }
}
