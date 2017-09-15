
/*
** nbench1.c
*/

/********************************
**       BYTEmark (tm)         **
** BYTE NATIVE MODE BENCHMARKS **
**       VERSION 2             **
**                             **
** Included in this source     **
** file:                       **
**  Numeric Heapsort           **
**  String Heapsort            **
**  Bitfield test              **
**  Floating point emulation   **
**  Fourier coefficients       **
**  Assignment algorithm       **
**  IDEA Encyption             **
**  Huffman compression        **
**  Back prop. neural net      **
**  LU Decomposition           **
**    (linear equations)       **
** ----------                  **
** Rick Grehan, BYTE Magazine  **
*********************************
**
** BYTEmark (tm)
** BYTE's Native Mode Benchmarks
** Rick Grehan, BYTE Magazine
**
** Creation:
** Revision: 3/95;10/95
**  10/95 - Removed allocation that was taking place inside
**   the LU Decomposition benchmark. Though it didn't seem to
**   make a difference on systems we ran it on, it nonetheless
**   removes an operating system dependency that probably should
**   not have been there.
**
** DISCLAIMER
** The source, executable, and documentation files that comprise
** the BYTEmark benchmarks are made available on an "as is" basis.
** This means that we at BYTE Magazine have made every reasonable
** effort to verify that the there are no errors in the source and
** executable code.  We cannot, however, guarantee that the programs
** are error-free.  Consequently, McGraw-HIll and BYTE Magazine make
** no claims in regard to the fitness of the source code, executable
** code, and documentation of the BYTEmark.
**  Furthermore, BYTE Magazine, McGraw-Hill, and all employees
** of McGraw-Hill cannot be held responsible for any damages resulting
** from the use of this code or the results obtained from using
** this code.
*/

/*
** INCLUDES
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include "nmglobal.h"
#include "nbench1.h"
#include "wordcat.h"

/*********************************************
 * Externally defined functions              *
 *********************************************
 * These functions are defined in App.cpp,   *
 * but nbench doesn't know about it; we must *
 * define them at link time and leave them as*
 * extern here                               *
 ********************************************/
/*Memory management*/
extern void app_AllocateMemory(size_t size);
extern void app_AllocateMemory2(size_t size);
extern void app_AllocateMemory3(size_t size);
extern void app_AllocateMemory4(size_t size);
extern void app_AllocateMemory5(size_t size);

extern void app_FreeMemory();
extern void app_FreeMemory2();
extern void app_FreeMemory3();
extern void app_FreeMemory4();
extern void app_FreeMemory5();

//Numsort test
extern void app_LoadNumArrayWithRand(unsigned long arraysize, unsigned int numarrays);
extern void app_NumHeapSort(unsigned long base_offset,unsigned long bottom, unsigned long top);

//StringSort test
extern unsigned long app_LoadStringArray(unsigned int numarrays, unsigned long arraysize);
extern void app_StrHeapSort(unsigned long oparrayOffset, unsigned long strarrayOffset, unsigned long numstrings, unsigned long bottom, unsigned long top);
extern void app_call_StrHeapSort(unsigned long nstrings, unsigned int numarrays, unsigned long arraysize);

//Floating Point test
extern void app_SetupCPUEmFloatArrays(ulong arraysize);

//BitSort
extern unsigned long app_bitSetup(long bitfieldarraysize, long bitoparraysize);
extern void app_ToggleBitRun(unsigned long bit_addr, unsigned long nbits, unsigned int val);
extern void app_FlipBitRun(long bit_addr,long nbits);   

//Fourier
extern void app_DoFPUTransIteration(unsigned long arraysize);

//Assignment
extern void app_LoadAssignArrayWithRand(unsigned long numarrays);
extern void app_call_AssignmentTest(unsigned int numarrays);

/*IDEAsort*/
extern void app_loadIDEA(unsigned long arraysize);
extern void app_callIDEA(unsigned long arraysize, unsigned short* Z, unsigned short* DK, unsigned long nloops);

/*Neural Net*/
extern void app_set_numpats(int npats);
extern double app_get_in_pats(int patt, int element);
extern void app_set_in_pats(int patt, int element, double val);
extern void app_set_out_pats(int patt, int element, double val);
extern void app_DoNNetIteration(unsigned long nloops);

/*LU Decomposition*/
extern void app_build_problem();
extern void app_moveSeedArrays(unsigned long numarrays);
extern void app_call_lusolve(unsigned long numarrays);

/*Huffman Compression*/
extern void app_buildHuffman(unsigned long arraysize);
extern void app_callHuffman(unsigned long nloops, unsigned long arraysize);

/********************************************/
/*********************
** NUMERIC HEAPSORT **
**********************
** This test implements a heapsort algorithm, performed on an
** array of longs.
*/

/**************
** DoNumSort **
***************
** This routine performs the CPU numeric sort test.
** NOTE: Last version incorrectly stated that the routine
**  returned result in # of longword sorted per second.
**  Not so; the routine returns # of iterations per sec.
*/

void DoNumSort(void)
{
SortStruct *numsortstruct;      /* Local pointer to global struct */
farlong *arraybase;     /* Base pointers of array */
long accumtime;         /* Accumulated time */
double iterations;      /* Iteration counter */
char *errorcontext;     /* Error context string pointer */
int systemerror;        /* For holding error codes */
//memory errors handle inside of the enclave
systemerror=0;

/*
** Link to global structure
*/
numsortstruct=&global_numsortstruct;

/*
** Set the error context string.
*/
errorcontext="CPU:Numeric Sort";

/*
** See if we need to do self adjustment code.
*/
if(numsortstruct->adjust==0)
{
	/*
	** Self-adjustment code.  The system begins by sorting 1
	** array.  If it does that in no time, then two arrays
	** are built and sorted.  This process continues until
	** enough arrays are built to handle the tolerance.
	*/
	numsortstruct->numarrays=1;
	while(1)
	{
		/*
		** Allocate space for arrays
		*/
		/*arraybase=(farlong *)*/
		app_AllocateMemory(sizeof(long)*numsortstruct->numarrays*numsortstruct->arraysize);

		/*
		** Do an iteration of the numeric sort.  If the
		** elapsed time is less than or equal to the permitted
		** minimum, then allocate for more arrays and
		** try again.
		*/
		if(DoNumSortIteration(arraybase,
			numsortstruct->arraysize,
			numsortstruct->numarrays)>global_min_ticks)
			break;          /* We're ok...exit */

		app_FreeMemory(/*(farvoid *)arraybase,&systemerror*/);
		if(numsortstruct->numarrays++>NUMNUMARRAYS)
		{       printf("CPU:NSORT -- NUMNUMARRAYS hit.\n");
			ErrorExit();
		}
	}
}
else
{       /*
	** Allocate space for arrays
	*/
	app_AllocateMemory(sizeof(long)*numsortstruct->numarrays*numsortstruct->arraysize);
}
/*
** All's well if we get here.  Repeatedly perform sorts until the
** accumulated elapsed time is greater than # of seconds requested.
*/
accumtime=0L;
iterations=(double)0.0;

do {
	accumtime+=DoNumSortIteration(arraybase,
		numsortstruct->arraysize,
		numsortstruct->numarrays);
	iterations+=(double)1.0;
} while(TicksToSecs(accumtime)<numsortstruct->request_secs);

/*
** Clean up, calculate results, and go home.  Be sure to
** show that we don't have to rerun adjustment code.
*/
app_FreeMemory();

numsortstruct->sortspersec=iterations *
	(double)numsortstruct->numarrays / TicksToFracSecs(accumtime);

if(numsortstruct->adjust==0)
	numsortstruct->adjust=1;
return;
}

/***********************
** DoNumSortIteration **
************************
** This routine executes one iteration of the numeric
** sort benchmark.  It returns the number of ticks
** elapsed for the iteration.
*/
static ulong DoNumSortIteration(farlong *arraybase,
		ulong arraysize,
		uint numarrays)
{
ulong elapsed;          /* Elapsed ticks */
ulong i;
/*
** Load up the array with random numbers
*/
app_LoadNumArrayWithRand(/*arraybase,*/arraysize,numarrays);

/*
** Start the stopwatch
*/
elapsed=StartStopwatch();

/*
** Execute a heap of heapsorts
*/

//This is where the function call would happen to the enclave
for(i=0;i<numarrays;i++)
	app_NumHeapSort(i*arraysize,0L,arraysize-1L);

/*
** Get elapsed time
*/
elapsed=StopStopwatch(elapsed);
return(elapsed);
}

