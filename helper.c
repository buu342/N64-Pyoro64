/*========================================================
				Useful/helpful functions
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include "config.h"

#include "assets/sprites/fnt_numbers_outlined.h"
#include "assets/sprites/fnt_numbers.h"
#include "assets/sprites/fnt_text.h"
#include "assets/sprites/fnt_text_outlined.h"


/*=================================
         Global Variables
=================================*/

unsigned char* global_text_sprite = fnt_text;
char global_text_alpha = 255;
unsigned short* global_text_tlut = fnt_text_tlut_white;


/*=================================
	     SetDefaultSave
      Reset the saved game
=================================*/

void SetDefaultSave()
{
    int i = 0;
    
    // 0 all the global values
    for (i=0;i<8;i++)
        global_save_magic[i] = 0;
    for (i=0;i<8;i++)
        global_save_unlocks[i] = 0;
    for (i=0;i<8;i++)
        global_save_settings[i] = 0;
    for (i=0;i<8;i++)
        global_save_highscores[i] = 0;
        
    // Set the magic number to specify that the EEPROM is saved correctly.
    global_save_magic[0] = 342/2;
    
    // Write them to EEPROM (for when the game is booted for the first time)
    if (global_eeprom_loaded == 1)
    {
        nuEepromWrite(SAVESLOT_MAGIC, global_save_magic, 1);
        nuEepromWrite(SAVESLOT_UNLOCKS, global_save_unlocks, 1);
        nuEepromWrite(SAVESLOT_SETTINGS, global_save_settings, 1);
        nuEepromWrite(SAVESLOT_HIGHSCORES, global_save_highscores, 1);
    }
}


/*=================================
	    	Rom2Ram
Read data from the ROM and transfer 
        it to the RAM.
=================================*/

void Rom2Ram(void *from_addr, void *to_addr, s32 seq_size)
{
    // Cannot transfer if size is an odd number, so make it even
    if(seq_size & 0x00000001) 
        seq_size++;

    nuPiReadRom((u32)from_addr, to_addr, seq_size);
}


/*=================================
          ClearBackground
 Wipe the background with a color
=================================*/

void ClearBackground(u8 r, u8 g, u8 b)
{
    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetDepthImage(glistp++, nuGfxZBuffer);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, nuGfxZBuffer);
    gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, nuGfxCfb_ptr);
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(r, g, b, 1) << 16 | GPACK_RGBA5551(r, g, b, 1)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPPipeSync(glistp++);
}


/*=================================
              DrawText
           Draw a string.
=================================*/

