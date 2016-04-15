
root=../../..

all: suIntro $(root)/$(binDir)/$(libsDir)/su \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libsu.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libsu.so
	@echo "	"$(DONE)

suIntro:
	@echo "	☞ su"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/su

#exécutable
$(root)/$(binDir)/$(libsDir)/su: $(root)/$(objDir)/$(libsDir)/su/su.o \
						  $(root)/$(objDir)/$(libsDir)/su/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/su $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/su $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libsu.a: $(root)/$(objDir)/$(libsDir)/su/su.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libsu.so: $(root)/$(objDir)/$(libsDir)/su/su.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/su/%.o: $(root)/$(srcDir)/$(libsDir)/su/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/su
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"