# Makefile for polka2 icon resources

BASE_LOCATION := ../..

include $(BASE_LOCATION)/Makefile.platform.inc

PNGCONV = gdk-pixbuf-csource
PNGCONV_FLAGS = --raw

pngs := $(wildcard *.png)
GDK_PIX := $(pngs:%.png=$(ICON_LOCATION)/%.c)
ICON_TARGET := $(ICON_LOCATION)/icons.c

.PHONY: all dirs install

all: dirs $(ICON_TARGET)

clean:
	@echo Removing icons ...
	@$(RMDIR) $(ICON_LOCATION)

$(ICON_TARGET): $(GDK_PIX) Makefile
	@echo Generating icon resources...
	@echo // include all icon files > $(ICON_TARGET)
	@$(ECHO) $(foreach ICON, $(GDK_PIX),$(NL)#include \"$(ICON:$(ICON_LOCATION)/%=%)\") >> $(ICON_TARGET)
	@$(ECHO) "$(NL)$(NL)static const struct { const char *name; const guint8 *pixbuf; } Icons[] = {" >> $(ICON_TARGET)
	@$(ECHO) $(foreach ICON, $(GDK_PIX),$(NL){ \"$(ICON:$(ICON_LOCATION)/%.c=%)\", $(ICON:$(ICON_LOCATION)/%.c=%) },) >> $(ICON_TARGET)
	@$(ECHO) "$(NL){ \"EOL\", 0 } };$(NL)" >> $(ICON_TARGET)

$(ICON_LOCATION)/%.c: %.png
	@$(PNGCONV) $(PNGCONV_FLAGS) --name $* $< > $@

dirs:
	@$(MKDIR) -p $(ICON_LOCATION)

install: icons
	# install


