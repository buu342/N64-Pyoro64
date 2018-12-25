N64KITDIR    = c:\nintendo\n64kit

CODEFILES = bac_bordercorner.c \
            bac_bridge.c \
            bac_castle.c \
            bac_city.c \
            bac_city2.c \
            bac_clock.c \
            bac_comet.c \
            bac_eiffel.c \
            bac_giant.c \
            bac_hiscore.c \
            bac_house.c \
            bac_house_alien.c \
            bac_lamp.c \
            bac_menu_back.c \
            bac_moon.c \
            bac_moon2.c \
            bac_mountains.c \
            bac_pause.c \
            bac_pause_continue.c \
            bac_pause_selection.c \
            bac_pause_quit.c \
            bac_planet.c \
            bac_planet2.c \
            bac_score.c \
            bac_skyscraper.c \
            bac_skyscraper_alien.c \
            bac_solid.c \
            bac_ufo.c \
            bac_ufo2.c \
            bac_wheel.c \
            bac_wind.c \
            fnt_numbers.c \
            fnt_text.c \
            fnt_numbers_outlined.c \
            fnt_text_outlined.c \
            spr_angel.c \
            spr_bean.c \
            spr_bean_leaf.c \
            spr_block.c \
            spr_buttonicon_1.c \
            spr_buttonicon_2.c \
            spr_logo.c \
            spr_logo_eyes.c \
            spr_menu_button.c \
            spr_menu_button_select.c \
            spr_menu_button_unlock.c \
            spr_menu_cursor.c \
            spr_menu_slider.c \
            spr_mouth.c \
            spr_pyoro.c \
            spr_pyoro_accessories.c \
            spr_puff.c \
            spr_scores.c \
            spr_tongue.c \
            spr_twirl.c
            
OBJECTNAME = sprites

############################################################################
# DO NOT CHANGE ANYTHING BELOW THIS LINE UNLESS YOU KNOW WHAT YOU'RE DOING #
############################################################################

NUSYSINCDIR  = $(N64KITDIR)\nusys\include
NUSYSLIBDIR  = $(N64KITDIR)\nusys\lib
NUSTDINCDIR  = $(N64KITDIR)\nustd\include
NUSTDLIBDIR  = $(N64KITDIR)\nustd\lib

N64GCC_PATH  =	$(ROOT)\gcc\mipse
N64GCC_BIN   =	$(N64GCC_PATH)\bin
N64GCC_INC   =	$(ROOT)\usr\include
USR_INC      =	..\include

CC =		$(N64GCC_BIN)\gcc
AS =		$(N64GCC_BIN)\as
LD =		$(N64GCC_BIN)\ld
MAKEROM =	mild

AUDIO_CDEFS = -DNUALSTL -DN_AUDIO
NUAUDIOLIB = -lnualstl_n -ln_numus -ln_gaudio_sc
GBI = -DF3DEX_GBI_2

CC_INC = -I. -I$(NUSTDINCDIR) -I$(N64GCC_INC)\pr -I$(N64GCC_INC) -I$(NUSYSINCDIR) -I$(USR_INC)

default: 
	$(CC) -g -G 0 -c $(CC_INC) $(CC_DEF) $(CODEFILES)
    $(LD) -r $(CODEFILES:.c=.o) -o $(OBJECTNAME).o

CC_INC =	-I. -I$(NUSTDINCDIR) -I$(N64GCC_INC)\pr -I$(N64GCC_INC) -I$(NUSYSINCDIR) -I$(USR_INC)