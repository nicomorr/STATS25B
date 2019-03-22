/***************************************************************
*    STSGRAF1.C  graphics base file
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
#include <math.h>
#include <graph.h>
#include <malloc.h>

#include <dw.h>             /* greenleaf */
#include <dwmenu.h>
#include <ibmkeys.h>
#include <sftd.h>

#include <stats.h>          /* WCS */
#include <summ.h>
#include <strun.h>
#include <extern.h>
#include <st_graph.h>
#include <stxfunct.h>

            /* external global definitions */
extern struct ST_SUMM *sum;      /* summary */
extern struct HDR_DEF *hd;       /* header */
extern struct PRO_DEF *pd;       /* project */
extern struct OPT_DEF  *opt;     /* options */
extern struct GRAF_DEF *graf;    /* graphics */
extern struct ANAL_DEF *andef;   /* analysis */
extern struct SPEC_DEF *spc;     /* specs */
extern struct ST_XSRC *xsr;      /* source misfire/crossfire stats */
extern struct EXT_DEF *ext;      /* external variables */
extern struct PRN_DEF *prn;      /* printer */
extern char *units[];            /* FEET/METRES  PSI/BARS  etc */
extern unsigned PrntErr;         /* Printer (ScreenDump) error return */
extern char ver[];               /* HEADSTAT version */

         /* external global graphics error array pointers */
extern char *sp_err;             /* SP error array */
extern char *sp_tmp;             /* temp ditto */
extern char *fn_err;             /* FN error array */
extern char *fn_tmp;             /* temp ditto */
extern char *tm_err;             /* SP timing error array */
extern char *tm_tmp;             /* temp ditto */
extern float *c_graf1_min;       /* cable 1 min depth error array */
extern float *c_graf1_max;       /* cable 1 max ditto */
extern float *c_graf2_min;       /* 2 */
extern float *c_graf2_max;
extern float *c_graf3_min;       /* 3 */
extern float *c_graf3_max;
extern float *c_graf4_min;       /* 4 */
extern float *c_graf4_max;
extern float *c_graf1_min_tmp;   /* temp as above for cables */
extern float *c_graf1_max_tmp;
extern float *c_graf2_min_tmp;
extern float *c_graf2_max_tmp;
extern float *c_graf3_min_tmp;
extern float *c_graf3_max_tmp;
extern float *c_graf4_min_tmp;
extern float *c_graf4_max_tmp;
extern float *srce_graf;         /* source timing error array */
extern float *srce_graf_tmp;     /* ditto temp */
extern char *gmsk_graf;          /* gun mask error array */
extern char *gmsk_graf_tmp;      /* ditto temp */
extern char *seq_graf;           /* firing sequence array pointer */
extern char *seq_graf_tmp;
extern struct FIRE_BUF *fire_buf; /* hold initial fire sequence text */
extern struct FIRE_BUF *fire_buf_tmp;  /* ditto temp */
extern unsigned *man_graf;       /* manifold air error array */
extern unsigned *man_graf_tmp;   /* ditto temp */
extern unsigned *vol_graf;       /* srce volumes error array */
extern unsigned *vol_graf_tmp;   /* ditto temp */
extern float *gundep_graf_min;      /* gun depth min error array */
extern float *gundep_graf_min_tmp;  /* ditto temp */
extern float *gundep_graf_max;      /* gun depth max error array */
extern float *gundep_graf_max_tmp;  /* ditto temp */
extern struct ERR_GRAF grerr;       /* no-error graph flags */

            /* global graphics structures (ST_GRAPH.H) */
struct L_BORD limits;               /* graph border limits */
struct IN_CREM inc;                 /* incr/decr flags */
struct MAKE_GRAF mak;               /* whether graphics required */
struct REP_GRAF rep;                /* report made flag */