/********************
** STRING HEAPSORT **
********************/

/*****************
** DoStringSort **
******************
** This routine performs the CPU string sort test.
** Arguments:
**      requested_secs = # of seconds to execute test
**      stringspersec = # of strings per second sorted (RETURNED)
*/
void DoStringSort(void)
{

SortStruct *strsortstruct;      /* Local for sort structure */
faruchar *arraybase;            /* Base pointer of char array */
long accumtime;                 /* Accumulated time */
double iterations;              /* # of iterations */
char *errorcontext;             /* Error context string pointer */
int systemerror;                /* For holding error code */
//memory errors handle inside of the enclave
systemerror=0;
/*
** Link to global structure
*/
strsortstruct=&global_strsortstruct;

/*
** Set the error context
*/
errorcontext="CPU:String Sort";

/*
** See if we have to perform self-adjustment code
*/
if(strsortstruct->adjust==0)
{
	/*
	** Initialize the number of arrays.
	*/
	strsortstruct->numarrays=1;
	while(1)
	{
		/*
		** Allocate space for array.  We'll add an extra 100
		** bytes to protect memory as strings move around
		** (this can happen during string adjustment)
		*/

		app_AllocateMemory((strsortstruct->arraysize+100L)*(long)strsortstruct->numarrays);


		/*
		** Do an iteration of the string sort.  If the
		** elapsed time is less than or equal to the permitted
		** minimum, then de-allocate the array, reallocate a
		** an additional array, and try again.
		*/

		if(DoStringSortIteration(arraybase,
			strsortstruct->numarrays,
			strsortstruct->arraysize)>global_min_ticks)
			break;          /* We're ok...exit */

				
		app_FreeMemory();		
		strsortstruct->numarrays+=1;
	}
}
else
{
	/*
	** We don't have to perform self adjustment code.
	** Simply allocate the space for the array.
	*/
	

	app_AllocateMemory((strsortstruct->arraysize+100L)*(long)strsortstruct->numarrays);
}
/*
** All's well if we get here.  Repeatedly perform sorts until the
** accumulated elapsed time is greater than # of seconds requested.
*/


accumtime=0L;
iterations=(double)0.0;

do {
	accumtime+=DoStringSortIteration(arraybase,
				strsortstruct->numarrays,
				strsortstruct->arraysize);
	iterations+=(double)strsortstruct->numarrays;
} while(TicksToSecs(accumtime)<strsortstruct->request_secs);

/*
** Clean up, calculate results, and go home.
** Set flag to show we don't need to rerun adjustment code.
*/
app_FreeMemory();
strsortstruct->sortspersec=iterations / (double)TicksToFracSecs(accumtime);
if(strsortstruct->adjust==0)
	strsortstruct->adjust=1;
return;
}

/**************************
** DoStringSortIteration **
***************************
** This routine executes one iteration of the string
** sort benchmark.  It returns the number of ticks
** Note that this routine also builds the offset pointer
** array.
*/
static ulong DoStringSortIteration(faruchar *arraybase,
		uint numarrays,ulong arraysize)
{
farulong *optrarray;            /* Offset pointer array */
unsigned long elapsed;          /* Elapsed ticks */ 
unsigned long nstrings;         /* # of strings in array */
int syserror;                   /* System error code */
unsigned int i;                 /* Index */
unsigned long tempobase;            /* Temporary offset pointer base */
unsigned long tempsbase;            /* Temporary string base pointer */

/*
** Load up the array(s) with random numbers
*/

nstrings = app_LoadStringArray(numarrays, arraysize);

/*
** Set temp base pointers...they will be modified as the
** benchmark proceeds.
*/




/*Start the stopwatch*/
elapsed=StartStopwatch();

/*Execute heapsorts*/
app_call_StrHeapSort(nstrings, numarrays, arraysize);

/*Record elapsed time*/
elapsed=StopStopwatch(elapsed);


/*
** Release the offset pointer array built by
** LoadStringArray()
*/

app_FreeMemory2();
/*
** Return elapsed ticks.
*/
return(elapsed);
}
/************************
** BITFIELD OPERATIONS **
*************************/

/*************
** DoBitops **
**************
** Perform the bit operations test portion of the CPU
** benchmark.  Returns the iterations per second.
*/
void DoBitops(void)
{
BitOpStruct *locbitopstruct;    /* Local bitop structure */
farulong *bitarraybase;         /* Base of bitmap array */
farulong *bitoparraybase;       /* Base of bitmap operations array */
ulong nbitops;                  /* # of bitfield operations */
ulong accumtime;                /* Accumulated time in ticks */
double iterations;              /* # of iterations */
char *errorcontext;             /* Error context string */
int systemerror;                /* For holding error codes */
//memory errors handle inside of the enclave
systemerror=0;
int ticks;
/*
** Link to global structure.
*/

locbitopstruct=&global_bitopstruct;
/*
** Set the error context.
*/
errorcontext="CPU:Bitfields";

/*
** See if we need to run adjustment code.
*/
if(locbitopstruct->adjust==0)
{

	//Memory allocation inside of the enclave
	app_AllocateMemory(locbitopstruct->bitfieldarraysize *
		sizeof(ulong));

	/*
	** Initialize bitfield operations array to [2,30] elements
	*/
	locbitopstruct->bitoparraysize=30L;
	while(1)
	{
		/*
		** Allocate space for operations array
		*/
		//Second memory allocation inside of the enclave
		app_AllocateMemory2(locbitopstruct->bitoparraysize*2L*
			sizeof(ulong));
		
		/*Will be deleted*/			

		/*
		** Do an iteration of the bitmap test.  If the
		** elapsed time is less than or equal to the permitted
		** minimum, then de-allocate the array, reallocate a
		** larger version, and try again.
		*/
		ticks=DoBitfieldIteration(bitarraybase,
					   bitoparraybase,
					   locbitopstruct->bitoparraysize,
					   &nbitops);
#ifdef DEBUG
#ifdef LINUX
	        if (locbitopstruct->bitoparraysize==30L){
		  /* this is the first loop, write a debug file */
		  FILE *file;
		  unsigned long *running_base; /* same as farulong */
		  long counter;
		  file=fopen("debugbit.dat","w");
		  running_base=bitarraybase;
		  for (counter=0;counter<(long)(locbitopstruct->bitfieldarraysize);counter++){
#ifdef LONG64
		    fprintf(file,"%08X",(unsigned int)(*running_base&0xFFFFFFFFL));
		    fprintf(file,"%08X",(unsigned int)((*running_base>>32)&0xFFFFFFFFL));
		    if ((counter+1)%4==0) fprintf(file,"\n");
#else
		    fprintf(file,"%08lX",*running_base);
		    if ((counter+1)%8==0) fprintf(file,"\n");
#endif
		    running_base=running_base+1;
		  }
		  fclose(file);
		  printf("\nWrote the file debugbit.dat, you may want to compare it to debugbit.good\n");
		}
#endif
#endif

		if (ticks>global_min_ticks) break;      /* We're ok...exit */

		app_FreeMemory2();		
		locbitopstruct->bitoparraysize+=100L;
	}
}
else
{
	/*
	** Don't need to do self adjustment, just allocate
	** the array space.
	*/

	app_AllocateMemory(locbitopstruct->bitfieldarraysize *
		sizeof(ulong));
	
	app_AllocateMemory2(locbitopstruct->bitoparraysize*2L*
		sizeof(ulong));
}

/*
** All's well if we get here.  Repeatedly perform bitops until the
** accumulated elapsed time is greater than # of seconds requested.
*/
accumtime=0L;
iterations=(double)0.0;
do {
	accumtime+=DoBitfieldIteration(bitarraybase,
			bitoparraybase,
			locbitopstruct->bitoparraysize,&nbitops);
	iterations+=(double)nbitops;
} while(TicksToSecs(accumtime)<locbitopstruct->request_secs);

/*
** Clean up, calculate results, and go home.
** Also, set adjustment flag to show that we don't have
** to do self adjusting in the future.
*/
app_FreeMemory();		
app_FreeMemory2();
		
locbitopstruct->bitopspersec=iterations /TicksToFracSecs(accumtime);
if(locbitopstruct->adjust==0)
	locbitopstruct->adjust=1;

return;
}

