COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_LNK_FLAGS = -ggdb -static-libgcc -static-libstdc++ -lwinhttp
DEBUG_CC_FLAGS = -g

all: $(OUT_DIR)/debug/http.exe $(OUT_DIR)/debug/tcatbin.dll

clean:
	rm -rf bin

.PHONY: all clean

# ----------------------------------------------------------------------
# http

HTTP_SRC = src/http/main.cpp
HTTP_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(HTTP_SRC)))

$(OUT_DIR)/debug/http.exe: $(HTTP_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	mkdir -p $(OUT_DIR)/debug
	$(LINK_CMD) -ggdb -o $@ $(HTTP_DEBUG_OBJ) -static-libgcc -static-libstdc++ -static -lwinhttp -Lbin/out/debug -ltcatlib

$(HTTP_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	mkdir -p $(OBJ_DIR)/debug/http
	$(COMPILE_CMD) -ggdb -c -Wall $< -o $@

# ----------------------------------------------------------------------
# tcatlib

TCATLIB_SRC = src/tcatlib/impl.cpp
TCATLIB_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(TCATLIB_SRC)))

$(OUT_DIR)/debug/tcatlib.lib: $(TCATLIB_DEBUG_OBJ)
	mkdir -p $(OUT_DIR)/debug
	ar rvs $@ $<

$(TCATLIB_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	mkdir -p $(OBJ_DIR)/debug/tcatlib
	$(COMPILE_CMD) -ggdb -c -Wall $< -o $@

# ----------------------------------------------------------------------
# tcatbin

TCATBIN_SRC = src/tcatbin/metadata.cpp src/tcatbin/impl.cpp
TCATBIN_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(TCATBIN_SRC)))

$(OUT_DIR)/debug/tcatbin.dll: $(TCATBIN_DEBUG_OBJ)
	mkdir -p $(OUT_DIR)/debug
	$(LINK_CMD) -shared -ggdb -o $@ $(TCATBIN_DEBUG_OBJ) -static-libgcc -static-libstdc++ -static

$(TCATBIN_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	mkdir -p $(OBJ_DIR)/debug/tcatbin
	$(COMPILE_CMD) -ggdb -c -Wall $< -o $@
