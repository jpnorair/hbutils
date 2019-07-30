CC := gcc
LD := ld

MODNAME      := main

HBUTILS_CC	    ?= $(CC)
HBUTILS_CFLAGS ?= -std=gnu99 -O3
HBUTILS_DEF    ?= 
HBUTILS_INC    ?= 
HBUTILS_LIB    ?= 

BUILDDIR    := ../$(HBUTILS_BUILDDIR)/$(MODNAME)
TARGETDIR   := .
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o
LIB         := $(HBUTILS_LIB) 
INC         := $(patsubst -I./%, -I./../%, $(HBUTILS_INC)) 
INCDEP      := $(INC)

SOURCES     := $(shell find . -type f -name "*.$(SRCEXT)")
OBJECTS     := $(patsubst ./%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))


all: resources $(MODNAME)
obj: $(OBJECTS)
remake: cleaner all


#Copy Resources from Resources Directory to Target Directory
resources: directories

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objects
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Direct build of the test app with objects
$(MODNAME): $(OBJECTS)
	$(HBUTILS_CC) -o $(TARGETDIR)/$(MODNAME) $^ $(LIB)

#Compile Stages
$(BUILDDIR)/%.$(OBJEXT): ./%.$(SRCEXT)
	@mkdir -p $(dir $@)
ifneq (,$(findstring gcc,$(HBUTILS_CC)))
	$(HBUTILS_CC) $(HBUTILS_CFLAGS) $(HBUTILS_DEF) $(INC) -c -o $@ $<
	@$(HBUTILS_CC) $(HBUTILS_CFLAGS) $(HBUTILS_DEF) $(INCDEP) -MM ./$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp
else
	$(HBUTILS_CC) $(HBUTILS_CFLAGS) $(HBUTILS_DEF) $(INC) -c $(CCOUT)$@ $<
endif

#Non-File Targets
.PHONY: all remake clean cleaner resources

