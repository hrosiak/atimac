atimac_path=../
libs=$(atimac_path)/libatimac.a $(atimac_path)/libdedx.a $(atimac_path)/libbspline.a $(atimac_path)/libcolsys.a $(atimac_path)/libcmlib.a
GCCARG=-std=gnu99 -I$(atimac_path) -L$(atimac_path) -lgfortran -lm 
GPPARG=-std=c++11 -I$(atimac_path) -L$(atimac_path) -lgfortran -lm

all: capp cppapp

capp: capp.c
	gcc $(GCCARG) -Wall -o capp capp.c $(libs)

cppapp: cppapp.cpp
	g++ $(GPPARG) -Wall -o cppapp cppapp.cpp $(libs) $(atimac_path)/libatimapp.a
	
clean:
	-rm *.o capp cppapp
