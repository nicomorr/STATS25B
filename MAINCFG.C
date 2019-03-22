/**************************************************************
*   MAINCFG.C   master configuration
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
* NOTE FORMAT NUMBERS - Contractor Numbers:
*      0  ...............  GECO
*      1  ...............  PRAKLA
*      2  ...............  WESTERN
*      3  ...............  SSL
*      4  ...............  CGG
*      5  ...............  DIGICON
*      6  ...............  HGS
*                       Format Numbers
*     0  ................  FORMAT NOT AVAILABLE/NOT USED
*     1  - 60              GECO  (variable formats)
*     61 - 70              WESTERN
*                            61 & 62 Formats 1 & 2 NOT IMPLEMENTED
*                            63  Format 3 - 385 byte (nom 384)
*                            64  Format 4 - 641 byte (nom 640)
*     71 - 80              DIGICON
*                            71  Trace0 Format - 2048 byte
*                                (current 7/91)
*     81 - 90              HGS   (not implemented yet)
*     91 - 100             PRAKLA
*                            91  SYN V2.3 - 512 byte
*                                max twin cable (32 depths)
*                                multisource VZAD 60 guns max
*                                Mintrop 7/91
****************************************************************
*  (c) West Country Systems Ltd, Cyprus, 1991.
****************************************************************/

#include <stdio.h>         /* microsoft */

#include <dw.h>            /* greenleaf */
#include <dwmenu.h>
#include <sftd.h>
#include <ibmkeys.h>

#include <extern.h>        /* wcs */
#include <stats.h>
#include <stxfunct.h>		/* function prototypes */

				/* external global declarations */
extern struct HDR_DEF *hd; 	  /* header list pointer */
extern struct PRO_DEF *pd; 	  /* pointer to project list */
extern struct EXT_DEF *ext;	  /* external variables */

				/* GECO formats */
char *formats[] = {
	"NONE",                /* format 0 */
	"FREETEXT",            /* code 1 */
	"JOBCONFG",            /* code 2 */
	"NESFILNO",
	"DFSFILNO 1",          /* code 4 */
	"DFSFILNO 2",
	"NORSTAR 2",           /* code 6 */
	"MAGNAVOX",
	"LITTON 1",
	"LITTON 2/3",          /* code 9 */
	"GUNDA 1",
	"GUNDA 2",             /* code 11 */
	"DC292 1",
	"DC292 2",
	"CLS20",
	"SYNTRAK",
	"DC293",               /* code 16 */
	"ATLAS",
	"GM2B 1/2",            /* code 18 */
	"GM2B 3/4",
	"PLL",
	"PULSECLK",
	"TAILBUOY",
	"GUNDEPTH",
	"CLS12",
	"MAGGIE",
	"RS_3D",
	"MANIFOLD",            /* code 27 */
	"HPR_MSS",             /* code 28  */
	"RSLITTON",
	"NORSTAR 3",			  /* code 30 */
	'\0',
};

			/* units selection */
char *units[] = {
   "FEET",            /*  =0  */
   "METRES",
   "PSI",
   "BARS",
   "CU/INS",
   "CU/CMS",
   "KNOTS",
   "M/SEC",           /*  =7  */
	'\0'
};


