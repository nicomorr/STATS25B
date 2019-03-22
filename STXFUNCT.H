/***************************************************************
*  STXFUNCT.H    Contains function prototypes
****************************************************************
*
*
*  Compiler:  MSC V6.00  /AL /W4 /Os /Gs /Zp /Zl /G2
*  Linker:    MSC V      /
*  Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWL.LIB
*             Greenleaf Superfunctions (MSC) V1.00 GSFL.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*  (c) West Country Systems Ltd, Cyprus, 1991.
****************************************************************/

            /* some release/debug etc options */
#define SECURITY	0			/* Invoke MicroPhar Dongle Security */
#define FORCEDIR  0        /* Force use of \ECLSTAT directory */
#define RELEASE	1			/* Change to 1 for Debugging program */
#define DOS       1        /* DOS operating system */
#define MSC       1        /* Microsoft C Compiler */

#ifdef RELEASE
	#undef DEBUG
#endif

#ifndef TRANSACTION       /* for datawindows converted functs */
	#include <stdio.h>
	#include <dw.h>
	#include <dwdata.h>
#endif

#ifndef MENUITEM          /* for calling help with ->item0 */
	#include <stdio.h>
	#include <dw.h>
	#include <dwmenu.h>
#endif

#ifndef MSF_FLAGS         /* define a misfire flags structure */
typedef struct MSF_FLAGS {
	unsigned s1_efl, s2_efl, s3_efl, s4_efl;   /* source enabled */
	unsigned s1_mfl, s2_mfl, s3_mfl, s4_mfl;   /* source NOF/AUT */
	unsigned s1_1fl, s2_1fl, s3_1fl, s4_1fl;   /* source spec 1 */
	unsigned s1_2fl, s2_2fl, s3_2fl, s4_2fl;   /* source spec 2 */
};
#endif

#ifndef NEWATTR         /* new attribute table (STS.C) */
	#define BACK3D 17					/* 3D shading */
	#define YELGRN 18					/* Yellow/Green */
	#define BLINKBLUEWH 19			/* blinking */
   #define SHADOW3D SHADOW25     /* 25% shadow for 3D effects */
#endif

            /* file search structure definition */
typedef struct FNAME {
   char f[70];   /* long enough to hold comment & date fields as well */
   struct FNAME *next;    /* pointer to next file on list */
};

				/* function prototypes CABCFG.C */
void view_cab(int, unsigned, unsigned);   /* view cable depth cfg */
int add_cab(int,unsigned);                /* add a cable depth */
int del_cab(int);                         /* delete a cable depth */
void save_cab(int);                       /* save cable depth cfg */
int load_cab(int);                        /* load cable depth cfg */
struct CAB_DEF *findcab(char *);          /* find cable depth name */
void cab_store(struct CAB_DEF *, struct CAB_DEF **,
               struct CAB_DEF **);        /* linked list store */
int defldval_cab (TRANSACTION *);         /* field validate */
int defldval_cabdel (void);               /* field validate */
void cab_help(void);                      /* cable depth help screen */

			/* function prototypes CONVERT.C */
long conv_bin (int, int, char *, int);   /* convert binary integer */
long conv_bcd (int, int, char *);        /* convert BCD */
double conv_real4 (int, char *);         /* convert REAL*4  */
double conv_real8 (int, char *);         /* convert REAL*8  */
int conv_2comp2(char *);                 /* convert 2 bytes 2s compl */
int conv_2comp1(char *);                 /* convert 1 byte 2s compl */
unsigned bit_set (char);					  /* return bits set in a byte */
void conv_ascii(int, char *, char *, int);  /* read ASCII string */

				/* function prototypes DATAFUN1.C */
int EscapeKey(TRANSACTION *);        /* ESCape key function */
unsigned KeySim(unsigned (*)(), TRANSACTION *);  /* a push function*/
int detranval_glob (TRANSACTION *);  /* global validation function */

			/* function prototypes FINDSP1.C */