/****************************************************************
*   select print, screen only, printer configuration, or help
****************************************************************/
void graf_init()
{
   HWND Wtop, Wmsg;                   /* windows */
	MENUHDR *mmain;
   MENUITEM *screen_graf, *print_graf, *graph_diff, *print_diff;
   MENUITEM *select;                  /* selection pointer */
	mmain = MNUCreateHdr(POPUP);       /* create  */
	mmain->toprow = 3;
   mmain->topcol = 10;
	mmain->battr = NORML;
	mmain->fattr = EMPHNORML;
	mmain->uattr = NORML;
	mmain->sattr = REVNORML;
	mmain->dattr = NORML;
   mmain->lattr = REVERR;
	mmain->d3 = 1;                    /* turn on 3D */
	mmain->shadowchar = SHADOW3D;     /* set 3D shading % */
	mmain->shadowcor = BOTTOMRIGHT;   /* set 3D direction */
	mmain->hattr = BACK3D;            /* set 3D attribute */
      /*# scr_graf() prt_graf() ch_prn() ch_graf() gfxpr_help()
          print_help() graf_help() */
   screen_graf = MNUAddItem("Display Graphics Without Printing", NULL,
                                       'D', 0, NULL, mmain, scr_graf);
   print_graf = MNUAddItem("Print Graphics With Screen Display", NULL,
                                       'P', 0, NULL, mmain, prt_graf);
   print_diff = MNUAddItem("Change Printer Configuration", NULL,
                                       'C', 0, NULL, mmain, ch_prn);
   graph_diff = MNUAddItem("Change Graphics Configuration", NULL,
                                       'C', 0, NULL, mmain, ch_graf);
      /* the various help functions - ignore param warnings */
   screen_graf->helpfn = gfxpr_help;
   print_graf->helpfn =  gfxpr_help;
   print_diff->helpfn = print_help;
   graph_diff->helpfn = graf_help;
      /* leave menu up */
   screen_graf->leaveup = NO;
   print_graf->leaveup = NO;
   print_diff->leaveup = NO;
   graph_diff->leaveup = NO;
      /* repaint menu */
   screen_graf->repaint = NO;
   print_graf->repaint = NO;
   print_diff->repaint = NO;
   graph_diff->repaint = NO;
      /* separator lines */
   print_diff->separator = YES;
      /* top line window */
   Wtop = vcreat(1, 45, REVNORML, NO);
   vlocate(Wtop, 0, 0);
   vatputf(Wtop, 0, 0, "%-16s GRAPHICS", ver);
      /* bottom line window */
   Wmsg = vcreat(1, 80, REVNORML, NO);
   vlocate(Wmsg, 24, 0);
   vratputs(Wmsg, 0, 36, REVERR, "  F1  ");
   visible(Wmsg, YES, NO);
   visible (Wtop, YES, NO);
	do {
      select = MNUDoSelect(mmain, UpdateClockWindow);
      pcuroff();
	} while (select);
	MNUDeleteMenu(mmain);
   vdelete(Wtop, NONE);
   vdelete(Wmsg, NONE);
}


/****************************************************************
*   print graphics help function
****************************************************************/
void gfxpr_help()
{
   string_help("Printing Graphics");
}


/****************************************************************
*   graphics to screen only
****************************************************************/
void scr_graf()
{
   graf->print = 0;
   sts_grafklone();
}


/****************************************************************
*   graphics to screen & printer
****************************************************************/
void prt_graf()
{
   if (KeySure()) {
      graf->print = 1;
      sts_grafklone();
   }
}


/****************************************************************
*   make a dummy window over whole screen so that an empty
*   screen is overwritten by the graphics video mode
****************************************************************/
void sts_grafklone()
{
   HWND Wklone;
   Wklone = vcreat(25, 80, NORML, NO);
   vlocate(Wklone, 0, 0);
   visible(Wklone, YES, NO);
   sts_graf();
   vdelete(Wklone, NONE);
}



