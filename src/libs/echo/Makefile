<<<<<<< HEAD
=======

>>>>>>> 0826c3f54b366449997a9c9708357ec19e4348e9
root=../../..

all: echoIntro $(root)/$(binDir)/$(libsDir)/echo \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libecho.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libecho.so
	@echo "	"$(DONE)

echoIntro:
	@echo "	☞ echo"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/echo

#exécutable
$(root)/$(binDir)/$(libsDir)/echo: $(root)/$(objDir)/$(libsDir)/echo/echo.o \
						  $(root)/$(objDir)/$(libsDir)/echo/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/echo $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/echo $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libecho.a: $(root)/$(objDir)/$(libsDir)/echo/echo.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libecho.so: $(root)/$(objDir)/$(libsDir)/echo/echo.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/echo/%.o: $(root)/$(srcDir)/$(libsDir)/echo/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/echo
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
<<<<<<< HEAD
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
=======
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
>>>>>>> 0826c3f54b366449997a9c9708357ec19e4348e9