/************************
** DoBitfieldIteration **
*************************
** Perform a single iteration of the bitfield benchmark.
** Return the # of ticks accumulated by the operation.
*/
static ulong DoBitfieldIteration(farulong *bitarraybase,
		farulong *bitoparraybase,
		long bitoparraysize,
		ulong *nbitops)
{
long i;                         /* Index */
ulong bitoffset;                /* Offset into bitmap */
ulong elapsed;                  /* Time to execute */
/*
** Clear # bitops counter
*/
*nbitops=0L;

/*
** Construct a set of bitmap offsets and run lengths.
** The offset can be any random number from 0 to the
** size of the bitmap (in bits).  The run length can
** be any random number from 1 to the number of bits
** between the offset and the end of the bitmap.
** Note that the bitmap has 8192 * 32 bits in it.
** (262,144 bits)
*/
/*
** Reset random number generator so things repeat.
** Also reset the bit array we work on.
** added by Uwe F. Mayer
*/

//set up function

*nbitops = app_bitSetup(global_bitopstruct.bitfieldarraysize,bitoparraysize);

elapsed=StartStopwatch();

/*
** Loop through array off offset/run length pairs.
** Execute operation based on modulus of index.
*/
for(i=0;i<bitoparraysize;i++)
{
	
	//pass two parameters. offset1 i+i offset2 i+i+1 and 1 or 0 //3 parameters total
	switch(i % 3)
	{

		case 0: /* Set run of bits */
				//printf("b4 case 0\n");
			app_ToggleBitRun(i+i, i+i+1,1);
				//printf("case 0\n");
			break;

		case 1: /* Clear run of bits */
			//printf("b4 case 1\n");
			app_ToggleBitRun(i+i, i+i+1,0);
			//printf("case 1\n");
			
			break;

		case 2: /* Complement run of bits */
			//printf("b4 case 2\n");
			app_FlipBitRun(i+i,i+i+1);
			//printf("case 2\n");
			break;
	}
}

/*
** Return elapsed time
*/
return(StopStopwatch(elapsed));
}
/*****************************
** FLOATING-POINT EMULATION **
*****************************/
/**************
** DoEmFloat **
***************
** Perform the floating-point emulation routines portion of the
** CPU benchmark.  Returns the operations per second.
*/
void DoEmFloat(void)
{
EmFloatStruct *locemfloatstruct;        /* Local structure */
InternalFPF *abase;             /* Base of A array */
InternalFPF *bbase;             /* Base of B array */
InternalFPF *cbase;             /* Base of C array */
ulong accumtime;                /* Accumulated time in ticks */
double iterations;              /* # of iterations */
ulong tickcount;                /* # of ticks */
char *errorcontext;             /* Error context string pointer */
int systemerror;                /* For holding error code */
//memory errors handle inside of the enclave
systemerror=0;
ulong loops;                    /* # of loops */

/*
** Link to global structure
*/
locemfloatstruct=&global_emfloatstruct;

/*
** Set the error context
*/
errorcontext="CPU:Floating Emulation";


/*
** Test the emulation routines.
*/


app_AllocateMemory(locemfloatstruct->arraysize*sizeof(InternalFPF));
app_AllocateMemory2(locemfloatstruct->arraysize*sizeof(InternalFPF));
app_AllocateMemory3(locemfloatstruct->arraysize*sizeof(InternalFPF));

/*
** Set up the arrays
*/

app_SetupCPUEmFloatArrays(locemfloatstruct->arraysize);

/*
** See if we need to do self-adjusting code.
*/
if(locemfloatstruct->adjust==0)
{
	locemfloatstruct->loops=0;

	/*
	** Do an iteration of the tests.  If the elapsed time is
	** less than minimum, increase the loop count and try
	** again.
	*/
	for(loops=1;loops<CPUEMFLOATLOOPMAX;loops+=loops)
	{       tickcount=DoEmFloatIteration(abase,bbase,cbase,
			locemfloatstruct->arraysize,
			loops);
		if(tickcount>global_min_ticks)
		{       locemfloatstruct->loops=loops;
			break;
		}
	}
}

/*
** Verify that selft adjustment code worked.
*/
if(locemfloatstruct->loops==0)
{       printf("CPU:EMFPU -- CMPUEMFLOATLOOPMAX limit hit\n");
	app_FreeMemory();
	app_FreeMemory2();
	app_FreeMemory3();
	ErrorExit();
}

/*
** All's well if we get here.  Repeatedly perform floating
** tests until the accumulated time is greater than the
** # of seconds requested.
** Each iteration performs arraysize * 3 operations.
*/
accumtime=0L;
iterations=(double)0.0;
do {
	accumtime+=DoEmFloatIteration(abase,bbase,cbase,
			locemfloatstruct->arraysize,
			locemfloatstruct->loops);
	iterations+=(double)1.0;
} while(TicksToSecs(accumtime)<locemfloatstruct->request_secs);


/*
** Clean up, calculate results, and go home.
** Also, indicate that adjustment is done.
*/

app_FreeMemory();
app_FreeMemory2();
app_FreeMemory3();

locemfloatstruct->emflops=(iterations*(double)locemfloatstruct->loops)/
		(double)TicksToFracSecs(accumtime);
if(locemfloatstruct->adjust==0)
	locemfloatstruct->adjust=1;
return;
}

/*************************
** FOURIER COEFFICIENTS **
*************************/

/**************
** DoFourier **
***************
** Perform the transcendental/trigonometric portion of the
** benchmark.  This benchmark calculates the first n
** fourier coefficients of the function (x+1)^x defined
** on the interval 0,2.
*/
void DoFourier(void)
{
FourierStruct *locfourierstruct;        /* Local fourier struct */
fardouble *abase;               /* Base of A[] coefficients array */
fardouble *bbase;               /* Base of B[] coefficients array */
unsigned long accumtime;        /* Accumulated time in ticks */
double iterations;              /* # of iterations */
char *errorcontext;             /* Error context string pointer */
int systemerror;                /* For error code */
//memory errors handle inside of the enclave
systemerror=0;
/*
** Link to global structure
*/
locfourierstruct=&global_fourierstruct;

/*
** Set error context string
*/
errorcontext="FPU:Transcendental";

/*
** See if we need to do self-adjustment code.
*/
if(locfourierstruct->adjust==0)
{
	locfourierstruct->arraysize=100L;       /* Start at 100 elements */
	while(1)
	{

		app_AllocateMemory(locfourierstruct->arraysize*sizeof(double));

		app_AllocateMemory2(locfourierstruct->arraysize*sizeof(double));
		/*
		** Do an iteration of the tests.  If the elapsed time is
		** less than or equal to the permitted minimum, re-allocate
		** larger arrays and try again.
		*/
		if(DoFPUTransIteration(abase,bbase,
			locfourierstruct->arraysize)>global_min_ticks)
			break;          /* We're ok...exit */

		/*
		** Make bigger arrays and try again.
		*/
		app_FreeMemory();
		app_FreeMemory2();
		locfourierstruct->arraysize+=50L;
	}
}
else
{       /*
	** Don't need self-adjustment.  Just allocate the
	** arrays, and go.
	*/
	app_AllocateMemory(locfourierstruct->arraysize*sizeof(double));

	app_AllocateMemory2(locfourierstruct->arraysize*sizeof(double));
}
/*
** All's well if we get here.  Repeatedly perform integration
** tests until the accumulated time is greater than the
** # of seconds requested.
*/
accumtime=0L;
iterations=(double)0.0;
do {
	accumtime+=DoFPUTransIteration(abase,bbase,locfourierstruct->arraysize);
	iterations+=(double)locfourierstruct->arraysize*(double)2.0-(double)1.0;
} while(TicksToSecs(accumtime)<locfourierstruct->request_secs);


/*
** Clean up, calculate results, and go home.
** Also set adjustment flag to indicate no adjust code needed.
*/
app_FreeMemory();
app_FreeMemory2();

locfourierstruct->fflops=iterations/(double)TicksToFracSecs(accumtime);

if(locfourierstruct->adjust==0)
	locfourierstruct->adjust=1;

return;
}

/************************
** DoFPUTransIteration **
*************************
** Perform an iteration of the FPU Transcendental/trigonometric
** benchmark.  Here, an iteration consists of calculating the
** first n fourier coefficients of the function (x+1)^x on
** the interval 0,2.  n is given by arraysize.
** NOTE: The # of integration steps is fixed at
** 200.
*/
static ulong DoFPUTransIteration(fardouble *abase,      /* A coeffs. */
			fardouble *bbase,               /* B coeffs. */
			ulong arraysize)                /* # of coeffs */
{
double omega;           /* Fundamental frequency */
unsigned long i;        /* Index */
unsigned long elapsed;  /* Elapsed time */

/*
** Start the stopwatch
*/
elapsed=StartStopwatch();

app_DoFPUTransIteration(arraysize);

/*
** All done, stop the stopwatch
*/
return(StopStopwatch(elapsed));
}
/*************************
** ASSIGNMENT ALGORITHM **
*************************/