int runfind (int);                       /* do run time summary */
void go_help(void);                      /* run time help screen */

			/* function prototypes FINDSP2.C */
int find_spr (FILE *, int);				  /* find a SP range */
void view_spr (void);                    /* view selected range */

			/* function prototypes FUNCTS.C */
void find_tm (int, unsigned, char *);   /* puts time in tim[9] */
int find_jd (int , unsigned , char *);  /* returns the julian date */
long find_fn (int, unsigned, char *);   /* returns the filenumber */
long find_sp (int, unsigned, char *);   /* returns the shotpoint number */
float find_wat (int, unsigned, char *); /* ditto water depth */
float find_hed (int, unsigned, char *); /* ditto system heading */
void find_ln (int, unsigned, char *, char *);  /* line name */
void find_cl (int, unsigned, char *, char *);  /* client name */
void find_vs (int, unsigned, char *, char *);  /* vessel name */
void find_ar (int, unsigned, char *, char *);  /* area/project name */

				/* function prototypes GUNDPCFG.C */
void view_gundp(int, unsigned, unsigned);      /* view gun depth cfg */
int add_gundp(int, unsigned);                  /* add a gun depth */
int del_gundp(int);                            /* delete a gun depth */
void save_gundp(int);                          /* save gun depth cfg */
int load_gundp(int);                           /* load gun depth cfg */
struct GUNDP_DEF *findgundp(char *);           /* find a gun depth */
void gundp_store(struct GUNDP_DEF *, struct GUNDP_DEF **,
                 struct GUNDP_DEF **);         /* linked list store */
int defldval_gundp (TRANSACTION *);   /* field validation */
int defldval_gundpdel (void); 		  /* field validation */
void gundp_help(void);                /* gun depth help screen */

				/* function prototypes HEADCFG.C */
int select_fmt (unsigned);					/* format select function */
int defldval_hdr (void);					/* field validation */
int defldset_hdr (TRANSACTION *);		/* popup field menu */
void ch_head (void);                   /* view/change header */
void save_hdr (void);                  /* save to disk */
int load_hdr (int);                    /* load from disk */
void head_help(void);                  /* header help screen */

				/* function prototypes HELPMAIN.C */
void help_main (void);        /* main help menu */
void run_help (MENUITEM *);   /* actual calls from menu to HELPMAIN */
void string_help( char *);    /* calls HELPMAIN with a string */
void br_man( void ); 			/* view document files */

				/* function prototypes MAINCFG.C */
int maincfg (void);           /* main configuration menu */
void sel_geco (void);         /* select geco contractor */
void sel_prakla (int);        /* select prakla contractor */
void sel_west3 (int);         /* select western fmt 3 */
void sel_west4 (int);         /* select western fmt 4 */
void sel_digicon (int);       /* select digicon contractor */
void set_add_cab (void);      /* fnptr add cable depth */
void set_add_gun (void);      /* fnptr add gun */
void set_add_wing (void);     /* fnptr add wing angle */
void set_add_gundp (void);    /* fnptr add gun depth */
void set_ch_head (void);      /* fnptr view/change header */
void set_del_cab (void);      /* fnptr delete cable depth */
void set_del_wing (void);     /* fnptr delete wing angle */
void set_del_gun (void);      /* fnptr delete gun */
void set_del_gundp (void);    /* fnptr delete gun depth */
void set_load_cab (void);     /* fnptr load cable depth cfg */
void set_load_wing (void);    /* fnptr load wing angle cfg */
void set_load_gun (void);     /* fnptr load srce cfg */
void set_load_gundp (void);   /* fnptr load gun depth cfg */
void set_load_hdr (void);     /* fnptr load header cfg */
void set_load_pro (void);     /* fnptr load project cfg */
void set_save_cab ( void);    /* fnptr save cable depth cfg */
void set_save_wing ( void);   /* fnptr save wing angle cfg */
void set_save_gun (void);     /* fnptr save srce cfg */
void set_save_gundp (void);   /* fnptr save gun depth cfg */
void set_sel_prakla (void);   /* fnptr select prakla */
void set_sel_west3 (void);    /* fnptr select western fmt 3 */
void set_sel_west4 (void);    /* fnptr select western fmt 4 */
void set_sel_digicon (void);  /* fnptr select digicon */
void set_view_cab (void);     /* fnptr view cable depth cfg */
void set_view_wing (void);    /* fnptr view wing angle cfg */
void set_view_gun (void);     /* fnptr view srce cfg */
void set_view_gundp (void);   /* fnptr view gun depth cfg */
void v_fixed_fmt (char *);    /* view a fixed header format */
void set_load_prn(void);      /* fnptr load printer cfg */
void geco_init (void);        /* initialise geco to NONE */
void head_help_gec(void);     /* help on Geco formats */
void head_help_wes(void);     /* help on Western formats */
void head_help_pra(void);     /* help on Prakla formats */
void head_help_ssl(void);     /* help on SSL formats */
void head_help_cgg(void);     /* help on CGG formats */
void head_help_dig(void);     /* help on Digicon formats */
void head_help_hgs(void);     /* help on HGS formats */

			/* function prototypes OPTFUN1.C */
