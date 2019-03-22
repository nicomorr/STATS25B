/***************************************************************
*	 VGR.C	 Graphics charting routines for speeds
*				 Called from STS.EXE (versions 2.4 & above)
*				 via STS_VGR.EXE (intermediate spawn program)
****************************************************************
*
*	Version:   1.10
*	Compiler:  MSC V6.00  /AL /W4 /Os /Gs/Zl /G2
*	Linker:	  MSC V5.10  /NOE /NOD /EXEPACK
*	Libraries: MSC V6.00 LLIBCE.LIB (inc GRAPHICS.LIB)
				  MC006D.LIB Quinn-Curtis V7
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
*  Include:   Quinn-Curtis as below
*	Fonts:	  MSC6.00 fonts must be in current directory
*  Args:      <file> <units> <print?> <line> <polyorder>
*				  .GRF file 	argv[1] - full path .GRF file
*					units 		argv[2] - KNOTS or M/S
*					print?		argv[3] - YES/NO
*					order			argv[4] - force low order
*					printer		argv[5] - printer type
*					comport		argv[6] - comms port
*					res			argv[7] - printer mode
*					xm 			argv[8] - x multiplier
*					ym 			argv[9] - y multiplier
*					orient		argv[10] - portrait/landscape
*					ff 			argv[11] - formfeed after print
*					tmar			argv[12] - top margin
*					lmar			argv[13] - left margin
*					pl 			argv[14] - page length
*	Errors:	  Quinn-Curtis error handling is poor; library
*             functions crash without freeing memory. This
*				  program checks available memory & sets poly order
*             to a level it thinks it can cope with. This
*             setting can be overridden by the command line
*				  argument if necessary ( argv[4] );
*	Include:   "realtype.h" - realtype is defined as double as
*				  floating point numbers are too small for the
*				  higher order polynomial fits.
*	Returns:   Nothing ( type void ) but first argument of
*				  spawnlp contains error code:
*				100 = all went correctly.
*               0 = a Quinn-Curtis error/exit (StatsErr 16003).
*           16002 = unable to open data file  (StatsErr 16002).
*           16001 = data in data file corrupt (StatsErr 16001).
*						  (not interpolated within 3 iterations).
*				16004 = not enough memory (StatsErr 16004).
*
*	Problem:	  Quinn-Curtis internal errors retirn 0!
****************************************************************
*	(c) West Country Systems Ltd, Cyprus, 1992.
****************************************************************/

#include <stdio.h>			 /* Microsoft */
#include <math.h>
#include <stdlib.h>
#include <graph.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <malloc.h>
#include <bios.h>

#include "realtype.h"		 /* Quinn-Curits */
#include "worlddr.h"
#include "segraph.h"
#include "curvefit.h"
#include "miscio.h"
# include "screen.h"
				/* function prototypes */
void main( int, char **);	/* main prototype */
void WCS_AutoAxes( realtype, realtype, realtype, realtype );
void PrntCh( char );

				/* global variables */
int port;		/* printer port */

#ifdef DEBUG
	#undef DEBUG
#endif

