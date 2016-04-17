
root=../../..

all: moreIntro $(root)/$(binDir)/$(libsDir)/more \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libmore.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libmore.so
	@echo "	"$(DONE)

moreIntro:
	@echo "	☞ more"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/more

#exécutable
$(root)/$(binDir)/$(libsDir)/more: $(root)/$(objDir)/$(libsDir)/more/more.o \
						  $(root)/$(objDir)/$(libsDir)/more/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/more $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/more $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libmore.a:\
	$(root)/$(objDir)/$(libsDir)/more/more.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libmore.so:\
	$(root)/$(objDir)/$(libsDir)/more/more.o \
	$(root)/$(objDir)/$(libsDir)/more/lib.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/more/%.o: $(root)/$(srcDir)/$(libsDir)/more/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/more
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