void st_mem (void);           /* DOS/Expanded memory status */
void get_free(char , float *, float *);  /* free disk space (BIOS) */
int dos_mem(void);            /* get DOS memory free/status */
int ext_mem(void);            /* extended/expanded mem status */
void _far hhandler( unsigned deverr, unsigned doserr,
                    unsigned _far *hdr );  /* critical error handler */
void syst_help(void);				  /* system status help screen */

            /* function prototypes PRINTCFG.C */
void ch_prn(void);					  /* view/change printer cfg */
int defldset_prn (TRANSACTION *);  /* field set function */
int defldval_prn (TRANSACTION *);  /* field validate function */
int select_prn(unsigned);          /* select printer type */
int select_port(unsigned);         /* select printer port */
void save_prn (void);              /* save printer cfg (PRINTER.SET) */
int load_prn (int);                /* load printer cfg */
void print_help(void);             /* printer help screen */
void res_help(void);               /* printer resolution help screen */
void mul_help(void);               /* printer multipliers help screen */
void oth_help(void);               /* margins etc help screen */

				/* function prototypes PROCFG.C */
void set_pro (void);               /* view/change project cfg */
void save_pro (void);              /* save project cfg */
int load_pro (int);                /* load project cfg */
int select_unit (void);            /* format select function */
int defldset_pro (TRANSACTION *);  /* popup field menu */
int defldval_pro (TRANSACTION *);  /* field validation */
int detranval_pro (TRANSACTION *);	  /* transaction validation */
void pro_help(void);               /* project help screen */

			/* function prototypes RUNFUN1.C */
void proc_sp (HWND, long, long, char *, long, FILE *);  /* SP errors */
void proc_fn (HWND, long, long, char *, long, FILE *);  /* FN errors */
void proc_dcom (HWND, long, char *, long, FILE *);  /* cable depth COMMS */
void proc_dep1(HWND, long, char *, long, FILE *); /* cable depth1 */
void proc_dep2(HWND, long, char *, long, FILE *); /* cable depth2 */
void proc_clk(HWND, long, char *, long, FILE *, FILE *); /* timing */

			/* function prototypes RUNFUN2.C */
void proc_srce(HWND, long, char *, long, FILE *); /* source checking */
void srce_vol (struct MSF_FLAGS, long, long,    /* volumes */
             HWND, FILE *, unsigned, char *);
void srce_msf (struct MSF_FLAGS, long, long,    /* misfire series etc */
                       HWND, FILE *, char *);


         /* function prototypes RUNFUN3.C */