/***************************************************************
*   MAIN
****************************************************************/
void main( int argc, char **argv ) {
	realtype *indvar, *depvar, *yest;  /* pointers to graphics */
	realtype *resid, *coef, *coefsig;  /* not employed but required */
	realtype rsq, r, see;
	realtype minval1, minval2, maxval1, maxval2, speedclip, headclip;
	register int i, n;
	int speed = 0, head = 0;
	int corrupt = 0, clipped = 0;
	int err, numobs, maxX, maxY;
	char error, buff[30], buff1[71], buff2[71], buff3[71];
	FILE *fi;
#ifdef DEBUG
	printf("\n%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
	argv[1],
	argv[2],
	argv[3],
	argv[4],
	argv[5],
	argv[6],
	argv[7],
	argv[8],
	argv[9],
	argv[10],
	argv[11],
	argv[12],
	argv[13],
	argv[14],
	argv[15] );
	return;
#endif
	speedclip = ( realtype ) atof( argv[15] );   /* speed clipwindow */
	headclip = ( realtype ) atof( argv[16] );    /* heading clipwindow */
	port = atoi( argv[6] );   /* set printer port for PrntCh() */
	if( (fi = fopen( argv[1], "rb" ) ) == NULL)
		spawnlp( P_OVERLAY, "VGR_STS.EXE", "VGR_STS.EXE", "100", "16002" );
	fread( &speed, sizeof( int ), 1, fi );	 /* whether speed ON */
	fread( &head, sizeof( int ), 1, fi );	 /* whether heading ON */
	fread( &numobs, sizeof( int ), 1, fi );   /* number of dataset */
	fread( buff1, sizeof( char ), 70, fi );   /* first string */
	fread( buff2, sizeof( char ), 70, fi );   /* second string */
	buff1[70] = '\0';
	buff2[70] = '\0';
	i = 69;
	while( buff1[i] == '\x20') {
		buff1[i] = '\0';
		--i;
	}
	i = 69;
	while( buff2[i] == '\x20') {
		buff2[i] = '\0';
		--i;
	}
				/* Allocate memory for curvefitting & charting routines */
	indvar = GetMem( numobs );
	if ( BadMem( indvar ) ) {
		i = 16004;
		goto FREEM;
	}
	depvar = GetMem( numobs );
	if ( BadMem( depvar ) ) {
		i = 16004;
		goto FREEM;
	}
	coef = GetMem( numobs );
	if ( BadMem( coef ) ) {
		i = 16004;
		goto FREEM;
	}
	coefsig = GetMem( numobs );
	if ( BadMem( coefsig ) ) {
		i = 16004;
		goto FREEM;
	}
	yest = GetMem( numobs );
	if ( BadMem( yest ) ) {
		i = 16004;
		goto FREEM;
	}
	resid = GetMem( numobs );
	if ( BadMem( resid ) ) {
		i = 16004;
		goto FREEM;
	}
			/* read the SP numbers for the X axis */
	for( i = 0; i < numobs; i++ ) {
		fscanf( fi, "%6s", buff );	/* read SP numbers */
		indvar[i] = ( realtype ) atol( buff );
	}
			/* correct for zero SP numbers by interpolation */
	for( i = 0; i < numobs; i++ ) {
		n = 1;
		while( !indvar[i] ) {
			++corrupt;
			if( i < 3)
				indvar[i] = indvar[i + n];
			else if( i > (numobs - 3) )
				indvar[i] = indvar[i - n];
			else {
				indvar[i] = ( indvar [i - n] + indvar[i + n])/2.0F;
				if( indvar[i] > 0.0F )
					indvar[i] = floor( indvar[i] );
				else
					indvar[i] = ceil( indvar[i] );
			}
			if( n > 3 ) {    /* allow only 3 iterations */
				i = 16001;
				goto FREEM;
			}
			++n;
		}
	}
	if( !speed ) goto HEAD;
				/* read the (ground) speed values for the Y axis */
	for( i = 0; i < numobs; i++ ) {
		fscanf( fi, "%9s", buff );
		depvar[i] = ( realtype ) atof( buff );
	}
			/* correct for zero values by interpolation */
	for( i = 0; i < numobs; i++ ) {
		n = 1;
		while( !depvar[i] ) {
			if( n == 1 ) ++corrupt;
			if( i < 3)
				depvar[i] = depvar[i + n];
			else if( i > (numobs - 3) )
				depvar[i] = depvar[i - n];
			else
				depvar[i] = (depvar [i - n] + depvar[i + n])/2.0F;
			if( n > 3 ) {    /* 3 iterations only */
				i = 16001;
				goto FREEM;
			}
			++n;
		}
	}
		/* run past the clip window */
	for( i = 1; i < numobs; i++ ) {
		if( depvar[i] > ( fabs( depvar[i - 1] ) + speedclip ) ) {
			++clipped;
			if( i == 1 ) depvar[i] = depvar[i - 1];
			else if( i == 2 ) depvar[i] = ( depvar[i - 1]
													  + depvar[i - 2] )/2.0F;
			else depvar[i] = ( depvar[i - 3] + depvar[i - 2]
														+ depvar[i - 1] )/3.0F;
		}
	}
		/* read the (water) speed values for the Y axis */
	for(i=0; i<numobs; i++) {
		fscanf( fi, "%9s", buff ); /* read water speed */
		yest[i] = ( realtype ) atof( buff );
	}
			/* correct for zero values by interpolation */
	for( i = 0; i < numobs; i++ ) {
		n = 1;
		while( !yest[i] ) {
			if( n == 1 ) ++corrupt;
			if( i < 3)
				yest[i] = yest[i + n];
			else if( i > (numobs - 3) )
				yest[i] = yest[i - n];
			else
				yest[i] = (yest [i - n] + yest[i + n])/2.0F;
			if( n > 3 ) {    /* 3 iterations only */
				i = 16001;
				goto FREEM;
			}
			++n;
		}
	}
		/* run past the clip window */
	for( i = 1; i < numobs; i++ ) {
		if( yest[i] > ( fabs( yest[i - 1] ) +  speedclip ) ) {
			++clipped;
			if( i == 1 ) yest[i] = yest[i - 1];
			else if( i == 2 ) yest[i] = ( yest[i - 1]
													  + yest[i - 2] )/2.0F;
			else yest[i] = ( yest[i - 3] + yest[i - 2]
														+ yest[i - 1] )/3.0F;
		}
	}
		/* initialise graphics & define windows */
	InitSEGraphics( -3, NULL, 0 );
	GetMaxCoords( &maxX, &maxY );
	DefGraphWindow( 1, 1, maxX, maxY / 3, 3 );
	DefGraphWindow( 1, ( maxY / 3) - 1, maxX, maxY - 15, 4 );
	DefGraphWindow( 1, maxY - 15, maxX, maxY, 5 );
	SetWin2PlotRatio( 3, 0.15, 0.13, 0.05, 0.15 );
	SetWin2PlotRatio( 4, 0.15, 0.1, 0.05, 0.13 );
		/* do first (comparison speeds) window */
	SetCurrentWindow( 3 );
	ClearWindow();
	SetAxesType( 0, 0 );
	BorderCurrentWindow( 15 );
		/* find the max/min of the two raw speeds */
	FindMinMax( depvar, numobs, &minval1, &maxval1 );
	FindMinMax( yest, numobs, &minval2, &maxval2 );
	if( maxval2 >= maxval1 ) maxval1 = maxval2;
	if( minval2 <= minval1 ) minval1 = minval2;
	WCS_AutoAxes(indvar[0], indvar[numobs-1], minval1, maxval1 );
	LinePlotData( indvar, depvar, numobs, 1, 0 );   /* plot first speed */
	SelectColor( 3 );
	TitleWindow( buff1 );
	settextstyleXX( 0, 0, 8, 8 );
	TitleYAxis( argv[2] , 0 );   /* speed units either KNOTS or M/S */
	settextstyleXX( 0, 0, 10, 10 );
	SelectColor(6);
	LinePlotData( indvar, yest, numobs, 4, 0 );   /* plot second speed */
	setlinestyleXX( 0, 0, 1 );
		/* do second (current) window */
	SetCurrentWindow (4 );
		/* calculate current by subtraction */
	for( i = 0; i < numobs; i++ )
		depvar[i] = depvar[i] - yest[i];
	ClearWindow();
	SetAxesType( 0, 0 );
	BorderCurrentWindow( 15 );
	FindMinMax( depvar, numobs, &minval1, &maxval1 );
	WCS_AutoAxes(indvar[0], indvar[numobs-1], minval1, maxval1 );
	LinePlotData( indvar, depvar, numobs, 1, 0 );   /* plot raw comparison */
	SelectColor( 6 );
	DrawGridY( 10 );
	SelectColor( 3 );
		/* check memory availability for polynomial order fit */
		/* multregs requires mucho mem (approx SPs x order * 6) */
		/* only plot fitted curve if memory available */
	rsq = ( realtype )_memmax();   /* rsq is unused in this application */
	n = 8;   /* max practical polyorder */
	if( (n = atoi( argv[4] ) ) == 0)  /* set to argument unless arg is 0 */
	n = 8;
	while(rsq < (9.0F * (realtype) numobs * (realtype) n ) ) --n;
	if( n >= 2 ) {
		PolyCurveFit( indvar, depvar, numobs, n, coef, yest, resid,
									 &see, coefsig, &rsq, &r, &error );
		LinePlotData( indvar, yest, numobs, 4, 0 );   /* plot fitted curve */
		sprintf( buff3, "POLYORDER: %d", n);
	} else strcpy( buff3, "NO FIT" );
	if( corrupt ) {
		strcat( buff3, "   CORRUPT: " );
		strcat( buff3, itoa( corrupt, buff, 10 ) );
	}
	if( clipped ) {
		strcat( buff3, "   CLIPPED: " );
		strcat( buff3, itoa( clipped, buff, 10 ) );
	}
	SelectColor( 4 );
	settextstyleXX( 2, 0, 1, 1 );
	LabelGraphWindow( 700, 50, buff3, 1, 1 );
	SelectColor( 3 );
	settextstyleXX( 0, 0, 8, 8 );
	TitleWindow( buff2 );
	settextstyleXX( 0, 0, 8, 8 );
	TitleXAxis( "SHOTPOINT" , 0 );
	SelectColor( 14 );
	LabelGraphWindow( 50, 40, "SPEED COMPARISON", 0, 1 );
	SelectColor( 3 );
	TitleYAxis( argv[2], 0 );      /* KNOTS or M/S */
	settextstyleXX( 0, 0, 10, 10 );
	setlinestyleXX( 1, 0, 1 );
		/* window for print or not */
	SetCurrentWindow( 5 );
	ClearWindow();
	SelectColor( 2 );
	LabelGraphWindow( 10, 500, "(P) to Print   Any Other Key Returns To Menu", 0, 1 );
	LabelGraphWindow( 990, 500, "West Country Systems", 2, 1 );
	strcpy( buff,"  " );
	error = (char) getch();   /* use error as a spare character */
	buff[0] = error;
	buff[1] = 0;
	if( ( buff[0] == 'p' ) || ( buff[0] == 'P' ) ) {
		i = ( int ) _bios_printer( _PRINTER_STATUS, port, 0 );
		if( ( i & 0x29 ) || !( i & 0x80 ) || !( i & 0x10 ) ) {
			ClearWindow();
			SelectColor( 4 );
			LabelGraphWindow( 10, 500, "PRINTER NOT READY? Correct & (C)ontinue (Any Other Key Returns)", 0, 1 );
			error = (char) getch();
			buff[0] = error;
			if( ( buff[0] != 'c' ) && ( buff[0] != 'C' ) ) {
				CloseSEGraphics();
				goto HEAD;
			}
		}
		ClearWindow();
			/* set up printer */
		if( ( atoi( argv[5] ) < 2 ) || ( atoi( argv[5] ) > 4 ) ) {	/* EPSON FX/MX/LQ */
			PrntCh('\x1b');  /* set form length in lines */
			PrntCh('C');
			PrntCh( (char) atoi( argv[14] ) );
			PrntCh('\x1b');  /* set left margin in 1/80 */
			PrntCh('\x6c');
			PrntCh( (char) atoi( argv[13] ) );
			PrntCh('\x1b');	/* set 1/6" line spacing */
			PrntCh('\x32');
			for( i = 0; i < atoi( argv[12] ); i++ ) { /* CR/LF top margin */
				PrntCh( '\x0d' );
				PrntCh( '\x0a');
			}
		}
			/* do hi-res screen dump - full screen */
		ScreenDump( atoi( argv[5] ),			/* printer type MX = 5 */
									  port,			/* port LPT1 = 0 */
						atoi( argv[7] ),			/* resolution MX max = 3 */
						atof( argv[8] ),			/* x mult  MX = 3.0 */
						atof( argv[9] ),			/* y mult  MX = 1.0 */
										  0,			/* rv (0 = print colours) */
						atoi( argv[10] ), 		/* orient (portrait = 0) */
						atoi( argv[11] ),			/* FF  (formfeed = 0) */
										  0,        /* full screen window */
						&err);
		SelectColor( 2 );
		LabelGraphWindow( 500, 500, "PRINTING COMPLETE", 1, 1 );
		getch();
	}
	CloseSEGraphics();
					/* PLOT HEADINGS */
	HEAD:
	if( !head ) goto FREEM;
	corrupt = 0;
	clipped = 0;
			/* read the system heading values for the Y axis */
	for( i = 0; i < numobs; i++ ) {
		fscanf( fi, "%9s", buff );   /* read system head */
		depvar[i] = ( realtype ) atof( buff );
	}
	for (i = 0; i < numobs; i++ ) {
		fscanf( fi, "%9s", buff );   /* read gyro head */
		yest[i] = ( realtype ) atof( buff );
	}
		/* initialise graphics & define windows */
	InitSEGraphics( -3, NULL, 0 );
	GetMaxCoords( &maxX, &maxY );
	DefGraphWindow( 1, 1, maxX, maxY / 3, 3 );
	DefGraphWindow( 1, ( maxY / 3) - 1, maxX, maxY - 15, 4 );
	DefGraphWindow( 1, maxY - 15, maxX, maxY, 5 );
	SetWin2PlotRatio( 3, 0.15, 0.13, 0.05, 0.15 );
	SetWin2PlotRatio( 4, 0.15, 0.1, 0.05, 0.13 );
		/* do first (comparison speeds) window */
	SetCurrentWindow( 3 );
	ClearWindow();
	SetAxesType( 0, 0 );
	BorderCurrentWindow( 15 );
		/* find the max/min of the two raw headings */
	FindMinMax( depvar, numobs, &minval1, &maxval1 );
	FindMinMax( yest, numobs, &minval2, &maxval2 );
	if( maxval2 >= maxval1 ) maxval1 = maxval2;
	if( minval2 <= minval1 ) minval1 = minval2;
	WCS_AutoAxes(indvar[0], indvar[numobs-1], minval1, maxval1 );
	LinePlotData( indvar, depvar, numobs, 1, 0 );   /* plot system head */
	SelectColor( 3 );
	TitleWindow( buff1 );
	settextstyleXX( 0, 0, 8, 8 );
	TitleYAxis( "DEGREES" , 0 );
	settextstyleXX( 0, 0, 10, 10 );
	SelectColor(6);
	LinePlotData( indvar, yest, numobs, 4, 0 );   /* plot gyro head */
	setlinestyleXX( 0, 0, 1 );
		/* do second (crab angle) window */
	SetCurrentWindow (4 );
		/* calculate crab angle by subtraction */
	for( i = 0; i < numobs; i++ ) {
		if( depvar[i] > yest[i] ) {
			if( ( depvar[i] - yest[i] ) > 180.0F )
				depvar[i] = 360.0F - depvar[i] + yest[i];
			else
				depvar[i] = depvar[i] - yest[i];
		} else if( yest[i] > depvar[i] ) {
			if( ( yest[i] - depvar[i]  ) > 180.0F )
				depvar[i] = 360.0F + depvar[i] - yest[i];
			else
				depvar[i] = depvar[i] - yest[i];
		} else depvar[i] = 0.0F;
	}
	fclose( fi );
	ClearWindow();
	SetAxesType( 0, 0 );
	BorderCurrentWindow( 15 );
		/* find the max/min of the two raw speeds */
	FindMinMax( depvar, numobs, &minval1, &maxval1 );
	WCS_AutoAxes(indvar[0], indvar[numobs-1], minval1, maxval1 );
	LinePlotData( indvar, depvar, numobs, 1, 0 );   /* plot raw comparison */
	SelectColor( 6 );
	DrawGridY( 10 );
	SelectColor( 3 );
		/* check memory availability for polynomial order fit */
		/* multregs requires mucho mem (approx SPs x order * 6) */
		/* only plot fitted curve if memory available */
	rsq = ( realtype )_memmax();   /* rsq is unused in this application */
	n = 8;   /* max practical polyorder */
	if( (n = atoi( argv[4] ) ) == 0)  /* set to argument unless arg is 0 */
	n = 8;
	while(rsq < (9.0F * (realtype) numobs * (realtype) n ) ) --n;
	if( n >= 2 ) {
		PolyCurveFit( indvar, depvar, numobs, n, coef, yest, resid,
									 &see, coefsig, &rsq, &r, &error );
		LinePlotData( indvar, yest, numobs, 4, 0 );   /* plot fitted curve */
		sprintf( buff3, "POLYORDER: %d", n);
	} else strcpy( buff3, "NO FIT " );
	if( corrupt ) {
		strcat( buff3, "   CORRUPT: " );
		strcat( buff3, itoa( corrupt, buff, 10 ) );
	}
/*****************  NO heading clipping at present
	if( clipped ) {
		strcat( buff3, "   CLIPPED: " );
		strcat( buff3, itoa( clipped, buff, 10 ) );
	}
***************/
	SelectColor( 4 );
	settextstyleXX( 2, 0, 1, 1 );
	LabelGraphWindow( 700, 50, buff3, 1, 1 );
	SelectColor( 3 );
	settextstyleXX( 0, 0, 8, 8 );
	TitleWindow( buff2 );
	settextstyleXX( 0, 0, 8, 8 );
	TitleXAxis( "SHOTPOINT" , 0 );
	SelectColor( 14 );
	LabelGraphWindow( 50, 40, "HEADING COMPARISON", 0, 1 );
	SelectColor( 3 );
	TitleYAxis( "DEGREES", 0 );
	settextstyleXX( 0, 0, 10, 10 );
	setlinestyleXX( 1, 0, 1 );
		/* window for print or not */
	SetCurrentWindow( 5 );
	ClearWindow();
	SelectColor( 2 );
	LabelGraphWindow( 10, 500, "(P) to Print   Any Other Key Returns To Menu", 0, 1 );
	LabelGraphWindow( 990, 500, "West Country Systems", 2, 1 );
	strcpy( buff,"  " );
	error = (char) getch();   /* use error as a spare character */
	buff[0] = error;
	buff[1] = 0;
	if( ( buff[0] == 'p' ) || ( buff[0] == 'P' ) ) {
		i = ( int ) _bios_printer( _PRINTER_STATUS, port, 0 );
		if( ( i & 0x29 ) || !( i & 0x80 ) || !( i & 0x10 ) ) {
			ClearWindow();
			SelectColor( 4 );
			LabelGraphWindow( 10, 500, "PRINTER NOT READY? Correct & (C)ontinue (Any Other Key Returns)", 0, 1 );
			error = (char) getch();
			buff[0] = error;
			if( ( buff[0] != 'c' ) && ( buff[0] != 'C' ) ) {
				CloseSEGraphics();
				goto FREEM;
			}
		}
		ClearWindow();
			/* set up printer */
		if( ( atoi( argv[5] ) < 2 ) || ( atoi( argv[5] ) > 4 ) ) {	/* EPSON FX/MX/LQ */
			PrntCh('\x1b');  /* set form length in lines */
			PrntCh('C');
			PrntCh( (char) atoi( argv[14] ) );
			PrntCh('\x1b');  /* set left margin in 1/80 */
			PrntCh('\x6c');
			PrntCh( (char) atoi( argv[13] ) );
			PrntCh('\x1b');	/* set 1/6" line spacing */
			PrntCh('\x32');
			for( i = 0; i < atoi( argv[12] ); i++ ) { /* CR/LF top margin */
				PrntCh( '\x0d' );
				PrntCh( '\x0a');
			}
		}
			/* do hi-res screen dump - full screen */
		ScreenDump( atoi( argv[5] ),			/* printer type MX = 5 */
									  port,			/* port LPT1 = 0 */
						atoi( argv[7] ),			/* resolution MX max = 3 */
						atof( argv[8] ),			/* x mult  MX = 3.0 */
						atof( argv[9] ),			/* y mult  MX = 1.0 */
										  0,			/* rv (0 = print colours) */
						atoi( argv[10] ), 		/* orient (portrait = 0) */
						atoi( argv[11] ),			/* FF  (formfeed = 0) */
										  0,        /* full screen window */
						&err);
		SelectColor( 2 );
		LabelGraphWindow( 500, 500, "PRINTING COMPLETE", 1, 1 );
		getch();
	}

	CloseSEGraphics();
	FREEM:
	fcloseall();
	if( indvar ) free( indvar );
	if( depvar ) free( depvar );
	if ( yest ) free ( yest );
	if ( coef ) free( coef );
	if ( coefsig ) free( coefsig );
	if ( resid ) free( resid );
	spawnlp( P_OVERLAY, "VGR_STS.EXE", "VGR_STS.EXE",
											"100", itoa( i, buff1, 10 ), NULL );
}


/***************************************************************
*	Q-C AutoAxes function converted to take pre-converted args.
****************************************************************/
void WCS_AutoAxes(realtype xx1, realtype xx2, realtype yy1, realtype yy2 )
{
	realtype ts1, ts2;
	int lsx, lsy;
	RoundAxes( &xx1, &xx2, &ts1 );
	RoundAxes( &yy1, &yy2, &ts2);
	ScalePlotArea( xx1, yy1, xx2, yy2 );
	SetXYIntercepts( xx1, yy1 );
	DrawYAxis( ts2, 0);
	DrawXAxis( ts1, 0);
	lsy = 10;
	lsx = 10;
	LabelYAxis( lsy, 0 );
	LabelXAxis( lsx, 0 );

}


/****************************************************************
*   send a printer character
****************************************************************/
void PrntCh(char ch)
{
	_bios_printer(_PRINTER_WRITE, port, ch);
}
