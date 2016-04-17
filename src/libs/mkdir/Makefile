
root=../../..

all: mkdirIntro $(root)/$(binDir)/$(libsDir)/mkdir \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libmkdir.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libmkdir.so
	@echo "	"$(DONE)

mkdirIntro:
	@echo "	☞ mkdir"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/mkdir

#exécutable
$(root)/$(binDir)/$(libsDir)/mkdir: $(root)/$(objDir)/$(libsDir)/mkdir/mkdir.o \
						  $(root)/$(objDir)/$(libsDir)/mkdir/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/mkdir $^ -lm $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/mkdir $^ -lm $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libmkdir.a:\
	$(root)/$(objDir)/$(libsDir)/mkdir/mkdir.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libmkdir.so:\
	$(root)/$(objDir)/$(libsDir)/mkdir/mkdir.o \
	$(root)/$(objDir)/$(libsDir)/mkdir/lib.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/mkdir/%.o: $(root)/$(srcDir)/$(libsDir)/mkdir/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/mkdir
	@$(CC) $(CFLAGS) $(libCFLAGS)  $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