void proc_man(HWND, long, char *, long, FILE *);   /* pressure */
void proc_gmsk(HWND, long, char *, long, FILE *, int); /* gun mask */
void proc_fseq(long, long, FILE *);                /* firing sequence */
void proc_sptim(HWND, long, char *, long, FILE *); /* SP timing */
void proc_gundp(HWND, long, char *, long, FILE *); /* gun depths */
void proc_wadep(HWND, long, char *, long, FILE *); /* water depths */
void proc_wing(HWND, long, char *, long, FILE *);  /* wing angles */
void proc_sal(HWND, long, char *, long, FILE *);	/* ground speed */
void proc_ws(HWND, long, char *, long, FILE *); 	/* water speed */
void proc_current( long );									/* current statistics */
void proc_syhd( HWND, long, char *, long, FILE * ); /* system heading */
void proc_gyro( HWND, long, char *, long, FILE * ); /* gyro heading */
void proc_heading( long );  /* crab angle */

			/* function prototypes RUNUTIL1.C */
int find_file (int, char *, unsigned, char *);   /* select a file */
int key_int (HWND, long, long, char *, FILE *); /* interrupt stats */
void scr_error (char *, char *, long, long, HWND, FILE *); /* screen & disk */
int check_drive (unsigned, char *);      /* check drive/dir exists */
FILE *log_file (void);    /* open & initialise logging (.LOG) file */
FILE *prn_file (void);    /* open & initialise timing (.PRN) file */
FILE *spn_file (void);    /* open and initialise a SP/FN (.SPN) file */
FILE *seq_file (void);    /* open & initialise sequence (.SEQ) file */
FILE *grf_file (void);	  /* open & initialise graphics (.GRF) file */
void set_waypoint (void); /* set a waypoint to stop during run */
int detranval_way (TRANSACTION *); /* validate waypoint transaction */
void SetFileString(struct FNAME *, char *);   /* set fname to std fmt */
int GetFileName(int, char *, char *);   /* get a filename from a list */
int GetDatFields(char *, struct FNAME *); /* get comment fld & date/time */

			/* function prototypes RUNUTIL2.C */
int select_sp (void);         /* data entry for SP range */
int defldval_sp (void);       /* field validate */
void pr_spfn (void);          /* print SP/FN/Time Range */
void spfn_help(void);         /* SP/FN list help */

			/* function prototypes SETANAL1.C */
void setanal (void);                       /* analysis menu */
int load_anl (int);                        /* load analysis cfg */
void set_load_anl (void);                  /* ditto + argument */
void save_anl (void);                      /* save analysis cfg */

			/* function prototypes SETANAL2.C */
int ch_anl (void);                         /* change analyses set */
int defldset_anl (TRANSACTION *);			 /* field set */
int defldval_anl (TRANSACTION *);          /* field validate */
int StepThrough(TRANSACTION *, HWND);      /* stepthrough YES/NO */
void anl_help (void);                      /* analysis help screen */

         /* function prototypes SETGRAF1.C */
void setgraf (void);                       /* graphics menu */
void graf_help (void);                     /* graphics help screen */
void set_load_graf (void);                 /* load graphics cfg + arg */
void save_graf (void);                     /* save graphics cfg */
int load_graf (int);                       /* load graphics cfg */
void ch_graf (void);								 /* change graphics settings */
int defldval_graf (TRANSACTION *);         /* field validation */
int defldset_graf (TRANSACTION *);         /* field set */

			/* function prototypes SETOPTS1.C */