/*************
** DoAssign **
**************
** Perform an assignment algorithm.
** The algorithm was adapted from the step by step guide found
** in "Quantitative Decision Making for Business" (Gordon,
**  Pressman, and Cohn; Prentice-Hall)
**
**
** NOTES:
** 1. Even though the algorithm distinguishes between
**    ASSIGNROWS and ASSIGNCOLS, as though the two might
**    be different, it does presume a square matrix.
**    I.E., ASSIGNROWS and ASSIGNCOLS must be the same.
**    This makes for some algorithmically-correct but
**    probably non-optimal constructs.
**
*/
void DoAssign(void)
{
AssignStruct *locassignstruct;  /* Local structure ptr */
farlong *arraybase;
char *errorcontext;
int systemerror;
//memory errors handle inside of the enclave
systemerror=0;
ulong accumtime;
double iterations;

/*
** Link to global structure
*/
locassignstruct=&global_assignstruct;

/*
** Set the error context string.
*/
errorcontext="CPU:Assignment";

/*
** See if we need to do self adjustment code.
*/
if(locassignstruct->adjust==0)
{
	/*
	** Self-adjustment code.  The system begins by working on 1
	** array.  If it does that in no time, then two arrays
	** are built.  This process continues until
	** enough arrays are built to handle the tolerance.
	*/
	locassignstruct->numarrays=1;
	while(1)
	{
		/*
		** Allocate space for arrays
		*/
		app_AllocateMemory(sizeof(long)*
			ASSIGNROWS*ASSIGNCOLS*locassignstruct->numarrays);

		/*
		** Do an iteration of the assignment alg.  If the
		** elapsed time is less than or equal to the permitted
		** minimum, then allocate for more arrays and
		** try again.
		*/
		if(DoAssignIteration(arraybase,
			locassignstruct->numarrays)>global_min_ticks)
			break;          /* We're ok...exit */
				printf("after\n");
		app_FreeMemory();
		locassignstruct->numarrays++;
	}
}
else
{       /*
	** Allocate space for arrays
	*/
	app_AllocateMemory(sizeof(long)*
			ASSIGNROWS*ASSIGNCOLS*locassignstruct->numarrays);

}

/*
** All's well if we get here.  Do the tests.
*/
accumtime=0L;
iterations=(double)0.0;

do {
	accumtime+=DoAssignIteration(arraybase,
		locassignstruct->numarrays);
	iterations+=(double)1.0;
} while(TicksToSecs(accumtime)<locassignstruct->request_secs);

/*
** Clean up, calculate results, and go home.  Be sure to
** show that we don't have to rerun adjustment code.
*/


app_FreeMemory();
locassignstruct->iterspersec=iterations *
	(double)locassignstruct->numarrays / TicksToFracSecs(accumtime);

if(locassignstruct->adjust==0)
	locassignstruct->adjust=1;

return;

}

/**********************
** DoAssignIteration **
***********************
** This routine executes one iteration of the assignment test.
** It returns the number of ticks elapsed in the iteration.
*/
static ulong DoAssignIteration(farlong *arraybase,
	ulong numarrays)
{
longptr abase;                  /* local pointer */
ulong elapsed;          /* Elapsed ticks */
ulong i;

/*
** Set up local pointer
*/
abase.ptrs.p=arraybase;

/*
** Load up the arrays with a random table.
*/

app_LoadAssignArrayWithRand(numarrays);


/*
** Start the stopwatch
*/
elapsed=StartStopwatch();

/*
** Execute assignment algorithms
*/


app_call_AssignmentTest(numarrays);

/*
** Get elapsed time
*/
return(StopStopwatch(elapsed));
}
/********************
** IDEA Encryption **
*********************
** IDEA - International Data Encryption Algorithm.
** Based on code presented in Applied Cryptography by Bruce Schneier.
** Which was based on code developed by Xuejia Lai and James L. Massey.
** Other modifications made by Colin Plumb.
**
*/
/***********
** DoIDEA **
************
** Perform IDEA encryption.  Note that we time encryption & decryption
** time as being a single loop.
*/
void DoIDEA(void)
{
IDEAStruct *locideastruct;      /* Loc pointer to global structure */
int i;
IDEAkey Z,DK;
u16 userkey[8];
ulong accumtime;
double iterations;
char *errorcontext;
int systemerror;
//memory errors handle inside of the enclave
systemerror=0;
faruchar *plain1;               /* First plaintext buffer */
faruchar *crypt1;               /* Encryption buffer */
faruchar *plain2;               /* Second plaintext buffer */

/*
** Link to global data
*/
locideastruct=&global_ideastruct;

/*
** Set error context
*/
errorcontext="CPU:IDEA";

/*
** Re-init random-number generator.
*/
/* randnum(3L); */
randnum((int32)3);

/*
** Build an encryption/decryption key
*/
for (i=0;i<8;i++)
        /* userkey[i]=(u16)(abs_randwc(60000L) & 0xFFFF); */
	userkey[i]=(u16)(abs_randwc((int32)60000) & 0xFFFF);
for(i=0;i<KEYLEN;i++)
	Z[i]=0;

/*
** Compute encryption/decryption subkeys
*/
en_key_idea(userkey,Z);
de_key_idea(Z,DK);

/*
** Allocate memory for buffers.  We'll make 3, called plain1,
** crypt1, and plain2.  It works like this:
**   plain1 >>encrypt>> crypt1 >>decrypt>> plain2.
** So, plain1 and plain2 should match.
** Also, fill up plain1 with sample text.
*/

app_AllocateMemory(locideastruct->arraysize);
app_AllocateMemory2(locideastruct->arraysize);
app_AllocateMemory3(locideastruct->arraysize);

/*
** Note that we build the "plaintext" by simply loading
** the array up with random numbers.
*/


app_loadIDEA(locideastruct->arraysize);


/*
** See if we need to perform self adjustment loop.
*/
if(locideastruct->adjust==0)
{
	/*
	** Do self-adjustment.  This involves initializing the
	** # of loops and increasing the loop count until we
	** get a number of loops that we can use.
	*/
	for(locideastruct->loops=100L;
	  locideastruct->loops<MAXIDEALOOPS;
	  locideastruct->loops+=10L)
		if(DoIDEAIteration(plain1,crypt1,plain2,
		  locideastruct->arraysize,
		  locideastruct->loops,
		  Z,DK)>global_min_ticks) break;
}

/*
** All's well if we get here.  Do the test.
*/
accumtime=0L;
iterations=(double)0.0;

do {
	accumtime+=DoIDEAIteration(plain1,crypt1,plain2,
		locideastruct->arraysize,
		locideastruct->loops,Z,DK);
	iterations+=(double)locideastruct->loops;
} while(TicksToSecs(accumtime)<locideastruct->request_secs);

/*
** Clean up, calculate results, and go home.  Be sure to
** show that we don't have to rerun adjustment code.
*/

app_FreeMemory();
app_FreeMemory2();
app_FreeMemory3();
locideastruct->iterspersec=iterations / TicksToFracSecs(accumtime);

if(locideastruct->adjust==0)
	locideastruct->adjust=1;

return;

}

/********************
** DoIDEAIteration **
*********************
** Execute a single iteration of the IDEA encryption algorithm.
** Actually, a single iteration is one encryption and one
** decryption.
*/
static ulong DoIDEAIteration(faruchar *plain1,
			faruchar *crypt1,
			faruchar *plain2,
			ulong arraysize,
			ulong nloops,
			IDEAkey Z,
			IDEAkey DK)
{
register ulong i;
register ulong j;
ulong elapsed;


/*
** Start the stopwatch.
*/
elapsed=StartStopwatch();

/*
** Do everything for nloops.
*/

app_callIDEA(arraysize, Z, DK, nloops);

return(StopStopwatch(elapsed));
}