/*************************************************************
*	 main configuration menu - called from STS.C
**************************************************************/
maincfg (void)
{
	HWND		Wmsg;
	MENUHDR	*mmain;
	MENUITEM *header, *project, *source, *cable, *wing, *gundepth, *printer, /* top */
		*cheader, *lheader, *sheader,					/* header */
		*select_contr, 									/* contractor */
		*cproject, *lproject, *sproject, 			/* project */
		*vsource, *add_source, *del_source, *lsource, *ssource, /* sources */
		*vcable, *add_cable, *del_cable, *lcable, *scable, 	  /* cables */
		*vwing, *add_wing, *del_wing, *lwing, *swing,   /* wing angles */
		*vgundepth, *add_gundepth, *del_gundepth, *lgundepth, *sgundepth, /* gun depths */
		*cprinter, *lprinter, *sprinter; 					/* printer */
	MENUITEM *con_geco, *con_western, *con_prakla, *con_ssl,
				*con_cgg, *con_digicon, *con_hgs;			/* contractors */
	MENUITEM *fmt_west_1, *fmt_west_2, *fmt_west_3, *fmt_west_4;

	mmain = MNUCreateHdr(PULLDOWN);       /* create  */
	mmain->toprow = 0;
	mmain->spaces = 4;
	mmain->poprows = 0;
	mmain->popcols = 0;
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
	mmain->mouseselect = YES;
	mmain->downindicator = NO;
				/* TOPLEVEL MENU ITEMS */
	header = MNUAddItem("HEADER", NULL, 'H', 0, NULL, mmain, NULLF);
	project = MNUAddItem("PROJECT", NULL, 'P', 0, NULL, mmain, NULLF);
	source = MNUAddItem("SOURCE", NULL, 'S', 0, NULL, mmain, NULLF);
	cable = MNUAddItem("CABDEPTH", NULL, 'C', 0, NULL, mmain, NULLF);
	wing = MNUAddItem("WINGANGLE", NULL, 'W', 0, NULL, mmain, NULLF);
	gundepth = MNUAddItem("GUNDEPTH", NULL, 'G', 0, NULL, mmain, NULLF);
	printer = MNUAddItem("PRINTER", NULL, 'P', 0, NULL, mmain, NULLF);
				/* set toplevel items to pulldown automatically */
	header->popnext = YES;
	project->popnext = YES;
	source->popnext = YES;
	cable->popnext = YES;
	wing->popnext = YES;
	gundepth->popnext = YES;
	printer->popnext = YES;
				/* set repaint off for toplevel items */
	header->repaint = NO;
	project->repaint = NO;
	source->repaint = NO;
	cable->repaint = NO;
	wing->repaint = NO;
	gundepth->repaint = NO;
   printer->repaint = NO;
            /* set item help function */
      /*# head_help() pro_help() srce_help() cab_help()
          wing_help() gundp_help() print_help() */
   header->helpfn = head_help;            /* ignore param warning */
   project->helpfn = pro_help;            /* ignore param warning */
   source->helpfn = srce_help;            /* ignore param warning */
   cable->helpfn = cab_help;              /* ignore param warning */
   wing->helpfn = wing_help;              /* ignore param warning */
   gundepth->helpfn = gundp_help;         /* ignore param warning */
   printer->helpfn = print_help;          /* ignore param warning */
            /* SECOND LEVEL MENU ITEMS */
      /*# set_ch_head() set_load_hdr() save_hdr() */
cheader = MNUAddItem("View/Change Header", NULL, 'V', 0, header, mmain, set_ch_head);
lheader = MNUAddItem("Load Header", NULL, 'L', 0, header, mmain, set_load_hdr);
sheader = MNUAddItem("Save Header", NULL, 'S', 0, header, mmain, save_hdr);
select_contr = MNUAddItem("Select Contractor", NULL, 'S', 0, header, mmain, NULLF);
	select_contr->popnext = YES;
	select_contr->separator = YES;
   cheader->leaveup = NO;
   cheader->repaint = NO;
   cheader->helpfn = head_help;           /* ignore param warning */
   lheader->helpfn = head_help;           /* ignore param warning */
   sheader->helpfn = head_help;           /* ignore param warning */
   select_contr->helpfn = head_help;      /* ignore param warning */
      /*# set_pro() set_load_pro() save_pro() */
cproject = MNUAddItem("View/Change Project", NULL, 'V', 0, project, mmain, set_pro);
lproject = MNUAddItem("Load Project", NULL, 'L', 0, project, mmain, set_load_pro);
sproject = MNUAddItem("Save Project", NULL, 'S', 0, project, mmain, save_pro);
	cproject->leaveup = NO;
   cproject->repaint = NO;
   cproject->helpfn = pro_help;           /* ignore param warning */
   lproject->helpfn = pro_help;           /* ignore param warning */
   sproject->helpfn = pro_help;           /* ignore param warning */
      /*# set_view_gun() set_add_gun() set_del_gun() set_load_gun()
          set_save_gun() */
vsource = MNUAddItem("View Source", NULL, 'V', 0, source, mmain, set_view_gun);
add_source = MNUAddItem("Add a Gun", NULL, 'A', 0, source, mmain, set_add_gun);
del_source = MNUAddItem("Delete a Gun", NULL, 'D', 0, source, mmain, set_del_gun);
lsource = MNUAddItem("Load Source", NULL, 'L', 0, source, mmain, set_load_gun);
ssource = MNUAddItem("Save Source", NULL, 'S', 0, source, mmain, set_save_gun);
	vsource->leaveup = NO;
	add_source->leaveup = NO;
	del_source->leaveup = NO;
	vsource->repaint = NO;
	add_source->repaint = NO;
   del_source->repaint = NO;
   vsource->helpfn = srce_help;           /* ignore param warning */
   add_source->helpfn = srce_help;        /* ignore param warning */
   del_source->helpfn = srce_help;        /* ignore param warning */
   lsource->helpfn = srce_help;           /* ignore param warning */
   ssource->helpfn = srce_help;           /* ignore param warning */
      /*# set_view_cab() set_add_cab() set_del_cab() set_load_cab()
          set_save_cab() */
vcable = MNUAddItem("View Depth", NULL, 'V', 0, cable, mmain, set_view_cab);
add_cable = MNUAddItem("Add a Depth", NULL, 'A', 0, cable, mmain, set_add_cab);
del_cable = MNUAddItem("Delete a Depth", NULL, 'D', 0, cable, mmain, set_del_cab);
lcable = MNUAddItem("Load Depth", NULL, 'L', 0, cable, mmain, set_load_cab);
scable = MNUAddItem("Save Depth", NULL, 'S', 0, cable, mmain, set_save_cab);
	vcable->leaveup = NO;
	add_cable->leaveup = NO;
	del_cable->leaveup = NO;
	vcable->repaint = NO;
	add_cable->repaint = NO;
   del_cable->repaint = NO;
   vcable->helpfn = cab_help;             /* ignore param warning */
   add_cable->helpfn = cab_help;          /* ignore param warning */
   del_cable->helpfn = cab_help;          /* ignore param warning */
   lcable->helpfn = cab_help;             /* ignore param warning */
   scable->helpfn = cab_help;             /* ignore param warning */
      /*# set_view_wing() set_add_wing() set_del_wing() set_load_wing()
          set_save_wing() */
vwing = MNUAddItem("View Wing Angle", NULL, 'V', 0, wing, mmain, set_view_wing);
add_wing = MNUAddItem("Add a Wing Angle", NULL, 'A', 0, wing, mmain, set_add_wing);
del_wing = MNUAddItem("Delete a Wing Angle", NULL, 'D', 0, wing, mmain, set_del_wing);
lwing = MNUAddItem("Load Wing Angle", NULL, 'L', 0, wing, mmain, set_load_wing);
swing = MNUAddItem("Save Wing Angle", NULL, 'S', 0, wing, mmain, set_save_wing);
	vwing->leaveup = NO;
	add_wing->leaveup = NO;
	del_wing->leaveup = NO;
	vwing->repaint = NO;
	add_wing->repaint = NO;
   del_wing->repaint = NO;
   vwing->helpfn = wing_help;             /* ignore param warning */
   add_wing->helpfn = wing_help;          /* ignore param warning */
   del_wing->helpfn = wing_help;          /* ignore param warning */
   lwing->helpfn = wing_help;             /* ignore param warning */
   swing->helpfn = wing_help;             /* ignore param warning */
      /*# set_view_gundp() set_add_gundp() set_del_gundp() set_load_gundp()
          set_save_gundp() */
vgundepth = MNUAddItem("View Gundepth", NULL, 'V', 0, gundepth, mmain, set_view_gundp);
add_gundepth = MNUAddItem("Add a Gundepth", NULL, 'A', 0, gundepth, mmain, set_add_gundp);
del_gundepth = MNUAddItem("Delete a Gundepth", NULL, 'D', 0, gundepth, mmain, set_del_gundp);
lgundepth = MNUAddItem("Load Gundepth", NULL, 'L', 0, gundepth, mmain, set_load_gundp);
sgundepth = MNUAddItem("Save Gundepth", NULL, 'S', 0, gundepth, mmain, set_save_gundp);
	vgundepth->leaveup = NO;
	add_gundepth->leaveup = NO;
	del_gundepth->leaveup = NO;
	vgundepth->repaint = NO;
	add_gundepth->repaint = NO;
   del_gundepth->repaint = NO;
   vgundepth->helpfn = gundp_help;        /* ignore param warning */
   add_gundepth->helpfn = gundp_help;     /* ignore param warning */
   del_gundepth->helpfn = gundp_help;     /* ignore param warning */
   lgundepth->helpfn = gundp_help;        /* ignore param warning */
   sgundepth->helpfn = gundp_help;        /* ignore param warning */
      /*# ch_prn() set_load_prn() save_prn() */
cprinter = MNUAddItem("View/Change Printer", NULL, 'V', 0, printer, mmain, ch_prn);
lprinter = MNUAddItem("Load Printer", NULL, 'L', 0, printer, mmain, set_load_prn);
sprinter = MNUAddItem("Save Printer", NULL, 'S', 0, printer, mmain, save_prn);
	cprinter->leaveup = YES;
	cprinter->repaint = YES;
   cprinter->helpfn = print_help;         /* ignore param warning */
   lprinter->helpfn = print_help;         /* ignore param warning */
   sprinter->helpfn = print_help;         /* ignore param warning */
         /* third level menu items */
      /*# sel_geco() set_sel_prakla() NonImpl() set_sel_digicon() */
con_geco = MNUAddItem("Geco", NULL, 'G', 0, select_contr, mmain, sel_geco);
con_western = MNUAddItem("Western", NULL, 'W', 0, select_contr, mmain, NULLF);
con_prakla = MNUAddItem("Prakla", NULL, 'P', 0, select_contr, mmain, set_sel_prakla);
con_ssl = MNUAddItem("SSL", NULL, 'S', 0, select_contr, mmain, NonImpl);
con_cgg = MNUAddItem("CGG", NULL, 'C', 0, select_contr, mmain, NonImpl);
con_digicon = MNUAddItem("Digicon", NULL, 'D', 0, select_contr, mmain, set_sel_digicon);
con_hgs = MNUAddItem("HGS", NULL, 'H', 0, select_contr, mmain, NonImpl);
      /*# head_help_gec() head_help_wes() head_help_pra()
          head_help_ssl() head_help_cgg() head_help_dig()
          head_help_hgs() */
   con_geco->helpfn = head_help_gec;      /* ignore param warning */
   con_western->helpfn = head_help_wes;   /* ignore param warning */
   con_prakla->helpfn = head_help_pra;    /* ignore param warning */
   con_ssl->helpfn = head_help_ssl;       /* ignore param warning */
   con_cgg->helpfn = head_help_cgg;       /* ignore param warning */
   con_digicon->helpfn = head_help_dig;   /* ignore param warning */
   con_hgs->helpfn = head_help_hgs;       /* ignore param warning */
      /* fourth level menu items */
   /*# set_sel_west3() set_sel_west4() NonImpl() */
fmt_west_1 = MNUAddItem("Format 1", NULL, '1', 0, con_western, mmain, NonImpl);
fmt_west_2 = MNUAddItem("Format 2", NULL, '2', 0, con_western, mmain, NonImpl);
fmt_west_3 = MNUAddItem("Format 3", NULL, '3', 0, con_western, mmain, set_sel_west3);
fmt_west_4 = MNUAddItem("Format 4", NULL, '4', 0, con_western, mmain, set_sel_west4);
   fmt_west_1->helpfn = head_help_wes;     /* ignore param warning */
   fmt_west_2->helpfn = head_help_wes;     /* ignore param warning */
   fmt_west_3->helpfn = head_help_wes;     /* ignore param warning */
   fmt_west_4->helpfn = head_help_wes;     /* ignore param warning */
		/* make the HELP message window */
	Wmsg = vcreat(1, 80, REVNORML, NO);
	vlocate(Wmsg, 24, 0);
	vratputs(Wmsg, 0, 36, REVERR, "  F1  ");
	visible (Wmsg, YES, NO);
	while(MNUDoSelect(mmain, UpdateClockWindow));
	MNUDeleteMenu(mmain);
	vdelete(Wmsg, NONE);
	return (0);
}


