
root=../../..

all: psIntro $(root)/$(binDir)/$(libsDir)/ps \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libps.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libps.so
	@echo "	"$(DONE)

psIntro:
	@echo "	☞ ps"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/ps

#exécutable
$(root)/$(binDir)/$(libsDir)/ps: $(root)/$(objDir)/$(libsDir)/ps/ps.o \
						  $(root)/$(objDir)/$(libsDir)/ps/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/ps $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/ps $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libps.a: $(root)/$(objDir)/$(libsDir)/ps/ps.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libps.so: $(root)/$(objDir)/$(libsDir)/ps/ps.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/ps/%.o: $(root)/$(srcDir)/$(libsDir)/ps/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/ps
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
