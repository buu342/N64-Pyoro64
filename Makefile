include $(ROOT)/usr/include/make/PRdefs

N64KITDIR	= c:/nintendo/n64kit

TARGET		= pyoro
CODEFILES	= main.c graphic.c helper.c
OBJECTFILES = assets/include/sprites.o \
              assets/include/objects.o \
              assets/include/stages.o

DATAFILES   = 

############################################################################
# DO NOT CHANGE ANYTHING BELOW THIS LINE UNLESS YOU KNOW WHAT YOU'RE DOING #
############################################################################


OPTIMIZER	= -O2

NUSYSINCDIR  = $(N64KITDIR)/nusys/include
NUSYSLIBDIR  = $(N64KITDIR)/nusys/lib
NUSTDINCDIR  = $(N64KITDIR)/nustd/include
NUSTDLIBDIR  = $(N64KITDIR)/nustd/lib

LIB = $(ROOT)/usr/lib
LPR = $(LIB)/PR
INC = $(ROOT)/usr/include
CC  = gcc
LD  = ld
MAKEROM = mild

LCDEFS =	-DNDEBUG -D_FINALROM -DF3DEX_GBI_2
LCINCS =	-I. -I$(NUSYSINCDIR) -I$(NUSTDINCDIR) -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0
LDFLAGS = $(MKDEPOPT) -L$(LIB) -L$(NUSYSLIBDIR) -L$(NUSTDLIBDIR) $(NUAUDIOLIB) -lnustd_d -lnusys_d -lgultra_rom -L$(GCCDIR)/mipse/lib -lkmc

NUAUDIOLIB = -lnualstl_n_d -ln_gmus_d -ln_gaudio_sc
OPTIMIZER =	-g

CODESEGMENT	= codesegment.o
OBJECTS		= $(CODESEGMENT) $(DATAOBJECTS)
CODEOBJECTS = $(CODEFILES:.c=.o) $(OBJECTFILES) $(NUSYSLIBDIR)/nusys.o

OUT = $(TARGET).out
ROM	= $(TARGET).n64

default:$(ROM)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS) Makefile
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(ROM) :	$(OBJECTS)
		$(MAKEROM) spec -I$(NUSYSINCDIR) -r $(ROM) -e $(OUT) -s 16

# This space is needed or makefile errors