/*************************************************************
*	 set which contractor & format to view
**************************************************************/
void set_ch_head ()
{
   unsigned i=0;
	switch(hd->contr) {
      case 0:                   /* GECO */
         while(formats[i]) i++;
         if(hd->sp >= i)        /* basic corrupted format check */
				StatsErr("23001");
			ch_head();
			break;
		case 1:						  /* PRAKLA */
         v_fixed_fmt("PRAKLA SYN V2.3 TWIN CABLE/MULTI SOURCE - 512 BYTE");
			break;
		case 2:						  /* WESTERN */
			switch(hd->west) {
				case 2:				  /* Fmt 3 */
               v_fixed_fmt("WESTERN FORMAT 3 - 385 BYTE");
					break;
				case 3:				  /* Fmt 4 */
               v_fixed_fmt("WESTERN FORMAT 4 - 641 BYTE");
					break;
				default:
					NonImpl();
					break;
			}
			break;
      case 5:                   /* DIGICON */
         v_fixed_fmt("DIGICON TRACE0 - 2048 BYTE");
			break;
		default:
			NonImpl();
			break;
	}
}


/*************************************************************
*	 load header configuration
**************************************************************/
void set_load_hdr()
{
   load_hdr(1);             /* load from disk */
}


/*************************************************************
*	 load project configuration
**************************************************************/
void set_load_pro()
{
   load_pro(1);
}


