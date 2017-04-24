
MODE = zero

include Makefile.in

#### specify the target OS
OS = _WINDOWS_
OS = _LINUX_
COPTS += -D$(OS)

############################### Various ##############################
### -rpath arguments for finding .so objects in pre-specified locations
MY_DIR = .
RPATH = -rpath=$(MY_DIR)
### decide on debugging level(s)
COPTS += -D  _DEBUG_
COPTS += -DNO_DEBUG2_

############################### Target ##############################
all:
	$(CXX) -c $(COPTS) pk_stat.cpp
	$(CC) -c $(COPTS) main.c
	$(CXX) $(LOPTS) main.o pk_stat.o


clean:
	rm -f  *.o *.mod a.out 

