
CC = gcc
CFLAGS = -c -Wall -std=c99
LDFLAGS=

debug = -c

objDir   = obj
binDir   = bin
srcDir   = src
libsDir  = libs
shellDir = shell

LIBS= #librairies à compiler
EXEC= knutShell #le shell à compiler

#
# Règles
#

all: libs shell clean
	@echo "KnutShell is ready. Let's run $(binDir)/$(EXEC)"

.PHONY: clean
clean:
	@echo "☞ Cleaning"
	@rm -rf $(objDir)
	@echo "✓ Done"

################################################################################
#							Compilation des libs							   #
################################################################################
libs: libsIntro libsBuild
	@echo "✓ Libs Done"

libsIntro:
	@echo "☞ Libs"

libsBuild:

################################################################################
#							Compilation du shell							   #
################################################################################
shell: shellIntro shellBuild

shellIntro:
	@echo "☞ Compiling Shell"

shellBuild: $(objDir)/$(shellDir)/main.o
	$(CC) -o $(binDir)/$(EXEC) $^ $(LDFLAGS)
	@echo "✓ Compiling Shell Done"

$(objDir)/$(shellDir)/main.o: $(srcDir)/$(shellDir)/main.c
	@echo "	⇾ Compiling $@"
	@mkdir -p $(objDir)/$(shellDir)
	@$(CC) $(CFLAGS) $(debug) -o $@ $^
	@echo "	$(CC) $(CFLAGS) $(debug) -o $@ $^"
	@echo "	✓ Done"

