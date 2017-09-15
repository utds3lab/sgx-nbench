/**
*   Copyright(C) 2011-2015 Intel Corporation All Rights Reserved.
*
*   The source code, information  and  material ("Material") contained herein is
*   owned  by Intel Corporation or its suppliers or licensors, and title to such
*   Material remains  with Intel Corporation  or its suppliers or licensors. The
*   Material  contains proprietary information  of  Intel or  its  suppliers and
*   licensors. The  Material is protected by worldwide copyright laws and treaty
*   provisions. No  part  of  the  Material  may  be  used,  copied, reproduced,
*   modified, published, uploaded, posted, transmitted, distributed or disclosed
*   in any way  without Intel's  prior  express written  permission. No  license
*   under  any patent, copyright  or  other intellectual property rights  in the
*   Material  is  granted  to  or  conferred  upon  you,  either  expressly,  by
*   implication, inducement,  estoppel or  otherwise.  Any  license  under  such
*   intellectual  property  rights must  be express  and  approved  by  Intel in
*   writing.
*
*   *Third Party trademarks are the property of their respective owners.
*
*   Unless otherwise  agreed  by Intel  in writing, you may not remove  or alter
*   this  notice or  any other notice embedded  in Materials by Intel or Intel's
*   suppliers or licensors in any way.
*/

#ifndef _ENCLAVE_H_
#define _ENCLAVE_H_

#include <stdlib.h>
#include <assert.h>

#if defined(__cplusplus)
extern "C" {
#endif

void printf(const char *fmt, ...);
void test_function(const char *str);
void nothing();
/*Setup*/
void encl_AllocateMemory(size_t size);
void encl_AllocateMemory2(size_t size);
void encl_AllocateMemory3(size_t size);
void encl_FreeMemory();
void encl_FreeMemory2();
void encl_FreeMemory3();

/*NumSort*/
void encl_LoadNumArrayWithRand(unsigned long arraysize,unsigned int numarrays);
void encl_NumHeapSort( unsigned long base_offset,unsigned long bottom,unsigned long top);

/*StringSort*/
void encl_StrHeapSort(unsigned long *optrarray, unsigned char *strarray, unsigned long numstrings, unsigned long bottom, unsigned long top);
unsigned long encl_LoadStringArray(unsigned int numarrays,unsigned long arraysize);
void encl_call_StrHeapSort(unsigned long nstrings, unsigned int numarrays, unsigned long arraysize);

/*Floating Point*/
void encl_SetupCPUEmFloatArrays(unsigned long arraysize);
void encl_DoEmFloatIteration(unsigned long arraysize, unsigned long loops);

/*Fourier*/
void encl_DoFPUTransIteration(unsigned long arraysize); 

/*Assignment*/
void encl_LoadAssignArrayWithRand(unsigned long numarrays);
void encl_call_AssignmentTest(unsigned int numarrays);

/*Neural Net*/
void encl_set_numpats(int npats);
double encl_get_in_pats(int patt, int element);
void encl_set_in_pats(int patt, int element, double val);
void encl_set_out_pats(int patt, int element, double val);
void encl_DoNNetIteration(unsigned long nloops);

/*LU Decomposition*/
void encl_moveSeedArrays(unsigned long numarrays);
void encl_build_problem();

/*Huffman*/
void encl_buildHuffman(unsigned long arraysize);
void encl_callHuffman(unsigned long nloops, unsigned long arraysize);

#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */
