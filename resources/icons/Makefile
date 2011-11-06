# Makefile for polka2 icon resources

PNGCONV = gdk-pixbuf-csource
PNGCONV_FLAGS = --raw

pngs := $(wildcard *.png)
PNGS := $(wildcard *.PNG)
GDK_PIX := $(pngs:%.png=%.c) $(PNGS:%.PNG=%.c)


all: icons

clean:
	@echo Removing icons ...
	@$(RM) -rf *.c

icons: $(GDK_PIX) Makefile
	@echo "// include all icon files" > icons.c
	@echo $(foreach ICON, $(GDK_PIX),\\n\#include \"icons/$(ICON)\") >> icons.c
	@echo "\\n\\nstatic const struct { const char *name; const guint8 *pixbuf; } Icons[] = {" >> icons.c
	@echo $(foreach ICON, $(GDK_PIX),\\n{ \"$(ICON:%.c=%)\", $(ICON:%.c=%) },) >> icons.c
	@echo "\\n{ \"EOL\", 0 } };\\n" >> icons.c
	
#	@echo "" > icons.c
#	@.for i in ${GDK_PIX}
#	@	echo "#include "icons/" ${i:Q} ".c\n"
#	@.endfor
		

%.c: %.png
	@$(PNGCONV) $(PNGCONV_FLAGS) --name $* $< > $@

%.c: %.PNG
	@$(PNGCONV) $(PNGCONV_FLAGS) --name $* $< > $@

install: icons
	# install

.PHONY: all icons

