CC := gcc
LD := ld

HBUTILS_CC  ?= $(CC)

# Default Configuration

EXT_DEF     ?= 
EXT_INC     ?= 
EXT_LIBS    ?= 
VERSION     ?= 0.2.0

DEFAULT_INC := ./include
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o
THISMACHINE ?= $(shell uname -srm | sed -e 's/ /-/g')
THISSYSTEM  ?= $(shell uname -s)
TARGET      ?= $(THISMACHINE)

LIBMODULES      := 
BUILDDIR        := build/$(THISMACHINE)
PRODUCTDIR      := bin/$(THISMACHINE)
PACKAGEDIR      ?= ./../_hbpkg/$(THISMACHINE)/hbutils.$(VERSION)
HBUTILS_LIBTOOL := libtool
CFLAGS          ?= -std=gnu99 -O3 -pthread -fPIC
HBUTILS_CFLAGS  := $(CFLAGS)
HBUTILS_DEF     := 
HBUTILS_INC     := -I$(DEFAULT_INC) -I./../_hbsys/$(TARGET)/include $(EXT_INC)
HBUTILS_LIB     := -L./../_hbsys/$(TARGET)/lib $(EXT_LIBS)

ifeq ($(THISSYSTEM),Darwin)
    # Mac can't do conditional selection of static and dynamic libs at link time.
    #	PRODUCT_LIBS := libhbutils.$(THISSYSTEM).dylib libhbutils.$(THISSYSTEM).a
    PRODUCT_LIBS := libhbutils.$(THISSYSTEM).a
else ifeq ($(THISSYSTEM),Linux)
#    PRODUCT_LIBS := libhbutils.$(THISSYSTEM).so libhbutils.GNU.a
    PRODUCT_LIBS := libhbutils.GNU.a
else ifeq ($(THISSYSTEM),CYGWIN_NT-10.0)
    PRODUCT_LIBS := libhbutils.GNU.a
else
    error "THISSYSTEM set to unknown value: $(THISSYSTEM)"
endif

# Export the following variables to the shell: will affect submodules
HBUTILS_BUILDDIR   := $(BUILDDIR)
HBUTILS_PRODUCTDIR := $(PRODUCTDIR)
export HBUTILS_BUILDDIR
export HBUTILS_PRODUCTDIR
export HBUTILS_CC
export HBUTILS_CFLAGS
export HBUTILS_DEF
export HBUTILS_INC
export HBUTILS_LIB

# Derived Parameters
#SUBMODULES  := main ext
SUBMODULES  := main
INCDEP      := -I$(DEFAULT_INC)


# Global vars that get exported to sub-makefiles
deps: $(LIBMODULES)
lib: $(PRODUCT_LIBS)
all: lib test
remake: cleaner all
pkg: deps lib install

install: 
	@rm -rf $(PACKAGEDIR)
	@mkdir -p $(PACKAGEDIR)
	@cp -R ./$(PRODUCTDIR)/* ./$(PACKAGEDIR)/
	@rm -f $(PACKAGEDIR)/../hbutils
	@ln -s hbutils.$(VERSION) ./$(PACKAGEDIR)/../hbutils
	cd ../_hbsys && $(MAKE) sys_install INS_MACHINE=$(TARGET) INS_PKGNAME=hbutils

directories:
	@mkdir -p $(PRODUCTDIR)
	@mkdir -p $(BUILDDIR)
	@cp -R ./include/* ./$(PRODUCTDIR)/

#Clean only Objects
clean:
	@$(RM) -rf $(BUILDDIR)
	@$(RM) -rf $(PRODUCTDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf bin
	@$(RM) -rf build

# Test
# Tests not working right now, commented out
test: $(PRODUCT_LIBS)
#	$(eval MKFILE := $(notdir $@))
#	cd ./$@ && $(MAKE) -f $(MKFILE).mk obj


#Build the static library
#There are several supported variants.
libhbutils.Darwin.a: $(SUBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	$(HBUTILS_LIBTOOL) -static -o libhbutils.a $(LIBTOOL_OBJ)
	@mv libhbutils.a $(PRODUCTDIR)/

libhbutils.GNU.a: $(SUBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	ar -rcs $(PRODUCTDIR)/libhbutils.a $(LIBTOOL_OBJ)
	ranlib $(PRODUCTDIR)/libhbutils.a

libhbutils.c2000.a: $(SUBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	ar2000 -a libhbutils.a $(LIBTOOL_OBJ)
	@mv libhbutils.a $(PRODUCTDIR)/

#Build Shared Library
libhbutils.Linux.so: $(SUBMODULES)
	$(eval LIBTOOL_OBJ := $(shell find $(BUILDDIR) -type f -name "*.$(OBJEXT)"))
	$(HBUTILS_CC) -shared -fPIC -Wl,-soname,libhbutils.so.1 $(HBUTILS_INC) -o $(PRODUCTDIR)/libhbutils.so.$(VERSION) $(LIBTOOL_OBJ) $(HBUTILS_LIB) -lc 


#Library dependencies (not in hbutils sources)
$(LIBMODULES): %: 
#	cd ./../$@ && $(MAKE) lib TARGET=$(TARGET) && $(MAKE) install
	cd ./../$@ && $(MAKE) pkg

#hbutils submodules
$(SUBMODULES): %: directories
	$(eval MKFILE := $(notdir $@))
	cd ./$@ && $(MAKE) -f $(MKFILE).mk obj



#Non-File Targets
.PHONY: all lib pkg remake install clean cleaner

