/*========================================================
		Copyright and disclaimer level on game boot
========================================================*/
/*=================================
            Libraries
=================================*/

#include <nusys.h>
#include <nualstl_n.h>
#include "definitions.h"
#include "../../config.h"
#include "../sounds/definitions.h"

#include "../sprites/bac_solid.h"
#include "../sprites/fnt_text.h"
#include "../sprites/spr_pyoro.h"
#include "../sprites/spr_tongue.h"


/*=================================
             Prototypes
=================================*/

static void DrawPyoroDisclaimer();


/*=================================
         Global Variables
=================================*/

#define DISCLAIMER_MESSAGE "\
Pyoro64 is a fan made game.\n\
It is not licensed or endorsed by\n\
Nintendo.\n\
\nNo cartridges are for sale, nor\n\
is profit being made in any way.\n\n\
This game was made for the love\n\
of Pyoro and the Nintendo 64.\n\n\
You can support the official\n\
release by purchasing the DSiWare\n\
versions.\n\n\
Please Nintendon't sue.\n\n\n\n\
"

#define CONTROLLER_MISSING "\
No Controller detected.\nPlease turn \
off power to the\nControl Deck and \
insert a\nController.\
"

#define ERONGI { \
    189, 179, 185, 132, 183, 172, 179, 185, 176, 168, \
    178, 139, 184, 132, 184, 179, 185, 167, 172, 132, \
    110, 184, 172, 169, 132, 167, 165, 182, 184, 182, \
    173, 168, 171, 169, 132, 187, 172, 173, 176, 169, \
    132, 184, 172, 169, 132, 171, 165, 177, 169, 110, \
    173, 183, 132, 182, 185, 178, 178, 173, 178, 171, \
    133, 100 \
}

#define ESAELP { \
    68, 111, 32, 110, 111, 116, 32, 99, 104, \
    97, 110, 103, 101, 32, 116, 104, 101, 32, \
    118, 97, 108, 117, 101, 32, 111, 102, 32, \
    116, 104, 105, 115, 32, 88, 32, 112, 108, \
    101, 97, 115, 101, 33, 0 \
}

#define SKNAHT { \
    189, 179, 185, 132, 183, 172, 179, 185, 176, 168, \
    178, 139, 184, 132, 177, 179, 168, 173, 170, 189, \
    110, 182, 165, 178, 168, 179, 177, 132, 186, 165, \
    176, 185, 169, 183, 132, 173, 178, 110, 177, 169, \
    177, 179, 182, 189, 133, 100 \
}

// Stage related
static char text1_alpha;
static char text2_alpha;
static s8 message;
static OSTime timer;

// Pyoro related
static float pyoro_pos_x;
static float pyoro_pos_y;
static char pyoro_state;
static unsigned char* pyoro_sprite;
static OSTime pyoro_timer;

// Tongue related
static int tongue_x;
static int tongue_y;

static char memory[42];

u8 can;
u32 pol1;
u32 pol2;
u32 pol3;

/*=================================
       stage_disclaimer_init
       Initialize the stage.
=================================*/

void stage_disclaimer_init(void)
{
    int i;
    u32 pol0;
    char temp[] = ESAELP;
    strcpy(memory, temp);
    for (i=0; i<sizeof(temp);i++)
        temp[i] = 0;
        
    can = 0;
    text1_alpha = 0;
    text2_alpha = 0;
    message = 0;
    timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(2000000);
    pyoro_pos_x = SCREEN_WD+16;
    pyoro_pos_y = SCREEN_HT-24;
    pyoro_state = 0;
    pyoro_sprite = spr_pyoro_walk1;
    pyoro_timer = 0;
    tongue_x = 0;
    tongue_y = 0;
    pol1 = -1;
    pol2 = -1;
    pol3 = 0;
    
    osPiReadIo(0xB80002EC, &pol0);
    if (pol0 == 0x55444556)
        can = 1;
}


/*=================================
      stage_disclaimer_update
   Update variables and objects
=================================*/

