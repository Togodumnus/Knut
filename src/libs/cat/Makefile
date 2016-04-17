
root=../../..

all: catIntro $(root)/$(binDir)/$(libsDir)/cat \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libcat.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libcat.so
	@echo "	"$(DONE)

catIntro:
	@echo "	☞ cat"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/cat

#exécutable
$(root)/$(binDir)/$(libsDir)/cat: $(root)/$(objDir)/$(libsDir)/cat/cat.o \
						  $(root)/$(objDir)/$(libsDir)/cat/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/cat $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/cat $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libcat.a: $(root)/$(objDir)/$(libsDir)/cat/cat.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libcat.so: $(root)/$(objDir)/$(libsDir)/cat/cat.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/cat/%.o: $(root)/$(srcDir)/$(libsDir)/cat/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/cat
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
