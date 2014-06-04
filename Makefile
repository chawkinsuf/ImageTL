CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

CC=g++
CFLAGS=-c -DIMAGETL_LIBRARY_COMPILE
LDFLAGS=

lib/libimagetl.a: $(OBJ_FILES)
	ar rcs $@ $^

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -o $@ $<