/*************************************************************
*   load printer configuration
**************************************************************/
void set_load_prn()
{
   load_prn(1);
}

/*************************************************************
*	 view source configuration
**************************************************************/
void set_view_gun()
{
	view_gun(ext->numbergun, hd->gu, hd->gu_num);
}


/*************************************************************
*	 add a gun
**************************************************************/
void set_add_gun()
{
	ext->numbergun = add_gun(ext->numbergun, hd->gu, hd->gu_num);
}


/*************************************************************
*	 delete a gun
**************************************************************/
void set_del_gun()
{
	ext->numbergun = del_gun(ext->numbergun);
}


/*************************************************************
*	 save source configuration
**************************************************************/
void set_save_gun()
{
	save_gun(ext->numbergun);
}


/*************************************************************
*	 load source configuration
**************************************************************/
void set_load_gun()
{
	ext->numbergun = load_gun(1);
}


/*************************************************************
*	 view cable configuration
**************************************************************/
void set_view_cab()
{
	view_cab(ext->numberru, hd->ca, hd->ca_num);
}


/*************************************************************
*	 add an RU
**************************************************************/
void set_add_cab()
{
	ext->numberru = add_cab(ext->numberru, hd->ca_num);
}


/*************************************************************
*	 delete an RU
**************************************************************/
void set_del_cab()
{
	ext->numberru = del_cab(ext->numberru);
}


