/***************************************************************
*	 STS_VGR.C	 Spawn communications file for HeadStat V2.40++
****************************************************************
*
*	Version:   1.10
*	Compiler:  MSC V6.00  /AS /G2
*	Linker:	  MSC V5.10
*	Libraries: MSC V6.00 SLIBCE.LIB
*  DOS 3.3++  CoProcessor emulated if not available
*  80286++    Instruction set only
****************************************************************
*	(c) West Country Systems Ltd, Cyprus, 1992.
****************************************************************/

#include <stdio.h>         /* Microsoft */
#include <process.h>

void main( int, char ** );

/****************************************************************
*	Spawn VGR.EXE - graphics add-on for Stats
****************************************************************/
void main( int argc, char **argv )
{
	spawnlp( P_OVERLAY, "VGR.EXE", "VGR.EXE", argv[1],
				argv[2], argv[3], argv[4], argv[5], argv[6],
				argv[7], argv[8], argv[9], argv[10], argv[11],
				argv[12], argv[13], argv[14], argv[15], argv[16], NULL );
	return;
}
