#Macros
default: objects
CC = gcc
CFLAGS = -p -g -s -static -O0
DEFINES= -DLINUX $(NO_UNAME)

#Dependencies
sysinfoc.c: Makefile
	./sysinfo.sh $(CC) $(DEFINES) $(CFLAGS)

sysinfo.c: Makefile
	./sysinfo.sh $(CC) $(DEFINES) $(CFLAGS)
	

hardware.o: hardware.c hardware.h Makefile
	$(CC) $(DEFINES) $(CFLAGS)\
		-c hardware.c

nbench0.o: nbench0.h nbench0.c nmglobal.h pointer.h hardware.h\
	   Makefile sysinfo.c sysinfoc.c
	$(CC) $(DEFINES) $(CFLAGS)\
		-c nbench0.c

emfloat.o: emfloat.h emfloat.c nmglobal.h pointer.h Makefile
	$(CC) $(DEFINES) $(CFLAGS)\
		-c emfloat.c

pointer.h: pointer Makefile
	$(CC) $(DEFINES) $(CFLAGS)\
		-o pointer pointer.c
	rm -f pointer.h
	if [ "4" = `./pointer` ] ; then touch pointer.h ;\
	else echo "#define LONG64" >pointer.h ; fi

misc.o: misc.h misc.c Makefile
	$(CC) $(DEFINES) $(CFLAGS)\
		-c misc.c

nbench1.o: nbench1.h nbench1.c wordcat.h nmglobal.h pointer.h Makefile
	$(CC) $(DEFINES) $(CFLAGS)\
		-c nbench1.c

sysspec.o: sysspec.h sysspec.c nmglobal.h pointer.h Makefile
	$(CC) $(DEFINES) $(CFLAGS)\
		-c sysspec.c

objects: emfloat.o misc.o nbench0.o nbench1.o sysspec.o hardware.o

##########################################################################
clean:
	- /bin/rm -f *.o *~ \#* core a.out sysinfo.c sysinfoc.c \
		 bug pointer pointer.h debugbit.dat
