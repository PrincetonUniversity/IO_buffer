SOURCES = fortran_api.cpp fortranAPI_handler.cpp LiFo_policy.cpp	\
mapper.cpp buffer.cpp 
HEADERS = buffer.hpp mapper.hpp fortran_api.hpp abstract_policy.hpp LiFo_policy.hpp	\
list_policy.hpp singleton.hpp fortranAPI_handler.hpp 
TEMPLATES = buffer.tpp

OBJECTS = $(SOURCES:%.cpp=%.o)

MACHINE = $(shell uname -n)
ifeq ($(MACHINE),tiger2)
CPP=icpc
else
CPP=g++
endif

CFLAGS= -D_REENTRANT -std=c++11 -Wall -Wextra -g -pedantic
LFLAGS= -lpthread

# uncomment to make code faster but use less error-checking
CFLAGS += -D FORBUF_FAST

all:	libforbuf.a binary_to_ascii

binary_to_ascii:	binary_to_ascii.o
	$(CPP) binary_to_ascii.o -o binary_to_ascii

libforbuf.a:	$(OBJECTS)
	ar vr $@ $?
	ranlib $@

test:	$(OBJECTS) testit.o
	$(CPP) testit.o $(OBJECTS) -o test $(LFLAGS)

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean:	
	rm *.o *.a

depend:	$(PACKAGES_DEPEND) 
	@makedepend -fmake.dep  testit.cpp $(SOURCES) $(HEADERS) $(TEMPLATES)
#
#make.dep:	createmdep depend
#
#PHONY:	createmdep
#
#createmdep:
#	touch make.dep

include make.dep
