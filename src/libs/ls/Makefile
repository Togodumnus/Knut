
root=../../..

all: lsIntro $(root)/$(binDir)/$(libsDir)/ls \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libls.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libls.so
	@echo "	"$(DONE)

lsIntro:
	@echo "	☞ ls"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/ls

#exécutable
$(root)/$(binDir)/$(libsDir)/ls: $(root)/$(objDir)/$(libsDir)/ls/ls.o \
						  $(root)/$(objDir)/$(libsDir)/ls/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/ls $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/ls $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libls.a:\
	$(root)/$(objDir)/$(libsDir)/ls/ls.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libls.so:\
	$(root)/$(objDir)/$(libsDir)/ls/ls.o \
	$(root)/$(objDir)/$(libsDir)/ls/lib.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/ls/%.o: $(root)/$(srcDir)/$(libsDir)/ls/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/ls
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