/********
** mul **
*********
** Performs multiplication, modulo (2**16)+1.  This code is structured
** on the assumption that untaken branches are cheaper than taken
** branches, and that the compiler doesn't schedule branches.
*/
static u16 mul(register u16 a, register u16 b)
{
register u32 p;
if(a)
{       if(b)
	{       p=(u32)(a*b);
		b=low16(p);
		a=(u16)(p>>16);
		return(b-a+(b<a));
	}
	else
		return(1-a);
}
else
	return(1-b);
}

/********
** inv **
*********
** Compute multiplicative inverse of x, modulo (2**16)+1
** using Euclid's GCD algorithm.  It is unrolled twice
** to avoid swapping the meaning of the registers.  And
** some subtracts are changed to adds.
*/
static u16 inv(u16 x)
{
u16 t0, t1;
u16 q, y;

if(x<=1)
	return(x);      /* 0 and 1 are self-inverse */
t1=0x10001 / x;
y=0x10001 % x;
if(y==1)
	return(low16(1-t1));
t0=1;
do {
	q=x/y;
	x=x%y;
	t0+=q*t1;
	if(x==1) return(t0);
	q=y/x;
	y=y%x;
	t1+=q*t0;
} while(y!=1);
return(low16(1-t1));
}

/****************
** en_key_idea **
*****************
** Compute IDEA encryption subkeys Z
*/
static void en_key_idea(u16 *userkey, u16 *Z)
{
int i,j;

/*
** shifts
*/
for(j=0;j<8;j++)
	Z[j]=*userkey++;
for(i=0;j<KEYLEN;j++)
{       i++;
	Z[i+7]=(Z[i&7]<<9)| (Z[(i+1) & 7] >> 7);
	Z+=i&8;
	i&=7;
}
return;
}

/****************
** de_key_idea **
*****************
** Compute IDEA decryption subkeys DK from encryption
** subkeys Z.
*/
static void de_key_idea(IDEAkey Z, IDEAkey DK)
{
IDEAkey TT;
int j;
u16 t1, t2, t3;
u16 *p;
p=(u16 *)(TT+KEYLEN);

t1=inv(*Z++);
t2=-*Z++;
t3=-*Z++;
*--p=inv(*Z++);
*--p=t3;
*--p=t2;
*--p=t1;

for(j=1;j<ROUNDS;j++)
{       t1=*Z++;
	*--p=*Z++;
	*--p=t1;
	t1=inv(*Z++);
	t2=-*Z++;
	t3=-*Z++;
	*--p=inv(*Z++);
	*--p=t2;
	*--p=t3;
	*--p=t1;
}
t1=*Z++;
*--p=*Z++;
*--p=t1;
t1=inv(*Z++);
t2=-*Z++;
t3=-*Z++;
*--p=inv(*Z++);
*--p=t3;
*--p=t2;
*--p=t1;
/*
** Copy and destroy temp copy
*/
for(j=0,p=TT;j<KEYLEN;j++)
{       *DK++=*p;
	*p++=0;
}

return;
}

/*
** MUL(x,y)
** This #define creates a macro that computes x=x*y modulo 0x10001.
** Requires temps t16 and t32.  Also requires y to be strictly 16
** bits.  Here, I am using the simplest form.  May not be the
** fastest. -- RG
*/
/* #define MUL(x,y) (x=mul(low16(x),y)) */

/****************
** cipher_idea **
*****************
** IDEA encryption/decryption algorithm.
*/
static void cipher_idea(u16 in[4],
		u16 out[4],
		register IDEAkey Z)
{
register u16 x1, x2, x3, x4, t1, t2;
/* register u16 t16;
register u16 t32; */
int r=ROUNDS;

x1=*in++;
x2=*in++;
x3=*in++;
x4=*in;

do {
	MUL(x1,*Z++);
	x2+=*Z++;
	x3+=*Z++;
	MUL(x4,*Z++);

	t2=x1^x3;
	MUL(t2,*Z++);
	t1=t2+(x2^x4);
	MUL(t1,*Z++);
	t2=t1+t2;

	x1^=t1;
	x4^=t2;

	t2^=x2;
	x2=x3^t1;
	x3=t2;
} while(--r);
MUL(x1,*Z++);
*out++=x1;
*out++=x3+*Z++;
*out++=x2+*Z++;
MUL(x4,*Z);
*out=x4;
return;
}

/************************
** HUFFMAN COMPRESSION **
************************/

/**************
** DoHuffman **
***************
** Execute a huffman compression on a block of plaintext.
** Note that (as with IDEA encryption) an iteration of the
** Huffman test includes a compression AND a decompression.
** Also, the compression cycle includes building the
** Huffman tree.
*/
void DoHuffman(void)
{
HuffStruct *lochuffstruct;      /* Loc pointer to global data */
char *errorcontext;
int systemerror;
//memory errors handle inside of the enclave
systemerror=0;
ulong accumtime;
double iterations;
farchar *comparray;
farchar *decomparray;
farchar *plaintext;

/*
** Link to global data
*/
lochuffstruct=&global_huffstruct;

/*
** Set error context.
*/
errorcontext="CPU:Huffman";

/*
** Allocate memory for the plaintext and the compressed text.
** We'll be really pessimistic here, and allocate equal amounts
** for both (though we know...well, we PRESUME) the compressed
** stuff will take less than the plain stuff.
** Also note that we'll build a 3rd buffer to decompress
** into, and we preallocate space for the huffman tree.
** (We presume that the Huffman tree will grow no larger
** than 512 bytes.  This is actually a super-conservative
** estimate...but, who cares?)
*/

app_AllocateMemory(lochuffstruct->arraysize);
app_AllocateMemory2(lochuffstruct->arraysize);
app_AllocateMemory3(lochuffstruct->arraysize);
app_AllocateMemory4(sizeof(huff_node) * 512);

/*
** Build the plaintext buffer.  Since we want this to
** actually be able to compress, we'll use the
** wordcatalog to build the plaintext stuff.
*/
/*
** Reset random number generator so things repeat.
** added by Uwe F. Mayer
*/

app_buildHuffman(lochuffstruct->arraysize);

/*
** See if we need to perform self adjustment loop.
*/


if(lochuffstruct->adjust==0)
{
	/*
	** Do self-adjustment.  This involves initializing the
	** # of loops and increasing the loop count until we
	** get a number of loops that we can use.
	*/
	
	
	//app_selfAdjustment(lochuffstruct->loops, lochuffstruct->arraysize);
for(lochuffstruct->loops=100L;
	  lochuffstruct->loops<MAXHUFFLOOPS;
	  lochuffstruct->loops+=10L)
		if(DoHuffIteration(plaintext,
			comparray,
			decomparray,
		  lochuffstruct->arraysize,
		  lochuffstruct->loops,
		  hufftree)>global_min_ticks) break;
}

/*
** All's well if we get here.  Do the test.
*/
accumtime=0L;
iterations=(double)0.0;

do {
	accumtime+=DoHuffIteration(plaintext,
		comparray,
		decomparray,
		lochuffstruct->arraysize,
		lochuffstruct->loops,
		hufftree);
	iterations+=(double)lochuffstruct->loops;
} while(TicksToSecs(accumtime)<lochuffstruct->request_secs);

/*
** Clean up, calculate results, and go home.  Be sure to
** show that we don't have to rerun adjustment code.
*/
app_FreeMemory();
app_FreeMemory2();
app_FreeMemory3();
app_FreeMemory4();

lochuffstruct->iterspersec=iterations / TicksToFracSecs(accumtime);

if(lochuffstruct->adjust==0)
	lochuffstruct->adjust=1;

}
/********************
** DoHuffIteration **
*********************
** Perform the huffman benchmark.  This routine
**  (a) Builds the huffman tree
**  (b) Compresses the text
**  (c) Decompresses the text and verifies correct decompression
*/
static ulong DoHuffIteration(farchar *plaintext,
	farchar *comparray,
	farchar *decomparray,
	ulong arraysize,
	ulong nloops,
	huff_node *hufftree)
{
int i;                          /* Index */
long j;                         /* Bigger index */
int root;                       /* Pointer to huffman tree root */
float lowfreq1, lowfreq2;       /* Low frequency counters */
int lowidx1, lowidx2;           /* Indexes of low freq. elements */
long bitoffset;                 /* Bit offset into text */
long textoffset;                /* Char offset into text */
long maxbitoffset;              /* Holds limit of bit offset */
long bitstringlen;              /* Length of bitstring */
int c;                          /* Character from plaintext */
char bitstring[30];             /* Holds bitstring */
ulong elapsed;                  /* For stopwatch */
#ifdef DEBUG
int status=0;
#endif


/*
** Start the stopwatch
*/
elapsed=StartStopwatch();


app_callHuffman(nloops, arraysize);
/*
** Do everything for nloops
*/

return(StopStopwatch(elapsed));
}

