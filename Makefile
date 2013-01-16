SOURCES = fortran_api.cpp 
HEADERS = buffer.hpp mapper.hpp fortran_api.hpp pools.hpp
TEMPLATES = buffer.tpp

OBJECTS = $(SOURCES:%.cpp=%.o)

CPP=g++

all:	libforbuf.a 

libforbuf.a:	$(OBJECTS)
	ar vr $@ $?
	ranlib $@

test:	$(OBJECTS) testit.cpp
	$(CPP) testit.o $(OBJECTS) -o test

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean:	
	rm *.o *.a

depend:	$(PACKAGES_DEPEND) 
	@makedepend -fmake.dep $(CFLAGS) $(SOURCES) $(HEADERS) $(TEMPLATES)

make.dep:	createmdep depend

createmdep:
	touch make.dep

include make.dep