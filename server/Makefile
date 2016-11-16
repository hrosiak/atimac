AP=../atima.1.3/lib
atimalibs=$(AP)/libdedx.a $(AP)/libbspline.a $(AP)/libcolsys.a $(AP)/libcmlib.a 
LIBS=-lcppcms -lbooster ../atimapp.o ../atimac.a ${atimalibs} -lgfortran -lm
INCS= -I../

all: acppcms

acppcms: atima_cppcms.cpp
	$(CXX) -O2 -Wall ${INCS} -o acppcms atima_cppcms.cpp ${LIBS}


clean:
	rm -fr acppcms app
