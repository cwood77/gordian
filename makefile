COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall -D cdwDebugMode -D cdwTest -Wno-invalid-offsetof
RELEASE_CC_FLAGS = -O3 -c -Wall -D cdwTest -Wno-invalid-offsetof
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static
RELEASE_LNK_FLAGS_POST = -static-libgcc -static-libstdc++ -static

SCRIPTLIB = scriptlib/xcopy-deploy.bat

all: \
	$(OUT_DIR)/debug/tcatbin.dll \
	$(OUT_DIR)/release/tcatbin.dll \
	$(OUT_DIR)/debug/archive.dll \
	$(OUT_DIR)/release/archive.dll \
	$(OUT_DIR)/debug/console.dll \
	$(OUT_DIR)/release/console.dll \
	$(OUT_DIR)/debug/console.test.dll \
	$(OUT_DIR)/release/console.test.dll \
	$(OUT_DIR)/debug/curator.dll \
	$(OUT_DIR)/release/curator.dll \
	$(OUT_DIR)/debug/exec.dll \
	$(OUT_DIR)/release/exec.dll \
	$(OUT_DIR)/debug/file.dll \
	$(OUT_DIR)/release/file.dll \
	$(OUT_DIR)/debug/file.test.dll \
	$(OUT_DIR)/release/file.test.dll \
	$(OUT_DIR)/debug/http.dll \
	$(OUT_DIR)/release/http.dll \
	$(OUT_DIR)/debug/store.dll \
	$(OUT_DIR)/release/store.dll \
	$(OUT_DIR)/debug/test.exe \
	$(OUT_DIR)/release/test.exe \
	$(OUT_DIR)/debug/gordian.exe \
	$(OUT_DIR)/release/gordian.exe \
	$(subst scriptlib,$(OUT_DIR)/debug,$(SCRIPTLIB)) \
	$(subst scriptlib,$(OUT_DIR)/release,$(SCRIPTLIB))
	$(OUT_DIR)/debug/test.exe
	$(OUT_DIR)/release/test.exe

clean:
	rm -rf bin

.PHONY: all clean

# ----------------------------------------------------------------------
# scriptlib

$(OUT_DIR)/debug/%.bat: scriptlib/%.bat
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@cp $< $@

$(OUT_DIR)/release/%.bat: scriptlib/%.bat
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@cp $< $@

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

TCATLIB_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(TCATLIB_SRC)))

$(OUT_DIR)/release/tcatlib.lib: $(TCATLIB_RELEASE_OBJ)
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@ar crs $@ $<

$(TCATLIB_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/tcatlib
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

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

TCATBIN_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(TCATBIN_SRC)))

$(OUT_DIR)/release/tcatbin.dll: $(TCATBIN_RELEASE_OBJ)
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(TCATBIN_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST)

$(TCATBIN_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/tcatbin
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# archive

ARCHIVE_SRC = \
	src/archive/archive.cpp \
	src/archive/compress.cpp \
	src/archive/keyExpert.cpp \
	src/archive/sign.cpp \
	src/archive/split.cpp \

ARCHIVE_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(ARCHIVE_SRC)))

