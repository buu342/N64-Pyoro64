N64KITDIR    = c:\nintendo\n64kit

CODEFILES = stage_disclaimer.c \
            stage_menu.c \
            stage_game1.c \
            stage_game2.c
            
OBJECTNAME = stages

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