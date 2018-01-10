prefix?=/usr/local
target=$(DESTDIR)$(prefix)

VERSION=1.1
DOCS_DIR := man
BIN_DIR := bin
CMDS := $(addprefix $(BIN_DIR)/,stl_header stl_merge stl_transform stl_count stl_bbox stl_cube stl_sphere stl_cylinder stl_cone stl_torus stl_empty stl_threads stl_normals stl_convex stl_borders stl_spreadsheet stl_area)
CSGJS_CMDS := $(addprefix $(BIN_DIR)/,stl_boolean)

ALL_CMDS := $(CSGJS_CMDS) $(CMDS)

CC := g++
#FLAGS=-Og -g -std=c++11
FLAGS=-O3 -std=c++11

all: $(CMDS) bin/stl_boolean

$(BIN_DIR)/%: src/%.cpp src/stl_util.h
	$(CC) $(FLAGS) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $(OUTPUT_OPTION) $<

$(BIN_DIR)/stl_boolean: src/stl_boolean.cpp src/csgjs/*.cpp src/csgjs/math/*.cpp src/csgjs/math/*.h src/csgjs/*.h
	$(CC) $(FLAGS) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) src/stl_boolean.cpp src/csgjs/*.cpp src/csgjs/math/*.cpp -Isrc -o bin/stl_boolean 

$(CMDS): | $(BIN_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

$(DOCS_DIR):
	mkdir $(DOCS_DIR)

docs: $(DOCS_DIR) $(CMDS)
	for cmd in $(ALL_CMDS); do \
	  help2man $$cmd --name="$$($$cmd --help 2>&1 | head --lines=1)" --no-discard-stderr --version-string="v$(VERSION)" --no-info > $(DOCS_DIR)/$$(basename $$cmd).1; \
	done

installDocs: docs
	install -d $(target)/share/man/man1
	for cmd in $(ALL_CMDS); do \
	  gzip -c -9 $(DOCS_DIR)/$$(basename $$cmd).1 > $(target)/share/man/man1/$$(basename $$cmd).1.gz; \
	done

uninstallDocs:
	for cmd in $(ALL_CMDS); do \
	  rm $(target)/share/man/man1/$$(basename $$cmd).1.gz; \
	done

install: all
	install -d $(target)/bin
	for cmd in $(ALL_CMDS); do \
	  install $$cmd $(target)/$$cmd; \
	done

uninstall: 
	for cmd in $(ALL_CMDS); do \
	  rm $(target)/$$cmd; \
	done
