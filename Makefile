#makefile for gen html cpp
INC= -I ./plog/include/
gen: gen.o
	g++ gen.o -o gen
gen.o: gen.cpp
	g++ $(INC) -c gen.cpp 
clean:
	rm gen gen.o a.out