void DrawText(int x, int y, char* text, char halign, char valign)
{
    int i=0, j=0, posx=-1, posy=0;
    char newline_total = 0;
    char current_line = 0;
    int string_length = strlen(text);
    char TEXT_W = 8;
    char TEXT_H = 8;
    short TEXT_HPAD = 0;
    short TEXT_VPAD = 2;

    // Change some settings if using an outlined text
    if (global_text_sprite == fnt_text_outlined)
    {
        TEXT_H = 10;
        TEXT_VPAD = 0;
        TEXT_HPAD = 0;
    }
    
    // Count the number of newlines
    for (i=0;i<string_length;i++)
        if (text[i] == '\n')
            newline_total++;
            
            
    // Calculate the vertical alignment based on the number of newlines
    if (valign == TEXT_VALIGN_MIDDLE)
        posy -= (newline_total*TEXT_H)/2 + TEXT_H/2  + (TEXT_VPAD*newline_total)/2;
    else if (valign == TEXT_VALIGN_BOTTOM)
        posy -= (newline_total*TEXT_H) + TEXT_H + (TEXT_VPAD*newline_total);
    
    
    // Set drawing mode
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16 );
    gDPSetPrimDepth(glistp++, 10, 0);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, global_text_alpha);
    
    gDPLoadTLUT_pal16(glistp++, 0, global_text_tlut);
    
    
    // Draw the text
    for (i=0;i<string_length;i++)
    {
        char offsetx;
        float offsety;
        
        
        // If on a newline, calculate the horizontal alignment
        if (posx == -1)
        {
            posx = 0;
            if (halign != TEXT_HALIGN_LEFT)
            {
                char chars_to_newline = 0;
                char current_newline = 0;
                
                for (j=0;j<string_length;j++)
                {
                    if (current_newline == current_line)
                        break;
                    if (text[j] == '\n')
                        current_newline++;
                }
                
                for (;j<string_length;j++)
                {
                    if (text[j] == '\n')
                        break;
                    else
                        chars_to_newline++;
                }
                if (halign == TEXT_HALIGN_RIGHT)
                    posx -= (chars_to_newline*TEXT_W)+TEXT_HPAD*chars_to_newline;
                else
                    posx -= ((chars_to_newline)*TEXT_W)/2 + ((chars_to_newline)*TEXT_HPAD)/2;
            }
        }
        
        
        // If we found a new line, move the text position
        if (text[i] == '\n')
        {
            posy+=TEXT_H+TEXT_VPAD;
            posx=-1;
            current_line++;
            continue;
        }
        
        // Don't render if we're out of the screen
        if (x+posx+TEXT_W+TEXT_HPAD > SCREEN_WD)
            continue;
        
        // Offset the font sprite based on what character we're currently drawing.
        if (text[i] >= '0' && text[i] <= '9' && (global_text_sprite == fnt_text || global_text_sprite == fnt_text_outlined))
        {
            char * sprite;
            if (global_text_sprite == fnt_text)
                sprite = fnt_numbers;
            else
                sprite = fnt_numbers_outlined;
            offsety = text[i] - '0';
            gDPLoadTextureTile_4b(glistp++, sprite, G_IM_FMT_CI, TEXT_W, TEXT_H, 0, (int)(offsety*TEXT_H), TEXT_W, TEXT_H + (int)(offsety*TEXT_H), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, x+posx << 2, y+posy << 2, x+TEXT_W+posx << 2, y+posy+TEXT_H << 2,  G_TX_RENDERTILE,  0 << 5, (int)offsety*TEXT_H << 5,  1 << 10, 1 << 10);
        }
        else
        {
            if (text[i] >= 'a' && text[i] <= 'z')
                text[i] = text[i] + ('A' - 'a');
                
            if (text[i] >= 'A' && text[i] <= 'P')
            {
                offsetx = text[i] - 'A';
                offsety = 0;
            }
            else if (text[i] >= 'Q' && text[i] <= 'Z')
            {
                if (global_text_sprite == fnt_text_outlined)
                    offsety = 0.9;
                else
                    offsety = 0.875; // To fix a hardware rendering bug. Change to 1 to melt your brain
                offsetx = text[i] - 'A';
            }
            else
            {
                switch (text[i])
                {
                    case ',': offsety = 1; offsetx = 10; break;
                    case '.': offsety = 1; offsetx = 11; break;
                    case '!': offsety = 1; offsetx = 12; break;
                    case '?': offsety = 1; offsetx = 13; break;
                    case '\'': offsety = 1; offsetx = 14; break;
                    case '-': offsety = 1; offsetx = 15; break;
                    case ':': offsety = 2; offsetx = 0; break;
                    case '\a': offsety = 2; offsetx = 1; break;
                    case '[': offsety = 2; offsetx = 2; break;
                    case ']': offsety = 2; offsetx = 3; break;
                    case '(': offsety = 2; offsetx = 4; break;
                    case ')': offsety = 2; offsetx = 5; break;
                    case '{': offsety = 2; offsetx = 6; break;
                    case '}': offsety = 2; offsetx = 7; break;
                    default: posx+=TEXT_W+TEXT_HPAD; continue;
                }
            }
            
            gDPLoadTextureTile_4b(glistp++, global_text_sprite, G_IM_FMT_CI, 128, 32, offsetx*TEXT_W, (int)(offsety*TEXT_H), TEXT_W + (offsetx*TEXT_W), TEXT_H + (int)(offsety*TEXT_H), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, x+posx << 2, y+posy << 2, x+posx+TEXT_W << 2, y+posy+TEXT_H << 2,  G_TX_RENDERTILE,  offsetx*TEXT_W << 5, (int)(offsety*TEXT_H) << 5,  1 << 10, 1 << 10);
        }
        posx+=TEXT_W+TEXT_HPAD;
    }
    gDPPipeSync(glistp++);
}


/*=================================
                max
     Return the largest number
=================================*/

int max(int a, int b)
{
    return ((a)>(b) ? a:b);
}


/*=================================
             ceilfloor
     Floor if < 0, Ceil if > 0
=================================*/

float ceilfloor(float n)
{
    // ceilf/floorf cause crashes on hardware for some reason...
    if (n >= 0)
        n = ceil(n);
    else
        n = floor(n);
    return n;
}


