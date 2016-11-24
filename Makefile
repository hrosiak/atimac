AP=atima.1.3/lib
obj=atimaspline.o
libs=$(AP)/libdedx.a $(AP)/libbspline.a $(AP)/libcolsys.a $(AP)/libcmlib.a 
FC=gfortran -fPIC 
GCCARG=-std=gnu99 
GPPARG=-std=c++11

all: libcomp $(obj) static shared

libcomp:
	cd atima.1.3/src/atima && $(MAKE)
	cp $(AP)/* .

atimac.o: atimac.h atimac.c
	gcc -fPIC $(GCCARG) -c atimac.c 

libatimac.a: atimac.o $(obj)
	ar rvs $@ $^

libatimac.so: libatimac.a
	gcc -shared atimac.o $(obj) $(libs) -o libatimac.so

atimapp.o: atimapp.cpp atimapp.h atimac.c 
	g++ -fPIC $(GPPARG) -c atimapp.cpp

libatimapp.a: atimapp.o
	ar rvs $@ $^

libatimapp.so: atimapp.o libatimac.a
	g++ -shared -o libatimapp.so atimapp.o libatimac.a
	
shared: libatimapp.so libatimac.so

static: libatimapp.a libatimac.a
	
test: atimac_tests.c atimac.h atimac.c libatimac.a 
	gcc atimac_tests.c  libatimac.a $(libs) -lgfortran -lm -lcmocka -o cmockatest
	./cmockatest

py: atimacpy.pyx atimacpy.pxd libatimac.a
	python setup.py build_ext -i

tar:
	tar cvf atima.tar *.h *.c *.f *.cpp *.pyx *.pxd setup.py test.py Makefile server/Makefile server/*.js server/*.cpp server/*.py

clean:
	-rm *.o *.so *.a cmockatest atimacpy.c *.tar
	cd atima.1.3/src/atima && $(MAKE) purge