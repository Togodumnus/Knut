
root=../../..

all: pwdIntro $(root)/$(binDir)/$(libsDir)/pwd \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libpwd.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libpwd.so
	@echo "	"$(DONE)

pwdIntro:
	@echo "	☞ pwd"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/pwd

#exécutable
$(root)/$(binDir)/$(libsDir)/pwd: $(root)/$(objDir)/$(libsDir)/pwd/pwd.o \
						  $(root)/$(objDir)/$(libsDir)/pwd/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/pwd $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/pwd $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libpwd.a:\
	$(root)/$(objDir)/$(libsDir)/pwd/pwd.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libpwd.so:\
	$(root)/$(objDir)/$(libsDir)/pwd/pwd.o \
	$(root)/$(objDir)/$(libsDir)/pwd/lib.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/pwd/%.o: $(root)/$(srcDir)/$(libsDir)/pwd/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/pwd
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
