EXE1	= control

#OS Version
ROOTCONFIG   := $(ROOTSYS)/bin/root-config
ROOTCINT     := $(ROOTSYS)/bin/rootcint
ARCH         := $(shell $(ROOTCONFIG) --arch)
PLATFORM     := $(shell $(ROOTCONFIG) --platform)

# Compiler version
CC        = g++
ifeq ($(ARCH),macosx64)
CC        = clang++ -stdlib=libc++
endif

# Primary directory
MAINDIR	= $(PWD)

# Directories
OBJ		= obj
DICT	= dict
LIB		= lib
BIN		= bin
SRC		= src
MAIN	= main

# Directory commands
RMDIRS	= @rm -rf $(OBJ); rm -rf $(DICT); rm -rf $(LIB); rm -rf $(BIN)
MKOBJ	= @mkdir -p $(OBJ)
MKDICT  = @mkdir -p $(DICT)
MKLIB   = @mkdir -p $(LIB)
MKBIN   = @mkdir -p $(BIN)

# Build Soft Links
#RMLINKS      = @rm mc; rm data;
#WORKAREA     = /afs/cern.ch/work/m/mamartin
#WORKAREA     = /afs/cern.ch/work/$(echo $(USER) | head -c 1)/$(USER)
#LNDIRS       = $(RMLINKS) ln -s $(WORKAREA)/D02KsPiPi/mc mc; ln -s $(WORKAREA)/D02KsPiPi/data data

# Include directories
FHH 	= $(MAINDIR)/$(SRC)
ROOTHH 	= $(ROOTSYS)/include

# ROOT library directory
ROOTSO	= $(ROOTSYS)/lib

#RooFit library
RFSO = $(ROOTSO)

# File lists
CCLIST	 = $(wildcard $(FHH)/*.cpp)
HHLIST	 = $(wildcard $(FHH)/*.h)
OBJLIST	 = $(notdir $(patsubst %.cpp,%.o,$(CCLIST)))
DICTLIST = $(notdir $(patsubst %.h,%Dict.o,$(HHLIST)))

# Files to copy
PCMFILES = $(patsubst %.pcm,$(DICT)/%.pcm,$(notdir $(patsubst %.cpp,%Dict_rdict.pcm,$(CCLIST))))

# Output name files
LIBNAME = RootUtils
LIBF	= $(MAINDIR)/$(LIB)/lib$(LIBNAME).so

# Compiler options
CCOPT   = -std=c++11 -O2
CCWARN	= -Wall -Wno-deprecated -Wno-write-strings -Wformat -Wsign-compare
CCINCL	= -I. -I$(FHH) `$(ROOTCONFIG) --cflags` -fPIC
CCLIBSO	= -L$(RFSO) `$(ROOTCONFIG) --libs` -lRooFitCore -lRooFit -lRooStats -lHtml -lMinuit -lThread -lTreePlayer -lFoam \
	-lProof -lProofBench -lProofDraw -lProofPlayer

ifeq ($(ARCH),macosx64)
# MacOS X with cc (GNU cc 2.95.2 and gcc 3.3)
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
ifeq ($(MACOSX_MINOR), $(filter $(MACOSX_MINOR), 6 7 8 9 10))
CCOPT	      += -m64
MACOSX_MINOR  = 4
endif
ifeq ($(MACOSX_MINOR),5)
MACOSX_MINOR  = 4
endif
UNDEFOPT      = dynamic_lookup
ifneq ($(MACOSX_MINOR),4)
ifneq ($(MACOSX_MINOR),3)
UNDEFOPT      = suppress
endif
endif
SOFLAGS       = -dynamiclib -single_module -undefined $(UNDEFOPT)
endif

ifneq ($(ARCH),macosx64)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxx8664gcc)
CCOPT	      += -m64
# CCLIBSO       += -lMathMore
endif

default: $(BIN)/$(EXE1) CPPCMFILES
#	$(LNDIRS)

$(DICT)/%Dict.cpp:
	@echo "Making $@"
	$(MKDICT)
	@$(ROOTCINT) -f $@ -c $(patsubst $(DICT)/%Dict.cpp,$(FHH)/%.h,$@)

$(DICT)/%Dict.o: $(DICT)/%Dict.cpp
	@echo "Compiling $@"
	$(MKDICT)
	@$(CC) $(CCOPT) $(CCWARN) $(CCINCL) -c $(patsubst %.o,%.cpp,$@) -o $@

$(OBJ)/%.o: $(FHH)/%.cpp $(FHH)/%.h
	@echo "Compiling $(notdir $(patsubst %.o,%.cpp,$@))"
	$(MKOBJ)
	@$(CC) $(CCOPT) $(CCWARN) $(CCINCL) -c $(FHH)/$(notdir $(patsubst %.o,%.cpp,$@)) -o $@

$(LIBF): $(addprefix $(OBJ)/, $(OBJLIST)) $(addprefix $(DICT)/, $(DICTLIST)) $(OBJ)/$(LIBNAME)_dict.o
	@echo "Make libraries"
	$(MKLIB)
	$(CC) $(CCOPT) $(CCWARN) $(addprefix $(OBJ)/, $(OBJLIST)) $(addprefix $(DICT)/, $(DICTLIST)) $(SOFLAGS) $(CCLIBSO) -o $(LIBF)
ifeq ($(ARCH),macosx)
	@rm -f $(patsubst %.so,%.dylib,$(LIBF))
	@ln -s $(LIBF) $(patsubst %.so,%.dylib,$(LIBF))

endif

$(OBJ)/$(LIBNAME)_dict.o: $(OBJ)/$(LIBNAME)_dict.cxx

$(OBJ)/$(LIBNAME)_dict.cxx: $(wildcard $(FHH)/*.h) $(DICT)/$(LIBNAME)_dict.h $(DICT)/$(LIBNAME)_LinkDef.h
	$(ROOTCINT) -f $@ -c -I$(SRC) -I$(DICT) $(DICT)/$(LIBNAME)_dict.h $(DICT)/$(LIBNAME)_LinkDef.h

$(BIN)/$(EXE1): $(MAIN)/$(EXE1).cc $(LIBF)
	@echo "Make exe1"
	$(MKBIN)
	$(CC) $(CCOPT) $(CCWARN) $(CCINCL) -L$(PWD)/$(LIB) $(CCLIBSO) -l$(LIBNAME) $(MAIN)/$(EXE1).cc -o $(BIN)/$(EXE1)

CPPCMFILES: $(PCMFILES)
	@echo Copying files $(PCMFILES) to $(LIB)/
	@cp $(PCMFILES) $(LIB)

cleanlib:
	rm -f $(LIBF)
	rm -f $(LIB)/*.pcm

ifeq ($(ARCH),macosx)
	rm -f $(patsubst %.so,%.dylib,$(LIBF))
endif

clean: cleanlib
#	$(RMLINKS)
	rm -rf $(BIN)/$(EXE1)
	rm -rf $(OBJ)
	rm -rf $(DICT)/*Dict*
	rm -f *~
	rm -f $(SRC)/*~
	rm -f $(MAIN)/*~

cleanall: clean
	$(RMDIRS)
#	$(RMLINKS)
	@echo "remove directories"
