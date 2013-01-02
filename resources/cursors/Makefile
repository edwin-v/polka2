# Makefile for polka2 cursor resources

BASE_LOCATION := ../..

include $(BASE_LOCATION)/Makefile.platform.inc

PNGCONV = gdk-pixbuf-csource
PNGCONV_FLAGS = --raw

pngs := $(wildcard *.png)
GDK_PIX := $(pngs:%.png=$(CURSOR_LOCATION)/%.c)
CURSOR_TARGET := $(CURSOR_LOCATION)/cursors.c

# additional quotes for sed in unix shells
ifneq ($(PLATFORM), win32)
  SED_DLTR := '
else
  SED_DLTR :=
endif

.PHONY: all dirs install

all: dirs $(CURSOR_TARGET)

clean:
	@echo Removing cursors ...
	@$(RMDIR) $(CURSOR_LOCATION)

$(CURSOR_TARGET): $(GDK_PIX) Makefile
	@echo Generating cursor resources...
	@echo // include all cursor files > $(CURSOR_TARGET)
	@$(ECHO) $(foreach cursor, $(GDK_PIX),$(NL)#include \"$(cursor:$(CURSOR_LOCATION)/%=%)\") >> $(CURSOR_TARGET)
	@$(ECHO) "$(NL)$(NL)static const struct { const char *name; const guint8 *pixbuf; const guint x; const guint y; } Cursors[] = {" >> $(CURSOR_TARGET)
	@$(ECHO) $(foreach cursor, $(GDK_PIX),$(NL){ \"$(shell echo $(cursor:$(CURSOR_LOCATION)/%.c=%) | $(SED) s/_[0-9]*x[0-9]*//)\", \
	                                             $(cursor:$(CURSOR_LOCATION)/%.c=%), \
	                                             $(shell echo $(cursor:$(CURSOR_LOCATION)/%.c=%) | $(SED) $(SED_DLTR)s/.*_\([0-9]*\)x[0-9]*/\1/$(SED_DLTR)), \
	                                             $(shell echo $(cursor:$(CURSOR_LOCATION)/%.c=%) | $(SED) $(SED_DLTR)s/.*_[0-9]*x\([0-9]*\)/\1/$(SED_DLTR)) }, \
	          ) >> $(CURSOR_TARGET)
	@$(ECHO) "$(NL){ \"EOL\", 0, 0, 0 } };$(NL)" >> $(CURSOR_TARGET)
	
		

$(CURSOR_LOCATION)/%.c: %.png
	@$(PNGCONV) $(PNGCONV_FLAGS) --name $* $< > $@

dirs:
	@$(MKDIR) -p $(CURSOR_LOCATION)

install: cursors
	# install


