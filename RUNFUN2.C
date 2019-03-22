/***************************************************************
*    RUNFUN2.C  source analysis
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
#include <math.h>
#include <string.h>

#include <dw.h>            /* greenleaf */

#include <stats.h>         /* wcs */
#include <summ.h>
#include <extern.h>
#include <stxfunct.h>
#include <st_graph.h>

			/* external global declarations */
extern struct SRCE_ERR *gun;  /* pointer to integer gunspecs */
extern struct ST_SRCE *sr1;   /* source 1 stats */
extern struct ST_SRCE *sr2;   /* source 2 stats */
extern struct ST_SRCE *sr3;   /* source 3 stats */
extern struct ST_SRCE *sr4;   /* source 4 stats */
extern struct ST_XSRC *xsr;   /* source crossfire stats */
extern struct ST_MISF *msf;   /* consecutive & series misfire stats */
extern struct ERR_GRAF grerr;   /* no-error graph flags */
extern unsigned *s1m1,  /* source 1 spec1 array for 'x in Y' misfires */
                *s2m1,        /* ditto source 2 */
                *s3m1,        /* ditto 3 */
                *s4m1,        /* ditto 4 */
                *s1m2,  /* source 1 spec2 array for 'x in Y' misfires */
                *s2m2,        /* ditto source 2 */
                *s3m2,        /* ditto 3 */
                *s4m2;        /* ditto 4 */
extern struct HDR_DEF *hd;    /* header  list pointer */
extern struct OPT_DEF  *opt;  /* pointer to options list */
extern struct SPEC_DEF *spc;  /* pointer to specifications list */
extern struct ANAL_DEF *andef;   /* pointer to set/analysis list */
extern struct PRO_DEF *pd;    /* pointer to project list */
extern struct SRCE_DEF *stsrce;  /* pointer to start of source list */
extern struct EXT_DEF *ext;
extern struct GRAF_DEF *graf; /* pointer to graphics list */
extern float *srce_graf_tmp;  /* pointer to source timing graphics array */
extern unsigned *vol_graf_tmp;   /* srce volume graphics error array */
extern char *data;            /* pointer to data buffer */
extern char *units[];         /* FEET/METRES PSI/BARS etc */