/****************************************************************
*   Run the Graphics routines
****************************************************************/
void sts_graf()
{
   register int i;
   short a = 10;      /* vertical pixel line counter */
   short c;           /* general variable */
   int sp_div;        /* modulus divisor for X axis big ticks (SPs) */
   int tic_div;       /* modulus divisor for X axis small ticks */
   unsigned key;      /* for abort if required during print */
   int num_graf = 1;  /* number of graphs to be made */
   int n_page = 0;    /* set if FF so miniheader can be made */
   int no_graf = 0;   /* number graphs without errors */
   int num = 1;       /* add one each time graph is made */
                      /* also modulus 2 graphs for a screen page */
                      /* also modulus 6 graphs for a printer formfeed */
   char t_buf[60];    /* general buffer */
   double h_mult;     /* horizontal multiplier for window */
   int maxX = 639;    /* set full screen viewport for screen dump */
   int maxY = 199;
   int minX = 0;
   int minY = 0 ;
   _heapmin();        /* defragment heap */
      /* register microsoft graphics font MODERN.FON */
   if( (c = _registerfonts("MODERN.FON")) <= 0 ) {
      if(c == 0)      /* think this is correct (undocumented??) */
         StatsErr("14002");     /* not enough memory to load font */
      else if(c == -1)
         StatsErr("14004");     /* unable to locate font 'modern.fon' */
      else if( (c == -2) || (c == -3) )
         StatsErr("14005");     /* damaged or invalid font file */
      else StatsErr("14006");   /* unknown error loading font */
      _clearscreen(_GCLEARSCREEN);
      _setvideomode(_DEFAULTMODE);   /* return to default (text) mode */
      return;
   }
      /* set CGA 640*200 BW graphics mode */
   if(!(_setvideomode(_HRESBW))) {
      _unregisterfonts();
      StatsErr("14003");
      return;
   }
   _heapmin();        /* defragment heap */
   _clearscreen(_GCLEARSCREEN);
   sp_div = (int)labs(sum->fsp-sum->lsp);  /* inital tick value */
   h_mult = ((double)sp_div)/600.0;        /* window multiplier */
         /* work out number of big ticks (SPs) to  show on axis */
   sp_div = make_sp_div(sp_div);
          /* work out small ticks on horizontal axis */
   tic_div = make_tic_div(sp_div);
          /* initialise make-graph flags */
   mak.sp = 0;             /* sp numbering */
   mak.fn = 0;             /* fn numbering */
   mak.tm = 0;             /* sp time intervals */
   mak.fr = 0;             /* fire sequence */
   mak.gm = 0;             /* gun masks */
   mak.gu = 0;             /* srce timing */
   mak.pr = 0;             /* air pressure */
   mak.vl = 0;             /* srce volumes */
   mak.tb = 0;             /* system timing */
   mak.c1 = 0;             /* cable depth 1 */
   mak.c2 = 0;             /* cable depth 2 */
   mak.c3 = 0;             /* cable depth 3 */
   mak.c4 = 0;             /* cable depth 4 */
   mak.gd = 0;             /* gun depths */
   mak.s1 = 0;             /* spare 1 */
   mak.s2 = 0;             /* spare 2 */
          /* initialise no-error line made flags */
   rep.sp = 0;             /* sp numbering */
   rep.fn = 0;             /* fn numbering */
   rep.tm = 0;             /* sp time intervals */
   rep.fr = 0;             /* fire sequence */
   rep.gm = 0;             /* gun masks */
   rep.gu = 0;             /* srce timing */
   rep.pr = 0;             /* air pressure */
   rep.vl = 0;             /* srce volumes */
   rep.tb = 0;             /* system timing */
   rep.c1 = 0;             /* cable depth 1 */
   rep.c2 = 0;             /* cable depth 2 */
   rep.c3 = 0;             /* cable depth 3 */
   rep.c4 = 0;             /* cable depth 4 */
   rep.gd = 0;             /* gun depths */
   rep.s1 = 0;             /* spare 1 */
   rep.s2 = 0;             /* spare 2 */
          /* decide whether SPs & FNs incrementing or not */
   inc.sp_inc = 0;
   inc.fn_inc = 0;
   if(sum->lsp > sum->fsp) {  /* if SPs incrementing */
      inc.fsp = sum->fsp;
      inc.lsp = sum->lsp;
      inc.sp_inc = 1;
   } else {                   /* decrement */
      inc.lsp = sum->fsp;
      inc.fsp = sum->lsp;
   }
   if(sum->lfn > sum->ffn) {  /* if FNs incrementing */
      inc.ffn = sum->ffn;
      inc.lfn = sum->lfn;
      inc.fn_inc = 1;
   } else {
      inc.lfn = sum->ffn;     /* decrement */
      inc.ffn = sum->lfn;
   }
             /* work out how many graphs there will be */
             /* & set which graphs to make flags */
      /* SP numbering errors */
   if(hd->sp && graf->sp_anl && andef->sp_anl) {
      mak.sp = 1;
      if(!grerr.sp) ++no_graf;
      else num_graf++;
   }
      /* FN numbering errors - GECO & DIGICON TRACE0 */
   if( (!hd->contr || hd->contr == 5) && hd->fn && graf->fn_anl
                                              && andef->fn_anl) {
      mak.fn = 1;
      if(!grerr.fn) ++no_graf;
      else num_graf++;
   }
      /* SP Time interval errors */
   if(hd->tm && graf->tim_anl && andef->tim_anl) {
      mak.tm = 1;
      if(!grerr.tm) ++no_graf;
      else num_graf++;
   }
      /* cable depth errors */
   if(hd->ca && ext->numberru && graf->dep_anl1 && andef->dep_anl1) {
      if (pd->loc_strm) {
         mak.c1 = 1;
         if(!grerr.c1) ++no_graf;
         else num_graf++;
      }
      if (pd->loc_strm > 1) {
         mak.c2 = 1;
         if(!grerr.c2) ++no_graf;
         else num_graf++;
      }
      if (pd->loc_strm > 2) {
         mak.c3 = 1;
         if(!grerr.c3) ++no_graf;
         else num_graf++;
      }
      if (pd->loc_strm > 3) {
         mak.c4 = 1;
         if(!grerr.c4) ++no_graf;
         else num_graf++;
      }
   }
      /* source timing errors */
   if(hd->gu && ext->numbergun && graf->gun_anl1 && andef->gun_anl1) {
      mak.gu = 1;
      if(!grerr.gu) ++no_graf;
      else num_graf++;
   }
      /* gun mask errors */
   if(!hd->contr && hd->gm && graf->gmsk_anl && andef->gmsk_anl) {
      mak.gm = 1;
      if(!grerr.gm) ++no_graf;
      else num_graf++;
   }
      /* fire sequence (initial sequence & then errors */
   if(!hd->contr && hd->gm && graf->seq_anl && andef->seq_anl) {
      num_graf++;   /* initial firing sequence always */
      mak.fr = 1;
      if(!grerr.fr) ++no_graf;
      else num_graf++;   /* graph if errors */
   }
      /* air pressure errors */
   if( (hd->contr != 2) && hd->ma && graf->man_anl1 && andef->man_anl1) {
      mak.pr = 1;
      if(!grerr.pr) ++no_graf;
      else num_graf++;
   }
      /* source volume errors */
   if(hd->gu && ext->numbergun && graf->vol_anl1 && andef->vol_anl1) {
      mak.vl = 1;
      if(!grerr.vl) ++no_graf;
      else num_graf++;
   }
      /* source depth errors */
   if(hd->gd && ext->numberdp && graf->gd_anl && andef->gd_anl) {
      mak.gd = 1;
      if(!grerr.gd) ++no_graf;
      else num_graf++;
   }
      /* system timing errors */
   if( (hd->contr != 2) && hd->clk && graf->clk_anl && andef->clk_anl) {
      mak.tb = 1;
      if(!grerr.tb) ++no_graf;
      else num_graf++;
   }
      /* add the box or boxes of 'no errors reported' */
   if(no_graf) num_graf++;   /* first 12 'no errors' - 1 box */
   if(no_graf > 12) num_graf++;  /* next 12 - 2 boxes maximum */
      /* error if no graphs selected to do */
   if(num_graf<=1) {
      ScreenErr(4);
      _unregisterfonts();
      _heapmin();        /* defragment heap */
      _clearscreen(_GCLEARSCREEN);
      _setvideomode(_DEFAULTMODE);
      return;
   }
   if(graf->print) {  /* check printer ready if print enabled */
      if(ScreenErr(1)) {
         _unregisterfonts();
         _heapmin();        /* defragment heap */
         _clearscreen(_GCLEARSCREEN);
         _setvideomode(_DEFAULTMODE);
         return;
      }
      if( (prn->prndr<2)||(prn->prndr>4) ) {   /* if an EPSON FX/MX/LQ */
         OutPrnCh('\x1b');  /* set form length in lines */
         OutPrnCh('C');
         OutPrnCh( (char)prn->lines );
         OutPrnCh('\x1b');  /* set left margin in 1/80 */
         OutPrnCh('\x6c');
         OutPrnCh( (char)prn->left );
         OutPrnCh('\x1b');  /* 1/6" line spacing */
         OutPrnCh('\x32');
         for(i=0;i<prn->top;i++) { /* CR/LF top margin */
            OutPrnCh( '\x0d' );
            OutPrnCh( '\x0a');
         }
      }
   }
   if(mousinst())
      ctlmscur(OFF);      /* turn mouse cursor OFF if installed */
/************** MAKE THE GENERAL GRAPHICS HEADER ****************/
   a = head_top(a);

/*********** DO FIRST NO-ERROR BOX IF NEEDED ********************/
   if(no_graf) {
      ++num;
      if(n_page) n_page = new_page();   /* check for new page header */
      no_errbox(a);                     /* make first no-error box */
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;   /* END FIRST NO-ERROR BOX */

/*********** DO SECOND NO-ERROR BOX IF NEEDED *******************/
   if(no_graf > 12) {
      ++num;
      if(n_page) n_page = new_page();
      no_errbox(a);                     /* make first no-error box */
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;   /* END SECOND NO-ERROR BOX */

/************************** SP STIKOGRAM  ***********************/
   if(mak.sp && grerr.sp) {
      ++num;
      inc.tsp = 0;
      _setviewport(0, a, 635, a+90);
      _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
      _setfont("t'Modern'h7w7");
      _setgtextvector(0, -1);
      _moveto_w(25, a+18);
      _outgtext("JMP");
      c = _getgtextextent("FRZ");
      _moveto_w(25, a + 73 - c);
      _outgtext("FRZ");
      SemaphoreBorder(a);
      _setfont("t'Modern'h10w10");
      _setgtextvector(1, 0);
      _moveto_w((double)inc.fsp, 2.6);
      _outgtext("SP ERRORS");
      MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,   /* X axis */
               sum->lsp, inc.fsp, inc.lsp, -2.0, -2.15, -2.3, -2.4);
         /* make the graph */
      sp_tmp = sp_err; /* set the graph pointer */
      for(i=0; i<(int)sum->act_sp; i++, inc.tsp++, sp_tmp++) {
         if(*sp_tmp == '1') SemaphoreMove(1, -1.4, inc.fsp + inc.tsp, h_mult);
         if(*sp_tmp == '2') SemaphoreMove(1, 1.4, inc.fsp + inc.tsp, h_mult);
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END SP */

/************************** FN STIKOGRAM  ***********************/
   if(mak.fn && grerr.fn) {
      ++num;
      inc.tsp=0;
      if(n_page) n_page = new_page();
      _setviewport(0, a, 635, a+90);
      _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
      _setfont("t'Modern'h7w7");
      _setgtextvector(0, -1);
      _moveto_w(25, a+18);
      _outgtext("JMP");
      c = _getgtextextent("FRZ");
      _moveto_w(25, a + 73 - c);
      _outgtext("FRZ");
      SemaphoreBorder(a);
      _setfont("t'Modern'h10w10");
      _setgtextvector(1, 0);
      _moveto_w((double)inc.ffn, 2.6);
      _outgtext("FN ERRORS");
      MakeX(sp_div, tic_div, inc.fn_inc, h_mult, sum->ffn,  /* X axis */
               sum->lfn, inc.ffn, inc.lfn, -2.0, -2.15, -2.3, -2.4);
         /* make the graph */
      fn_tmp = fn_err;
      for(i=0;i<(int)sum->act_sp;i++) {
         if(*fn_tmp == '1') SemaphoreMove(1, -1.4, inc.ffn + inc.tsp, h_mult);
         if(*fn_tmp == '2') SemaphoreMove(1, 1.4, inc.ffn + inc.tsp, h_mult);
         fn_tmp++;
         inc.tsp++;
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END FN */

/******************* SP TIMING STIKOGRAM  ***********************/
   if(mak.tm && grerr.tm) {
      ++num;
      inc.tsp=0;
      if(n_page) n_page = new_page();
      _setviewport(0, a, 635, a+90);
      _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
      _setfont("t'Modern'h7w7");
      _setgtextvector(0, -1);
      _moveto_w(25, a+18);
      _outgtext("LNG");
      c = _getgtextextent("SHT");
      _moveto_w(25, a + 73 - c);
      _outgtext("SHT");
      SemaphoreBorder(a);
      _setfont("t'Modern'h10w10");
      _setgtextvector(1, 0);
      _moveto_w((double)inc.fsp, 2.6);
      _outgtext("SP TIMING ERRORS");
      sprintf(t_buf, "SPEC: %.1f <> %.1f SECS", spc->t_min, spc->t_max);
      _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)), 2.6);
      _outgtext(t_buf);
      MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,   /* X axis */
               sum->lsp, inc.fsp, inc.lsp, -2.0, -2.15, -2.3, -2.4);
         /* make the graph */
      tm_tmp = tm_err;
      for(i=0;i<(int)sum->act_sp;i++) {
         if(*tm_tmp == '2') SemaphoreMove(1, -1.4, inc.fsp + inc.tsp, h_mult);
         if(*tm_tmp == '1') SemaphoreMove(1, 1.4, inc.fsp + inc.tsp, h_mult);
         tm_tmp++;
         inc.tsp++;
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END SP TIMING */

/******************* INITIAL FIRING SEQUENCE *****************/
   if(mak.fr) {
      ++num;
      if(n_page) n_page = new_page();
      _setviewport(34, a, 635, a+90);
      _setwindow(FALSE, 0.0, 0.0, 601.0, 60.0);
      _rectangle_w(_GBORDER, 0.0, 10.0, 601.0, 60.0 );
      _moveto_w(0.0, 20.0);
      _lineto_w(601.0, 20.0);
      _setfont("t'Modern'h10w10");
      _moveto_w( 0.0, 4.0);
      strcpy(t_buf, "INITIAL FIRING SEQUENCE");
      _outgtext(t_buf);
      _moveto_w( 10.0, 12.0);
      strcpy(t_buf, " SP     FN     TIME       SOURCE FIRED");
      _outgtext(t_buf);
      fire_buf_tmp = fire_buf;
      c = 23;
      for(i=0;i<(int)pd->seq_len;i++) {
         _moveto_w( 10.0, (double) c);
         strcpy(t_buf, fire_buf_tmp->fire_info);
         _outgtext(t_buf);
         fire_buf_tmp++;
         c += 9;
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;   /* END INITIAL FIRE SEQ */

/************* FIRING SEQUENCE STIKOGRAM ERRORS) ****************/
   if(mak.fr && grerr.fr) {
      ++num;
      inc.tsp=0;
      if(n_page) n_page = new_page();
      _setviewport(0, a, 635, a+90);
      _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
      SemaphoreBorder(a);
      _setfont("t'Modern'h10w10");
      _setgtextvector(1, 0);
      _moveto_w((double)inc.fsp, 2.6);
      _outgtext("FIRING SEQUENCE");
      strcpy(t_buf, "DOWN CHANGE/UP NEW SEQ");
      _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)), 2.6);
      _outgtext(t_buf);
      MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,   /* X axis */
               sum->lsp, inc.fsp, inc.lsp, -2.0, -2.15, -2.3, -2.4);
         /* make the graph */
      seq_graf_tmp = seq_graf;
      for(i=0;i<(int)sum->act_sp;i++) {
         if(*seq_graf_tmp == '1') SemaphoreMove(1, -1.4, inc.fsp + inc.tsp, h_mult);
         if(*seq_graf_tmp == '2') SemaphoreMove(1, 1.4, inc.fsp + inc.tsp, h_mult);
         seq_graf_tmp++;
         inc.tsp++;
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;  /* END FIRE SEQ STIKOGRAM */

/******************* GUN MASK STIKOGRAM  ***********************/
   if(mak.gm && grerr.gm) {
      ++num;
      inc.tsp=0;
      if(n_page) n_page = new_page();
      _setviewport(0, a, 635, a+90);
      _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
      SemaphoreBorder(a);
      _setfont("t'Modern'h10w10");
      _setgtextvector(1, 0);
      _moveto_w((double)inc.fsp, 2.6);
      _outgtext("GUN MASK ERRORS");
      strcpy(t_buf, "DOWN NONE/UP TOO MANY FIRED");
      _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)), 2.6);
      _outgtext(t_buf);
      MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,   /* X axis */
               sum->lsp, inc.fsp, inc.lsp, -2.0, -2.15, -2.3, -2.4);
         /* make the graph */
      gmsk_graf_tmp = gmsk_graf;
      for(i=0;i<(int)sum->act_sp;i++) {
         if(*gmsk_graf_tmp == '1') SemaphoreMove(1, -1.4, inc.fsp + inc.tsp, h_mult);
         if(*gmsk_graf_tmp == '2') SemaphoreMove(1, 1.4, inc.fsp + inc.tsp, h_mult);
         gmsk_graf_tmp++;
         inc.tsp++;
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END GUN MASK */

/******************* SOURCE TIMING VARFILL **********************/
   if(mak.gu && grerr.gu) {
      ++num;
      inc.tsp=0;
      if(n_page) n_page = new_page();
      _setviewport(0, a, 635, a+90);
      _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
      _setfont("t'Modern'h7w7");     /* put "MS" sideways */
      _setgtextvector(0, -1);
      c = _getgtextextent("MS");
      _moveto_w(25, a + 75 - c);
      _outgtext("MS");
      _setfont("t'Modern'h8w8");     /* put the unsigned mS limit at top */
      _setgtextvector(1, 0);
      _moveto_w(15.0, (double) a + 15.0);
      sprintf(t_buf, "%d", (int)graf->gun_scale);
      _outgtext(t_buf);
       LimitBorder(a, graf->gun_scale, 0.0);   /* make a limit window */
      _setfont("t'Modern'h10w10");
      _moveto_w((double)inc.fsp, 5.6*graf->gun_scale);
      if(xsr->tot_sp)                /* calculate misfire percent */
         xsr->p_spec1 = ((float)xsr->spec1)*100.0F/((float)xsr->tot_sp);
      else xsr->p_spec1 = 0.0F;
      sprintf(t_buf, "GUN TIMING ERRORS:  %.1f %%", xsr->p_spec1);
      _outgtext(t_buf);
      sprintf(t_buf, "SPEC: %.2f MSEC", spc->gun1);
      _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)),5.6*graf->gun_scale);
      _outgtext(t_buf);
      MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,   /* X axis */
               sum->lsp, inc.fsp, inc.lsp, 1.0*graf->gun_scale,
               0.85*graf->gun_scale, 0.7*graf->gun_scale, 0.6*graf->gun_scale);
         /* make the graph */
      srce_graf_tmp = srce_graf;
      for(i=0;i<(int)sum->act_sp;i++) {
         if((float)fabs((double)(*srce_graf_tmp)) > spc->gun1)
            LimitMove (fabs((double)(*srce_graf_tmp)) , inc.fsp + inc.tsp);
         srce_graf_tmp++;
         inc.tsp++;
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END SOURCE TIMING */

/******************* AIR PRESSURE VARFILL **********************/
   if(mak.pr && grerr.pr) {
      ++num;
      inc.tsp=0;
      if(n_page) n_page = new_page();
      _setviewport(0, a, 635, a+90);
      _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
      _setfont("t'Modern'h7w7");     /* put scale range sideways */
      _setgtextvector(0, -1);
      _moveto_w(20.0, (double) a + 20.0);
      sprintf(t_buf, "%u %s", graf->air_scale, units[pd->pr_un]);
      _outgtext(t_buf);
      _setgtextvector(1, 0);
       LimitBorder(a, graf->air_scale, graf->air_scale);   /* make a limit window */
      _setfont("t'Modern'h10w10");
      _moveto_w((double)inc.fsp, 5.6*graf->air_scale);
      strcpy(t_buf, "AIR PRESSURE ERRORS");
      _outgtext(t_buf);
      sprintf(t_buf, "SPEC: %u %s", spc->ma1, units[pd->pr_un]);
      _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)),5.6*graf->air_scale);
      _outgtext(t_buf);
      MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,   /* X axis */
               sum->lsp, inc.fsp, inc.lsp, 1.0*graf->air_scale,
               0.85*graf->air_scale, 0.7*graf->air_scale, 0.6*graf->air_scale);
         /* make the graph */
      man_graf_tmp = man_graf;
      for(i=0;i<(int)sum->act_sp;i++) {
         if(*man_graf_tmp)
            LimitMove ( ((double) *man_graf_tmp) - ((double) spc->ma1),
                         inc.fsp + inc.tsp);
         man_graf_tmp++;
         inc.tsp++;
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END AIR PRESSURE */

/******************* SRCE VOLUME VARFILL **********************/
   if(mak.vl && grerr.vl) {
      ++num;
      inc.tsp=0;
      if(n_page) n_page = new_page();
      _setviewport(0, a, 635, a+90);
      _setwindow(FALSE, 0.0, (double) a, 635.0, (double) a + 90.0);
      _setfont("t'Modern'h7w7");     /* put scale range sideways */
      _setgtextvector(0, -1);
      _moveto_w(20.0, (double) a + 10.0);
      sprintf(t_buf, "%u %s", graf->vol_scale, units[pd->vol_un]);
      _outgtext(t_buf);
      _setgtextvector(1, 0);
       LimitBorder(a, graf->vol_scale, graf->vol_scale);   /* make a limit window */
      _setfont("t'Modern'h10w10");
      _moveto_w((double)inc.fsp, 5.6*graf->vol_scale);
      strcpy(t_buf, "SOURCE VOLUME ERRORS");
      _outgtext(t_buf);
      sprintf(t_buf, "SPEC: %u %s", spc->gvol1, units[pd->vol_un]);
      _moveto_w( (double) inc.lsp  -
              (h_mult * (double) _getgtextextent(t_buf)),5.6*graf->vol_scale);
      _outgtext(t_buf);
      MakeX(sp_div, tic_div, inc.sp_inc, h_mult, sum->fsp,   /* X axis */
               sum->lsp, inc.fsp, inc.lsp, 1.0*graf->vol_scale,
               0.85*graf->vol_scale, 0.7*graf->vol_scale, 0.6*graf->vol_scale);
         /* make the graph */
      vol_graf_tmp = vol_graf;
      for(i=0;i<(int)sum->act_sp;i++) {
         if(*vol_graf_tmp)
            LimitMove ( ((double) *vol_graf_tmp) - ((double) spc->gvol1),
                         inc.fsp + inc.tsp);
         vol_graf_tmp++;
         inc.tsp++;
      }
      a += 95;
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END SRCE VOLUME */

/******************* SYSTEM TIMING ERRORS ***********************/
   if(mak.tb && grerr.tb) {
      ++num;
      if(n_page) n_page = new_page();
      a =  time_graf(a, h_mult, sp_div, tic_div);
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END SYSTEM TIMING */

/******************* CABLE DEPTH VARFILLS ***********************/
   if(mak.c1 && grerr.c1) {              /* CABLE 1 VARFILL */
      ++num;
      if(n_page) n_page = new_page();
      a =  cab_graf(a, 1, c_graf1_min_tmp, c_graf1_min,
               c_graf1_max_tmp, c_graf1_max, h_mult, sp_div, tic_div);
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;
   if(mak.c2 && grerr.c2) {              /* CABLE 2 VARFILL */
      ++num;
      if(n_page) n_page = new_page();
      a =  cab_graf(a, 2, c_graf2_min_tmp, c_graf2_min,
               c_graf2_max_tmp, c_graf2_max, h_mult, sp_div, tic_div);
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;
   if(mak.c3 && grerr.c3) {              /* CABLE 3 VARFILL */
      ++num;
      if(n_page) n_page = new_page();
      a =  cab_graf(a, 3, c_graf3_min_tmp, c_graf3_min,
                  c_graf3_max_tmp, c_graf3_max, h_mult, sp_div, tic_div);
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;
   if(mak.c4 && grerr.c4) {             /* CABLE 4 VARFILL */
      ++num;
      if(n_page) n_page = new_page();
      a =  cab_graf(a, 4, c_graf4_min_tmp, c_graf4_min,
               c_graf4_max_tmp, c_graf4_max, h_mult, sp_div, tic_div);
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;
       /* END CABLE DEPTHS */

/******************* SOURCE DEPTH ERRORS ***********************/
   if(mak.gd && grerr.gd) {
      ++num;
      if(n_page) n_page = new_page();
      a =  gundp_graf(a, gundep_graf_min_tmp, gundep_graf_min,
               gundep_graf_max_tmp, gundep_graf_max, h_mult, sp_div, tic_div);
      a = PrintIt(a, num, num_graf, minX, minY, maxX, maxY, &n_page);
      if(!a) goto EsCapeGraph;
   }
   if(gfkbhit()) key = vgetkey(NULLF);
   if(key == ESC) goto EsCapeGraph;      /* END SOURCE DEPTHS */

/*********************  DO TERMINAL PART *********************/
   if(!graf->print) MouseKey(1, 0);
   if ( (num%2) && graf->print) {    /* if an unfinished screen page to print */
      ScreenDump(prn->prndr, prn->res, prn->xm,
                 prn->ym, 0, prn->orient, 0,
                 minX, minY, maxX, maxY);
      if(prnerr(PrntErr)) ScreenErr(2);
   } else if (graf->print && (num%prn->page) ) OutPrnCh(12); /* else FF */
   EsCapeGraph:   /* the 'goto' label */
   if(!graf->print && (!(num%2)) ) MouseKey (1,0);
   if(graf->print) {
      if( (prn->prndr<2) || (prn->prndr>4) ) {   /* if an EPSON FX/MX/LQ */
         OutPrnCh('\x1b');  /* reset left margin */
         OutPrnCh('\x6c');
         OutPrnCh('\x00');
         OutPrnCh('\x1b');  /* 1/6" line spacing */
         OutPrnCh('\x32');
      }
   }
   _unregisterfonts();
   _heapmin();        /* defragment heap */
	_clearscreen(_GCLEARSCREEN);
   _setvideomode(_DEFAULTMODE);
   if(mousinst())
      ctlmscur(ON);      /* turn mouse cursor back ON if installed */
   _heapmin();        /* defragment heap */
}
