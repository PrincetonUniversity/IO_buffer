# Requires two defined variables to be passed to the makefile
# CXX = ...
# target = opt/debug   (note that we don't actually check for anything other than opt)


SOURCES = fortran_api.cpp fortranAPI_handler.cpp LiFo_policy.cpp	\
mapper.cpp buffer.cpp 
HEADERS = buffer.hpp mapper.hpp fortran_api.hpp abstract_policy.hpp LiFo_policy.hpp	\
list_policy.hpp singleton.hpp fortranAPI_handler.hpp spin_lock.hpp
TEMPLATES = buffer.tpp

OBJECTS = $(SOURCES:%.cpp=%.o)

ifeq ($(target), opt)
    #CFLAGS= -D_REENTRANT -std=c++11 -Wall -Wextra -g -pedantic
    CFLAGS=  -D_REENTRANT -std=c++0x -O3 -Wall -Wextra -g -pedantic -D FORBUF_FAST
else
    CFLAGS = -D_REENTRANT -std=c++0x -O0 -ggdb -Wall -Wextra -g -pedantic 
endif

LFLAGS= -lpthread

# uncomment to have fortran code write DEBUG info
#CFLAGS += -D DEBUG_FORBUF

all:	libforbuf.a binary_to_ascii

binary_to_ascii:	binary_to_ascii.o
	$(CXX) binary_to_ascii.o -o binary_to_ascii

libforbuf.a:	$(OBJECTS)
	ar vr $@ $?
	ranlib $@

test:	$(OBJECTS) testit.o for_buf_tester
	$(CXX) testit.o $(OBJECTS) -o test $(LFLAGS)

for_buf_tester:	libforbuf.a for_buf_tester.f90
	./compileTest.sh

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $@

clean:	
	rm *.o *.a

#depend:	$(PACKAGES_DEPEND) 
#	@makedepend -fmake.dep  testit.cpp $(SOURCES) $(HEADERS) $(TEMPLATES)
#
#make.dep:	createmdep depend
#
#PHONY:	createmdep
#
#createmdep:
#	touch make.dep

include make.dep
