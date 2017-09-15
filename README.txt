Benchmark for Intel SGX

This program is an adaptation of nbench-byte-2.2.3 to work on Intel SGX hardware with the purpose of evaluating the performance of different benchmarks inside of an enclave application and to compare its overhead, if any, against unmodified applications. 

How To Run
	first compile all the files using the command make ($make)
	after the files have been compiled, run the application ($./app)
	
Files Included in the Project: 
Makefile    
README.txt  
sampleExecution.txt
bdoc.txt
Changes
COM.DAT
debugbit.good.gz
emfloat.c
emfloat.h
f.c
gmon.out
hardware
hardware.c
hardware.h
Makefile
Makefile2
Makefile.justchanged
Makefile.withchanges
misc.c
misc.h
nbench0.c
nbench0.c.orig
nbench0.h
nbench1.c
nbench1 (copy).c
nbench1.h
nmglobal.h
NNET.DAT
pointer.c
raul.c
README
README.motorola
README.nonlinux
README.submit
RESULTS
sgxPort.c
sysinfoc.c.example
sysinfoc.c.template
sysinfo.c.example
sysinfo.c.template
sysinfo.sh
sysspec.c
sysspec.h
wordcat.h
x.h
user_types.h
Enclave.config.xml
Enclave.cpp
Enclave.cpp.orig
Enclave.edl
Enclave.edl.orig
Enclave.h
Enclave.lds
Enclave_private.pem
encl_emfloat.c
App.cpp
App.cpp.orig
App.h
App.h.orig

How To Clean Compiled Project
	simply run the command make clean to delete all compiled files ($make clean)

Possible Problems Compiling
	the file sysinfo.sh inside the directory /nbenchPortal needs to have executing privileges. If an error 	occurs, change permissions on the file using chmod ($chmod +x nbenchPortal/sysinfo.sh) 
