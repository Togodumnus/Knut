
root=../../..

all: duIntro $(root)/$(binDir)/$(libsDir)/du \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libdu.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libdu.so
	@echo "	"$(DONE)

duIntro:
	@echo "	☞ du"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/du

#exécutable
$(root)/$(binDir)/$(libsDir)/du: $(root)/$(objDir)/$(libsDir)/du/du.o \
						  $(root)/$(objDir)/$(libsDir)/du/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/du $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/du $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libdu.a:\
	$(root)/$(objDir)/$(libsDir)/du/du.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libdu.so:\
	$(root)/$(objDir)/$(libsDir)/du/du.o \
	$(root)/$(objDir)/$(libsDir)/du/lib.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/du/%.o: $(root)/$(srcDir)/$(libsDir)/du/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/du
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
