SOURCES = fortran_api.cpp fortranAPI_handler.cpp LiFo_policy.cpp mapper.cpp buffer.cpp
HEADERS = buffer.hpp mapper.hpp fortran_api.hpp pools.hpp
TEMPLATES = buffer.tpp

OBJECTS = $(SOURCES:%.cpp=%.o)

CPP=g++

CFLAGS= -std=c++11 -Wall

all:	libforbuf.a 

libforbuf.a:	$(OBJECTS)
	ar vr $@ $?
	ranlib $@

test:	$(OBJECTS) testit.o
	$(CPP) testit.o $(OBJECTS) -o test

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean:	
	rm *.o *.a

depend:	$(PACKAGES_DEPEND) 
	@makedepend -fmake.dep  $(SOURCES) $(HEADERS) $(TEMPLATES)
#
#make.dep:	createmdep depend
#
#PHONY:	createmdep
#
#createmdep:
#	touch make.dep

include make.dep