$(OUT_DIR)/debug/archive.dll: $(ARCHIVE_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(ARCHIVE_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -lcabinet -lbcrypt -lncrypt -Lbin/out/debug -ltcatlib

$(ARCHIVE_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/archive
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

ARCHIVE_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(ARCHIVE_SRC)))

$(OUT_DIR)/release/archive.dll: $(ARCHIVE_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(ARCHIVE_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -lcabinet -lbcrypt -lncrypt -Lbin/out/release -ltcatlib

$(ARCHIVE_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/archive
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# console

CONSOLE_SRC = \
	src/console/commandLineParser.cpp \
	src/console/log.cpp \

CONSOLE_TEST_SRC = \
	src/console/arg.test.cpp \

CONSOLE_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(CONSOLE_SRC)))

$(OUT_DIR)/debug/console.dll: $(CONSOLE_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(CONSOLE_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(CONSOLE_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/console
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

CONSOLE_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(CONSOLE_SRC)))

$(OUT_DIR)/release/console.dll: $(CONSOLE_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(CONSOLE_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(CONSOLE_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/console
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

CONSOLE_TEST_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(CONSOLE_TEST_SRC)))

$(OUT_DIR)/debug/console.test.dll: $(CONSOLE_TEST_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(CONSOLE_TEST_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(CONSOLE_TEST_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/console.test
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

CONSOLE_TEST_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(CONSOLE_TEST_SRC)))

$(OUT_DIR)/release/console.test.dll: $(CONSOLE_TEST_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(CONSOLE_TEST_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(CONSOLE_TEST_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/console.test
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# curator

CURATOR_SRC = \
	src/curator/curator.install.cpp \
	src/curator/curator.list.cpp \
	src/curator/curator.uninstall.cpp \
	src/curator/curator.upgrade.cpp \
	src/curator/directory.cpp \
	src/curator/facade.cpp \
	src/curator/instr.cpp \
	src/curator/recipe.list.cpp \
	src/curator/recipes.cpp \
	src/curator/subcurator.cpp \

CURATOR_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(CURATOR_SRC)))

$(OUT_DIR)/debug/curator.dll: $(CURATOR_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(CURATOR_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(CURATOR_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/curator
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

CURATOR_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(CURATOR_SRC)))

$(OUT_DIR)/release/curator.dll: $(CURATOR_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(CURATOR_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(CURATOR_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/curator
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# exec

EXEC_SRC = \
	src/exec/scriptRunner.cpp \

EXEC_TEST_SRC = \
	src/exec/sst.test.cpp \

EXEC_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(EXEC_SRC)))

$(OUT_DIR)/debug/exec.dll: $(EXEC_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(EXEC_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(EXEC_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/exec
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

EXEC_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(EXEC_SRC)))

$(OUT_DIR)/release/exec.dll: $(EXEC_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(EXEC_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(EXEC_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/exec
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# file

FILE_SRC = \
	src/file/api.cpp \
	src/file/manager.cpp \
	src/file/packager.cpp \
	src/file/parse.cpp \
	src/file/parse.test.cpp \

FILE_TEST_SRC = \
	src/file/sst.test.cpp \

FILE_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(FILE_SRC)))

$(OUT_DIR)/debug/file.dll: $(FILE_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(FILE_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(FILE_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/file
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

FILE_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(FILE_SRC)))

$(OUT_DIR)/release/file.dll: $(FILE_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(FILE_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(FILE_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/file
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

FILE_TEST_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(FILE_TEST_SRC)))

$(OUT_DIR)/debug/file.test.dll: $(FILE_TEST_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(FILE_TEST_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(FILE_TEST_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/file.test
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

FILE_TEST_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(FILE_TEST_SRC)))

$(OUT_DIR)/release/file.test.dll: $(FILE_TEST_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(FILE_TEST_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(FILE_TEST_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/file.test
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# http

HTTP_SRC = \
	src/http/main.cpp \

HTTP_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(HTTP_SRC)))

$(OUT_DIR)/debug/http.dll: $(HTTP_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(HTTP_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -lwinhttp -Lbin/out/debug -ltcatlib

$(HTTP_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/http
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

HTTP_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(HTTP_SRC)))

$(OUT_DIR)/release/http.dll: $(HTTP_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(HTTP_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -lwinhttp -Lbin/out/release -ltcatlib

$(HTTP_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/http
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# store

STORE_SRC = \
	src/store/basic.cpp \
	src/store/passthru.cpp \

STORE_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(STORE_SRC)))

$(OUT_DIR)/debug/store.dll: $(STORE_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -shared -o $@ $(STORE_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(STORE_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/store
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

STORE_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(STORE_SRC)))

$(OUT_DIR)/release/store.dll: $(STORE_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -shared -o $@ $(STORE_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(STORE_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/store
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# test

TEST_SRC = \
	src/test/assert.cpp \
	src/test/main.cpp \

TEST_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(TEST_SRC)))

$(OUT_DIR)/debug/test.exe: $(TEST_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -o $@ $(TEST_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(TEST_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/test
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

TEST_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(TEST_SRC)))

$(OUT_DIR)/release/test.exe: $(TEST_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -o $@ $(TEST_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(TEST_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/test
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# gordian

GORDIAN_SRC = \
	src/gordian/curatorCommand.cpp \
	src/gordian/init.verb.cpp \
	src/gordian/install.verb.cpp \
	src/gordian/keys.verb.cpp \
	src/gordian/list.verb.cpp \
	src/gordian/main.cpp \
	src/gordian/packager.verb.cpp \
	src/gordian/scrub.verb.cpp \
	src/gordian/uninstall.verb.cpp \
	src/gordian/upgrade.verb.cpp \

GORDIAN_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(GORDIAN_SRC)))

$(OUT_DIR)/debug/gordian.exe: $(GORDIAN_DEBUG_OBJ) $(OUT_DIR)/debug/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -o $@ $(GORDIAN_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -ltcatlib

$(GORDIAN_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/gordian
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

GORDIAN_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(GORDIAN_SRC)))

$(OUT_DIR)/release/gordian.exe: $(GORDIAN_RELEASE_OBJ) $(OUT_DIR)/release/tcatlib.lib
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -o $@ $(GORDIAN_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -ltcatlib

$(GORDIAN_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/gordian
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@