/********************************
** BACK PROPAGATION NEURAL NET **
*********************************
** This code is a modified version of the code
** that was submitted to BYTE Magazine by
** Maureen Caudill.  It accomanied an article
** that I CANNOT NOW RECALL.
** The author's original heading/comment was
** as follows:
**
**  Backpropagation Network
**  Written by Maureen Caudill
**  in Think C 4.0 on a Macintosh
**
**  (c) Maureen Caudill 1988-1991
**  This network will accept 5x7 input patterns
**  and produce 8 bit output patterns.
**  The source code may be copied or modified without restriction,
**  but no fee may be charged for its use.
**
** ++++++++++++++
** I have modified the code so that it will work
** on systems other than a Macintosh -- RG
*/

/***********
** DoNNet **
************
** Perform the neural net benchmark.
** Note that this benchmark is one of the few that
** requires an input file.  That file is "NNET.DAT" and
** should be on the local directory (from which the
** benchmark program in launched).
*/
void DoNNET(void)
{
NNetStruct *locnnetstruct;      /* Local ptr to global data */
char *errorcontext;
ulong accumtime;
double iterations;

/*
** Link to global data
*/
locnnetstruct=&global_nnetstruct;

/*
** Set error context
*/
errorcontext="CPU:NNET";

/*
** Init random number generator.
** NOTE: It is important that the random number generator
**  be re-initialized for every pass through this test.
**  The NNET algorithm uses the random number generator
**  to initialize the net.  Results are sensitive to
**  the initial neural net state.
*/
/* randnum(3L); */
randnum((int32)3);

/*
** Read in the input and output patterns.  We'll do this
** only once here at the beginning.  These values don't
** change once loaded.
*/
if(read_data_file()!=0)
   ErrorExit();


/*
** See if we need to perform self adjustment loop.
*/
if(locnnetstruct->adjust==0)
{
	/*
	** Do self-adjustment.  This involves initializing the
	** # of loops and increasing the loop count until we
	** get a number of loops that we can use.
	*/
	for(locnnetstruct->loops=1L;
	  locnnetstruct->loops<MAXNNETLOOPS;
	  locnnetstruct->loops++)
	  {     /*randnum(3L); */
		randnum((int32)3);
		if(DoNNetIteration(locnnetstruct->loops)
			>global_min_ticks) break;
	  }
}

/*
** All's well if we get here.  Do the test.
*/
accumtime=0L;
iterations=(double)0.0;

do {
	/* randnum(3L); */    /* Gotta do this for Neural Net */
	randnum((int32)3);    /* Gotta do this for Neural Net */
	accumtime+=DoNNetIteration(locnnetstruct->loops);
	iterations+=(double)locnnetstruct->loops;
} while(TicksToSecs(accumtime)<locnnetstruct->request_secs);

/*
** Clean up, calculate results, and go home.  Be sure to
** show that we don't have to rerun adjustment code.
*/
locnnetstruct->iterspersec=iterations / TicksToFracSecs(accumtime);

if(locnnetstruct->adjust==0)
	locnnetstruct->adjust=1;


return;
}

/********************
** DoNNetIteration **
*********************
** Do a single iteration of the neural net benchmark.
** By iteration, we mean a "learning" pass.
*/
static ulong DoNNetIteration(ulong nloops)
{
ulong elapsed;          /* Elapsed time */
int patt;

/*
** Run nloops learning cycles.  Notice that, counted with
** the learning cycle is the weight randomization and
** zeroing of changes.  This should reduce clock jitter,
** since we don't have to stop and start the clock for
** each iteration.
*/
elapsed=StartStopwatch();
app_DoNNetIteration(nloops);
return(StopStopwatch(elapsed));
}

/*************************
** do_mid_forward(patt) **
**************************
** Process the middle layer's forward pass
** The activation of middle layer's neurode is the weighted
** sum of the inputs from the input pattern, with sigmoid
** function applied to the inputs.
**/
static void  do_mid_forward(int patt)
{
double  sum;
int     neurode, i;

for (neurode=0;neurode<MID_SIZE; neurode++)
{
	sum = 0.0;
	for (i=0; i<IN_SIZE; i++)
	{       /* compute weighted sum of input signals */
		sum += mid_wts[neurode][i]*in_pats[patt][i];
	}
	/*
	** apply sigmoid function f(x) = 1/(1+exp(-x)) to weighted sum
	*/
	sum = 1.0/(1.0+exp(-sum));
	mid_out[neurode] = sum;
}
return;
}

/*********************
** do_out_forward() **
**********************
** process the forward pass through the output layer
** The activation of the output layer is the weighted sum of
** the inputs (outputs from middle layer), modified by the
** sigmoid function.
**/
static void  do_out_forward()
{
double sum;
int neurode, i;

for (neurode=0; neurode<OUT_SIZE; neurode++)
{
	sum = 0.0;
	for (i=0; i<MID_SIZE; i++)
	{       /*
		** compute weighted sum of input signals
		** from middle layer
		*/
		sum += out_wts[neurode][i]*mid_out[i];
	}
	/*
	** Apply f(x) = 1/(1+exp(-x)) to weighted input
	*/
	sum = 1.0/(1.0+exp(-sum));
	out_out[neurode] = sum;
}
return;
}

/*************************
** display_output(patt) **
**************************
** Display the actual output vs. the desired output of the
** network.
** Once the training is complete, and the "learned" flag set
** to TRUE, then display_output sends its output to both
** the screen and to a text output file.
**
** NOTE: This routine has been disabled in the benchmark
** version. -- RG
**/
/*
void  display_output(int patt)
{
int             i;

	fprintf(outfile,"\n Iteration # %d",iteration_count);
	fprintf(outfile,"\n Desired Output:  ");

	for (i=0; i<OUT_SIZE; i++)
	{
		fprintf(outfile,"%6.3f  ",out_pats[patt][i]);
	}
	fprintf(outfile,"\n Actual Output:   ");

	for (i=0; i<OUT_SIZE; i++)
	{
		fprintf(outfile,"%6.3f  ",out_out[i]);
	}
	fprintf(outfile,"\n");
	return;
}
*/

/**********************
** do_forward_pass() **
***********************
** control function for the forward pass through the network
** NOTE: I have disabled the call to display_output() in
**  the benchmark version -- RG.
**/
static void  do_forward_pass(int patt)
{
do_mid_forward(patt);   /* process forward pass, middle layer */
do_out_forward();       /* process forward pass, output layer */
/* display_output(patt);        ** display results of forward pass */
return;
}

/***********************
** do_out_error(patt) **
************************
** Compute the error for the output layer neurodes.
** This is simply Desired - Actual.
**/
static void do_out_error(int patt)
{
int neurode;
double error,tot_error, sum;

tot_error = 0.0;
sum = 0.0;
for (neurode=0; neurode<OUT_SIZE; neurode++)
{
	out_error[neurode] = out_pats[patt][neurode] - out_out[neurode];
	/*
	** while we're here, also compute magnitude
	** of total error and worst error in this pass.
	** We use these to decide if we are done yet.
	*/
	error = out_error[neurode];
	if (error <0.0)
	{
		sum += -error;
		if (-error > tot_error)
			tot_error = -error; /* worst error this pattern */
	}
	else
	{
		sum += error;
		if (error > tot_error)
			tot_error = error; /* worst error this pattern */
	}
}
avg_out_error[patt] = sum/OUT_SIZE;
tot_out_error[patt] = tot_error;
return;
}

/***********************
** worst_pass_error() **
************************
** Find the worst and average error in the pass and save it
**/
static void  worst_pass_error()
{
double error,sum;

int i;

error = 0.0;
sum = 0.0;
for (i=0; i<numpats; i++)
{
	if (tot_out_error[i] > error) error = tot_out_error[i];
	sum += avg_out_error[i];
}
worst_error = error;
average_error = sum/numpats;
return;
}

