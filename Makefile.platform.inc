#
# include file to determine platform
#

# check platform (and implicitly the availability of gcc)
SYS := $(shell gcc -dumpmachine)
ifneq (, $(findstring linux, $(SYS)))
 # Linux platform. Don't care which
 PLATFORM := linux
endif
ifneq (, $(findstring mingw, $(SYS)))
  # Windows platform. Assume 32-bit.
  PLATFORM := win32
endif

ifeq ($(PLATFORM), )
  # Do things for others
  $(error Unknown platform: $(SYS))
endif

# use mkdir capabale of recursive directory creation 
ifeq ($(PLATFORM), win32)
  MKDIR := $(shell which mkdir.exe)
else
  MKDIR := mkdir
endif

ifeq ($(MKDIR), )
  $(error Building requires mkdir.exe from UnxUtils.)
endif

# use unix rm to remove directories
ifeq ($(PLATFORM), win32)
  RMDIR := $(shell which rm.exe) -rf
else
  RMDIR := rm -rf
endif

ifeq ($(RMDIR), )
  $(error Building requires rmdir.exe from UnxUtils.)
endif

# command for deleting separate files
ifeq ($(PLATFORM), win32)
  RM := del
else
  RM := rm
endif

# use mkdir capabale of recursive directory creation 
ifeq ($(PLATFORM), win32)
  ECHO := $(shell which echo.exe)
else
  ECHO := echo
endif

ifeq ($(MKDIR), )
  $(error Building requires mkdir.exe from UnxUtils.)
endif

# newlines for echoing because unix shells parse escapes
ifeq ($(PLATFORM), win32)
  NL := \n
else
  NL := \\n
endif

# check if base location is set
ifeq ($(BASE_LOCATION), )
  $(error The required BASE_LOCATION is not set)
endif

# require the stream editor sed
ifeq ($(PLATFORM), win32)
  SED := $(shell which sed.exe)
else
  SED := sed
endif

ifeq ($(SED), )
  $(error Building requires sed.exe, please install UnxUtils.)
endif

# create location variables
PLATFORM_LOCATION := $(BASE_LOCATION)/derived/$(PLATFORM)
BIN_LOCATION := $(BASE_LOCATION)/derived/bin
DEP_LOCATION := $(PLATFORM_LOCATION)/deps
OBJ_LOCATION := $(PLATFORM_LOCATION)/objs
ICON_LOCATION := $(PLATFORM_LOCATION)/src/icons
CURSOR_LOCATION := $(PLATFORM_LOCATION)/src/cursors