void stage_disclaimer_update(void)
{
    // Read the controller data
    nuContDataGetEx(contdata, 0);
    
    // Check if controller 1 is plugged in
    if (!(CheckBit(contPattern,0)))
    {
        message = -1;
        return;
    }
    
    // Stuff
    if (can)
    {
        osPiReadIo(0xB80002F8, &pol1);
        if (pol2 == -1)
            pol2 = pol1;
        if (pol2 != pol1)
            pol3 = 1;
    }
    
    // Fade in the "Made by Buu342" text
    if (text1_alpha < 250)
        text1_alpha +=5;
    
    // Stage animation
    if (message == 0 && timer < osGetTime() && pyoro_state == 0) // Set pyoro to his first state if time elapsed
    {
        pyoro_state = 1;
        pyoro_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(75000);
    }
    else if (message == 1 && pyoro_state == 16 && contdata[0].trigger & A_BUTTON) // Make the warning text disappear once you hit A 
    {
        sndHandle = nuAuStlSndPlayerPlay(SND_GAME_START);
        message++;
        timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
    }
    else if (message == 2 && timer < osGetTime()) // Move to the menu
    {
        if (global_eeprom_loaded == 0)
            message++;
        else
            global_stage = STAGE_MENU;
    }
    else if (message == 3 && contdata[0].trigger & A_BUTTON)
        global_stage = STAGE_MENU;
    
    // Pyoro manipulation (Could be a switch statement but meh)
    if (pyoro_state == 1) // Moving to the left
    {
        pyoro_pos_x -= 1.5;
        if (pyoro_timer < osGetTime())
        {
            if (pyoro_sprite == spr_pyoro_walk1)
                pyoro_sprite = spr_pyoro_walk2;
            else
                pyoro_sprite = spr_pyoro_walk1;
            sndHandle = nuAuStlSndPlayerPlay(SND_PYORO_WALK);
            pyoro_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(75000);
        }
        
        // Go to the next state once we reach far enough
        if (pyoro_pos_x < (SCREEN_WD - 64))
        {
            pyoro_state++;
            pyoro_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(500000);
            pyoro_sprite = spr_pyoro_walk1;
        }
    }
    else if (pyoro_state == 2 && pyoro_timer < osGetTime()) // Wait for a bit and extend the tongue
    {
        pyoro_state++;
        pyoro_sprite = spr_pyoro_attack;
        sndHandle = nuAuStlSndPlayerPlay(SND_TONGUE_OUT);
        tongue_x = pyoro_pos_x-6;
        tongue_y = pyoro_pos_y-2;
    }
    else if (pyoro_state == 3) // Manipulate the tongue
    {
        tongue_x -= 2;
        tongue_y -= 2;
        
        // Check if the tongue hit the limit
        if (tongue_y <= SCREEN_HT/2)
        {
            pyoro_state++;
            message++;
            pyoro_timer = 0;
            nuAuStlSndPlayerSndStop(sndHandle, 1);
            sndHandle = nuAuStlSndPlayerPlay(SND_BEAN_GET);
        }
    }
    else if (pyoro_state == 4) // Move the tongue back
    {
        tongue_x += 8;
        tongue_y += 8;
        if (tongue_x >= pyoro_pos_x)
            pyoro_state++;
    }
    else if (pyoro_state == 5) // Start chewing
    {
        pyoro_sprite = spr_pyoro_chew2;
        pyoro_state++;
        pyoro_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(75000);
    }
    else if (pyoro_state >= 6 && pyoro_state < 13) // Chew for a bit
    {
        if (pyoro_timer < osGetTime())
        {
            pyoro_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(75000);
            pyoro_state++;
            if (pyoro_sprite == spr_pyoro_chew2)
                pyoro_sprite = spr_pyoro_walk1;
            else
                pyoro_sprite = spr_pyoro_chew2;
        }
    }
    else if (pyoro_state == 13) // Wait for a bit
    {
        pyoro_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(500000);
        pyoro_state++;
    }
    else if (pyoro_state == 14) // Once done waiting, go to the next state
    {
        if (pyoro_timer < osGetTime())
            pyoro_state++;
    }
    else if (pyoro_state == 15) // Walk back
    {
        pyoro_pos_x += 1.5;
        if (pyoro_timer < osGetTime())
        {
            if (pyoro_sprite == spr_pyoro_walk1)
                pyoro_sprite = spr_pyoro_walk2;
            else
                pyoro_sprite = spr_pyoro_walk1;
            sndHandle = nuAuStlSndPlayerPlay(SND_PYORO_WALK);
            pyoro_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(75000);
        }
        
        // Go to last state once outside the screen
        if (pyoro_pos_x > SCREEN_WD+16)
            pyoro_state++;
    }
}


/*=================================
       stage_disclaimer_draw
          Draw the stage
=================================*/