/*******************
** do_mid_error() **
********************
** Compute the error for the middle layer neurodes
** This is based on the output errors computed above.
** Note that the derivative of the sigmoid f(x) is
**        f'(x) = f(x)(1 - f(x))
** Recall that f(x) is merely the output of the middle
** layer neurode on the forward pass.
**/
static void do_mid_error()
{
double sum;
int neurode, i;

for (neurode=0; neurode<MID_SIZE; neurode++)
{
	sum = 0.0;
	for (i=0; i<OUT_SIZE; i++)
		sum += out_wts[i][neurode]*out_error[i];

	/*
	** apply the derivative of the sigmoid here
	** Because of the choice of sigmoid f(I), the derivative
	** of the sigmoid is f'(I) = f(I)(1 - f(I))
	*/
	mid_error[neurode] = mid_out[neurode]*(1-mid_out[neurode])*sum;
}
return;
}

/*********************
** adjust_out_wts() **
**********************
** Adjust the weights of the output layer.  The error for
** the output layer has been previously propagated back to
** the middle layer.
** Use the Delta Rule with momentum term to adjust the weights.
**/
static void adjust_out_wts()
{
int weight, neurode;
double learn,delta,alph;

learn = BETA;
alph  = ALPHA;
for (neurode=0; neurode<OUT_SIZE; neurode++)
{
	for (weight=0; weight<MID_SIZE; weight++)
	{
		/* standard delta rule */
		delta = learn * out_error[neurode] * mid_out[weight];

		/* now the momentum term */
		delta += alph * out_wt_change[neurode][weight];
		out_wts[neurode][weight] += delta;

		/* keep track of this pass's cum wt changes for next pass's momentum */
		out_wt_cum_change[neurode][weight] += delta;
	}
}
return;
}

/*************************
** adjust_mid_wts(patt) **
**************************
** Adjust the middle layer weights using the previously computed
** errors.
** We use the Generalized Delta Rule with momentum term
**/
static void adjust_mid_wts(int patt)
{
int weight, neurode;
double learn,alph,delta;

learn = BETA;
alph  = ALPHA;
for (neurode=0; neurode<MID_SIZE; neurode++)
{
	for (weight=0; weight<IN_SIZE; weight++)
	{
		/* first the basic delta rule */
		delta = learn * mid_error[neurode] * in_pats[patt][weight];

		/* with the momentum term */
		delta += alph * mid_wt_change[neurode][weight];
		mid_wts[neurode][weight] += delta;

		/* keep track of this pass's cum wt changes for next pass's momentum */
		mid_wt_cum_change[neurode][weight] += delta;
	}
}
return;
}

/*******************
** do_back_pass() **
********************
** Process the backward propagation of error through network.
**/
void  do_back_pass(int patt)
{

do_out_error(patt);
do_mid_error();
adjust_out_wts();
adjust_mid_wts(patt);

return;
}


/**********************
** move_wt_changes() **
***********************
** Move the weight changes accumulated last pass into the wt-change
** array for use by the momentum term in this pass. Also zero out
** the accumulating arrays after the move.
**/
static void move_wt_changes()
{
int i,j;

for (i = 0; i<MID_SIZE; i++)
	for (j = 0; j<IN_SIZE; j++)
	{
		mid_wt_change[i][j] = mid_wt_cum_change[i][j];
		/*
		** Zero it out for next pass accumulation.
		*/
		mid_wt_cum_change[i][j] = 0.0;
	}

for (i = 0; i<OUT_SIZE; i++)
	for (j=0; j<MID_SIZE; j++)
	{
		out_wt_change[i][j] = out_wt_cum_change[i][j];
		out_wt_cum_change[i][j] = 0.0;
	}

return;
}

/**********************
** check_out_error() **
***********************
** Check to see if the error in the output layer is below
** MARGIN*OUT_SIZE for all output patterns.  If so, then
** assume the network has learned acceptably well.  This
** is simply an arbitrary measure of how well the network
** has learned -- many other standards are possible.
**/
static int check_out_error()
{
int result,i,error;

result  = T;
error   = F;
worst_pass_error();     /* identify the worst error in this pass */

/*
#ifdef DEBUG
printf("\n Iteration # %d",iteration_count);
#endif
*/
for (i=0; i<numpats; i++)
{
/*      printf("\n Error pattern %d:   Worst: %8.3f; Average: %8.3f",
	  i+1,tot_out_error[i], avg_out_error[i]);
	fprintf(outfile,
	 "\n Error pattern %d:   Worst: %8.3f; Average: %8.3f",
	 i+1,tot_out_error[i]);
*/

	if (worst_error >= STOP) result = F;
	if (tot_out_error[i] >= 16.0) error = T;
}

if (error == T) result = ERR;


#ifdef DEBUG
/* printf("\n Error this pass thru data:   Worst: %8.3f; Average: %8.3f",
 worst_error,average_error);
*/
/* fprintf(outfile,
 "\n Error this pass thru data:   Worst: %8.3f; Average: %8.3f",
  worst_error, average_error); */
#endif

return(result);
}


/*******************
** zero_changes() **
********************
** Zero out all the wt change arrays
**/
static void zero_changes()
{
int i,j;

for (i = 0; i<MID_SIZE; i++)
{
	for (j=0; j<IN_SIZE; j++)
	{
		mid_wt_change[i][j] = 0.0;
		mid_wt_cum_change[i][j] = 0.0;
	}
}

for (i = 0; i< OUT_SIZE; i++)
{
	for (j=0; j<MID_SIZE; j++)
	{
		out_wt_change[i][j] = 0.0;
		out_wt_cum_change[i][j] = 0.0;
	}
}
return;
}


/********************
** randomize_wts() **
*********************
** Intialize the weights in the middle and output layers to
** random values between -0.25..+0.25
** Function rand() returns a value between 0 and 32767.
**
** NOTE: Had to make alterations to how the random numbers were
** created.  -- RG.
**/
static void randomize_wts()
{
int neurode,i;
double value;

/*
** Following not used int benchmark version -- RG
**
**        printf("\n Please enter a random number seed (1..32767):  ");
**        scanf("%d", &i);
**        srand(i);
*/

for (neurode = 0; neurode<MID_SIZE; neurode++)
{
	for(i=0; i<IN_SIZE; i++)
	{
	        /* value=(double)abs_randwc(100000L); */
		value=(double)abs_randwc((int32)100000);
		value=value/(double)100000.0 - (double) 0.5;
		mid_wts[neurode][i] = value/2;
	}
}
for (neurode=0; neurode<OUT_SIZE; neurode++)
{
	for(i=0; i<MID_SIZE; i++)
	{
	        /* value=(double)abs_randwc(100000L); */
		value=(double)abs_randwc((int32)100000);
		value=value/(double)10000.0 - (double) 0.5;
		out_wts[neurode][i] = value/2;
	}
}

return;
}


