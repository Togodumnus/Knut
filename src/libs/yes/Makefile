
root=../../..

all: yesIntro $(root)/$(binDir)/$(libsDir)/yes \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libyes.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libyes.so
	@echo "	"$(DONE)

yesIntro:
	@echo "	☞ yes"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/yes

#exécutable
$(root)/$(binDir)/$(libsDir)/yes: $(root)/$(objDir)/$(libsDir)/yes/yes.o \
						  $(root)/$(objDir)/$(libsDir)/yes/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/yes $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/yes $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libyes.a: $(root)/$(objDir)/$(libsDir)/yes/yes.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libyes.so: $(root)/$(objDir)/$(libsDir)/yes/yes.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/yes/%.o: $(root)/$(srcDir)/$(libsDir)/yes/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/yes
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