/*************************************************************
*	 save cable configuration
**************************************************************/
void set_save_cab()
{
	save_cab(ext->numberru);
}


/*************************************************************
*	 load cable configuration
**************************************************************/
void set_load_cab()
{
	ext->numberru	= load_cab(1);
}


/*************************************************************
*   view wing configuration
**************************************************************/
void set_view_wing()
{
   view_wing(ext->numberwing, hd->wi, hd->wi_num);
}


/*************************************************************
*   add a wing
**************************************************************/
void set_add_wing()
{
   ext->numberwing = add_wing(ext->numberwing, hd->wi_num);
}


/*************************************************************
*   delete a wing
**************************************************************/
void set_del_wing()
{
   ext->numberwing = del_wing(ext->numberwing);
}


/*************************************************************
*   save wing configuration
**************************************************************/
void set_save_wing()
{
   save_wing(ext->numberwing);
}


/*************************************************************
*   load wing configuration
**************************************************************/
void set_load_wing()
{
   ext->numberwing  = load_wing(1);
}


/*************************************************************
*	 view gundepth configuration
**************************************************************/
void set_view_gundp()
{
	view_gundp(ext->numberdp, hd->gd, hd->gd_num);
}


/*************************************************************
*	 add a gundepth
**************************************************************/
void set_add_gundp()
{
	ext->numberdp = add_gundp(ext->numberdp, hd->gd_num);
}