void stage_disclaimer_draw(void)
{
    // Assign our glist pointer to our glist array for ease of access
    glistp = glist;
    
    // Initialize the RCP
    RCPInit(glistp);

    // Wipe the background with a color
    ClearBackground(0, 0, 0);
    
    // Check if controller 1 is plugged in
    if (message == -1)
    {
        global_text_alpha = 255;
        DrawText(SCREEN_WD/2, SCREEN_HT/2, CONTROLLER_MISSING, TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
    }
        
    if (memory[32] != 88 || pol3)
    {
        char i, j;
        char text1[] = ERONGI;
        char text2[] = SKNAHT;
        char* text = text1;
        
        j = sizeof(text1);
        
        if (memory[32] != 88)
        {
            text = text2;
            j = sizeof(text2);
        }
            
        if (message > -2)
        {
            message = -2;
            timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(10000000);
            sndHandle = nuAuStlSndPlayerPlay(21);
        }
        
        if (timer < osGetTime())
        {
            message--;
            timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(10000000);
            nuAuStlSndPlayerSndStop(sndHandle, 1);
            sndHandle = nuAuStlSndPlayerPlay(21-(message+2));
        }
        for (i=0; i<j; i++)
            text[i] -=100;
        global_text_alpha = 255;
        if (message != -2 || (timer-osGetTime() < OS_USEC_TO_CYCLES_CORRECT(9100000)))
            global_text_sprite = spr_pyoro_walk1;
        
        DrawText(SCREEN_WD/2, SCREEN_HT/2, text, TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        global_text_sprite = fnt_text;
        
        if (message == -2 && timer-osGetTime() > OS_USEC_TO_CYCLES_CORRECT(9100000))
        {
            j = guRandom()%100;
            gDPSetRenderMode(glistp++,G_RM_NOOP,G_RM_NOOP2);
            gDPSetCycleType(glistp++, G_CYC_FILL);
            for (i=0; i<j; i++)
            {
                int x = guRandom()%SCREEN_WD;
                int y = guRandom()%SCREEN_HT;
                int w = guRandom()%5;
                int h = guRandom()%3;
                gDPSetFillColor(glistp++, (GPACK_RGBA5551(guRandom()%255, guRandom()%255, guRandom()%255, 1) << 16 |
                                           GPACK_RGBA5551(guRandom()%255, guRandom()%255, guRandom()%255, 1)));
                gDPFillRectangle(glistp++, x, y, x+w, y+h);
            }
            gDPPipeSync(glistp++);
        }
    }
    else
    {
        // Draw Pyoro
        DrawPyoroDisclaimer();    
        
        // Draw some text
        if (pyoro_state < 5)
        {
            int x = SCREEN_WD/2;
            int y = SCREEN_HT/2;
            
            if (pyoro_state == 4) // Manipulate the text position if the tongue is retracting
            {
                x = tongue_x;
                y = tongue_y;
            }
            global_text_alpha = text1_alpha;
            DrawText(x, y, "A game by Buu342", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        }
        
        // Draw the text if we haven't pressed A
        if (message > 0 && message < 2)
        {
            // Fade the text in if Pyoro caught the "Made by" text
            if (pyoro_state > 3 && text2_alpha < 250)
                text2_alpha +=5;
            
            global_text_alpha = text2_alpha;
            DrawText(SCREEN_WD/2, SCREEN_HT/2, DISCLAIMER_MESSAGE, TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            
            // Display the "Press A to continue" text if pyoro left the screen
            global_text_alpha = 255;
            if (pyoro_state == 16)
                DrawText(SCREEN_WD/2, SCREEN_HT-30, "Press \a to continue.", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        }
        
        // Warn if EEPROM was not found
        if (message == 3)
            DrawText(SCREEN_WD/2, SCREEN_HT/2, "EEPROM was not detected.\nThis game will be unable to save.\n\nPress \a to continue.", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
    }    
    // Syncronize the RCP and CPU
    gDPFullSync(glistp++);
    
    // Specify that our display list has ended
    gSPEndDisplayList(glistp++);
    
    // Start the display task
    nuGfxTaskStart(glist, (s32)(glistp - glist) * sizeof(Gfx), NU_GFX_UCODE_F3DEX, NU_SC_SWAPBUFFER);
}


/*=================================
        DrawPyoroDisclaimer         
     Draw Pyoro and his tongue
=================================*/

void DrawPyoroDisclaimer()
{
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    
    // Draw the tongue
    if (pyoro_state == 3 || pyoro_state == 4)
    {
        int xpos = pyoro_pos_x-6;
        int ypos = pyoro_pos_y-2;
        int drawx = tongue_x;
        int drawy = tongue_y;
        int i;
        
        // Draw tongue segments
        gDPLoadTLUT_pal16( glistp++, 0, spr_tongue_tlut_default);
        gDPLoadTextureTile_4b(glistp++, spr_tongue_body, G_IM_FMT_CI, 8, 8, 0, 0, 8, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        for (i=0;i<(((float)(xpos - tongue_x))/6);i++)
            gSPTextureRectangle(glistp++, xpos-(6*i)<<2, ypos-(6*i)<<2, 8+xpos-(6*i)<<2, 8+ypos-(6*i)<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
            
        // Draw the actual tongue
        gDPLoadTextureBlock_4b(glistp++, spr_tongue, G_IM_FMT_CI, 16, 16, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, tongue_x-8<<2, tongue_y-8<<2, tongue_x+8<<2, tongue_y+8<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );    
    }
    
    // Draw Pyoro
    gDPLoadTLUT_pal16( glistp++, 0, spr_pyoro_tlut_default);
    if (pyoro_state == 15) // Draw pyoro flipped
    {
        gDPLoadTextureBlock_4b(glistp++, pyoro_sprite, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_NOMIRROR, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, ((int)pyoro_pos_x-8)<<2, ((int)pyoro_pos_y-8)<<2, ((int)pyoro_pos_x+8)<<2, ((int)pyoro_pos_y+8)<<2, G_TX_RENDERTILE, 16 << 5, 0 << 5, 1 << 10, 1 << 10 );
    }    
    else
    {
        gDPLoadTextureBlock_4b(glistp++, pyoro_sprite, G_IM_FMT_CI, 16, 16, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, ((int)pyoro_pos_x-8)<<2, ((int)pyoro_pos_y-8)<<2, ((int)pyoro_pos_x+8)<<2, ((int)pyoro_pos_y+8)<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    }
    gDPPipeSync(glistp++);
    gDPSetTextureLUT( glistp++, G_TT_NONE );
}