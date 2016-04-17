
root=../../..

all: chownIntro $(root)/$(binDir)/$(libsDir)/chown \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libchown.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libchown.so
	@echo "	"$(DONE)

chownIntro:
	@echo "	☞ chown"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/chown

#exécutable
$(root)/$(binDir)/$(libsDir)/chown: $(root)/$(objDir)/$(libsDir)/chown/chown.o \
						  $(root)/$(objDir)/$(libsDir)/chown/utils.o \
						  $(root)/$(objDir)/$(libsDir)/chown/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/chown $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/chown $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libchown.a: $(root)/$(objDir)/$(libsDir)/chown/chown.o \
						  $(root)/$(objDir)/$(libsDir)/chown/utils.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libchown.so: $(root)/$(objDir)/$(libsDir)/chown/chown.o \
						  $(root)/$(objDir)/$(libsDir)/chown/utils.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/chown/%.o: $(root)/$(srcDir)/$(libsDir)/chown/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/chown
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