/*************************************************************
*	 delete a gundepth
**************************************************************/
void set_del_gundp()
{
	ext->numberdp = del_gundp(ext->numberdp);
}


/*************************************************************
*	 save gundepth configuration
**************************************************************/
void set_save_gundp()
{
	save_gundp(ext->numberdp);
}


/*************************************************************
*	 load gundepth configuration
**************************************************************/
void set_load_gundp()
{
	ext->numberdp	= load_gundp(1);
}


/*************************************************************
*	 select GECO
**************************************************************/
void sel_geco ()
{
   if(!KeySure()) return;
   geco_init();
}


/*************************************************************
*   set select PRAKLA
**************************************************************/
void set_sel_prakla()
{
	sel_prakla(1);
}


/*************************************************************
*   select PRAKLA SYN V2.3  512 byte
*   maximum twin streamer/twin source (Mintrop 7/91)
**************************************************************/
void sel_prakla (int i)
{
	if(i)
		if(!KeySure()) return;
	hd->contr = 1; 			/* PRAKLA = 1 */
   hd->prak = 0;           /* SYN V2.3 */
			/* hardcode header configuration */
   hd->h_len = 512;
	hd->sp = 91;
   hd->sp_ofst = 35;
   hd->fn = 0;          /* no FN in format */
   hd->fn_ofst = 0;
   hd->jd = 0;          /* no JD (year/month/day - may add later?) */
   hd->jd_ofst = 0;
	hd->tm = 91;
   hd->tm_ofst = 5;
   hd->ln = 0;          /* no line name in format */
   hd->ln_ofst = 0;
   hd->ar = 0;          /* no area in format */
   hd->ar_ofst = 0;
   hd->cl = 0;          /* no client in format */
   hd->cl_ofst = 0;
	hd->vs = 91;
   hd->vs_ofst = 299;   /* hardcoded as 'MINTROP' but read anyway */
	hd->ca = 91;
   hd->ca_num = 32;     /* twin cables - 32 depths max */
   hd->ca_ofst = 105;   /* note split cable blocks */
	hd->gu = 91;
   hd->gu_num = 60;     /* maximum 60 guns - twin sources */
   hd->gu_ofst = 308;   /* note single continuous gun block */
   hd->gm = 0;          /* no gun masks or status read at present */
	hd->gm_ofst = 0;
	hd->ma = 91;
   hd->ma_ofst = 271;
   hd->gs = 0;          /* no speed this format */
   hd->gs_ofst = 0;
	hd->wa = 91;
   hd->wa_ofst = 32;
	hd->gd = 91;
   hd->gd_ofst = 428;
   hd->gd_num = 32;     /* max 32 gun depths */
	hd->syhd = 91;
   hd->syhd_ofst = 30;
   hd->clk = 91;        /* three FTB clocks */
   hd->clk_ofst = 492;
   hd->wi = 0;          /* no wing angles this format */
   hd->wi_ofst = 0;
   hd->wi_num = 0;
}


/*************************************************************
*	 set select WESTERN Fmt 3
**************************************************************/
void set_sel_west3 ()
{
	sel_west3(1);
}


/*************************************************************
*	 select WESTERN Fmt 3
**************************************************************/
void sel_west3 (int i)
{
	if(i)
		if(!KeySure()) return;
	hd->contr = 2; 			/* WESTERN = 2 */
	hd->west = 2;				/* Format 3 */
			/* hardcode header configuration */
	hd->h_len = 385;
	hd->sp = 63;
	hd->sp_ofst = 132;
	hd->fn = 0;
	hd->fn_ofst = 0;
	hd->jd = 63;
	hd->jd_ofst = 136;
	hd->tm = 63;
	hd->tm_ofst = 148;
	hd->ln = 0;
	hd->ln_ofst = 0;
	hd->ar = 0;
	hd->ar_ofst = 0;
	hd->cl = 0;
	hd->cl_ofst = 0;
	hd->vs = 0;
	hd->vs_ofst = 0;
	hd->ca = 63;
	hd->ca_num = 40;
	hd->ca_ofst = 260;
	hd->gu = 63;
	hd->gu_num = 128;
	hd->gu_ofst = 4;
   hd->gm = 0;         /* will be 63 when RDC analysis done */
   hd->gm_ofst = 372;  /* correct for start of status words */
	hd->ma = 0;
	hd->ma_ofst = 0;
	hd->gs = 0;
	hd->gs_ofst = 0;
	hd->wa = 63;
	hd->wa_ofst = 156;
	hd->gd = 63;
	hd->gd_ofst = 512;
	hd->gd_num = 64;
	hd->syhd = 63;
	hd->syhd_ofst = 154;
	hd->clk = 0;
   hd->clk_ofst = 0;
   hd->wi = 0;
   hd->wi_ofst = 0;
   hd->wi_ofst = 0;
}


