# Makefile for polka2 cursor resources

PNGCONV = gdk-pixbuf-csource
PNGCONV_FLAGS = --raw

pngs := $(wildcard *.png)
PNGS := $(wildcard *.PNG)
GDK_PIX := $(pngs:%.png=%.c) $(PNGS:%.PNG=%.c)


all: cursors

clean:
	@echo Removing cursors ...
	@$(RM) -rf *.c

cursors: $(GDK_PIX) Makefile
	@echo "// include all cursor files" > cursors.c
	@echo $(foreach cursor, $(GDK_PIX),\\n\#include \"cursors/$(cursor)\") >> cursors.c
	@echo "\\n\\nstatic const struct { const char *name; const guint8 *pixbuf; const guint x; const guint y; } Cursors[] = {" >> cursors.c
	@echo $(foreach cursor, $(GDK_PIX),\\n{ \"$(shell echo ${cursor} | sed s/_[0-9]*x[0-9]*\\.c//)\", \
	                                          $(cursor:%.c=%), \
	                                          $(shell echo ${cursor} | sed 's/.*_\([0-9]*\)x[0-9]*\.c/\1/'), \
	                                          $(shell echo ${cursor} | sed 's/.*_[0-9]*x\([0-9]*\)\.c/\1/') }, \
	       ) >> cursors.c
	@echo "\\n{ \"EOL\", 0, 0, 0 } };\\n" >> cursors.c
	
#	@echo "" > cursors.c
#	@.for i in ${GDK_PIX}
#	@	echo "#include "cursors/" ${i:Q} ".c\n"
#	@.endfor
		

%.c: %.png
	@$(PNGCONV) $(PNGCONV_FLAGS) --name $* $< > $@

%.c: %.PNG
	@$(PNGCONV) $(PNGCONV_FLAGS) --name $* $< > $@

install: cursors
	# install

.PHONY: all cursors

