all: compile link clean

compile:
	g++ -std=c++11 -DSFML_STATIC -Isrc/include -c main.cpp

link:
	g++ main.o -o main -Lsrc/lib -lsfml-main -lsfml-graphics-s -lsfml-window-s -lopengl32 -lwinmm -lgdi32 -lfreetype -lsfml-system-s -lwinmm -static-libstdc++ -static-libgcc -static -mwindows

clean:
	  rm -f *.o 