void set_opts (void);      /* main options function */
void set_load_opt (void);	/* needed because load_opt() has argument */
void opthelp(void);        /* global options help file */
void v_txt( void );			/* view .txt files */
void v_log( void );			/* view .log files */
void v_seq( void );			/* view .seq files */
void v_prn( void );			/* view .prn files */
void v_spn( void );			/* view .spn files */
void ViewFile( unsigned, char *, char *);	/* the actual file view function */
void ShowTop(FILE *, HWND);	/* top screen page */
void ShowBottom(FILE *, HWND, long *, int);	 /* bottom page */
void ShowPage(FILE *, HWND, long);	 /* display page */
int GetPointers(FILE *, long *);		/* initialise pointer array */
void LineMessage(HWND, FILE *, long, long);	/* bottom line messages */
void ViewReports( void );	 /* file viewer utility */

			/* function prototypes SETOPTS2.C */
int load_opt (int);        /* load option configuration */
void save_opt (void);      /* save option cofiguration */
void set_dirs (void);      /* view/change options  */
int defldval_opt (TRANSACTION *);  /* field validation */
void opt_help(void);               /* help in data entry */
int defldset_opt (TRANSACTION *);  /* set help function */
void dir_help(void);               /* drive/dir help screen */
void prod_help(void);              /* prod calc help screen */
void depun_help(void);             /* depth units help screen */
void view_help( void ); 			  /* file viewer help screen

				/* function prototypes SETSPEC1.C */
void setspec (void);    /* specs menu */
int ch_spec (void);     /* needs return val for change during run */
int load_spec (int);          /* load specs cfg */
void set_load_spec (void);    /* load specs cfg + arg */
void save_spec (void);        /* save specs cfg */
void specs_help(void);               /* specs help screen */
int defldval_spec (TRANSACTION *);   /* field validation */
int fir_seq (void);                  /* check field numbering */

				/* function prototypes SRCECFG.C */
void view_gun(int, unsigned, unsigned);   /* view srce cfg */
int add_gun(int, unsigned, unsigned);     /* add a gun */
int del_gun(int);                         /* delete a gun */
void save_gun(int);                       /* save srce cfg */
int load_gun(int);                        /* load srce cfg */
struct SRCE_DEF *findsrce(char *);        /* find a gun name */
void srce_store(struct SRCE_DEF *, struct SRCE_DEF **,
                struct SRCE_DEF **);      /* linked list store */
int defldval_srce (TRANSACTION *);	  /* field validation */
int defldval_srcedel (void);			  /* field validation */
void srce_help(void);                 /* srce help screen */

			/* function prototypes STATINIT.C */
int init_stat (void);                 /* allocate memory */

			/* function prototypes STATLOG1.C */
void stat_log (void);              /* make an ASCII stats log */
void srce_log (FILE *);            /* source timing & volumes */
void cab_log (FILE *);             /* cable depths */

			/* function prototypes STATMAK1.C */
void stat_make (void);    /* master function going to actual statistics */
int srce_mak (HWND, register int);   /* source timing & volumes */
int stat_frame (void);   /* frame/window size calculatator */

			/* function prototypes STATRUN1.C */
int statrun ( FILE * );					/* run stats */
void free_stat_init( void );			/* free memory after run */
void heapstat( void );					/* walk the heap (debug) */

			/* function prototypes STATSDEF.C */
void loaddef( int, int );					/* load .CFG config files */
void VGAMenu( void );              /* select & run STS92.EXE */

				/* function prototypes STATSERR.C */
void StatsDosError (char *, int);  /* show a DOS error */
void StatsError (char *);          /* show a fixed error string */
void StatsErr (char *);            /* call error from STATERR.ERR */
void NonImpl (void);               /* not implemented error msge */
void StatsMemError (char *);       /* memory allocation error */
void StatsMessage (char *);        /* message to screen */
void PressKey (int, int);          /* press a key */
int KeySure (void);                /* make sure */
void StWait (void);                /* wait while busy message */
void EnWait (void);                /* end wait message */

				/* function prototypes STS.C */
void main (int, char **);          /* MAIN FUNCTION CALL */
unsigned _memo(TRANSACTION *, HWND);    /* stub - dw memsaver */
unsigned _subform(TRANSACTION *, HWND);   /* stub - dw memsaver */


			/* function prototypes STSARC1.C */
