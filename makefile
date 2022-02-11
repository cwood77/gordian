COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_LNK_FLAGS = -g -static-libgcc -static-libstdc++ -lwinhttp
DEBUG_CC_FLAGS = -g

all: $(OUT_DIR)/debug/http.exe

clean:
	rm -rf bin

.PHONY: all clean

# ----------------------------------------------------------------------
# http

HTTP_SRC = src/http/main.cpp
HTTP_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(HTTP_SRC)))

$(OUT_DIR)/debug/http.exe: $(HTTP_DEBUG_OBJ)
	mkdir -p $(OUT_DIR)/debug
	$(LINK_CMD) -g -o $@ $(HTTP_DEBUG_OBJ) -static-libgcc -static-libstdc++ -lwinhttp

$(HTTP_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	mkdir -p $(OBJ_DIR)/debug/http
	$(COMPILE_CMD) -g -c -Wall $< -o $@