/*************************************************************
*	 set select WESTERN FMT 4
**************************************************************/
void set_sel_west4 ()
{
	sel_west4(1);
}


/*************************************************************
*	 select WESTERN FMT 4
**************************************************************/
void sel_west4 (int i)
{
	if(i)
		if(!KeySure()) return;
	hd->contr = 2; 			/* WESTERN = 2 */
	hd->west = 3;				/* Format 4 */
			/* hardcode header configuration */
	hd->h_len = 641;
	hd->sp = 64;
	hd->sp_ofst = 132;
	hd->fn = 0;
	hd->fn_ofst = 0;
	hd->jd = 64;
	hd->jd_ofst = 136;
	hd->tm = 64;
	hd->tm_ofst = 148;
	hd->ln = 0;
	hd->ln_ofst = 0;
	hd->ar = 0;
	hd->ar_ofst = 0;
	hd->cl = 0;
	hd->cl_ofst = 0;
	hd->vs = 0;
	hd->vs_ofst = 0;
	hd->ca = 64;
	hd->ca_num = 40;
	hd->ca_ofst = 260;
	hd->gu = 64;
	hd->gu_num = 128;
	hd->gu_ofst = 4;
   hd->gm = 0;         /* will be 64 when RDC analysis written */
	hd->gm_ofst = 372;
	hd->ma = 0;
	hd->ma_ofst = 0;
	hd->gs = 0;
	hd->gs_ofst = 0;
	hd->wa = 64;
	hd->wa_ofst = 156;
	hd->gd = 64;
	hd->gd_ofst = 512;
	hd->gd_num = 64;
	hd->syhd = 64;
	hd->syhd_ofst = 154;
	hd->clk = 0;
   hd->clk_ofst = 0;
   hd->wi = 0;
   hd->wi_ofst = 0;
   hd->wi_ofst = 0;
}

/*************************************************************
*   set select DIGICON
**************************************************************/
void set_sel_digicon()
{
   sel_digicon(1);
}


/*************************************************************
*   select DIGICON TRACE0 - 2048 Byte
**************************************************************/
void sel_digicon (int i)
{
	if(i)
		if(!KeySure()) return;
   hd->contr = 5;          /* DIGICON = 5 */
   hd->digi = 0;           /* TRACE0 = 0 */
			/* hardcode header configuration */
   hd->h_len = 2048;      /* header length fixed at 2048 */
   hd->sp = 71;           /* SP number */
   hd->sp_ofst = 244;
   hd->fn = 71;           /* FN number */
   hd->fn_ofst = 242;
   hd->jd = 71;           /* julian day */
   hd->jd_ofst = 268;
   hd->tm = 71;           /* SP time */
   hd->tm_ofst = 270;
   hd->ln = 71;           /* line name */
   hd->ln_ofst = 248;
   hd->ar = 0;            /* NO AREA DEFINED THIS FORMAT */
   hd->ar_ofst = 0;
   hd->cl = 0;            /* NO CLIENT DEFINED THIS FORMAT */
   hd->cl_ofst = 0;
   hd->vs = 0;            /* NO VESSEL DEFINED THIS FORMAT */
   hd->vs_ofst = 0;
   hd->ca = 71;           /* cable depths */
   hd->ca_num = 70;       /* max number depth sensors */
   hd->ca_ofst = 320;
   hd->gu = 71;           /* guns */
   hd->gu_num = 126;      /* max number guns */
   hd->gu_ofst = 470;
   hd->gm = 0;            /* NO GUN MASK IMPLEMENTED */
   hd->gm_ofst = 0;       /* maybe later from remote fire status ?? */
   hd->ma = 71;           /* source pressure */
   hd->ma_ofst = 298;
   hd->gs = 71;           /* speed */
   hd->gs_ofst = 302;
   hd->wa = 71;           /* water depth */
   hd->wa_ofst = 276;
   hd->gd = 71;           /* gun depth sensors */
   hd->gd_ofst = 460;
   hd->gd_num = 8;        /* max number gun depth sensors */
   hd->syhd = 71;         /* heading */
   hd->syhd_ofst = 294;
   hd->clk = 71;          /* TB echo timing */
   hd->clk_ofst = 978;
   hd->wi = 0;            /* NO WING ANGLES THIS FORMAT */
   hd->wi_ofst = 0;
   hd->wi_num = 0;
}


