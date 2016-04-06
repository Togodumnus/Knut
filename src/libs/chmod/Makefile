
root=../../..

all: chmodIntro $(root)/$(binDir)/$(libsDir)/chmod \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libchmod.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libchmod.so
	@echo "	"$(DONE)

chmodIntro:
	@echo "	☞ chmod"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/chmod

#exécutable
$(root)/$(binDir)/$(libsDir)/chmod: $(root)/$(objDir)/$(libsDir)/chmod/chmod.o \
						  $(root)/$(objDir)/$(libsDir)/chmod/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/chmod $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/chmod $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libchmod.a: $(root)/$(objDir)/$(libsDir)/chmod/chmod.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libchmod.so: $(root)/$(objDir)/$(libsDir)/chmod/chmod.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/chmod/%.o: $(root)/$(srcDir)/$(libsDir)/chmod/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/chmod
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
