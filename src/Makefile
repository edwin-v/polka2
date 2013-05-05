# Makefile for polka2 main executable

BASE_LOCATION := ..

include $(BASE_LOCATION)/Makefile.platform.inc

CXX = g++
LD = ld
MAKE = make
CXXFLAGS += -g -Wall -std=c++0x
LDFLAGS = 

ifeq ($(PLATFORM), win32)
BIN_NAME = polka2.exe
else
BIN_NAME = polka2
endif

# subdirectories with source files
SUBDIRS := objects editors import export

SOURCES := $(wildcard *.cc) $(foreach dir, $(SUBDIRS), $(wildcard $(dir)/*.cc))
OBJECTS := $(SOURCES:%.cc=$(OBJ_LOCATION)/%.o)
DEPS := $(SOURCES:%.cc=$(DEP_LOCATION)/%.d)

INCLUDES := -I. $(foreach dir, $(SUBDIRS), -I$(dir)) -I$(ICON_LOCATION) -I$(CURSOR_LOCATION) \
            $(shell pkg-config gtkmm-3.0 --cflags)
LIBS := $(shell pkg-config gtkmm-3.0 --libs)

.PHONY: all clean install dirs

all: dirs $(BIN_LOCATION)/$(BIN_NAME)
	
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
	@$(CXX) -o $@ $(LDFLAGS) $(OBJECTS) $(LIBS)

$(OBJ_LOCATION)/%.o: %.cc
	@echo Compiling $<
	@$(CXX) -MMD -MF $(DEP_LOCATION)/$*.d -MP \
	        -o $@ $(CXXFLAGS) $(INCLUDES) -c $<

dirs:
	@$(MKDIR) -p $(BIN_LOCATION)
	@$(MKDIR) -p $(foreach dir, $(SUBDIRS), $(OBJ_LOCATION)/$(dir))
	@$(MKDIR) -p $(foreach dir, $(SUBDIRS), $(DEP_LOCATION)/$(dir))

install: $(BIN_LOCATION)/$(BIN_NAME)
	# install
