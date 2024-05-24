all: compile clean

compile: main 
	
main: main.o canvas.o
	g++ main.o canvas.o -o main -Lsrc/lib -lsfml-main -lsfml-graphics-s -lsfml-window-s -lopengl32 -lwinmm -lgdi32 -lfreetype -lsfml-system-s -lwinmm -static-libstdc++ -static-libgcc -static

canvas.o:
	g++ -std=c++11 -DSFML_STATIC -Isrc/include -c canvas.cpp

main.o:
	g++ -std=c++11 -DSFML_STATIC -Isrc/include -c main.cpp
clean:
	  rm -f *.o 