int arc_menu (void);     /* archive menu */
void arc_help(void);     /* archive help */

			/* function prototypes STSARC2.C */
void mcfg_arc (void);   /* archive current configuration */
void dcfg_arc (void);	/* retrieve an archived configuration */
void m_arc (void);		/* archive a data file (option + config) */
int d_arc (void);       /* dearchive data + option config */
int defldval_mcfg_arc (void); /* field validate for store filename */

            /* function prototypes STSGRAF1.C */
void sts_graf (void);         /* actual main graphics engine */
void sts_grafklone (void);    /* make a fake window for losing info */
void graf_init (void);        /* graphics menu */
void scr_graf (void);         /* screen only - no print */
void prt_graf (void);         /* print & screen */
void hlp_graf (void);         /* call help window */
void gfxpr_help (void);       /* graphics help function */

            /* function prototypes STSGRAF2.C */
int SemaphoreBorder(short);   /* make fixed stikogram border */
void SemaphoreMove(int , double, double, double); /* fill fixed st */
int LimitBorder(short, double, double);
void LimitMove(double, double);
int cab_graf(short, short, float *, float *, float *,
             float *, double, int, int);   /* cable depth graphs */
int gundp_graf(short, float *, float *, float *,
               float *, double, int, int); /* gun depth graphs */
int ScreenErr(int);                        /* graphics screen error */
void Int_Tick (double, double, double);    /* make scale ticks */
short PrintIt(short, int , int, int, int, int, int, int *); /* print */
void MakeX(int, int, int, double, long, long, long, long,
           double, double, double, double); /* Xaxis ticks & numbers */
int time_graf(short, double, int, int);   /* SP timing graph */
int new_page( void );          /* write the new page mini header */
int make_sp_div (int);         /* make large tick divisor */
int make_tic_div (int);        /* make small tick divisor */
short head_top (short);        /* make the graphics header top */
void no_errbox (short);        /* make a no-error box */

            /* function prototypes STSGRAF3.C */
void ScreenDump (int, int, float,float, int, int, int, int, int, int, int);
void HPLaserPlusScreenDump (int, float, float, int, int, int);
void ToshibaPScreenDump (int, float, float, int, int, int);
void EpsonMXScreenDump (int, float, float, int, int, int);
void EpsonFXScreenDump (int , float, float, int, int, int);
void EpsonLQScreenDump (int, float, float, int, int, int);
void HPThinkJetScreenDump (int, float, float, int, int, int);
int prnerr (int);
char GetLQByte (int, int, float, float, int, int);
int GetPrnMaxX (int);
int GetPrnMaxY (int);
int GetPrnPixel (int, int, int);
void OutPrnCh (char);
void OutPrnStr (char *);
char GetVertByte (int, int, int, float, float, float, int, int);
char GetHorizByte (int, int, float, int, int);

				/* function prototypes STXMOUSE.C */
int MouseOn (int);             /* turn on mouse cursor */
unsigned MouseKey (int, int);  /* key or mouse wait */

				/* function prototypes WCS_GNLF.C */
LBOXSTA *WCS_lboxinit(int,int,int,int,int,char *,int,char *,int,LISTITEM *);

            /* function prototypes WINGCFG.C */
void view_wing(int, unsigned, unsigned);    /* view wing cfg */
int add_wing(int,unsigned);                 /* add a wing angle */
int del_wing(int);                          /* delete a wing angle */
void save_wing(int);                        /* save wing cfg */
int load_wing(int);                         /* load wing cfg */
struct WING_DEF *findwing(char *);          /* find a wing name */
void wing_store(struct WING_DEF *, struct WING_DEF **,
                struct WING_DEF **);        /* linked list store */
int defldval_wing (TRANSACTION *);          /* field validate */
int defldval_wingdel (void);                /* field validate */
void wing_help(void);                       /* wing help screen */
