# Makefile for polka2 main executable

CXX = g++
LD = ld
RM = rm -f
RMDIR = rm -rf
MAKE = make
CXXFLAGS += -g -Wall -std=c++0x
LDFLAGS = 
MKDIR = mkdir -p
MKDIRW32 = md

DEP_LOCATION = ../derived/deps
OBJ_LOCATION = ../derived/objs
BIN_LOCATION = ../derived/bin
INCLUDESW32 = -ID:/Dev/gtk/include
BIN_NAME = polka2

main_SOURCES := $(wildcard *.cc)
editor_SOURCES := $(wildcard editors/*.cc)
object_SOURCES := $(wildcard objects/*.cc)
import_SOURCES := $(wildcard import/*.cc)
SOURCES := $(main_SOURCES) $(editor_SOURCES) $(object_SOURCES) $(import_SOURCES)
OBJECTS := $(SOURCES:%.cc=$(OBJ_LOCATION)/%.o)
DEPS := $(SOURCES:%.cc=$(DEP_LOCATION)/%.d)

INCLUDES := -I. -I../include/polka -Iobjects -Ieditors -Iimport       \
            $(shell pkg-config gtkmm-3.0 --cflags) -I../resources
LIBS := $(shell pkg-config gtkmm-3.0 --libs)

all: $(BIN_LOCATION)/$(BIN_NAME)
	
# include generated dependencies
-include $(DEPS)

clean:
	@echo Removing dependencies ...
	@$(RMDIR) $(DEP_LOCATION)
	@echo Removing objects ...
	@$(RMDIR) $(OBJ_LOCATION)
	@echo Removing binaries ...
	@$(RMDIR) $(BIN_LOCATION)

$(BIN_LOCATION)/$(BIN_NAME): $(OBJECTS)
	@echo Linking $@
	@$(MKDIR) $(BIN_LOCATION)
	@$(CXX) -o $@ $(LDFLAGS) $(OBJECTS) $(LIBS)

install: $(LIBRARY)
	# install

$(OBJ_LOCATION)/%.o: %.cc
	@echo Compiling $<
	@$(MKDIR) $(OBJ_LOCATION)
	@$(MKDIR) $(OBJ_LOCATION)/editors
	@$(MKDIR) $(OBJ_LOCATION)/objects
	@$(MKDIR) $(OBJ_LOCATION)/import
	@$(MKDIR) $(DEP_LOCATION)
	@$(MKDIR) $(DEP_LOCATION)/editors
	@$(MKDIR) $(DEP_LOCATION)/objects
	@$(MKDIR) $(DEP_LOCATION)/import
	@$(CXX) -MMD -MF $(DEP_LOCATION)/$*.d -MP \
	        -o $@ $(CXXFLAGS) $(INCLUDES) -c $<

win32:
	@echo Win32 build
	@$(MKDIRW32) $(BIN_LOCATION)
	@g++ -o $(BIN_LOCATION)/$(BIN_NAME).exe $(CXXFLAGS) $(INCLUDES) \
	        $(INCLUDESW32) $(SOURCES) $(LDFLAGS) $(LIBS)
	@strip $(BIN_LOCATION)/$(BIN_NAME).exe

.PHONY: all clean install win32

