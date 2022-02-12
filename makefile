COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static

all: $(OUT_DIR)/debug/http.dll $(OUT_DIR)/debug/tcatbin.dll $(OUT_DIR)/debug/gordian.exe $(OUT_DIR)/debug/test.exe

clean:
	rm -rf bin

.PHONY: all clean

# ----------------------------------------------------------------------
# test

TEST_SRC = src/test/main.cpp
TEST_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(TEST_SRC)))

$(OUT_DIR)/debug/test.exe: $(TEST_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -o $@ $(TEST_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(TEST_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/test
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# gordian

GORDIAN_SRC = src/gordian/main.cpp
GORDIAN_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(GORDIAN_SRC)))

$(OUT_DIR)/debug/gordian.exe: $(GORDIAN_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -o $@ $(GORDIAN_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(GORDIAN_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/gordian
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# http

HTTP_SRC = src/http/main.cpp
HTTP_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(HTTP_SRC)))

$(OUT_DIR)/debug/http.dll: $(HTTP_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(HTTP_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -lwinhttp -Lbin/out/debug -ltcatlib

$(HTTP_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/http
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# tcatlib

TCATLIB_SRC = src/tcatlib/api.cpp
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
	src/tcatbin/impl.cpp \
	src/tcatbin/metadata.cpp \
	src/tcatbin/tables.cpp \

TCATBIN_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(TCATBIN_SRC)))

$(OUT_DIR)/debug/tcatbin.dll: $(TCATBIN_DEBUG_OBJ)
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(TCATBIN_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST)

$(TCATBIN_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/tcatbin
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@