/***************************************************************
*   master source checking function
*      deals with misfires/times/volumes/overfires etc.
*      current for GECO LITTON 1 & 2/GECO GUNDA 1 & 2
*      also PRAKLA VZAD controller under ZXCD SYN V2.0
*      also WESTERN LITTON 3 & 4 without volume TB calculations
*      also DIGICON TRACE0
****************************************************************/
void proc_srce(HWND Werroll, long fn, char *errbuf, long sp, FILE *flog)
{
   int temp=0;  /* KILL AFTER TESTING DIGI */
	register int i=0, n=0;
	struct SRCE_DEF *info;
	char *bp, *bb;
	static struct MSF_FLAGS msf_flags;
	unsigned s1_vol=0, s2_vol=0, s3_vol=0, s4_vol=0;   /* vol previous SP */
	unsigned sp_vol=0;       /* volume enabled this SP */
	unsigned gunno;          /* gunda - gun number 1 to 8 */
	unsigned arrno;          /* gunda array no 1 to 8 */
	unsigned side;           /* gunda side Port=1 Stbd=0 */
	int error, num, wr_fl;   /* wr_fl if gunda gun & not in sourcedef */
	char t_buf[41];
   info = stsrce;
      /* reset source flags */
   msf_flags.s1_efl=0;   /* source enabled */
	msf_flags.s2_efl=0;
	msf_flags.s3_efl=0;
   msf_flags.s4_efl=0;
   msf_flags.s1_mfl=0;   /* source nofire-autofire */
	msf_flags.s2_mfl=0;
	msf_flags.s3_mfl=0;
   msf_flags.s4_mfl=0;
   msf_flags.s1_1fl=0;   /* out of spec 1 */
	msf_flags.s2_1fl=0;
	msf_flags.s3_1fl=0;
   msf_flags.s4_1fl=0;
   msf_flags.s1_2fl=0;   /* out of spec 2 */
	msf_flags.s2_2fl=0;
	msf_flags.s3_2fl=0;
   msf_flags.s4_2fl=0;
         /* GECO LITTON 1 or 2 */
	if((hd->gu == 8)||(hd->gu == 9)) {
		bb = data+opt->spn_fld+hd->gu_ofst;  /* set block start byte */
		for(i;i<ext->numbergun;i++) {    /* for NUMBER of GUNS in SRCEDEF */
			bp = bb + 4*(info->gun_num - 1); /* set gun start byte */
			if(*(bp+2)&0x60) {   /* if gun is enabled */
				info->en_num = info->en_num+1; /* add 1 SP gun enabled */
				if(info->srce==1) msf_flags.s1_efl = 1;  /* source 1 gun */
				if(info->srce==2) msf_flags.s2_efl = 1;  /* ditto source 2 */
				if(info->srce==3) msf_flags.s3_efl = 1;  /* ditto 3 */
				if(info->srce==4) msf_flags.s4_efl = 1;  /* ditto 4 */
						/* IF A NOFIRE/AUTOFIRE */
				if(*(bp)&0x40) {  /* gun misfired (outside TB window) */
					sprintf(t_buf, "Gun %-10s MISFIRE", info->gun_name);
               if((andef->lgun_anl1)||(andef->lgun_anl2))
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
					info->mf_num = info->mf_num+1;
					if(info->srce==1) {
						msf_flags.s1_mfl = 1;  /* source 1 gun */
						sr1->tot_mf = sr1->tot_mf+1;
					}
					if(info->srce==2) {
						msf_flags.s2_mfl = 1;  /* ditto source 2 */
						sr2->tot_mf = sr2->tot_mf+1;
					}
					if(info->srce==3) {
						msf_flags.s3_mfl = 1;  /* ditto 3 */
						sr3->tot_mf = sr3->tot_mf+1;
					}
					if(info->srce==4) {
						msf_flags.s4_mfl = 1;  /* ditto 4 */
						sr4->tot_mf = sr4->tot_mf+1;
					}
				} else {  /* enabled gun and not NOF/AUT so look at err time */
					error = (int) conv_bcd(0, 2, bp+2);
					error = error - 4512;
					if(andef->gun_anl1) {
						if (abs(error)>(int)gun->sp1) {
							sprintf(t_buf, "Gun %-10s %+-6.2f mS",
													 info->gun_name, ((float)error)/4.0F);
                     if(andef->lgun_anl1)
                        scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
							info->tot_sp1 = info->tot_sp1+1;
							if(info->srce==1) msf_flags.s1_1fl = 1;  /* source 1 gun */
							if(info->srce==2) msf_flags.s2_1fl = 1;  /* ditto source 2 */
							if(info->srce==3) msf_flags.s3_1fl = 1;  /* ditto 3 */
                     if(info->srce==4) msf_flags.s4_1fl = 1;  /* ditto 4 */
                     if(graf->gun_anl1) {   /* do graphics array */
                        if( (float)fabs(((float)error)/4.0F) > *srce_graf_tmp) {
                           *srce_graf_tmp = ((float)error)/4.0F;
                           grerr.gu = 1;
                        }
                     }
						}
					}
					if(andef->gun_anl2) {
						if (abs(error)>(int)gun->sp2) {
							sprintf(t_buf, "Gun %-10s * %+-6.2f mS",
													 info->gun_name, ((float)error)/4.0F);
                     if(andef->lgun_anl2)
                        scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
							info->tot_sp2 = info->tot_sp2+1;
							if(info->srce==1) msf_flags.s1_2fl = 1;  /* source 1 gun */
							if(info->srce==2) msf_flags.s2_2fl = 1;  /* ditto source 2 */
							if(info->srce==3) msf_flags.s3_2fl = 1;  /* ditto 3 */
							if(info->srce==4) msf_flags.s4_2fl = 1;  /* ditto 4 */
						}
					}
					sp_vol = sp_vol + info->gun_vol;
				}
			}
      info = info->next;
		}
   }  /* end geco litton */
			/* WESTERN FORMATS 3 & 4 (needs volume TB to be added */
	if((hd->gu == 63)||(hd->gu == 64)) {
		bb = data+opt->spn_fld+hd->gu_ofst;  /* set block start byte */
		for(i;i<ext->numbergun;i++) {    /* for NUMBER of GUNS in SRCEDEF */
			if(info->gun_num < 65)
				bp = bb + 2*(info->gun_num - 1); /* set gun start byte */
			if(info->gun_num > 64)              /* FMT 4 2nd Litton block */
				bp = bb + 2*(info->gun_num - 65) + 382;
			if(!(*(bp)&0xC0)) {   /* if gun is enabled */
				info->en_num = info->en_num+1; /* add 1 SP gun enabled */
				if(info->srce==1) msf_flags.s1_efl = 1;  /* source 1 gun */
				if(info->srce==2) msf_flags.s2_efl = 1;  /* ditto source 2 */
				if(info->srce==3) msf_flags.s3_efl = 1;  /* ditto 3 */
				if(info->srce==4) msf_flags.s4_efl = 1;  /* ditto 4 */
					/* IF A NOFIRE/AUTOFIRE */
				if(*(bp)&0x80) {  /* gun misfired (outside TB window) */
					sprintf(t_buf, "Gun %-10s MISFIRE", info->gun_name);
               if((andef->lgun_anl1)||(andef->lgun_anl2))
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
					info->mf_num = info->mf_num+1;
					if(info->srce==1) {
						msf_flags.s1_mfl = 1;  /* source 1 gun */
						sr1->tot_mf = sr1->tot_mf+1;
					}
					if(info->srce==2) {
						msf_flags.s2_mfl = 1;  /* ditto source 2 */
						sr2->tot_mf = sr2->tot_mf+1;
					}
					if(info->srce==3) {
						msf_flags.s3_mfl = 1;  /* ditto 3 */
						sr3->tot_mf = sr3->tot_mf+1;
					}
					if(info->srce==4) {
						msf_flags.s4_mfl = 1;  /* ditto 4 */
						sr4->tot_mf = sr4->tot_mf+1;
					}
				} else {  /* enabled gun and not NOF/AUT so look at err time */
					error = (int) conv_bcd(0, 2, bp);
					error = error - 512;
					if(andef->gun_anl1) {
                  if (abs(error)>(int)gun->sp1) {
                     if(andef->lgun_anl1) {
                        sprintf(t_buf, "Gun %-10s %+-6.2f mS",
                                     info->gun_name, ((float)error)/4.0F);
                        scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                     }
							info->tot_sp1 = info->tot_sp1+1;
							if(info->srce==1) msf_flags.s1_1fl = 1;  /* source 1 gun */
							if(info->srce==2) msf_flags.s2_1fl = 1;  /* ditto source 2 */
							if(info->srce==3) msf_flags.s3_1fl = 1;  /* ditto 3 */
                     if(info->srce==4) msf_flags.s4_1fl = 1;  /* ditto 4 */
                     if(graf->gun_anl1) {   /* do graphics array */
                        if( (float)fabs(((float)error)/4.0F) > *srce_graf_tmp) {
                           *srce_graf_tmp = ((float)error)/4.0F;
                           grerr.gu = 1;
                        }
                     }
						}
					}
					if(andef->gun_anl2) {
                  if (abs(error)>(int)gun->sp2) {
                     if(andef->lgun_anl2) {
                        sprintf(t_buf, "Gun %-10s * %+-6.2f mS",
                                     info->gun_name, ((float)error)/4.0F);
                        scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                     }
							info->tot_sp2 = info->tot_sp2+1;
							if(info->srce==1) msf_flags.s1_2fl = 1;  /* source 1 gun */
							if(info->srce==2) msf_flags.s2_2fl = 1;  /* ditto source 2 */
							if(info->srce==3) msf_flags.s3_2fl = 1;  /* ditto 3 */
							if(info->srce==4) msf_flags.s4_2fl = 1;  /* ditto 4 */
						}
					}
					sp_vol = sp_vol + info->gun_vol;
				}
			}
      info = info->next;
		}
   }  /* end western litton */
         /* PRAKLA SYN 2.0 VZAD */
	if(hd->gu == 91) {
		bb = data+opt->spn_fld+hd->gu_ofst;  /* set block start byte */
		for(i;i<ext->numbergun;i++) {    /* for NUMBER of GUNS in SRCEDEF */
			bp = bb + 2*(info->gun_num - 1); /* set gun start byte */
			if(*(bp)&0x08) {   /* if gun is enabled */
				info->en_num = info->en_num+1; /* add 1 SP gun enabled */
				if(info->srce==1) msf_flags.s1_efl = 1;  /* source 1 gun */
				if(info->srce==2) msf_flags.s2_efl = 1;  /* ditto source 2 */
				if(info->srce==3) msf_flags.s3_efl = 1;  /* ditto 3 */
				if(info->srce==4) msf_flags.s4_efl = 1;  /* ditto 4 */
						 /* IF A NOFIRE/AUTOFIRE */
				if(*(bp)&0x04) {  /* gun misfired (outside TB window) */
					sprintf(t_buf, "Gun %-10s MISFIRE", info->gun_name);
               if((andef->lgun_anl1)||(andef->lgun_anl2))
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
					info->mf_num = info->mf_num+1;
					if(info->srce==1) {
						msf_flags.s1_mfl = 1;  /* source 1 gun */
						sr1->tot_mf = sr1->tot_mf+1;
					}
					if(info->srce==2) {
						msf_flags.s2_mfl = 1;  /* ditto source 2 */
						sr2->tot_mf = sr2->tot_mf+1;
					}
					if(info->srce==3) {
						msf_flags.s3_mfl = 1;  /* ditto 3 */
						sr3->tot_mf = sr3->tot_mf+1;
					}
					if(info->srce==4) {
						msf_flags.s4_mfl = 1;  /* ditto 4 */
						sr4->tot_mf = sr4->tot_mf+1;
					}
				} else {  /* enabled gun and not NOF/AUT so look at err time */
					error =  conv_2comp1(bp+1);
					if(andef->gun_anl1) {
                  if (abs(error)>(int)gun->sp1) {
                     if(andef->lgun_anl1) {
                        sprintf(t_buf, "Gun %-10s %+-6.2f mS",
                                     info->gun_name, ((float)error)/10.0F);
                        scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                     }
							info->tot_sp1 = info->tot_sp1+1;
							if(info->srce==1) msf_flags.s1_1fl = 1;  /* source 1 gun */
							if(info->srce==2) msf_flags.s2_1fl = 1;  /* ditto source 2 */
							if(info->srce==3) msf_flags.s3_1fl = 1;  /* ditto 3 */
                     if(info->srce==4) msf_flags.s4_1fl = 1;  /* ditto 4 */
                     if(graf->gun_anl1) {   /* do graphics array */
                        if( (float)fabs(((float)error)/10.0F) > *srce_graf_tmp) {
                           *srce_graf_tmp = ((float)error)/10.0F;
                           grerr.gu = 1;
                        }
                     }
						}
					}
					if(andef->gun_anl2) {
                  if (abs(error)>(int)gun->sp2) {
                     if(andef->lgun_anl2) {
                        sprintf(t_buf, "Gun %-10s * %+-6.2f mS",
                                     info->gun_name, ((float)error)/10.0F);
                        scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                     }
							info->tot_sp2 = info->tot_sp2+1;
							if(info->srce==1) msf_flags.s1_2fl = 1;  /* source 1 gun */
							if(info->srce==2) msf_flags.s2_2fl = 1;  /* ditto source 2 */
							if(info->srce==3) msf_flags.s3_2fl = 1;  /* ditto 3 */
							if(info->srce==4) msf_flags.s4_2fl = 1;  /* ditto 4 */
						}
					}
					sp_vol = sp_vol + info->gun_vol;
				}
			}
      info = info->next;
		}
   }  /* end prakla vzad */
         /* GECO GUNDA 1 or 2 */
	if((hd->gu == 10)||(hd->gu == 11)) {
		bp = data+opt->spn_fld+hd->gu_ofst+4; /* set block start byte */
														  /* + 4 bytes control words */
		num = ((int)conv_bin(0, 2, bp, 0))/6; /* number of guns fired */
		if(num) {              /* at least 1 gun in data block */
			wr_fl=0;            /* set wrong_gun flag to 0 */
			bp = bp+2;          /* go to first gun data block */
         for(i=0; i<num; i++) {
				bb = bp +1;      /* go to gun id byte */
				gunno = *bb & 0x0F;          /* establish gun id */
				arrno = (*bb >> 4) & 0x07;
				side = (*bb >> 7) & 0x01;
				info = stsrce;
				for(n=0; n<ext->numbergun; n++) {
               if( (gunno == info->gunno) && (arrno == info->arrno) &&
                     (side == info->side) ) {
						wr_fl=1;   /* gun found in sourcedef OK */
						break;
					} else info = info->next;
				}
				if(!wr_fl) {    /* if gunda gun but not in sourcedef */
               if(andef->lgun_anl1 || andef->lgun_anl2)
                  scr_error("GUNDA - GUN NOT DEFINED",
                       errbuf, sp, fn, Werroll, flog);
				} else {          /* GUNDA GUN EXISTS IN SOURCDEF OK */
					info->en_num = info->en_num+1; /* add 1 SP gun enabled */
					if(info->side==0) msf_flags.s1_efl = 1;  /* source 1 gun */
					if(info->side==1) msf_flags.s2_efl = 1;  /* ditto source 2 */
					if(*bp & 0x20) {   /* IF A GUNDA MISFIRE */
                  if(andef->lgun_anl1 || andef->lgun_anl2) {
                     sprintf(t_buf, "Gun %-10s MISFIRE", info->gun_name);
                     scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                  }
						info->mf_num = info->mf_num+1;
                  if(!info->side) {
                     msf_flags.s1_mfl = 1;     /* source 1 gun */
							sr1->tot_mf = sr1->tot_mf+1;
                  } else {
                     msf_flags.s2_mfl = 1;     /* ditto source 2 */
							sr2->tot_mf = sr2->tot_mf+1;
                  }         /* GUNDA ONLY 2 SOURCES */
					} else {     /* gun did not misfire so get error */
						error = conv_2comp2(bp+6);  /* READ ERROR TIME */
						if(andef->gun_anl1) {
                     if (abs(error) > (int)gun->sp1) {
                        if(andef->lgun_anl1) {
                           sprintf(t_buf, "Gun %-10s %+-5.1f mS",
                                     info->gun_name, ((float)error)/10.0F);
                           scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                        }
								info->tot_sp1 = info->tot_sp1+1;
								if(info->side==0) msf_flags.s1_1fl = 1;  /* source 1 gun */
                        if(info->side==1) msf_flags.s2_1fl = 1;  /* ditto source 2 */
                        if(graf->gun_anl1) {   /* do graphics array */
                           if( (float)fabs(((float)error)/10.0F) > *srce_graf_tmp) {
                              *srce_graf_tmp = ((float)error)/10.0F;
                              grerr.gu = 1;
                           }
                        }
							}
						}
						if(andef->gun_anl2) {
							if (abs(error)>(int)gun->sp2) {
                        if(andef->lgun_anl2) {
                           sprintf(t_buf, "Gun %-10s * %+-5.1f mS",
                                     info->gun_name, ((float)error)/10.0F);
                           scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                        }
								info->tot_sp2 = info->tot_sp2+1;
								if(info->side==0) msf_flags.s1_2fl = 1;  /* source 1 gun */
								if(info->side==1) msf_flags.s2_2fl = 1;  /* ditto source 2 */
							}
						}
						sp_vol = sp_vol + info->gun_vol;
					}
				}
            bp = bp +12;
			}
		}
   } /* end geco gunda*/
         /* DIGICON TRACE0 */
   if(hd->gu == 71) {
		bb = data+opt->spn_fld+hd->gu_ofst;  /* set block start byte */
		for(i;i<ext->numbergun;i++) {    /* for NUMBER of GUNS in SRCEDEF */
			bp = bb + 2*(info->gun_num - 1); /* set gun start byte */
         if(*(bp)&0x80) {   /* if gun is enabled - bit 15 of 15 */
				info->en_num = info->en_num+1; /* add 1 SP gun enabled */
				if(info->srce==1) msf_flags.s1_efl = 1;  /* source 1 gun */
				if(info->srce==2) msf_flags.s2_efl = 1;  /* ditto source 2 */
				if(info->srce==3) msf_flags.s3_efl = 1;  /* ditto 3 */
				if(info->srce==4) msf_flags.s4_efl = 1;  /* ditto 4 */
                   /* IF AN AUTOFIRE */
            if(*(bp)&0x40) {  /* autofire - bit 14 of 15 */
               sprintf(t_buf, "Gun %-10s AUTOFIRE", info->gun_name);
               if((andef->lgun_anl1)||(andef->lgun_anl2))
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
					info->mf_num = info->mf_num+1;
					if(info->srce==1) {
						msf_flags.s1_mfl = 1;  /* source 1 gun */
						sr1->tot_mf = sr1->tot_mf+1;
					}
					if(info->srce==2) {
						msf_flags.s2_mfl = 1;  /* ditto source 2 */
						sr2->tot_mf = sr2->tot_mf+1;
					}
					if(info->srce==3) {
						msf_flags.s3_mfl = 1;  /* ditto 3 */
						sr3->tot_mf = sr3->tot_mf+1;
					}
					if(info->srce==4) {
						msf_flags.s4_mfl = 1;  /* ditto 4 */
						sr4->tot_mf = sr4->tot_mf+1;
					}
            } else {  /* enabled gun and not AUT so look at err time */
                  /* convert the 2 bytes as a binary integer */
                     /* add 256 bytes to get to actual fire times */
               error = (int)conv_bin(256, 2, bp, 0);
                  /* zero value is a NOFIRE */
               if(!error) {
                  sprintf(t_buf, "Gun %-10s NOFIRE", info->gun_name);
                  if((andef->lgun_anl1)||(andef->lgun_anl2))
                     scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                  info->mf_num = info->mf_num+1;
                  if(info->srce==1) {
                     msf_flags.s1_mfl = 1;  /* source 1 gun */
                     sr1->tot_mf = sr1->tot_mf+1;
                  }
                  if(info->srce==2) {
                     msf_flags.s2_mfl = 1;  /* ditto source 2 */
                     sr2->tot_mf = sr2->tot_mf+1;
                  }
                  if(info->srce==3) {
                     msf_flags.s3_mfl = 1;  /* ditto 3 */
                     sr3->tot_mf = sr3->tot_mf+1;
                  }
                  if(info->srce==4) {
                     msf_flags.s4_mfl = 1;  /* ditto 4 */
                     sr4->tot_mf = sr4->tot_mf+1;
                  }
               } else {  /* valid fire time so check it */
                     /* subtract 640 from fire time for error time */
                  error -= 640;
                  if(andef->gun_anl1) {
                     if (abs(error)>(int)gun->sp1) {
                        if(andef->lgun_anl1) {
                           sprintf(t_buf, "Gun %-10s %+-6.2f mS",
                                      info->gun_name, ((float)error)/10.0F);
                           scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                        }
                        info->tot_sp1 = info->tot_sp1+1;
                        if(info->srce==1) msf_flags.s1_1fl = 1;  /* source 1 gun */
                        if(info->srce==2) msf_flags.s2_1fl = 1;  /* ditto source 2 */
                        if(info->srce==3) msf_flags.s3_1fl = 1;  /* ditto 3 */
                        if(info->srce==4) msf_flags.s4_1fl = 1;  /* ditto 4 */
                        if(graf->gun_anl1) {   /* do graphics array */
                           if( (float)fabs(((float)error)/10.0F) > *srce_graf_tmp) {
                              *srce_graf_tmp = ((float)error)/10.0F;
                              grerr.gu = 1;
                           }
                        }
                     }
                  }
                  if(andef->gun_anl2) {
                     if (abs(error)>(int)gun->sp2) {
                        if(andef->lgun_anl2) {
                           sprintf(t_buf, "Gun %-10s * %+-6.2f mS",
                                     info->gun_name, ((float)error)/10.0F);
                           scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
                        }
                        info->tot_sp2 = info->tot_sp2+1;
                        if(info->srce==1) msf_flags.s1_2fl = 1;  /* source 1 gun */
                        if(info->srce==2) msf_flags.s2_2fl = 1;  /* ditto source 2 */
                        if(info->srce==3) msf_flags.s3_2fl = 1;  /* ditto 3 */
                        if(info->srce==4) msf_flags.s4_2fl = 1;  /* ditto 4 */
                     }
                  }
                  sp_vol = sp_vol + info->gun_vol;
               }
				}
			}
      info = info->next;
		}
   }  /* end digicon trace0 */
         /* source misfires */
   if(msf_flags.s1_mfl) sr1->mf = sr1->mf+1;
	if(msf_flags.s2_mfl) sr2->mf = sr2->mf+1;
	if(msf_flags.s3_mfl) sr3->mf = sr3->mf+1;
	if(msf_flags.s4_mfl) sr4->mf = sr4->mf+1;
         /* source timing spec1 */
   if(msf_flags.s1_1fl) sr1->sp1 = sr1->sp1 + 1;
   if(msf_flags.s2_1fl) sr2->sp1 = sr2->sp1 + 1;
   if(msf_flags.s3_1fl) sr3->sp1 = sr3->sp1 + 1;
   if(msf_flags.s4_1fl) sr4->sp1 = sr4->sp1 + 1;
         /* source timing spec2 */
   if(msf_flags.s1_2fl) sr1->sp2 = sr1->sp2 + 1;
   if(msf_flags.s2_2fl) sr2->sp2 = sr2->sp2 + 1;
   if(msf_flags.s3_2fl) sr3->sp2 = sr3->sp2 + 1;
   if(msf_flags.s4_2fl) sr4->sp2 = sr4->sp2 + 1;
         /* check for more than 1 local source fired */
	if((msf_flags.s1_efl+msf_flags.s2_efl+msf_flags.s3_efl+msf_flags.s4_efl)>1) {
      xsr->x_fire = xsr->x_fire+1;
      if(andef->lgun_anl1 || andef->lgun_anl2)
         scr_error("CONTROLLER > 1 SOURCE FIRED",
                       errbuf, sp, fn, Werroll, flog);
   }
         /* check if ANY local source fired */
   if(!(msf_flags.s1_efl+msf_flags.s2_efl+msf_flags.s3_efl+msf_flags.s4_efl)) {
      xsr->no_fire = xsr->no_fire+1;
      if(!pd->rem_srce) {  /* if no remote sources - flag error */
         if(andef->lgun_anl1 || andef->lgun_anl2)
            scr_error("CONTROLLER - NO SOURCE FIRED", errbuf, sp, fn, Werroll, flog);
      }
   } else xsr->tot_sp = xsr->tot_sp+1; /* SP at least 1 gun fired */
			/* simple overall misfire summary */
	if(msf_flags.s1_1fl || msf_flags.s2_1fl || msf_flags.s3_1fl ||  msf_flags.s4_1fl)   /* source spec 1 */
		xsr->spec1 = xsr->spec1+1;
	if(msf_flags.s1_2fl || msf_flags.s2_2fl || msf_flags.s3_2fl ||  msf_flags.s4_2fl)   /* source spec 2 */
      xsr->spec2 = xsr->spec2+1;
         /* source volunes */
   srce_vol (msf_flags, sp, fn, Werroll, flog, sp_vol, errbuf);
         /* consecutive, series, & running % misfires */
   srce_msf (msf_flags, sp, fn, Werroll, flog, errbuf);
         /* graphics update */
   if(graf->gun_anl1) srce_graf_tmp++;
   if(graf->vol_anl1) vol_graf_tmp++;
}


