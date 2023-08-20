CXX:=clang
LIBPATH:=./build
EXT:=./external
SOURCE:=*.c
LDPATHS:=-L$(LIBPATH)/SDL -L$(LIBPATH)/cglm
INCLUDE:=-I$(EXT)/SDL/include -I$(EXT)/glad/include -I$(EXT)/stb -I$(EXT)/cglm/include
LDFLAGS:=-lSDL2 -lSDL2main -lGL -lm
CXXFLAGS:=-std=c99 -Wall
OBJ:=$(LIBPATH)/glad/glad.o
PROG:=minescam

all: SDL glad
	$(CXX) $(CXXFLAGS) $(SOURCE) $(OBJ) $(LDPATHS) $(INCLUDE) $(LDFLAGS) -o $(PROG)

dbg: SDL glad
	$(CXX) $(CXXFLAGS) -g -DMDEBUG $(SOURCE) $(OBJ) $(LDPATHS) $(INCLUDE) $(LDFLAGS) -o $(PROG)

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

cglm: ensure-build
	@if ! [ -f "$(LIBPATH)/cglm/libcglm.so" ]; then\
		cd $(LIBPATH)/cglm\
		&& cmake ../../external/cglm && make\
		&& cd ../..;\
	fi

.PHONY: ensure-build
ensure-build:
	mkdir -p $(LIBPATH) && mkdir -p $(LIBPATH)/SDL && mkdir -p $(LIBPATH)/glad && mkdir -p $(LIBPATH)/cglm;

.PHONY: bear
bear:
	@which bear\
	@ if [[ $? > 0 ]]; then\
		echo "bear not found, please install it";\
	fi

.PHONY: setup-clangd
setup-clangd:
	@bear -- make all

.PHONY: clean
clean:
	rm -rf build *.so *.o *.a $(PROG)