/*********************
** read_data_file() **
**********************
** Read in the input data file and store the patterns in
** in_pats and out_pats.
** The format for the data file is as follows:
**
** line#   data expected
** -----   ------------------------------
** 1               In-X-size,in-y-size,out-size
** 2               number of patterns in file
** 3               1st X row of 1st input pattern
** 4..             following rows of 1st input pattern pattern
**                 in-x+2  y-out pattern
**                                 1st X row of 2nd pattern
**                 etc.
**
** Each row of data is separated by commas or spaces.
** The data is expected to be ascii text corresponding to
** either a +1 or a 0.
**
** Sample input for a 1-pattern file (The comments to the
** right may NOT be in the file unless more sophisticated
** parsing of the input is done.):
**
** 5,7,8                      input is 5x7 grid, output is 8 bits
** 1                          one pattern in file
** 0,1,1,1,0                  beginning of pattern for "O"
** 1,0,0,0,1
** 1,0,0,0,1
** 1,0,0,0,1
** 1,0,0,0,1
** 1,0,0,0,0
** 0,1,1,1,0
** 0,1,0,0,1,1,1,1            ASCII code for "O" -- 0100 1111
**
** Clearly, this simple scheme can be expanded or enhanced
** any way you like.
**
** Returns -1 if any file error occurred, otherwise 0.
**/
static int read_data_file()
{
FILE *infile;

int xinsize,yinsize,youtsize;
int patt, element, i, row;
int vals_read;
int val1,val2,val3,val4,val5,val6,val7,val8;

/* printf("\n Opening and retrieving data from file."); */

infile = fopen("nbenchPortal/NNET.DAT", "r");
//infile = fopen(inpath, "r");
if (infile == NULL)
{
	printf("\n CPU:NNET--error in opening file!");
	return -1 ;
}

vals_read =fscanf(infile,"%d  %d  %d",&xinsize,&yinsize,&youtsize);
if (vals_read != 3)
{
	printf("\n CPU:NNET -- Should read 3 items in line one; did read %d",vals_read);
	return -1;
}
vals_read=fscanf(infile,"%d",&numpats);
if (vals_read !=1)
{
	printf("\n CPU:NNET -- Should read 1 item in line 2; did read %d",vals_read);
	return -1;
}
if (numpats > MAXPATS)
	numpats = MAXPATS;

app_set_numpats(numpats);

for (patt=0; patt<numpats; patt++)
{
	element = 0;
	for (row = 0; row<yinsize; row++)
	{
		vals_read = fscanf(infile,"%d  %d  %d  %d  %d",
			&val1, &val2, &val3, &val4, &val5);
		if (vals_read != 5)
		{
			printf ("\n CPU:NNET -- failure in reading input!");
			return -1;
		}
		element=row*xinsize;

		app_set_in_pats(patt,element,(double)val1); element++;
		app_set_in_pats(patt,element,(double)val2); element++;
		app_set_in_pats(patt,element,(double)val3); element++;
		app_set_in_pats(patt,element,(double)val4); element++;
		app_set_in_pats(patt,element,(double)val5); element++;
	}
	for (i=0;i<IN_SIZE; i++)
	{
		if (app_get_in_pats(patt,i) >= 0.9)
			app_set_in_pats(patt,i,0.9);
		if (app_get_in_pats(patt,i) <= 0.1)
			app_set_in_pats(patt,i,0.1);
	}
	element = 0;
	vals_read = fscanf(infile,"%d  %d  %d  %d  %d  %d  %d  %d",
		&val1, &val2, &val3, &val4, &val5, &val6, &val7, &val8);

	app_set_out_pats(patt,element,(double)val1); element++;
	app_set_out_pats(patt,element,(double)val2); element++;
	app_set_out_pats(patt,element,(double)val3); element++;
	app_set_out_pats(patt,element,(double)val4); element++;
	app_set_out_pats(patt,element,(double)val5); element++;
	app_set_out_pats(patt,element,(double)val6); element++;
	app_set_out_pats(patt,element,(double)val7); element++;
	app_set_out_pats(patt,element,(double)val8); element++;
}

/* printf("\n Closing the input file now. "); */

fclose(infile);
return(0);
}

/*********************
** initialize_net() **
**********************
** Do all the initialization stuff before beginning
*/
/*
static int initialize_net()
{
int err_code;

randomize_wts();
zero_changes();
err_code = read_data_file();
iteration_count = 1;
return(err_code);
}
*/

/**********************
** display_mid_wts() **
***********************
** Display the weights on the middle layer neurodes
** NOTE: This routine is not used in the benchmark
**  test -- RG
**/
/* static void display_mid_wts()
{
int             neurode, weight, row, col;

fprintf(outfile,"\n Weights of Middle Layer neurodes:");

for (neurode=0; neurode<MID_SIZE; neurode++)
{
	fprintf(outfile,"\n  Mid Neurode # %d",neurode);
	for (row=0; row<IN_Y_SIZE; row++)
	{
		fprintf(outfile,"\n ");
		for (col=0; col<IN_X_SIZE; col++)
		{
			weight = IN_X_SIZE * row + col;
			fprintf(outfile," %8.3f ", mid_wts[neurode][weight]);
		}
	}
}
return;
}
*/
/**********************
** display_out_wts() **
***********************
** Display the weights on the output layer neurodes
** NOTE: This code is not used in the benchmark
**  test -- RG
*/
/* void  display_out_wts()
{
int             neurode, weight;

	fprintf(outfile,"\n Weights of Output Layer neurodes:");

	for (neurode=0; neurode<OUT_SIZE; neurode++)
	{
		fprintf(outfile,"\n  Out Neurode # %d \n",neurode);
		for (weight=0; weight<MID_SIZE; weight++)
		{
			fprintf(outfile," %8.3f ", out_wts[neurode][weight]);
		}
	}
	return;
}
*/

/***********************
**  LU DECOMPOSITION  **
** (Linear Equations) **
************************
** These routines come from "Numerical Recipes in Pascal".
** Note that, as in the assignment algorithm, though we
** separately define LUARRAYROWS and LUARRAYCOLS, the two
** must be the same value (this routine depends on a square
** matrix).
*/

/*********
** DoLU **
**********
** Perform the LU decomposition benchmark.
*/
void DoLU(void)
{
LUStruct *loclustruct;  /* Local pointer to global data */
char *errorcontext;
int systemerror;
//memory errors handle inside of the enclave
systemerror=0;
fardouble *a;
fardouble *b;
fardouble *abase;
fardouble *bbase;
LUdblptr ptra;
int n;
int i;
ulong accumtime;
double iterations;

/*
** Link to global data
*/
loclustruct=&global_lustruct;

/*
** Set error context.
*/
errorcontext="FPU:LU";


/*
** Our first step is to build a "solvable" problem.  This
** will become the "seed" set that all others will be
** derived from. (I.E., we'll simply copy these arrays
** into the others.
*/
app_AllocateMemory(sizeof(double) * LUARRAYCOLS * LUARRAYROWS);


app_AllocateMemory2(sizeof(double) * LUARRAYROWS);

n=LUARRAYROWS;

/*
** We need to allocate a temp vector that is used by the LU
** algorithm.  This removes the allocation routine from the
** timing.
*/
app_AllocateMemory3(sizeof(double)*LUARRAYROWS);


/*
** Build a problem to be solved.
*/

ptra.ptrs.p=a;                  /* Gotta coerce linear array to 2D array */

app_build_problem();


//build_problem(*ptra.ptrs.ap,n,b);

/*
** Now that we have a problem built, see if we need to do
** auto-adjust.  If so, repeatedly call the DoLUIteration routine,
** increasing the number of solutions per iteration as you go.
*/
if(loclustruct->adjust==0)
{
	loclustruct->numarrays=0;
	for(i=1;i<=MAXLUARRAYS;i++)
	{

		app_AllocateMemory4(sizeof(double) * LUARRAYCOLS*LUARRAYROWS*(i+1));


		app_AllocateMemory5(sizeof(double) * LUARRAYROWS*(i+1));	


		
		if(DoLUIteration(a,b,abase,bbase,i)>global_min_ticks)
		{       loclustruct->numarrays=i;
			break;
		}
		/*
		** Not enough arrays...free them all and try again
		*/

		app_FreeMemory4();
		app_FreeMemory5();

	}

	/*
	** Were we able to do it?
	*/
	if(loclustruct->numarrays==0)
	{       printf("FPU:LU -- Array limit reached\n");
		//LUFreeMem(a,b,abase,bbase);
		ErrorExit();
	}
}
else
{       /*
	** Don't need to adjust -- just allocate the proper
	** number of arrays and proceed.
	*/
	app_AllocateMemory4(sizeof(double) * LUARRAYCOLS*LUARRAYROWS*loclustruct->numarrays);
	app_AllocateMemory5(sizeof(double) * LUARRAYROWS*loclustruct->numarrays);

}
/*
** All's well if we get here.  Do the test.
*/
accumtime=0L;
iterations=(double)0.0;



do {
	accumtime+=DoLUIteration(a,b,abase,bbase,
		loclustruct->numarrays);
	iterations+=(double)loclustruct->numarrays;
} while(TicksToSecs(accumtime)<loclustruct->request_secs);

/*
** Clean up, calculate results, and go home.  Be sure to
** show that we don't have to rerun adjustment code.
*/
loclustruct->iterspersec=iterations / TicksToFracSecs(accumtime);

if(loclustruct->adjust==0)
	loclustruct->adjust=1;


app_FreeMemory();
app_FreeMemory2();
app_FreeMemory3();
app_FreeMemory4();
app_FreeMemory5();
return;
}



/******************
** DoLUIteration **
*******************
** Perform an iteration of the LU decomposition benchmark.
** An iteration refers to the repeated solution of several
** identical matrices.
*/
static ulong DoLUIteration(fardouble *a,fardouble *b,
		fardouble *abase, fardouble *bbase,
		ulong numarrays)
{
fardouble *locabase;
fardouble *locbbase;
LUdblptr ptra;  /* For converting ptr to 2D array */
ulong elapsed;
ulong j,i;              /* Indexes */


/*
** Move the seed arrays (a & b) into the destination
** arrays;
*/
app_moveSeedArrays(numarrays);

/*
** Do test...begin timing.
*/
elapsed=StartStopwatch();

app_call_lusolve(numarrays);

return(StopStopwatch(elapsed));
}
