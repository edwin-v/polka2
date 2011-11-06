# base makefile for polka2

all: 
	@cd resources/icons && make all
	@cd resources/cursors && make all
	@cd src && make all
	
clean:
	@cd resources/icons && make clean
	@cd resources/cursors && make clean
	@cd src && make clean

install:

win32:
	@cd src && make win32

.PHONY: all clean install win32

