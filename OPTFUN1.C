/*************************************************************
*   OPTFUN1.C   system status & critical error handler
****************************************************************
*
*
*	Compiler:  MSC V6.00  /AL /W4 /Os /Gs /Zp /Zl /G2 /ST:3072
*  Linker:    MSC V5.10  /NOE /SE:256 /NOD /EXEPACK
*  Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
*             Greenleaf DataWindows  (MSC) V2.10  GDWL.LIB
*             Greenleaf Superfunctions (MSC) V1.00 GSFL.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*	(c) West Country Systems Ltd, Cyprus, 1992
****************************************************************/

#include <stdio.h>      /* microsoft */
#include <stdlib.h>
#include <bios.h>
#include <dos.h>
#include <ctype.h>
#include <process.h>
#include <malloc.h>
#include <direct.h>

#include <dw.h>         /* greenleaf */
#include <sfemm.h>
#include <ibmkeys.h>
#include <sfdos.h>		/* note - had to modify sfdos.h & sf.h for */
#include <sftd.h> 		/* incorrect function prototypes */

#include <stxfunct.h>   /* wcs */
#include <stxconst.h>

extern HWND Wwait;              /* wait window (StWait() & EnWait() */


/**************************************************************
*	 DOS/Expanded/Extended/Memory/Disks/Etc. status
**************************************************************/
void st_mem (void)
{
   HWND Wmem;
   unsigned key;
	register int p=0, ver;
	float avail, total;					 /* available & total space */
   unsigned drive, num_drives, pstatus;
   char cur_dir[_MAX_PATH];        /* current directory */
      /* allocate structure for equipment status word */
	union {                           /* Access equiment either as:    */
		unsigned u;                    /*   unsigned or                 */
		struct {                       /*   bit fields                  */
			unsigned diskflag : 1;      /* Diskette drive installed?     */
			unsigned coprocessor : 1;   /* Coprocessor? (except on PC)   */
			unsigned sysram : 2;        /* RAM on system board           */
			unsigned video : 2;         /* Startup video mode            */
			unsigned disks : 2;         /* Drives 00=1, 01=2, 10=3, 11=4 */
			unsigned dma : 1;           /* 0=Yes, 1=No (1 for PC Jr.)    */
			unsigned comports : 3;      /* Serial ports                  */
			unsigned game : 1;          /* Game adapter installed?       */
			unsigned modem : 1;         /* Internal modem?               */
			unsigned printers : 2;      /* Number of printers            */
		 } bits;
	} equip;
	StWait();
   Wmem = vcreat (21, 41, NORML, NO);
   _heapmin();
   vatputf (Wmem, ++p, 2, "DOS Version: %d.%d", _osmajor, _osminor);
   vatputf (Wmem, ++p, 2, "DOS Memory Size: %uK", _bios_memsize());
   vatputf (Wmem, ++p, 2, "DOS Memory Available: %ld",
													  16L*(long)dos_mem());
   vatputf (Wmem, ++p, 2, "Near Heap Memory Free: %u", _memavl());
   vatputf (Wmem, ++p, 2, "Largest Free Block: %u", _memmax());
   vatputf (Wmem, ++p, 2, "Stack Memory Available: %u", stackavail());
	vatputf (Wmem, ++p, 2, "Extended Memory Size:  %uK", ext_mem());
	if ( !EMMPresent(1) || EMMGetStatus()<SFSUCCESS )
      vatputs(Wmem, ++p, 2, "EMM Not installed");
	else {
		ver = EMMGetVersion();
      vatputf(Wmem, ++p, 2, "EMM Version: %d.%d",
			((ver&0xf0)>>4), (ver&0x0f) );
      vatputf(Wmem, ++p, 2, "EMM Size: %dK",
					  16*EMMGetTotalPages() );
      vatputf(Wmem, ++p, 2, "EMM Available: %dK",
			16*EMMGetUnallocatedPageCount() );
	}
	equip.u = _bios_equiplist();
	vatputs(Wmem, ++p, 2, "Coprocessor:");
	if( equip.bits.coprocessor ) vatputs(Wmem, p, 16, "YES");
	else vratputs(Wmem, p, 16, REVERR, "NO");
	vatputf(Wmem, ++p, 2, "Serial ports:  %d", equip.bits.comports );
	 /* Fail if any error bit is on, or if either operation bit is off. */
	 pstatus = _bios_printer( _PRINTER_STATUS, LPT1, 0 );
	 if( (pstatus & 0x29) || !(pstatus & 0x80) || !(pstatus & 0x10) )
		  pstatus = 0;
	 else
		  pstatus = 1;
	 vatputs(Wmem, ++p, 2, "Printer Ready:");
	if( pstatus ) vatputs(Wmem, p, 18, "YES");
	else vratputs(Wmem, p, 18, REVERR, "NO");
	_dos_getdrive(&drive);   /* get current disk drive */
	getcwd(cur_dir, _MAX_PATH);	/* get current directory */
	_dos_setdrive(1, &num_drives);
	if(chdir("A:\\"))
      vratputs(Wmem, p+=2, 2, REVERR, "A: Media Unavailable");
	else {
		get_free('A', &avail, &total);
      vatputf(Wmem, p+=2, 2, "A:  %d K free of %d K",
			 (int)avail, (int)total);
	}
	if(equip.bits.disks) {
		_dos_setdrive(2, &num_drives);
		if(chdir("B:\\"))
         vratputs(Wmem, ++p, 2, REVERR, "B: Media Unavailable");
		else {
			get_free('B', &avail, &total);
         vatputf(Wmem, ++p, 2, "B:  %d K free of %d K",
				 (int)avail, (int)total);
		}
	}
	if(num_drives >2) {
		_dos_setdrive(3, &num_drives);
		if(!chdir("C:\\")) {
			get_free('C', &avail, &total);
			vatputf(Wmem, ++p, 2, "C:  %ld K free of %ld K",
					(long)avail, (long)total);
		}
	}
	if(num_drives >3) {
		_dos_setdrive(4, &num_drives);
		if(!chdir("D:\\")) {
			get_free('D', &avail, &total);
			vatputf(Wmem, ++p, 2, "D:  %ld K free of %ld K",
					(long)avail, (long)total);
		}
	}
	if(num_drives >4) {
		_dos_setdrive(5, &num_drives);
		if(!chdir("E:\\")) {
			get_free('E', &avail, &total);
			vatputf(Wmem, ++p, 2, "E:  %ld K free of %ld K",
					(long)avail, (long)total);
		}
	}
	_dos_setdrive(drive, &num_drives);	 /* return to work drive */
	chdir(cur_dir);							 /* return to current directory */
#ifdef DEBUG
	vratputs(Wmem, p+=2, 2, EMPHNORML, "<ALT-M> Walk Heap");
#endif
	vratputs(Wmem, p, 29, EMPHNORML, "<ESC> Exit");
   vwind (Wmem, p+=2, 41, 0, 0);
   vlocate (Wmem, 2, 18);
	vframe (Wmem, EMPHNORML, FRDOUBLE);
   vtitle (Wmem, EMPHNORML, " Status ");
	vshadow(Wmem, BACK3D, SHADOW3D, BOTTOMRIGHT);   /* add 3D effect */
	EnWait();
	visible(Wmem, YES, NO);
   while( (key = MouseKey(1, 1)) != ESC) {
      if(key == F1)
         syst_help();
#ifdef DEBUG
      if(key == ALTM)
			heapstat();   /* walk the heap */
#endif
	}
	vdelete(Wmem, NONE);
}


