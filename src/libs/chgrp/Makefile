
root=../../..

all: chgrpIntro $(root)/$(binDir)/$(libsDir)/chgrp \
			  $(root)/$(binDir)/$(libsDir)/$(staticDir)/libchgrp.a \
			  $(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libchgrp.so
	@echo "	"$(DONE)

chgrpIntro:
	@echo "	☞ chgrp"
	@mkdir -p $(root)/$(objDir)/$(libsDir)/chgrp

#exécutable
$(root)/$(binDir)/$(libsDir)/chgrp: $(root)/$(objDir)/$(libsDir)/chgrp/chgrp.o \
						  $(root)/$(objDir)/$(libsDir)/chown/utils.o \
						  $(root)/$(objDir)/$(libsDir)/chgrp/main.o
	@echo "		$(BOLD_C)- Executable$(NO_C)"
	@$(CC) -o $(root)/$(binDir)/$(libsDir)/chgrp $^ $(LDFLAGS)
	@echo "		$(CC) -o $(root)/$(binDir)/$(libsDir)/chgrp $^ $(LDFLAGS)"
	@echo "		"$(DONE)

#librairie statique
$(root)/$(binDir)/$(libsDir)/$(staticDir)/libchgrp.a:\
	$(root)/$(objDir)/$(libsDir)/chgrp/chgrp.o \
	$(root)/$(objDir)/$(libsDir)/chown/utils.o
	@echo "		$(BOLD_C)- Static$(NO_C)"
	@ar -cr $@ $^
	@echo "		ar -cr $@ $^"
	@ranlib $@
	@echo "		ranlib $@"
	@echo "		"$(DONE)

#librairie dynamique
$(root)/$(binDir)/$(libsDir)/$(dynamicDir)/libchgrp.so:/
	$(root)/$(objDir)/$(libsDir)/chgrp/chgrp.o \
	$(root)/$(objDir)/$(libsDir)/chown/utils.o \
	$(root)/$(objDir)/$(libsDir)/chown/lib.o
	@echo "		$(BOLD_C)- Dynamic$(NO_C)"
	@$(CC) -shared -o $@ $^
	@echo "		$(CC) -shared -o $@ $^"
	@echo "		"$(DONE)

#fichiers objets
$(root)/$(objDir)/$(libsDir)/chgrp/%.o: $(root)/$(srcDir)/$(libsDir)/chgrp/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/chgrp
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"

$(root)/$(objDir)/$(libsDir)/chown/%.o: $(root)/$(srcDir)/$(libsDir)/chown/%.c
	@mkdir -p $(root)/$(objDir)/$(libsDir)/chown
	@$(CC) $(CFLAGS) $(libCFLAGS) $(debug) -o $@ $^
	@echo "		$(OK_C)"`basename $@`"$(NO_C)"
