#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pwd.h>
#define MAX_PATH FILENAME_MAX
#include "sgx_urts.h"
// #include "sgx_status.h"
#include "App.h"
#include "Enclave_u.h"


extern "C"{
void mainn(int argc, char *argv[]);
};


/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Initialize the enclave:
 *   Step 1: retrive the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
int initialize_enclave(void)
{
    char token_path[MAX_PATH] = {'\0'};
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    
    /* Step 1: retrive the launch token saved by last transaction */
/* __GNUC__ */
    /* try to get the token saved in $HOME */
    const char *home_dir = getpwuid(getuid())->pw_dir;
    
    if (home_dir != NULL && 
        (strlen(home_dir)+strlen("/")+sizeof(TOKEN_FILENAME)+1) <= MAX_PATH) {
        /* compose the token path */
        strncpy(token_path, home_dir, strlen(home_dir));
        strncat(token_path, "/", strlen("/"));
        strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME)+1);
    } else {
        /* if token path is too long or $HOME is NULL */
        strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
    }

    FILE *fp = fopen(token_path, "rb");
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
    }

    if (fp != NULL) {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
            /* if token is invalid, clear the buffer */
            memset(&token, 0x0, sizeof(sgx_launch_token_t));
            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
        }
    }



    /* Step 2: call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);


    /* Step 3: save the launch token if it is updated */


/* __GNUC__ */
    if (updated == FALSE || fp == NULL) {
        /* if the token is not updated, or file handler is invalid, do not perform saving */
        if (fp != NULL) fclose(fp);
        return 0;
    }

    /* reopen the file with write capablity */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL) return 0;
    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
    if (write_num != sizeof(sgx_launch_token_t))
        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
    fclose(fp);
    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    //printf("%s", str);
}


/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        return -1;}

    mainn(argc, argv);

    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);
    
    printf("Info: SampleEnclave successfully returned.\n");

    

    return 0;
}

void app_AllocateMemory(size_t size){
    encl_AllocateMemory(global_eid,size);
}

void app_AllocateMemory2(size_t size){
    encl_AllocateMemory2(global_eid,size);
}

void app_AllocateMemory3(size_t size){
    encl_AllocateMemory3(global_eid,size);
}

void app_AllocateMemory4(size_t size){
    encl_AllocateMemory4(global_eid,size);
}

void app_AllocateMemory5(size_t size){
    encl_AllocateMemory5(global_eid,size);
}
void app_FreeMemory(){
    encl_FreeMemory(global_eid);
}

void app_FreeMemory2(){
    encl_FreeMemory2(global_eid);
}

void app_FreeMemory3(){
    encl_FreeMemory3(global_eid);
}

void app_FreeMemory4(){
    encl_FreeMemory4(global_eid);
}

void app_FreeMemory5(){
    encl_FreeMemory5(global_eid);
}


/*NumSort*/
void app_LoadNumArrayWithRand(unsigned long arraysize, unsigned int numarrays){
    encl_LoadNumArrayWithRand(global_eid,arraysize,numarrays);
}

void app_NumHeapSort(unsigned long base_offset,unsigned long bottom, unsigned long top){
 sgx_status_t t= encl_NumHeapSort(global_eid,base_offset,bottom,top);
}

/*StringSort*/
unsigned long app_LoadStringArray(unsigned int numarrays, unsigned long arraysize){
		unsigned long retval;
		encl_LoadStringArray(global_eid,&retval, numarrays,arraysize);
		return retval;
}

void app_StrHeapSort(unsigned long oparrayOffset, unsigned long strarrayOffset, unsigned long numstrings, unsigned long bottom, unsigned long top){
 	//encl_StrHeapSort(global_eid, oparrayOffset, strarrayOffset, numstrings, bottom, top);
}

void app_call_StrHeapSort(unsigned long nstrings, unsigned int numarrays, unsigned long arraysize){
	 encl_call_StrHeapSort(global_eid, nstrings, numarrays, arraysize);
}

/*BitSort*/
unsigned long app_bitSetup(long bitfieldarraysize, long bitoparraysize){
	unsigned long retval;
	encl_bitSetup(global_eid, &retval, bitfieldarraysize, bitoparraysize);
	return retval;
}

void app_ToggleBitRun(unsigned long bit_addr, unsigned long nbits, unsigned int val){
	encl_ToggleBitRun(global_eid, bit_addr, nbits, val);
}

void app_FlipBitRun(long bit_addr,long nbits){
	encl_FlipBitRun(global_eid, bit_addr, nbits);   
}

/*Floating Point*/
void app_SetupCPUEmFloatArrays(unsigned long arraysize){
    encl_SetupCPUEmFloatArrays(global_eid,arraysize);
}
void app_DoEmFloatIteration(ulong arraysize, ulong loops){
    encl_DoEmFloatIteration(global_eid, arraysize, loops);
}

/*Fourier*/
void app_DoFPUTransIteration(unsigned long arraysize){
    encl_DoFPUTransIteration(global_eid, arraysize);
}

/*Assignment*/

void app_LoadAssignArrayWithRand(unsigned long numarrays){
	encl_LoadAssignArrayWithRand(global_eid,numarrays);
}

void app_call_AssignmentTest(unsigned int numarrays){
	encl_call_AssignmentTest(global_eid, numarrays);
}

/*IDEAsort*/
void app_loadIDEA(unsigned long arraysize){
	encl_app_loadIDEA(global_eid, arraysize);
}

void app_callIDEA(unsigned long arraysize, unsigned short* Z, unsigned short* DK, unsigned long nloops){
	encl_callIDEA(global_eid, arraysize, Z, DK, nloops);
}

/*Neural Net*/
void app_set_numpats(int npats){
    encl_set_numpats(global_eid,npats);
}

double app_get_in_pats(int patt, int element){
    double retval;
    encl_get_in_pats(global_eid,&retval,patt,element);
    return retval;
}

void app_set_in_pats(int patt, int element, double val){
    encl_set_in_pats(global_eid,patt,element,val);
}

void app_set_out_pats(int patt, int element, double val){
    encl_set_out_pats(global_eid,patt,element,val);
}

void app_DoNNetIteration(unsigned long nloops){
    encl_DoNNetIteration(global_eid,nloops);
}

/*LU Decomposition*/
void app_build_problem(){
	encl_build_problem(global_eid);
}

void app_moveSeedArrays(unsigned long numarrays){
	encl_moveSeedArrays(global_eid, numarrays);
}

void app_call_lusolve(unsigned long numarrays){
	encl_call_lusolve(global_eid, numarrays);
}

/*Huffman Compression*/
void app_buildHuffman(unsigned long arraysize){
	encl_buildHuffman(global_eid, arraysize);
}

void app_callHuffman(unsigned long nloops, unsigned long arraysize){
	encl_callHuffman(global_eid, nloops,arraysize);
}




