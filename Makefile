# base makefile for polka2

BASE_LOCATION := .

include Makefile.platform.inc

# print starting message
ifeq ($(PLATFORM), linux)
  $(info Building Polka2 for the Linux platform.)
endif
ifeq ($(PLATFORM), win32)
  $(info Building for the Windows platform.)
endif

MAKE := $(MAKE) --no-print-directory

all: 
	@cd resources/icons && $(MAKE) all
	@cd resources/cursors && $(MAKE) all
	@cd src && $(MAKE) all
	
clean:
	@cd resources/icons && $(MAKE) clean
	@cd resources/cursors && $(MAKE) clean
	@cd src && $(MAKE) clean

install:

.PHONY: all clean install

