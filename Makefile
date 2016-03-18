
CC = gcc
CFLAGS = -c -Wall -std=c99
LDFLAGS=

libCFLAGS = -fPIC

debug = -c

objDir   = obj
binDir   = bin
srcDir   = src
libsDir  = libs
shellDir = shell

EXEC=knutShell#le shell à compiler

LIBS=yes #librairies à compiler

#Colors
NO_C=\033[0m
OK_C=\033[32;01m
ERROR_C=\033[31;01m
WARN_C=\033[33;01m

DONE = "$(OK_C)✓ Done$(NO_C)"

#
# Règles
#

all: libs shell clean
	@echo "KnutShell is ready ! \n\
- run $(WARN_C)$(binDir)/$(EXEC)Static$(NO_C) for knutShell using static \
libraries \n\
- run $(WARN_C)$(binDir)/$(EXEC)$(NO_C) for knutShell using dynamic \
libraries \n\
  (don't forget $(WARN_C)export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:\
$(shell pwd)$(NO_C))"

.PHONY: clean
clean:
	@echo "☞ Cleaning"
	@rm -rf $(objDir)
	@echo $(DONE)

################################################################################
#							Compilation des libs							   #
################################################################################
libs: libsIntro libsBuild
	@echo "$(OK_C)✓ Libs Done$(NO_C)"

libsIntro:
	@echo "☞ Libs"
	@mkdir -p $(binDir)/$(libsDir)

libsBuild: $(LIBS)

# Commande yes

yes: yesIntro $(binDir)/$(libsDir)/yes $(binDir)/$(libsDir)/libyes.a \
		$(binDir)/$(libsDir)/libyes.so
	@echo "	"$(DONE)

yesIntro:
	@echo "	☞ yes"

#exécutable
$(binDir)/$(libsDir)/yes: $(objDir)/$(libsDir)/yes/yes.o \
						  $(objDir)/$(libsDir)/yes/main.o
	@echo "		● Executable"
	@$(CC) -o $(binDir)/$(libsDir)/yes $^ $(LDFLAGS)
	@echo "		$(CC) -o $(binDir)/$(libsDir)/yes $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(binDir)/$(libsDir)/libyes.a: $(objDir)/$(libsDir)/yes/yes.o
	$(call make-static-lib,$@,$^)

#librairie dynamique
$(binDir)/$(libsDir)/libyes.so: $(objDir)/$(libsDir)/yes/yes.o
	$(call make-dynamic-lib,$@,$^)

$(objDir)/$(libsDir)/yes/%.o: $(srcDir)/$(libsDir)/yes/%.c
	@mkdir -p $(objDir)/$(libsDir)/yes
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"

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
	@echo "	"$(DONE)

# $(call make-static-lib, /path/to/maLib.a, <.o files>)
define make-static-lib
	@echo "		● Static"
	@ar -cr $1 $2
	@echo "		ar -cr $1 $2"
	@ranlib $1
	@echo "		ranlib $1"
	@echo "		"$(DONE)
endef

# $(call make-dynamic-lib,$@,$^)
define make-dynamic-lib
	@echo "		● Dynamic"
	@$(CC) -shared -o $1 $2
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)
endef