/*************************************************************
*   view a fixed format
**************************************************************/
void v_fixed_fmt (char *arg)
{
   HWND Wmsg;
   Wmsg = vcreat (1, 59, NORML, NO);
   vlocate (Wmsg, 4, 5);
   vframe (Wmsg, EMPHNORML, FRSINGLE);
   vtitle (Wmsg, EMPHNORML, " Current Format ");
	vshadow(Wmsg, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
	vatputf (Wmsg, 0, 0, "%s", arg);
	vwind (Wmsg, 1, strlen(arg), 0, 0);
   visible (Wmsg, YES, NO);
   MouseKey(1, 1);
	vdelete (Wmsg, NONE);
}

/*************************************************************
*   initialise GECO formats & offsets to 0
*      otherwise garbage may be displayed on initial config
**************************************************************/
void geco_init()
{
   hd->contr = 0;          /* GECO = 0 */
   hd->h_len = 0;
   hd->sp = 0;
   hd->sp_ofst = 0;
   hd->fn = 0;
   hd->fn_ofst = 0;
   hd->jd = 0;
   hd->jd_ofst = 0;
   hd->tm = 0;
   hd->tm_ofst = 0;
   hd->ln = 0;
   hd->ln_ofst = 0;
   hd->ar = 0;
   hd->ar_ofst = 0;
   hd->cl = 0;
   hd->cl_ofst = 0;
   hd->vs = 0;
   hd->vs_ofst = 0;
   hd->ca = 0;
   hd->ca_num = 0;
   hd->ca_ofst = 0;
   hd->gu = 0;
   hd->gu_num = 0;
   hd->gu_ofst = 0;
	hd->gm = 0;
	hd->gm_ofst = 0;
   hd->ma = 0;
   hd->ma_ofst = 0;
   hd->gs = 0;
   hd->gs_ofst = 0;
   hd->wa = 0;
   hd->wa_ofst = 0;
	hd->gd = 0;
	hd->gd_ofst = 0;
	hd->gd_num = 0;
   hd->syhd = 0;
   hd->syhd_ofst = 0;
   hd->clk = 0;
   hd->clk_ofst = 0;
   hd->wi = 0;
   hd->wi_num = 0;
	hd->wi_ofst = 0;
	hd->ws = 0;
	hd->ws_ofst = 0;
	hd->gy = 0;
	hd->gy_ofst = 0;
}


/*************************************************************
*   Geco formats help function
**************************************************************/
void head_help_gec()
{
   string_help("Geco Formats 1");
}


/*************************************************************
*   Western formats help function
**************************************************************/
void head_help_wes()
{
   string_help("Western Formats");
}


/*************************************************************
*   Prakla formats help function
**************************************************************/
void head_help_pra()
{
   string_help("Prakla Formats");
}


/*************************************************************
*   SSL formats help function
**************************************************************/
void head_help_ssl()
{
   string_help("SSL Formats");
}


/*************************************************************
*   CGG formats help function
**************************************************************/
void head_help_cgg()
{
   string_help("CGG Formats");
}


/*************************************************************
*   Digicon formats help function
**************************************************************/
void head_help_dig()
{
   string_help("Digicon Formats");
}


/*************************************************************
*   HGS (GSI) formats help function
**************************************************************/
void head_help_hgs()
{
   string_help("HGS Formats");
}
