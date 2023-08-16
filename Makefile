CXX:=clang
LIBPATH:=./build
EXT:=./external
SOURCE:=*.c
LDPATHS:=-L$(LIBPATH)/SDL
INCLUDE:=-I$(EXT)/SDL/include -I$(EXT)/glad/include -I$(EXT)/stb
LDFLAGS:=-lSDL2 -lSDL2main -lGL -lm
CXXFLAGS:=-std=c99
OBJ:=$(LIBPATH)/glad/glad.o
PROG:=minescam

all: SDL glad
	$(CXX) $(CXXFLAGS) $(SOURCE) $(OBJ) $(LDPATHS) $(INCLUDE) $(LDFLAGS) -o $(PROG)

dbg: SDL glad
	$(CXX) $(CXXFLAGS) -g $(SOURCE) $(OBJ) $(LDPATHS) $(INCLUDE) $(LDFLAGS) -o $(PROG)

# builds SDL using cmake...
SDL: ensure-build
	@if ! [ -f "$(LIBPATH)/SDL/libSDL2main.a" ]; then\
		cd $(LIBPATH)/SDL && cmake ../../external/SDL && cmake --build . && cd ../..;\
	fi

glad: ensure-build
	@if ! [ -f "$(LIBPATH)/glad/glad.o" ]; then\
		cd $(LIBPATH)/glad\
		&& $(CXX) -c ../../external/glad/src/glad.c -I../../external/glad/include -o glad.o\
		&& cd ../..;\
	fi

.PHONY: ensure-build
ensure-build:
	mkdir -p $(LIBPATH) && mkdir -p $(LIBPATH)/SDL && mkdir -p $(LIBPATH)/glad
