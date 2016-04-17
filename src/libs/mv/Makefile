
root=../../..

all: mvIntro $(root)/$(binDir)/$(libsDir)/mv \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libmv.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libmv.so
	@echo "	"$(DONE)

mvIntro:
	@echo "	☞ mv"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/mv

#exécutable
$(root)/$(binDir)/$(libsDir)/mv: $(root)/$(objDir)/$(libsDir)/mv/mv.o \
						  $(root)/$(objDir)/$(libsDir)/cp/utils.o \
						  $(root)/$(objDir)/$(libsDir)/rm/utils.o \
						  $(root)/$(objDir)/$(libsDir)/mv/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/mv $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/mv $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libmv.a: $(root)/$(objDir)/$(libsDir)/mv/mv.o \
							  $(root)/$(objDir)/$(libsDir)/cp/utils.o \
							  $(root)/$(objDir)/$(libsDir)/rm/utils.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libmv.so: $(root)/$(objDir)/$(libsDir)/mv/mv.o \
							  $(root)/$(objDir)/$(libsDir)/cp/utils.o \
							  $(root)/$(objDir)/$(libsDir)/rm/utils.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/mv/%.o: $(root)/$(srcDir)/$(libsDir)/mv/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/mv
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"

$(root)/$(objDir)/$(libsDir)/cp/%.o: $(root)/$(srcDir)/$(libsDir)/cp/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/cp
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"

$(root)/$(objDir)/$(libsDir)/rm/%.o: $(root)/$(srcDir)/$(libsDir)/rm/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/rm
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
