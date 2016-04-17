
root=../../..

all: cpIntro $(root)/$(binDir)/$(libsDir)/cp \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libcp.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libcp.so
	@echo "	"$(DONE)

cpIntro:
	@echo "	☞ cp"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/cp

#exécutable
$(root)/$(binDir)/$(libsDir)/cp: $(root)/$(objDir)/$(libsDir)/cp/cp.o \
						  $(root)/$(objDir)/$(libsDir)/cp/utils.o \
						  $(root)/$(objDir)/$(libsDir)/cp/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/cp $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/cp $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libcp.a: $(root)/$(objDir)/$(libsDir)/cp/cp.o \
						  $(root)/$(objDir)/$(libsDir)/cp/utils.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libcp.so: $(root)/$(objDir)/$(libsDir)/cp/cp.o \
						  $(root)/$(objDir)/$(libsDir)/cp/utils.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/cp/%.o: $(root)/$(srcDir)/$(libsDir)/cp/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/cp
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