/*************************************************************
* source volumes
*************************************************************/
void srce_vol (struct MSF_FLAGS msf_flags, long sp, long fn,
            HWND Werroll, FILE *flog, unsigned sp_vol, char *errbuf )
{
	char t_buf[41];
   if(andef->vol_anl1 && (sp_vol < spc->gvol1) ) {  /* if below Spec 1 */
         /* put value in graphics array */
      if(graf->vol_anl1) *vol_graf_tmp = sp_vol;
         /* source 1 */
		if(msf_flags.s1_efl) {
			sr1->vol1 = sr1->vol1+1;
         sr1->vol1_fl = 1;
         grerr.vl = 1;
         if( (sp_vol!=sr1->sp_vol) && andef->lvol_anl1){
            sprintf(t_buf, "Vol Change %s < Spec1: %u", pd->loc1, sp_vol);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			}
      }
         /* source 2 */
		if(msf_flags.s2_efl) {
			sr2->vol1 = sr2->vol1+1;
         sr2->vol1_fl = 1;
         grerr.vl = 1;
         if( (sp_vol!=sr2->sp_vol) && andef->lvol_anl1) {
            sprintf(t_buf, "Vol Change %s < Spec1: %u", pd->loc2, sp_vol);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			}
      }
         /* source 3 */
		if(msf_flags.s3_efl) {
			sr3->vol1 = sr3->vol1+1;
			sr3->vol1_fl = 1;
         grerr.vl = 1;
         if( (sp_vol!=sr3->sp_vol) && andef->lvol_anl1) {
            sprintf(t_buf, "Vol Change %s < Spec1: %u", pd->loc3, sp_vol);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			}
      }
         /* source 4 */
		if(msf_flags.s4_efl) {
			sr4->vol1 = sr4->vol1+1;
			sr4->vol1_fl = 1;
         grerr.vl = 1;
         if( (sp_vol!=sr4->sp_vol) && andef->lvol_anl1) {
            sprintf(t_buf, "Vol Change %s < Spec1: %u", pd->loc4, sp_vol);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			}
		}
	} else {
      if (andef->vol_anl1 && (sp_vol >= spc->gvol1) ) {
            /* source 1 */
			if(msf_flags.s1_efl&&sr1->vol1_fl) {
				sr1->vol1_fl = 0;
            if(andef->lvol_anl1) {
               sprintf(t_buf, "Vol Back %s > Spec1: %u", pd->loc1, sp_vol);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
            /* source 2 */
			if(msf_flags.s2_efl&&sr2->vol1_fl) {
				sr2->vol1_fl = 0;
            if(andef->lvol_anl1) {
               sprintf(t_buf, "Vol Back %s > Spec1: %u", pd->loc2, sp_vol);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
            /* source 3 */
			if(msf_flags.s3_efl&&sr3->vol1_fl) {
				sr3->vol1_fl = 0;
            if(andef->lvol_anl1) {
               sprintf(t_buf, "Vol Back %s > Spec1: %u", pd->loc3, sp_vol);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);\
            }
         }
            /* source 4 */
			if(msf_flags.s4_efl&&sr4->vol1_fl) {
				sr4->vol1_fl = 0;
            if(andef->lvol_anl1) {
               sprintf(t_buf, "Vol Back %s > Spec1: %u", pd->loc4, sp_vol);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
			}
		}
	}
   if(andef->vol_anl2 && (sp_vol < spc->gvol2) ) {
         /* source 1 */
		if(msf_flags.s1_efl) {
			sr1->vol2 = sr1->vol2+1;
			sr1->vol2_fl = 1;
         if( (sp_vol!=sr1->sp_vol) && andef->lvol_anl2) {
            sprintf(t_buf, "Vol Change %s < Spec2: %u", pd->loc1, sp_vol);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			}
      }
         /* source 2 */
		if(msf_flags.s2_efl) {
			sr2->vol2 = sr2->vol2+1;
			sr2->vol2_fl = 1;
         if( (sp_vol!=sr2->sp_vol) && andef->lvol_anl2) {
            sprintf(t_buf, "Vol Change %s < Spec2: %u", pd->loc2, sp_vol);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			}
      }
         /* source 3 */
		if(msf_flags.s3_efl) {
			sr3->vol2 = sr3->vol2+1;
			sr3->vol2_fl = 1;
         if( (sp_vol!=sr3->sp_vol) && andef->lvol_anl2) {
            sprintf(t_buf, "Vol Change %s < Spec2: %u", pd->loc3, sp_vol);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			}
      }
         /* source 4 */
		if(msf_flags.s4_efl) {
			sr4->vol2 = sr4->vol2+1;
			sr4->vol2_fl = 1;
         if( (sp_vol!=sr4->sp_vol) && andef->lvol_anl2) {
            sprintf(t_buf, "Vol Change %s < Spec2: %u", pd->loc4, sp_vol);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
			}
		}
	} else {
      if (andef->vol_anl2 && (sp_vol >= spc->gvol2) ) {
            /* source 1 */
         if(msf_flags.s1_efl&&sr1->vol2_fl) {
				sr1->vol2_fl = 0;
            if(andef->lvol_anl2) {
               sprintf(t_buf, "Vol Back %s > Spec2: %u", pd->loc1, sp_vol);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
            /* source 2 */
			if(msf_flags.s2_efl&&sr2->vol2_fl) {
				sr2->vol2_fl = 0;
            if(andef->lvol_anl2) {
               sprintf(t_buf, "Vol Back %s > Spec2: %u", pd->loc2, sp_vol);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
            /* source 3 */
			if(msf_flags.s3_efl&&sr3->vol2_fl) {
				sr3->vol2_fl = 0;
            if(andef->lvol_anl2) {
               sprintf(t_buf, "Vol Back %s > Spec2: %u", pd->loc3, sp_vol);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
            /* source 4 */
			if(msf_flags.s4_efl&&sr4->vol2_fl) {
				sr4->vol2_fl = 0;
            if(andef->lvol_anl2) {
               sprintf(t_buf, "Vol Back %s > Spec2: %u", pd->loc4, sp_vol);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
			}
		}
   }
      /* do SOL volumes */
   if(msf_flags.s1_efl) {    /* source 1 */
		if(!sr1->en_num) {
			sr1->sol_vol = sp_vol;
         if(andef->vol_anl1 || andef->vol_anl2) {
            sprintf(t_buf, "SOL VOLUME %s: %u %s", pd->loc1, sr1->sol_vol, units[pd->vol_un]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
		}
		sr1->en_num = sr1->en_num+1;
		sr1->sp_vol = sp_vol;
	}
   if(msf_flags.s2_efl) {    /* source 2 */
		if(!sr2->en_num) {
         sr2->sol_vol = sp_vol;
         if(andef->vol_anl1 || andef->vol_anl2) {
            sprintf(t_buf, "SOL VOLUME %s: %u %s", pd->loc2, sr2->sol_vol, units[pd->vol_un]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
		}
		sr2->en_num = sr2->en_num+1;
		sr2->sp_vol = sp_vol;
	}
   if(msf_flags.s3_efl) {    /* source 3 */
		if(!sr3->en_num) {
         sr3->sol_vol = sp_vol;
         if(andef->vol_anl1 || andef->vol_anl2) {
            sprintf(t_buf, "SOL VOLUME %s: %u %s", pd->loc3, sr3->sol_vol, units[pd->vol_un]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
		}
		sr3->en_num = sr3->en_num+1;
		sr3->sp_vol = sp_vol;
	}
   if(msf_flags.s4_efl) {    /* source 4 */
		if(!sr4->en_num) {
         sr4->sol_vol = sp_vol;
         if(andef->vol_anl1 || andef->vol_anl2) {
            sprintf(t_buf, "SOL VOLUME %s: %u %s", pd->loc4, sr4->sol_vol, units[pd->vol_un]);
            scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
         }
		}
		sr4->en_num = sr4->en_num+1;
		sr4->sp_vol = sp_vol;
   }
}


/*************************************************************
* consecutive, series, & running % misfires
*************************************************************/
void srce_msf (struct MSF_FLAGS msf_flags, long sp, long fn,
                     HWND Werroll, FILE *flog, char *errbuf )
{
   register int i;
   char t_buf[41];
      /* consecutive misfires */
   if (andef->mf1_anl) {
      if(msf_flags.s1_efl) {    /* source 1 */
         if(msf_flags.s1_1fl)
            msf->s1c1 = msf->s1c1 + 1;  /* add to consecutive total */
         else msf->s1c1 = 0;            /* else set consecutive total to 0 */
         if(msf_flags.s1_2fl)
            msf->s1c2 = msf->s1c2 + 1;
         else msf->s1c2 = 0;
      }
      if(msf_flags.s2_efl) {    /* source 2 */
         if(msf_flags.s2_1fl)
            msf->s2c1 = msf->s2c1 + 1;
         else msf->s2c1 = 0;
         if(msf_flags.s2_2fl)
            msf->s2c2 = msf->s2c2 + 1;
         else msf->s2c2 = 0;
      }
      if(msf_flags.s3_efl) {    /* source 3 */
         if(msf_flags.s3_1fl)
            msf->s3c1 = msf->s3c1 + 1;
         else msf->s3c1 = 0;
         if(msf_flags.s3_2fl)
            msf->s3c2 = msf->s3c2 + 1;
         else msf->s3c2 = 0;
      }
      if(msf_flags.s4_efl) {    /* source 4 */
         if(msf_flags.s4_1fl)
            msf->s4c1 = msf->s4c1 + 1;
         else msf->s4c1 = 0;
         if(msf_flags.s4_2fl)
            msf->s4c2 = msf->s4c2 + 1;
         else msf->s4c2 = 0;
      }
         /* add up totals */
      if(msf_flags.s1_efl) {    /* source 1 */
         if(msf->s1c1 > spc->mf1) {           /* source 1 spec 1 */
            msf->s1ct1 = msf->s1ct1 + 1;
            if(andef->lmf1_anl) {
               sprintf(t_buf, "%s: %u Consecutive > %-.2f mS",
                  pd->loc1, msf->s1c1, spc->gun1);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
         if(msf->s1c2 > spc->mf1) {           /* source 1 spec 2 */
            msf->s1ct2 = msf->s1ct2 + 1;
            if(andef->lmf1_anl) {
               sprintf(t_buf, "%s: %u Consecutive > %-.2f mS",
                  pd->loc1, msf->s1c2, spc->gun2);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
      }
      if(msf_flags.s2_efl) {    /* source 2 */
         if(msf->s2c1 > spc->mf1) {           /* source 2 spec 1 */
            msf->s2ct1 = msf->s2ct1 + 1;
            if(andef->lmf1_anl) {
               sprintf(t_buf, "%s: %u Consecutive > %-.2f mS",
                  pd->loc2, msf->s2c1, spc->gun1);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
         if(msf->s2c2 > spc->mf1) {           /* source 2 spec 2 */
            msf->s2ct2 = msf->s2ct2 + 1;
            if(andef->lmf1_anl) {
               sprintf(t_buf, "%s: %u Consecutive > %-.2f mS",
                  pd->loc2, msf->s2c2, spc->gun2);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
      }
      if(msf_flags.s3_efl) {    /* source 3 */
         if(msf->s3c1 > spc->mf1) {           /* source 3 spec 1 */
            msf->s3ct1 = msf->s3ct1 + 1;
            if(andef->lmf1_anl) {
               sprintf(t_buf, "%s: %u Consecutive > %-.2f mS",
                  pd->loc3, msf->s3c1, spc->gun1);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
         if(msf->s3c2 > spc->mf1) {           /* source 3 spec 2 */
            msf->s3ct2 = msf->s3ct2 + 1;
            if(andef->lmf1_anl) {
               sprintf(t_buf, "%s: %u Consecutive > %-.2f mS",
                  pd->loc3, msf->s3c2, spc->gun2);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
      }
      if(msf_flags.s4_efl) {    /* source 4 */
         if(msf->s4c1 > spc->mf1) {           /* source 4 spec 1 */
            msf->s4ct1 = msf->s4ct1 + 1;
            if(andef->lmf1_anl) {
               sprintf(t_buf, "%s: %u Consecutive > %-.2f mS",
                  pd->loc4, msf->s4c1, spc->gun1);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
         if(msf->s4c2 > spc->mf1) {           /* source 4 spec 2 */
            msf->s4ct2 = msf->s4ct2 + 1;
            if(andef->lmf1_anl) {
               sprintf(t_buf, "%s: %u Consecutive > %-.2f mS",
                  pd->loc4, msf->s4c2, spc->gun2);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
            }
         }
      }
   }
      /* misfire series 'x in Y' */
   if (andef->mf3_anl) {
      if(msf_flags.s1_efl) {    /* source 1 */
         if(sr1->en_num <= spc->mf3_ra)  {
            if(msf_flags.s1_1fl)
               s1m1[sr1->en_num -1] = 1;
            else s1m1[sr1->en_num -1] = 0;
            if(msf_flags.s1_2fl)
               s1m2[sr1->en_num -1] = 1;
            else s1m2[sr1->en_num -1] = 0;
         } else {
            for(i=0; i<(int)(spc->mf3_ra-1); i++) {
               s1m1[i] = s1m1[i+1];
               s1m2[i] = s1m2[i+1];
            }
            if(msf_flags.s1_1fl)
               s1m1[spc->mf3_ra-1] = 1;
            else s1m1[spc->mf3_ra-1] = 0;
            if(msf_flags.s1_2fl)
               s1m2[spc->mf3_ra-1] = 1;
            else s1m2[spc->mf3_ra-1] = 0;
         }
      }
      if(msf_flags.s2_efl) {    /* source 2 */
         if(sr2->en_num <= spc->mf3_ra)  {
            if(msf_flags.s2_1fl)
               s2m1[sr2->en_num -1] = 1;
            else s2m1[sr2->en_num -1] = 0;
            if(msf_flags.s2_2fl)
               s2m2[sr2->en_num -1] = 1;
            else s2m2[sr2->en_num -1] = 0;
         } else {
            for(i=0; i<(int)(spc->mf3_ra-1); i++) {
               s2m1[i] = s2m1[i+1];
               s2m2[i] = s2m2[i+1];
            }
            if(msf_flags.s2_1fl)
               s2m1[spc->mf3_ra-1] = 1;
            else s2m1[spc->mf3_ra-1] = 0;
            if(msf_flags.s2_2fl)
               s2m2[spc->mf3_ra-1] = 1;
            else s2m2[spc->mf3_ra-1] = 0;
         }
      }
      if(msf_flags.s3_efl) {    /* source 3 */
         if(sr3->en_num <= spc->mf3_ra)  {
            if(msf_flags.s3_1fl)
               s3m1[sr3->en_num -1] = 1;
            else s3m1[sr3->en_num -1] = 0;
            if(msf_flags.s3_2fl)
               s3m2[sr3->en_num -1] = 1;
            else s3m2[sr3->en_num -1] = 0;
         } else {
            for(i=0; i<(int)(spc->mf3_ra-1); i++) {
               s3m1[i] = s3m1[i+1];
               s3m2[i] = s3m2[i+1];
            }
            if(msf_flags.s3_1fl)
               s3m1[spc->mf3_ra-1] = 1;
            else s3m1[spc->mf3_ra-1] = 0;
            if(msf_flags.s3_2fl)
               s3m2[spc->mf3_ra-1] = 1;
            else s3m2[spc->mf3_ra-1] = 0;
         }
      }
      if(msf_flags.s4_efl) {    /* source 4 */
         if(sr4->en_num <= spc->mf3_ra)  {
            if(msf_flags.s4_1fl)
               s4m1[sr4->en_num -1] = 1;
            else s4m1[sr4->en_num -1] = 0;
            if(msf_flags.s4_2fl)
               s4m2[sr4->en_num -1] = 1;
            else s4m2[sr4->en_num -1] = 0;
         } else {
            for(i=0; i<(int)(spc->mf3_ra-1); i++) {
               s4m1[i] = s4m1[i+1];
               s4m2[i] = s4m2[i+1];
            }
            if(msf_flags.s4_1fl)
               s4m1[spc->mf3_ra-1] = 1;
            else s4m1[spc->mf3_ra-1] = 0;
            if(msf_flags.s4_2fl)
               s4m2[spc->mf3_ra-1] = 1;
            else s4m2[spc->mf3_ra-1] = 0;
         }
      }
      if(msf_flags.s1_efl) {    /* source 1 */
         if(sr1->en_num >= spc->mf3_ra)  {
            ext->a_num = ext->b_num = 0;
            for(i=0; i<(int)(spc->mf3_ra); i++) {
               ext->a_num = ext->a_num + s1m1[i];
               ext->b_num = ext->b_num + s1m2[i];
            }
            if(ext->a_num > spc->mf3) {
               msf->s1mt1 = msf->s1mt1 + 1;
               if(andef->lmf3_anl) {
               sprintf(t_buf, "%s: %u In %u > %-.2f mS",
                  pd->loc1, ext->a_num, spc->mf3_ra, spc->gun1);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
            if(ext->b_num > spc->mf3) {
               msf->s1mt2 = msf->s1mt2 + 1;
               if(andef->lmf3_anl) {
               sprintf(t_buf, "%s: %u In %u > %-.2f mS",
                  pd->loc1, ext->b_num, spc->mf3_ra, spc->gun2);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
      }
      if(msf_flags.s2_efl) {    /* source 2 */
         if(sr2->en_num >= spc->mf3_ra)  {
            ext->a_num = ext->b_num = 0;
            for(i=0; i<(int)(spc->mf3_ra); i++) {
               ext->a_num = ext->a_num + s2m1[i];
               ext->b_num = ext->b_num + s2m2[i];
            }
            if(ext->a_num > spc->mf3) {
               msf->s2mt1 = msf->s2mt1 + 1;
               if(andef->lmf3_anl) {
               sprintf(t_buf, "%s: %u In %u > %-.2f mS",
                  pd->loc2, ext->a_num, spc->mf3_ra, spc->gun1);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
            if(ext->b_num > spc->mf3) {
               msf->s2mt2 = msf->s2mt2 + 1;
               if(andef->lmf3_anl) {
               sprintf(t_buf, "%s: %u In %u > %-.2f mS",
                  pd->loc2, ext->b_num, spc->mf3_ra, spc->gun2);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
      }
      if(msf_flags.s3_efl) {    /* source 3 */
         if(sr3->en_num >= spc->mf3_ra)  {
            ext->a_num = ext->b_num = 0;
            for(i=0; i<(int)(spc->mf3_ra); i++) {
               ext->a_num = ext->a_num + s3m1[i];
               ext->b_num = ext->b_num + s3m2[i];
            }
            if(ext->a_num > spc->mf3) {
               msf->s3mt1 = msf->s3mt1 + 1;
               if(andef->lmf3_anl) {
               sprintf(t_buf, "%s: %u In %u > %-.2f mS",
                  pd->loc3, ext->a_num, spc->mf3_ra, spc->gun1);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
            if(ext->b_num > spc->mf3) {
               msf->s3mt2 = msf->s3mt2 + 1;
               if(andef->lmf3_anl) {
               sprintf(t_buf, "%s: %u In %u > %-.2f mS",
                  pd->loc3, ext->b_num, spc->mf3_ra, spc->gun2);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
      }
      if(msf_flags.s4_efl) {    /* source 4 */
         if(sr4->en_num >= spc->mf3_ra)  {
            ext->a_num = ext->b_num = 0;
            for(i=0; i<(int)(spc->mf3_ra); i++) {
               ext->a_num = ext->a_num + s4m1[i];
               ext->b_num = ext->b_num + s4m2[i];
            }
            if(ext->a_num > spc->mf3) {
               msf->s4mt1 = msf->s4mt1 + 1;
               if(andef->lmf3_anl) {
               sprintf(t_buf, "%s: %u In %u > %-.2f mS",
                  pd->loc4, ext->a_num, spc->mf3_ra, spc->gun1);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
            if(ext->b_num > spc->mf3) {
               msf->s4mt2 = msf->s4mt2 + 1;
               if(andef->lmf3_anl) {
               sprintf(t_buf, "%s: %u In %u > %-.2f mS",
                  pd->loc4, ext->b_num, spc->mf3_ra, spc->gun2);
               scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
      }
   }
      /* running misfire % */
   if (andef->mf4_anl) {
      if(msf_flags.s1_efl) {    /* source 1 */
         if(msf_flags.s1_1fl) {
            sr1->p_spec1 = ((float)sr1->sp1)*100.0F/((float)sr1->en_num);
            if(sr1->p_spec1 > (float)spc->mf4) {
               msf->s1pt1 = msf->s1pt1 + 1;
               if (andef->lmf4_anl) {
                  sprintf(t_buf, "%s: Errors > %-.2f mS: %-.1f %%",
                     pd->loc1, spc->gun1, sr1->p_spec1);
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
         if(msf_flags.s1_2fl) {
            sr1->p_spec2 = ((float)sr1->sp2)*100.0F/((float)sr1->en_num);
            if(sr1->p_spec2 > (float)spc->mf4) {
               msf->s1pt2 = msf->s1pt2 + 1;
               if (andef->lmf4_anl) {
                  sprintf(t_buf, "%s: Errors > %-.2f mS: %-.1f %%",
                     pd->loc1, spc->gun2, sr1->p_spec2);
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
      }
      if(msf_flags.s2_efl) {    /* source 2 */
         if(msf_flags.s2_1fl) {
            sr2->p_spec1 = ((float)sr2->sp1)*100.0F/((float)sr2->en_num);
            if(sr2->p_spec1 > (float)spc->mf4) {
               msf->s2pt1 = msf->s2pt1 + 1;
               if (andef->lmf4_anl) {
                  sprintf(t_buf, "%s: Errors > %-.2f mS: %-.1f %%",
                     pd->loc2, spc->gun1, sr2->p_spec1);
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
         if(msf_flags.s2_2fl) {
            sr2->p_spec2 = ((float)sr2->sp2)*100.0F/((float)sr2->en_num);
            if(sr2->p_spec2 > (float)spc->mf4) {
               msf->s2pt2 = msf->s2pt2 + 1;
               if (andef->lmf4_anl) {
                  sprintf(t_buf, "%s: Errors > %-.2f mS: %-.1f %%",
                     pd->loc2, spc->gun2, sr2->p_spec2);
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
      }
      if(msf_flags.s3_efl) {    /* source 3 */
         if(msf_flags.s3_1fl) {
            sr3->p_spec1 = ((float)sr3->sp1)*100.0F/((float)sr3->en_num);
            if(sr3->p_spec1 > (float)spc->mf4) {
               msf->s3pt1 = msf->s3pt1 + 1;
               if (andef->lmf4_anl) {
                  sprintf(t_buf, "%s: Errors > %-.2f mS: %-.1f %%",
                     pd->loc3, spc->gun1, sr3->p_spec1);
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
         if(msf_flags.s3_2fl) {
            sr3->p_spec2 = ((float)sr3->sp2)*100.0F/((float)sr3->en_num);
            sr3->p_spec2 = (float) (sr3->sp2/sr3->en_num) * 100.0;
            if(sr3->p_spec2 > (float)spc->mf4) {
               msf->s3pt2 = msf->s3pt2 + 1;
               if (andef->lmf4_anl) {
                  sprintf(t_buf, "%s: Errors > %-.2f mS: %-.1f %%",
                     pd->loc3, spc->gun2, sr3->p_spec2);
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
      }
      if(msf_flags.s4_efl) {    /* source 4 */
         if(msf_flags.s4_1fl) {
            sr4->p_spec1 = ((float)sr4->sp1)*100.0F/((float)sr4->en_num);
            if(sr4->p_spec1 > (float)spc->mf4) {
               msf->s4pt1 = msf->s4pt1 + 1;
               if (andef->lmf4_anl) {
                  sprintf(t_buf, "%s: Errors > %-.2f mS: %-.1f %%",
                     pd->loc4, spc->gun1, sr4->p_spec1);
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
         if(msf_flags.s4_2fl) {
            sr4->p_spec2 = ((float)sr4->sp2)*100.0F/((float)sr4->en_num);
            if(sr4->p_spec2 > (float)spc->mf4) {
               msf->s4pt2 = msf->s4pt2 + 1;
               if (andef->lmf4_anl) {
                  sprintf(t_buf, "%s: Errors > %-.2f mS: %-.1f %%",
                     pd->loc4, spc->gun2, sr4->p_spec2);
                  scr_error(t_buf, errbuf, sp, fn, Werroll, flog);
               }
            }
         }
      }
   }
}
