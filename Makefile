
CC = gcc
CFLAGS = -c -Wall -std=c99 -Werror
LDFLAGS= -ldl -rdynamic

libCFLAGS = -fPIC

debug = -g #à enlever à la fin du debug

objDir     = obj
binDir     = bin
srcDir     = src
libsDir    = libs
dynamicDir = dynamic
staticDir  = static
shellDir   = shell

EXEC=knutShell#le shell à compiler

#librairies à compiler
LIBS=yes 	\
	 cat    \

#Colors
NO_C		=\033[0m
OK_C		=\033[32;01m
ERROR_C		=\033[31;01m
WARN_C		=\033[33;01m
BOLD_C      =\033[1m
UNDERLINE_C =\033[4m
BLINK_C     =\033[5m
INVERSE_C   =\033[7m

DONE = "$(OK_C)✓ Done$(NO_C)"

export #export de toutes les variables par défaut

#
# Règles
#

all: libs shell
	@echo "KnutShell is ready ! \n\
- run $(WARN_C)$(binDir)/$(EXEC)Static$(NO_C) for knutShell with static \
libraries \n\
- run $(WARN_C)$(binDir)/$(EXEC)$(NO_C) for knutShell with dynamic \
libraries \n\
  (don't forget $(WARN_C)export LD_LIBRARY_PATH="'$$'"LD_LIBRARY_PATH:\
$(shell pwd)/$(binDir)/$(libsDir)/$(dynamicDir)$(NO_C))"

dev: CFLAGS += -DDEBUG_FLAG
dev:all

.PHONY: clean distclean

clean:
	@echo "$(BOLD_C)☞ Cleaning$(NO_C)"
	@rm -rf $(objDir)
	@echo $(DONE)

distclean:
	@echo "$(BOLD_C)☞ rm ./bin/$(NO_C)"
	@rm -rf $(binDir)
	@echo $(DONE)

################################################################################
#							Compilation des libs							   #
################################################################################
libs: libsIntro libsBuild
	@echo "$(OK_C)✓ Libs Done$(NO_C)"

libsIntro:
	@echo "$(BOLD_C)☞ Libs$(NO_C)"
	@mkdir -p $(binDir)/$(libsDir)
	@mkdir -p $(binDir)/$(libsDir)/$(staticDir)
	@mkdir -p $(binDir)/$(libsDir)/$(dynamicDir)

libsBuild:
	$(foreach lib, $(LIBS), $(MAKE) -C $(srcDir)/$(libsDir)/$(lib);)


################################################################################
#							Compilation du shell							   #
################################################################################
shell: shellIntro shellBuildDynamic shellBuildStatic

shellIntro:
	@echo "$(BOLD_C)☞ Compiling Shell$(NO_C)"

shellBuildDynamic: $(objDir)/$(shellDir)/main.o \
				   $(objDir)/$(shellDir)/libs.o \
				   $(objDir)/$(shellDir)/utils.o \
				   $(objDir)/$(shellDir)/shellCommands.o
	@echo "$(BOLD_C)- using dynamic librairies$(NO_C)"
	$(CC) -o $(binDir)/$(EXEC) $^ $(LDFLAGS)

shellBuildStatic: $(objDir)/$(shellDir)/main-Static.o \
				   $(objDir)/$(shellDir)/libs.o \
				   $(objDir)/$(shellDir)/utils.o \
				  $(objDir)/$(shellDir)/shellCommands.o
	@echo "$(BOLD_C)- using static librairies$(NO_C)"
	$(CC) \
		-o $(binDir)/$(EXEC)Static $^ \
		-L$(binDir)/$(libsDir)/$(staticDir) $(addprefix -l, $(LIBS)) $(LDFLAGS)
	@echo "$(OK_C)✓ Shell Done$(NO_C)"

$(objDir)/$(shellDir)/%.o: $(srcDir)/$(shellDir)/%.c
	$(call compile-shell-dep, $@, $^)

$(objDir)/$(shellDir)/%-Static.o: $(srcDir)/$(shellDir)/%.c
	$(call compile-shell-dep, $@, $^, -DLIB_STATIC)



################################################################################
#									Fonctions								   #
################################################################################

define compile-shell-dep
	@echo "	⇾ Compiling $1"
	@mkdir -p $(objDir)/$(shellDir)
	@$(CC) $(CFLAGS) $3 $(debug) -o $1 $2
	@echo "	$(CC) $(CFLAGS) $(debug) -o $1 $2"
	@echo "	"$(DONE)
endef