/*=================================
          tlut_transition
    Recolor a given TLUT slowly
=================================*/

void tlut_transition(u16 *change, u16 *changeto, char num)
{
    int i;
    for (i=0;i<num;i++)
    {
        // Read the color I want changed
        char change_r = (change[i] >> 11) & 0x1F;
        char change_g = (change[i] >> 6) & 0x1F;
        char change_b = (change[i] >> 1) & 0x1F;
        char change_a = change[i] & 1;
        
        // Read the color I want to change to
        char changeto_r = (changeto[i] >> 11) & 0x1F;
        char changeto_g = (changeto[i] >> 6) & 0x1F;
        char changeto_b = (changeto[i] >> 1) & 0x1F;
        
        // Calculate the change
        if (change_r != changeto_r) change_r += (change_r < changeto_r ? 1 : -1);
        if (change_g != changeto_g) change_g += (change_g < changeto_g ? 1 : -1);
        if (change_b != changeto_b) change_b += (change_b < changeto_b ? 1 : -1);

        // Apply the change
        change[i] = (change_r << 11) | (change_g << 6) | (change_b << 1) | change_a;
    }
}


/*=================================
             tlut_copy
Make a copy of the TLUT so we don't
      edit the array directly
=================================*/

unsigned short* tlut_copy(unsigned short* src, int len)
{
    int i;
    unsigned short* copy = (unsigned short *) malloc(len * sizeof(unsigned short));
    
    // Return NULL if we failed to allocate memory
    if (copy == NULL)
        return NULL;
    
    // Copy the TLUT into our allocated memory
    for(i=0;i<len;i++)
        copy[i] = src[i];
        
    // Return the value so that we can use it
    return copy;
}


/*=================================
            tlut_edit
Change a specific color in the TLUT
=================================*/

void tlut_edit(u16 *tlut, u16 change, u16 changeto, char num)
{
    int i;
    
    // Search the TLUT for the single value we're interested in
    for (i=0;i<num;i++)
    {
        if (tlut[i] == change)
        {
            tlut[i] = changeto;
            break;
        }
    }
}


/*=================================
         PlayerIsAttacking
Return 1 or 0 if the player pressed
 the attack button (checks config)
=================================*/

char PlayerIsAttacking(char player, char holding)
{
    char value;
    char check1;
    char check2;
    char check3;
    char check4;
    
    // Set the offset for our checking variables depending on what player called this function
    if (player == 0)
    {
        value = global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C;
        check1 = 0x00;
        check2 = 0x04;
        check3 = 0x08;
        check4 = 0x0C;
    }
    else
    {
        value = global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0;
        check1 = 0x00;
        check2 = 0x40;
        check3 = 0x80;
        check4 = 0xC0;
    }
    
    // Check if the player is attacking based on his control settings
    if (holding == 0)
    {
        if (value == check1) 
        {
            if (contdata[player].trigger & A_BUTTON)
                return 1;
            else
                return 0;
        }
        if (value == check2) return (contdata[player].trigger & B_BUTTON) ? 1 : 0;
        if (value == check3) return (contdata[player].trigger & Z_TRIG) ? 1 : 0;
        if (value == check4) return ((contdata[player].trigger & L_TRIG) || (contdata[player].trigger & R_TRIG)) ? 1 : 0;
    }
    else
    {
        if (value == check1) return (contdata[player].button & A_BUTTON) ? 1 : 0;
        if (value == check2) return (contdata[player].button & B_BUTTON) ? 1 : 0;
        if (value == check3) return (contdata[player].button & Z_TRIG) ? 1 : 0;
        if (value == check4) return ((contdata[player].button & L_TRIG) || (contdata[player].button & R_TRIG)) ? 1 : 0;
    }
    
    return 0;
}


/*=================================
     Game balance calculation
=================================*/

int getPt(int n1 , int n2 , float perc)
{
    int diff = n2 - n1;

    return n1 + ( diff * perc );
}

int getBezierY(int x, int y1, int y2, int y3, int y4)
{
    float i = ((float)x)/100;
    
    // The helper lines
    int ya = getPt( y1 , y2 , i );
    int yb = getPt( y2 , y3 , i );
    int yc = getPt( y3 , y4 , i );

    // The curve lines
    int ym = getPt( ya , yb , i );
    int yn = getPt( yb , yc , i );

    return getPt( ym , yn , i );
}