/**************************************************************
*   get total & free space on a drive
**************************************************************/
void get_free(char drv, float *avail, float *total)
{
	union REGS regs;
	struct SREGS segs;
	int dn;
         /* determines drive and sets drive number */
	if(drv) {
		drv = (char) toupper(drv);
		dn = drv - 'A' +1;
	} else {
		dn = 0;
	}
			/* sets up and makes the DOS function call */
	regs.h.ah = 0x36;
	regs.h.dl = (unsigned char) dn;
	intdosx(&regs, &regs, &segs);
	*avail = (((float)regs.x.ax * (float)regs.x.cx)/1024) * (float)regs.x.bx;
	*total = (((float)regs.x.ax * (float)regs.x.cx)/1024) * (float)regs.x.dx;
}


/*************************************************************
* return available DOS memory by trying to allocate too much
* memory - this fails and returns total available
* thanks andy
**************************************************************/
dos_mem()
{
	union REGS inregs, outregs;
	inregs.h.ah = 0x48;
	inregs.x.bx = 0xFFFF;
	intdos (&inregs, &outregs);
	return(outregs.x.bx);
}


/*************************************************************
* return total extended memory via BIOS call
**************************************************************/
ext_mem()
{
	union REGS inregs, outregs;
	inregs.h.ah = 0x88;
	int86 (0x15, &inregs, &outregs);
	return(outregs.x.ax);
}


/***************************************************************
*	Handler to deal with hard error codes. Since DOS is not
*	reentrant, it is not safe to use DOS calls for doing I/O
*	within the Dos Critical Error Handler (int 24h) used by
*	_harderr. Therefore, screen output and
*	keyboard input must be done through the BIOS.
****************************************************************/
void _far hhandler( unsigned deverr, unsigned doserr, unsigned _far *hdr )
{
	_hardretn( doserr );
}


/*************************************************************
*   system status help
**************************************************************/
void syst_help()
{
   string_help("System Status");
}
