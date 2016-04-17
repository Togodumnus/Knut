
root=../../..

all: rmIntro $(root)/$(binDir)/$(libsDir)/rm \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/librm.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/librm.so
	@echo "	"$(DONE)

rmIntro:
	@echo "	☞ rm"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/rm

#exécutable
$(root)/$(binDir)/$(libsDir)/rm: $(root)/$(objDir)/$(libsDir)/rm/rm.o \
						  $(root)/$(objDir)/$(libsDir)/rm/utils.o \
						  $(root)/$(objDir)/$(libsDir)/rm/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/rm $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/rm $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/librm.a: $(root)/$(objDir)/$(libsDir)/rm/rm.o \
						  $(root)/$(objDir)/$(libsDir)/rm/utils.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/librm.so: $(root)/$(objDir)/$(libsDir)/rm/rm.o \
						  $(root)/$(objDir)/$(libsDir)/rm/utils.o \
						  $(root)/$(objDir)/$(libsDir)/rm/lib.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/rm/%.o: $(root)/$(srcDir)/$(libsDir)/rm/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/rm
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
