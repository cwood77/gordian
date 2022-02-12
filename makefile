COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static

all: $(OUT_DIR)/debug/http.exe $(OUT_DIR)/debug/tcatbin.dll

clean:
	rm -rf bin

.PHONY: all clean

# ----------------------------------------------------------------------
# http

HTTP_SRC = src/http/main.cpp
HTTP_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(HTTP_SRC)))

$(OUT_DIR)/debug/http.exe: $(HTTP_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -o $@ $(HTTP_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -lwinhttp -Lbin/out/debug -ltcatlib

$(HTTP_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/http
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# tcatlib

TCATLIB_SRC = src/tcatlib/impl.cpp
TCATLIB_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(TCATLIB_SRC)))

$(OUT_DIR)/debug/tcatlib.lib: $(TCATLIB_DEBUG_OBJ)
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@ar crs $@ $<

$(TCATLIB_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/tcatlib
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# tcatbin

TCATBIN_SRC = \
	src/tcatbin/metadata.cpp \
	src/tcatbin/impl.cpp

TCATBIN_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(TCATBIN_SRC)))

$(OUT_DIR)/debug/tcatbin.dll: $(TCATBIN_DEBUG_OBJ)
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(TCATBIN_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST)

$(TCATBIN_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/tcatbin
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@
