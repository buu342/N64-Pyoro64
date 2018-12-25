N64KITDIR    = c:\nintendo\n64kit

CODEFILES = base.c pyoro.c pyoro2.c block.c bean.c puff.c tongue.c scores.c angel.c particle.c seeds.c puff_small.c leaf.c leaf_small.c
OBJECTNAME = objects

############################################################################
# DO NOT CHANGE ANYTHING BELOW THIS LINE UNLESS YOU KNOW WHAT YOU'RE DOING #
############################################################################

NUSYSINCDIR  = $(N64KITDIR)\nusys\include
NUSYSLIBDIR  = $(N64KITDIR)\nusys\lib
NUSTDINC     = $(N64KITDIR)\nustd\include
NUSTDLIB     = $(N64KITDIR)\nustd\lib

N64GCC_PATH =	$(ROOT)\gcc\mipse
N64GCC_BIN =	$(N64GCC_PATH)\bin
N64GCC_INC =	$(ROOT)\usr\include
USR_INC    =	..\include

CC =		$(N64GCC_BIN)\gcc
AS =		$(N64GCC_BIN)\as
LD =		$(N64GCC_BIN)\ld
MAKEROM =	mild

AUDIO_CDEFS = -DNUALSTL -DN_AUDIO
NUAUDIOLIB = -lnualstl_n -ln_numus -ln_gaudio_sc
GBI = -DF3DEX_GBI_2

CC_INC = -I. -I$(NUSTDINC) -I$(N64GCC_INC)\pr -I$(N64GCC_INC) -I$(NUSYSINCDIR) -I$(USR_INC)

default: 
	$(CC) -g -G 0 -c $(CC_INC) $(CC_DEF) $(CODEFILES)
    $(LD) -r $(CODEFILES:.c=.o) -o $(OBJECTNAME).o

CC_INC =	-I. -I$(NUSTDINC) -I$(N64GCC_INC)\pr -I$(N64GCC_INC) -I$(NUSYSINCDIR) -I$(USR_INC)