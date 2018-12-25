/*========================================================
                    Pyoro gameplay level
========================================================*/
/*=================================
            Libraries
=================================*/

#include <nusys.h>
#include <nualstl_n.h>
#include "definitions.h"
#include "../../config.h"
#include "../sounds/definitions.h"
#include "../music/definitions.h"
#include "../objects/base.h"

// GUI elements
#include "../sprites/bac_solid.h"
#include "../sprites/fnt_text_outlined.h"
#include "../sprites/fnt_numbers_outlined.h"
#include "../sprites/fnt_numbers.h"
#include "../sprites/fnt_text.h"
#include "../sprites/bac_score.h"
#include "../sprites/bac_hiscore.h"
#include "../sprites/bac_bordercorner.h"
#include "../sprites/bac_pause.h"
#include "../sprites/bac_pause_continue.h"
#include "../sprites/bac_pause_selection.h"
#include "../sprites/bac_pause_quit.h"

// Background elements
#include "../sprites/bac_stars.h"
#include "../sprites/bac_mountains.h"
#include "../sprites/bac_lamp.h"
#include "../sprites/bac_house.h"
#include "../sprites/bac_wheel.h"
#include "../sprites/bac_bridge.h"
#include "../sprites/bac_eiffel.h"
#include "../sprites/bac_skyscraper.h"
#include "../sprites/bac_ufo.h"
#include "../sprites/bac_moon.h"
#include "../sprites/bac_planet.h"
#include "../sprites/bac_city.h"
#include "../sprites/bac_comet.h"


/*=================================
             Prototypes
=================================*/

typedef struct color
{
    char r;
    char g;
    char b;
}Color;

static void DrawGradient(int x1, int y1, int x2, int y2, Color col1, Color col2);
static void DrawLandscape();
static void DrawHUD();


/*=================================
         Global Variables
=================================*/

// Game related variables
static OSTime nextspawn_timer;
static OSTime nextspeed_timer;
static OSTime particle_timer;
static char game_over;
static char paused;
static char fademode;
static char fade_alpha;
static char pause_select;
static OSTime pause_time;

// Flashing bean special effects
int bean3_destruction_y;
char bean3_destruction_player;
char bean3_spawnangels;
OSTime bean3_nextangel;

// Landscape elements alpha
static char bac_elem1_alpha;
static char bac_elem2_alpha;
static char bac_elem3_alpha;
static char bac_elem4_alpha;
static char bac_elem5_alpha;
static char bac_elem6_alpha;
static char bac_elem7_alpha;
static char bac_elem8_alpha;
static char bac_elem9_alpha;
static char bac_elem10_alpha;
static char bac_elem11_alpha;
static char bac_elem12_alpha;

// Landscape elements colors
static Color col1, col2;
static unsigned short *bac_elem1_tlut;
static unsigned short *bac_elem2_tlut;
static unsigned short *bac_elem3_tlut;
static unsigned short *bac_elem4_tlut;
static unsigned short *bac_elem5_tlut;
static unsigned short *bac_elem6_tlut;
static unsigned short *bac_elem7_tlut;
static unsigned short *bac_elem8_tlut;
static unsigned short *bac_elem9_tlut;
static unsigned short *bac_elem10_tlut;
static unsigned short *bac_elem11_tlut;
static u16 lights[8] = {
    0xFFFF,
    0x003F,
    0x07C1,
    0xF83F,
    0x07FF,
    0x0003,
    0xFFC1,
    0xF801
};

// Debug Variables
#if DEBUG_MODE == 1
    Gfx * glistbefore;
    Gfx * glistafter;
#endif

// Extra
static char extra;
static int comet_x;
static int comet_y;


/*=================================
          stage_game1_init
        Initialize the stage
=================================*/

void stage_game1_init(void)
{
    int i;
    
    // Set the game settings
    GameScore = 0;
    GameScore1 = 0;
    GameScore2 = 0;
    GameSpeed = 0;
    
    // Set the background gradient
    col1.r = 48;
    col1.g = 24;
    col1.b = 144;
    
    col2.r = 248;
    col2.g = 224;
    col2.b = 104;
    
    // Make a copy of the TLUT's so we're not modifying the actual values
    bac_elem1_tlut = tlut_copy(bac_lamp_tlut_day, 3);
    bac_elem2_tlut = tlut_copy(bac_mountains_tlut_day, 7);
    bac_elem3_tlut = tlut_copy(bac_house_tlut_day, 3);
    bac_elem4_tlut = tlut_copy(bac_wheel_tlut_day, 3);
    bac_elem5_tlut = tlut_copy(bac_bridge_tlut_sunset, 4);
    bac_elem6_tlut = tlut_copy(bac_eiffel_tlut_sunset, 3);
    bac_elem7_tlut = tlut_copy(bac_skyscraper_tlut_sunset, 3);
    bac_elem8_tlut = tlut_copy(bac_ufo_tlut_sunset, 3);
    bac_elem9_tlut = tlut_copy(bac_moon_tlut_night, 2);
    bac_elem10_tlut = tlut_copy(bac_planet_tlut_night, 3);
    bac_elem11_tlut = tlut_copy(bac_city_tlut_night, 4);
    
    /* 
        Originally, I wanted to have each part as a separate channel, but such a thing won't be possible
        without writing my own custom functions for the audio library. I had done this originally,
        creating a MusHandleSetChannelVolume function, however the way that instruments are assigned
        in each channel in the binary file is not consistent with what the Sound Tools show, so as a
        result, randomly, specific instruments would be muted for a while, then swap with another. While
        having 3 sequences playing is inefficient and annoying, it works, and is shy of the 4 song limit.
    */
    Rom2Ram((void *)SNG_MAIN1_1_START, (void *)tune1_buf, SNG_MAIN1_1_END-SNG_MAIN1_1_START);
    Rom2Ram((void *)SNG_MAIN1_2_START, (void *)tune2_buf, SNG_MAIN1_2_END-SNG_MAIN1_2_START);
    Rom2Ram((void *)SNG_MAIN1_3_START, (void *)tune3_buf, SNG_MAIN1_3_END-SNG_MAIN1_3_START);
    
    // Set the song's sample bank and play each part
    MusPtrBankSetSingle(ptr2_buf);
    seqHandle1 = MusStartSong(tune1_buf);
    MusPtrBankSetSingle(ptr2_buf);
    seqHandle2 = MusStartSong(tune2_buf);
    MusPtrBankSetSingle(ptr2_buf);
    seqHandle3 = MusStartSong(tune3_buf);
    
    // Set the volume to half for the first song, then 0 for the others
    MusHandleSetVolume(seqHandle1, 48);
    MusHandleSetVolume(seqHandle2, 0);
    MusHandleSetVolume(seqHandle3, 0);

    // Create our player(s) and the floor below
    if (global_save_settings[SAVE_SETTINGS_PLAYER] == 1)
    {
        int index = instance_create(OBJ_PYORO, 160+16, 189); // Player 2
        char value = 1;
        instance_create(OBJ_PYORO, 160-16, 189); // Player 1
        if (index != -1)
            instance_set(index, SET_PLAYER, &value);
    }
    else
        instance_create(OBJ_PYORO, 160, 189);
        
    for (i=0;i<=28;i++)
        instance_create(OBJ_BLOCK, 49+8*i, 201);
    
    // Set other variables
    nextspawn_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(2000000);
    nextspeed_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(5000000);
    particle_timer = 0;
    
    pause_select = 0;
    game_over = 0;
    paused = 0;
    fademode = FADE_OUT;
    fade_alpha = 255;
    pause_time = 0;
    
    bean3_destruction_y = 0;
    bean3_destruction_player = 0;
    bean3_spawnangels = 0;
    bean3_nextangel = 0;
    
    bac_elem1_alpha = 0;
    bac_elem2_alpha = 0;
    bac_elem3_alpha = 0;
    bac_elem4_alpha = 0;
    bac_elem5_alpha = 0;
    bac_elem6_alpha = 0;
    bac_elem7_alpha = 0;
    bac_elem8_alpha = 0;
    bac_elem9_alpha = 0;
    bac_elem10_alpha = 0;
    bac_elem11_alpha = 0;
    bac_elem12_alpha = 0;
    
    extra = 0;
    comet_x = 0;
    comet_y = 0;
}


/*=================================
        stage_game1_update
   Update variables and objects
=================================*/

void stage_game1_update(void)
{
    int score_before = GameScore;
        
    if (global_save_settings[SAVE_SETTINGS_PLAYER] == 1)
        GameScore = GameScore1+GameScore2;
    
    // Get controller input
    nuContDataGetExAll(contdata);
    
    // Pause the game if START is pressed
    if (!game_over && (((contdata[0].trigger & START_BUTTON || (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && contdata[1].trigger & START_BUTTON)) && game_over == 0) || (paused && pause_select == 0 && (contdata[0].trigger & A_BUTTON || (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && contdata[1].trigger & A_BUTTON)))))
    {
        paused = !paused;
        pause_select = 0;
        if (paused)
        {
            pause_time = osGetTime();
            MusHandleSetVolume(seqHandle1, 0);
            MusHandleSetVolume(seqHandle2, 0);
            MusHandleSetVolume(seqHandle3, 0);
            nuAuStlSndPlayerStop(0);
            sndHandle = nuAuStlSndPlayerPlay(SND_PAUSE);
        }
        else
        {
            MusHandleUnPause(seqHandle1);
            MusHandleUnPause(seqHandle2);
            MusHandleUnPause(seqHandle3);
            MusHandleSetVolume(seqHandle1, 48);
            if (GameScore >= SUNSET_MODE)
                MusHandleSetVolume(seqHandle2, 48);
            if (GameScore >= NIGHT_MODE)
                MusHandleSetVolume(seqHandle3, 48);
            sndHandle = nuAuStlSndPlayerPlay(SND_UNPAUSE);
            osSetTime(pause_time);
        }
        return;
    }
    
    // If the screen is fully black and the game is over, move to the menu
    if (game_over && fade_alpha == 255)
    {
        instance_destroy_all();
        GameSpeed = 0;
        free(bac_elem1_tlut);
        free(bac_elem2_tlut);
        free(bac_elem3_tlut);
        free(bac_elem4_tlut);
        free(bac_elem5_tlut);
        free(bac_elem6_tlut);
        free(bac_elem7_tlut);
        free(bac_elem8_tlut);
        free(bac_elem9_tlut);
        free(bac_elem10_tlut);
        free(bac_elem11_tlut);
        global_stage = STAGE_MENU;
        return;
    }
    
    // Stop any code from executing if paused, and reset the game time
    if (paused)
    {
        // This is separate from SetVolume because of a bug where the held note would still be audible
        // Nintendo pls fix
        MusHandlePause(seqHandle1);
        MusHandlePause(seqHandle2);
        MusHandlePause(seqHandle3);
        if (!game_over)
        {
            if ((contdata[0].trigger & R_CBUTTONS || contdata[0].trigger & R_JPAD || contdata[0].stick_x > 16) || (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && (contdata[1].trigger & R_CBUTTONS || contdata[1].trigger & R_JPAD || contdata[1].stick_x > 16)))
                pause_select = 1;
            else if ((contdata[0].trigger & L_CBUTTONS || contdata[0].trigger & L_JPAD || contdata[0].stick_x < -16) || (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && (contdata[1].trigger & L_CBUTTONS || contdata[1].trigger & L_JPAD || contdata[1].stick_x < -16)))
                pause_select = 0;
            else if ((pause_select == 1 && (contdata[0].trigger & A_BUTTON || (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && contdata[1].trigger & A_BUTTON))))
            {
                osSetTime(pause_time);
                paused = 0;
                game_over = 1;
                fademode = FADE_IN;
                nuAuStlSndPlayerStop(0);
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_BACK);
                returnmode = RETURN_FADE;
            }
        }
        return;
    }
    else if (pause_select == 1)
    {
        /* 
            This had to be done to prevent a bug where sound effects
            would no longer play once the stage changed.
            (Because I was pausing and stopping a sound handle in
            the same frame).
        */
        MusHandleStop(seqHandle1,1);
        MusHandleStop(seqHandle2,1);
        MusHandleStop(seqHandle3,1);
        return;
    }
    // Update all the objects in the game
    instance_update_all();
    
    // Make sure our score didn't go over the limit
    if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0 && GameScore > 999999)
        GameScore = 999999;
    
    // Play a jingle if Pyoro is dead
    if (instance_find(OBJ_PYORO) == -1 && game_over == 0)
    {
        u32 hscore = (global_save_highscores[3] << 24) | (global_save_highscores[2] << 16) | (global_save_highscores[1] << 8) | global_save_highscores[0];
        game_over = 1;
        Rom2Ram((void *)SNG_GAMEOVER_START, (void *)tune1_buf, SNG_GAMEOVER_END-SNG_GAMEOVER_START);
        MusPtrBankSetSingle(ptr2_buf);
        seqHandle1 = MusStartSong(tune1_buf);   
        MusHandleSetVolume(seqHandle1, 48);
        
        // Save unlocks and score if in singleplayer
        if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0)
        {
            // Store the highscore
            if (GameScore > hscore)
            {
                global_save_highscores[3] = (GameScore >> 24) & 0xFF;
                global_save_highscores[2] = (GameScore >> 16) & 0xFF;
                global_save_highscores[1] = (GameScore >> 8) & 0xFF;
                global_save_highscores[0] = GameScore & 0xFF;
                if (global_eeprom_loaded == 1)
                    nuEepromWrite(SAVESLOT_HIGHSCORES, global_save_highscores, 1);
            }
            
            // Unlock pyoro2 if we reached 10000 points
            if (GameScore >= 10000 && global_save_unlocks[SAVE_UNLOCK_PYORO2] == 0)
            {
                global_save_unlocks[SAVE_UNLOCK_PYORO2] = 1;
                if (global_eeprom_loaded == 1)
                    nuEepromWrite(SAVESLOT_UNLOCKS, global_save_unlocks, 1);
            }
        }
    }
    
    // Fade the screen out when any button is pressed during game over
    if (game_over && fademode != FADE_IN && ((contdata[0].trigger & START_BUTTON || contdata[0].trigger & A_BUTTON || contdata[0].trigger & B_BUTTON) || (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && (contdata[1].trigger & START_BUTTON || contdata[1].trigger & A_BUTTON || contdata[1].trigger & B_BUTTON))))
    {
        fademode = FADE_IN;
        MusHandleStop(seqHandle1,0);
        sndHandle = nuAuStlSndPlayerPlay(SND_MENU_BACK);
        
        // Choose how to fade the main menu when we return
        if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0)
            returnmode = RETURN_GAME1;
        else
            returnmode = RETURN_FADE;
    }
    
    // Move the destruction hitbox after catching a flashing bean
    if (bean3_destruction_y < 48)
        bean3_destruction_y = 0;
    else
        bean3_destruction_y -=3;
    
    // Spawn a lot of angels if we caught a flashing bean recently
    if ((bean3_spawnangels == 0 && bean3_nextangel == 0 && bean3_destruction_y != 0) || (bean3_nextangel < osGetTime() && bean3_nextangel != 0))
    {
        char i=0;
        int nearest_x = -1;
        float distance_x = -1;
        
        // Calculate what block to place
        for (i=0;i<=28;i++)
        {
            if (instance_collision_point(OBJ_BLOCK, 49+8*i, 201) == -1)
            {
                int index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &bean3_destruction_player);
                float pyoro_pos;
                int xpos = 49+8*i;
                
                if (index == -1)
                    break;
                    
                pyoro_pos = *(float*)instance_get(index, GET_X);
                
                if (pyoro_pos == -1)
                    break;
                    
                if (instance_collision_point(OBJ_ANGEL, xpos, 10) == -1 && (nearest_x == -1 || distance_x > (pyoro_pos - xpos)*(pyoro_pos - xpos)))
                {
                    nearest_x = xpos;
                    distance_x = (pyoro_pos - xpos)*(pyoro_pos - xpos);
                }
            }
        }
        
        // Increment the counter which keeps track of how many angels spawned
        if (nearest_x != -1 && bean3_spawnangels < 10)
        {
            int ent = instance_create(OBJ_ANGEL, nearest_x, 28);
            if (ent != -1)
            {
                sndHandle = nuAuStlSndPlayerPlay(SND_ANGEL_DOWN);
                instance_set(ent, SET_PITCH, &bean3_spawnangels);
            }
            bean3_nextangel = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(250000-((float)GameSpeed/100)*50000);
            bean3_spawnangels++;
        }
        else
        {
            bean3_nextangel = 0;
            bean3_spawnangels = 0;
        }
    }
    
    // Change the sky and music with the score
    if (GameScore >= DISCO_MODE)
    {
        if (particle_timer < osGetTime())
        {
            particle_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(10000);
            if (guRandom()%5 == 0)
                instance_create(OBJ_PARTICLE, 48 + (guRandom()%224), 163 + (guRandom()%33) );     
        }
    }
    if (GameScore >= LIGHT_MODE)
    {
        // Sync it to the beans spawning
        if (nextspawn_timer < osGetTime())
        {
            tlut_edit(bac_elem1_tlut, lights[extra], lights[(extra+1)%8], 3);
            tlut_edit(bac_elem3_tlut, lights[extra], lights[(extra+1)%8], 3);
            tlut_edit(bac_elem4_tlut, lights[extra], lights[(extra+1)%8], 3);
            tlut_edit(bac_elem5_tlut, lights[extra], lights[(extra+1)%8], 4);
            tlut_edit(bac_elem6_tlut, lights[extra], lights[(extra+1)%8], 3);
            tlut_edit(bac_elem7_tlut, lights[extra], lights[(extra+1)%8], 3);
            tlut_edit(bac_elem8_tlut, lights[extra], lights[(extra+1)%8], 3);
            tlut_edit(bac_elem10_tlut, lights[extra], lights[(extra+1)%8], 3);
            tlut_edit(bac_elem11_tlut, lights[extra], lights[(extra+1)%8], 4);
            extra = (extra+1)%8;
        }
    }
    else if (GameScore >= DARK_MODE)
    {
        char col1_r_to = 0;
        char col1_g_to = 0;
        char col1_b_to = 0;
        char col2_r_to = 0;
        char col2_g_to = 0;
        char col2_b_to = 0;
        if (GameSpeed < 90)
            GameSpeed = 90;
            
        col1.r = col1.r + (((float)(col1_r_to - col1.r))/((float)30));
        col1.g = col1.g + (((float)(col1_g_to - col1.g))/((float)30));
        col1.b = col1.b + (((float)(col1_b_to - col1.b))/((float)30));
        col2.r = col2.r + (((float)(col2_r_to - col2.r))/((float)30));
        col2.g = col2.g + (((float)(col2_g_to - col2.g))/((float)30));
        col2.b = col2.b + (((float)(col2_b_to - col2.b))/((float)30));
        
        tlut_transition(bac_elem2_tlut, bac_mountains_tlut_dark, 7);
        tlut_transition(bac_elem4_tlut, bac_wheel_tlut_dark, 3);
        tlut_transition(bac_elem5_tlut, bac_bridge_tlut_dark, 4);
        tlut_transition(bac_elem6_tlut, bac_eiffel_tlut_dark, 3);
        tlut_transition(bac_elem7_tlut, bac_skyscraper_tlut_dark, 3);
        tlut_transition(bac_elem8_tlut, bac_ufo_tlut_dark, 3);
        tlut_transition(bac_elem10_tlut, bac_planet_tlut_dark, 3);
        tlut_transition(bac_elem11_tlut, bac_city_tlut_dark, 4);
        
        if (extra == 1)
        {
            extra = 0;
            MusStop(MUSFLAG_SONGS,0);
            Rom2Ram((void *)SNG_MAIN3_START, (void *)tune1_buf, SNG_MAIN3_END-SNG_MAIN3_START);
            MusPtrBankSetSingle(ptr2_buf);
            seqHandle1 = MusStartSong(tune1_buf);
            MusHandleSetVolume(seqHandle1, 54);
        }
    }
    else if (GameScore >= BEIGE_MODE)
    {
        char col1_r_to = 176;
        char col1_g_to = 160;
        char col1_b_to = 128;
        char col2_r_to = 248;
        char col2_g_to = 248;
        char col2_b_to = 248;
        MusHandleSetTempo(seqHandle1, 0x80);
        if (GameSpeed < 80)
            GameSpeed = 80;
        
        col1.r = col1.r + (((float)(col1_r_to - col1.r))/((float)30));
        col1.g = col1.g + (((float)(col1_g_to - col1.g))/((float)30));
        col1.b = col1.b + (((float)(col1_b_to - col1.b))/((float)30));
        col2.r = col2.r + (((float)(col2_r_to - col2.r))/((float)30));
        col2.g = col2.g + (((float)(col2_g_to - col2.g))/((float)30));
        col2.b = col2.b + (((float)(col2_b_to - col2.b))/((float)30));
        
        tlut_transition(bac_elem1_tlut, bac_lamp_tlut_beige, 3);
        tlut_transition(bac_elem2_tlut, bac_mountains_tlut_beige, 7);
        tlut_transition(bac_elem3_tlut, bac_house_tlut_beige, 3);
        tlut_transition(bac_elem4_tlut, bac_wheel_tlut_beige, 3);
        tlut_transition(bac_elem5_tlut, bac_bridge_tlut_beige, 4);
        tlut_transition(bac_elem6_tlut, bac_eiffel_tlut_beige, 3);
        tlut_transition(bac_elem7_tlut, bac_skyscraper_tlut_beige, 3);
        tlut_transition(bac_elem8_tlut, bac_ufo_tlut_beige, 3);
        tlut_transition(bac_elem9_tlut, bac_moon_tlut_beige, 2);
        tlut_transition(bac_elem10_tlut, bac_planet_tlut_beige, 3);
        tlut_transition(bac_elem11_tlut, bac_city_tlut_beige, 4);
        
        if (extra == 0)
        {
            extra = 1;
            MusStop(MUSFLAG_SONGS,0);
            Rom2Ram((void *)SNG_MAIN2_START, (void *)tune1_buf, SNG_MAIN2_END-SNG_MAIN2_START);
            MusPtrBankSetSingle(ptr2_buf);
            seqHandle1 = MusStartSong(tune1_buf);
            MusHandleSetVolume(seqHandle1, 54);
        }
    }
    else if (GameScore >= NIGHT_MODE)
    {
        char col1_r_to = 0;
        char col1_g_to = 0;
        char col1_b_to = 0;
        char col2_r_to = 48;
        char col2_g_to = 24;
        char col2_b_to = 144;
        if (extra == 1)
        {
            extra = 0;
            MusHandleSetVolume(seqHandle3, 48);
        }
        if (GameSpeed < 65)
            GameSpeed = 65;
        tlut_transition(bac_elem4_tlut, bac_wheel_tlut_night, 3);
        tlut_transition(bac_elem5_tlut, bac_bridge_tlut_night, 4);
        tlut_transition(bac_elem6_tlut, bac_eiffel_tlut_night, 3);
        tlut_transition(bac_elem7_tlut, bac_skyscraper_tlut_night, 3);
        tlut_transition(bac_elem8_tlut, bac_ufo_tlut_night, 3);
        
        col1.r = col1.r + (((float)(col1_r_to - col1.r))/((float)30));
        col1.g = col1.g + (((float)(col1_g_to - col1.g))/((float)30));
        col1.b = col1.b + (((float)(col1_b_to - col1.b))/((float)30));
        col2.r = col2.r + (((float)(col2_r_to - col2.r))/((float)30));
        col2.g = col2.g + (((float)(col2_g_to - col2.g))/((float)30));
        col2.b = col2.b + (((float)(col2_b_to - col2.b))/((float)30));
        
        // If we got got a 1000 point bean, create a comet
        if (GameScore - score_before == 1000 && bac_elem12_alpha == 0 && GameScore > NIGHT_MODE+3000)
        {
            comet_x = 47 + guRandom()%120;
            comet_y = 40 + guRandom()%80;
            bac_elem12_alpha = 255;
            sndHandle = nuAuStlSndPlayerPlay(SND_COMET);
        }
    }
    else if (GameScore >= SUNSET_MODE)
    {
        char col2_r_to = 120;
        char col2_g_to = 40;
        char col2_b_to = 72;
        MusHandleSetVolume(seqHandle2, 48);
        if (GameSpeed < 50)
            GameSpeed = 50;
        tlut_transition(bac_elem4_tlut, bac_wheel_tlut_sunset, 3);
        
        col2.r = col2.r + (((float)(col2_r_to - col2.r))/((float)30));
        col2.g = col2.g + (((float)(col2_g_to - col2.g))/((float)30));
        col2.b = col2.b + (((float)(col2_b_to - col2.b))/((float)30));
        if (extra == 0)
        {
            int ent = instance_create(OBJ_BEAN, 54 + (guRandom()%215), 36);
            if (ent != -1)
            {
                char type = 3;
                float speed = 0.75;
                extra = 1;
                instance_set(ent, SET_TYPE, &type);
                instance_set(ent, SET_SPEED, &speed);
            }
        }
    }
    
    // Fade parts of the landscape in depending on the score
    if (bac_elem12_alpha > 0)
        bac_elem12_alpha -=3;
    if (GameScore >= 12000 && bac_elem11_alpha != 255)
        bac_elem11_alpha += 5;
    if (GameScore >= 11000 && bac_elem10_alpha != 255)
        bac_elem10_alpha += 5;
    if (GameScore >= 10000 && bac_elem9_alpha != 255)
        bac_elem9_alpha += 5;
    if (GameScore >= 9000 && bac_elem8_alpha != 255)
        bac_elem8_alpha += 5;
    if (GameScore >= 8000 && bac_elem7_alpha != 255)
        bac_elem7_alpha += 5;
    if (GameScore >= 7000 && bac_elem6_alpha != 255)
        bac_elem6_alpha += 5;
    if (GameScore >= 6000 && bac_elem5_alpha != 255)
        bac_elem5_alpha += 5;
    if (GameScore >= 4000 && bac_elem4_alpha != 255)
    {
        if (GameSpeed < 40)
            GameSpeed = 40;
        bac_elem4_alpha += 5;
    }
    if (GameScore >= 3000 && bac_elem3_alpha != 255)
    {
        if (GameSpeed < 30)
            GameSpeed = 30;
        bac_elem3_alpha += 5;
    }
    if (GameScore >= 2000 && bac_elem2_alpha != 255)
    {
        if (GameSpeed < 20)
            GameSpeed = 20;
        bac_elem2_alpha += 5;
    }
    if (GameScore >= 1000 && bac_elem1_alpha != 255)
    {
        if (GameSpeed < 10)
            GameSpeed = 10;
        bac_elem1_alpha += 5;
    }
        
    // Spawn beans at random
    if (nextspawn_timer < osGetTime())
    {
        int ent = instance_create(OBJ_BEAN, 52 + (guRandom()%217), 36);
        
        nextspawn_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(getBezierY(GameSpeed, 2500000, 1000000, 275000, 275000)-guRandom()%(1000000-9000*GameSpeed)); // 2500000 at 0 speed, 275000 (-100000) at 100.
        if (guRandom()%5 == 0 && ent != -1 && GameSpeed > 2)
        {
            char type = 2;
            instance_set(ent, SET_TYPE, &type);
        }
        else if (guRandom()%50 == 0 && ent != -1 && GameScore > SUNSET_MODE)
        {
            char type = 3;
            float speed = 0.5;
            instance_set(ent, SET_TYPE, &type);
            instance_set(ent, SET_SPEED, &speed);
        }
    }
    
    // Spawn a flashing bean if we got 1000 points
    if (GameScore - score_before == 1000 && GameScore > SUNSET_MODE+2000)
    {
        int ent = instance_create(OBJ_BEAN, 52 + (guRandom()%217), 36);
        if (ent != -1)
        {
            char type = 3;
            instance_set(ent, SET_TYPE, &type);
        }
    }
    
    // Increase the speed of the game
    if (nextspeed_timer < osGetTime() && GameSpeed < 100 && instance_find(OBJ_PYORO) != -1)
    {
        GameSpeed++;
        if (GameSpeed > 90)
            nextspeed_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(10000000);
        else if (GameSpeed > 80)
            nextspeed_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(8500000);
        else if (GameSpeed > 75)
            nextspeed_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(7000000);
        else if (GameSpeed > 50)
            nextspeed_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(4000000);
        else
            nextspeed_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(2000000);
        if (GameScore <= BEIGE_MODE && GameSpeed+0x80 < 175)
        {
            MusHandleSetTempo(seqHandle1, GameSpeed+0x80);
            MusHandleSetTempo(seqHandle2, GameSpeed+0x80);
            MusHandleSetTempo(seqHandle3, GameSpeed+0x80);
        }
    }
}


/*=================================
          stage_game1_draw
           Draw the stage
=================================*/

void stage_game1_draw(void)
{
    // Assign our glist pointer to our glist array for ease of access
    glistp = glist;
    #if DEBUG_MODE == 1
        glistbefore = glistp;
    #endif
    
    // Initialize the RCP
    RCPInit(glistp);

    // Wipe the background with a color
    ClearBackground(255, 0, 0);
    
    // Draw a nice gradient
    DrawGradient(45, 38, 276, 206, col1, col2);
    
    // Draw the stars, mountains, etc...
    DrawLandscape();
    
    // Draw all the objects
    instance_draw_all();
    
    // Draw the HUD
    DrawHUD();
    
    // Syncronize the RCP and CPU
    gDPFullSync(glistp++);
    
    // Specify that our display list has ended
    gSPEndDisplayList(glistp++);
    
    // Start the display task
    nuGfxTaskStart(glist, (s32)(glistp - glist) * sizeof(Gfx), NU_GFX_UCODE_F3DEX, NU_SC_SWAPBUFFER);
}


/*=================================
           DrawGradient
Draws a gradient between two colors
=================================*/

void DrawGradient(int x1, int y1, int x2, int y2, Color col1, Color col2)
{
    int i = 0;
    float amount = 24;
    int r = col1.r;
    int g = col1.g;
    int b = col1.b;
    
    for (i=1;i<amount;i++)
    {
        // Draw a rectangle
        gDPSetFillColor(glistp++, (GPACK_RGBA5551(r, g, b, 1) << 16 | GPACK_RGBA5551(r, g, b, 1)));
        gDPFillRectangle(glistp++, x1, y1 + i*((y2-y1)/amount), x2, y1 + (i+1)*((y2-y1)/amount));
        
        // Calculate the next color
        r = r + (((float)(col2.r - col1.r))/amount);
        g = g + (((float)(col2.g - col1.g))/amount);
        b = b + (((float)(col2.b - col1.b))/amount);
    }
    gDPPipeSync(glistp++);
}


/*=================================
           DrawLandscape
Draws the stars, mountains, etc...
=================================*/

void DrawLandscape()
{
    int i, j, x, y, w, h;
    unsigned char * mountain_sprite[3][8] = {
        {bac_mountains_1,  bac_mountains_2,  bac_mountains_3,  bac_mountains_4,  bac_mountains_5,  bac_mountains_6,  bac_mountains_7,  bac_mountains_8},
        {bac_mountains_9,  bac_mountains_10, bac_mountains_11, bac_mountains_12, bac_mountains_13, bac_mountains_14, bac_mountains_15, bac_mountains_16},
        {bac_mountains_17, bac_mountains_18, bac_mountains_19, bac_mountains_20, bac_mountains_21, bac_mountains_22, bac_mountains_23, bac_mountains_24}
    };

    // Draw small and big stars, using the arrays above to position them.
    gSPDisplayList(glistp++, dl_bac_bigstars);
    gSPDisplayList(glistp++, dl_bac_smallstars);
    
    // Set the default values
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 110, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
    
    // Draw a comet
    if (bac_elem12_alpha > 0)
    {
        x = comet_x;
        y = comet_y;
        w = 112;
        h = 30;
        gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem12_alpha); 
        gDPLoadTLUT_pal16(glistp++, 0, bac_comet_tlut);
        for(i = 0; i < h; i++)
        {
            gDPLoadMultiTile_4b(glistp++, bac_comet, 0, G_TX_RENDERTILE, G_IM_FMT_CI, w, h, 0, i, w - 1, i, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, x << 2, y + i << 2, x + w << 2, y + i+1 << 2,  G_TX_RENDERTILE,  0 << 5, i << 5,  1 << 10, 1 << 10);
        }
    }
    
    // Draw the bridge
    x = 154;
    y = 147;
    w = 86;
    h = 48;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem5_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem5_tlut);
    for(i = 0; i < h; i++)
    {
        gDPLoadMultiTile_4b(glistp++, bac_bridge, 0, G_TX_RENDERTILE, G_IM_FMT_CI, w, h, 0, i, w - 1, i, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x << 2, y + i << 2, x + w  << 2, y + i+1 << 2,  G_TX_RENDERTILE,  0 << 5, i << 5,  1 << 10, 1 << 10);
    }
    
    // Draw the Eiffel tower
    x = 129;
    y = 110;
    w = 40;
    h = 75;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem6_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem6_tlut);
    for(i = 0; i < h; i++)
    {
        gDPLoadMultiTile_4b(glistp++, bac_eiffel, 0, G_TX_RENDERTILE, G_IM_FMT_CI, w, h, 0, i, w - 1, i, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x << 2, y + i << 2, x + w << 2, y + i+1 << 2,  G_TX_RENDERTILE,  0 << 5, i << 5,  1 << 10, 1 << 10);
    }
    
    // Draw the skyscraper
    x = 98;
    y = 81;
    w = 16;
    h = 80;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem7_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem7_tlut);
    for(i = 0; i < h; i++)
    {
        gDPLoadMultiTile_4b(glistp++, bac_skyscraper, 0, G_TX_RENDERTILE, G_IM_FMT_CI, w, h, 0, i, w - 1, i, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x << 2, y + i << 2, x + w << 2, y + i+1 << 2,  G_TX_RENDERTILE,  0 << 5, i << 5,  1 << 10, 1 << 10);
    }
    
    // Draw a ufo
    x = 121;
    y = 77;
    w = 16;
    h = 8;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem8_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem8_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_ufo, G_IM_FMT_CI, w, h, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x<<2, y<<2, x+w<<2, y+h<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    // Draw the moon
    x = 182;
    y = 102;
    w = 32;
    h = 32;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem9_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem9_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_moon, G_IM_FMT_CI, w, h, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x<<2, y<<2, x+w<<2, y+h<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    // Draw a planet
    x = 62;
    y = 62;
    w = 32;
    h = 16;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem10_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem10_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_planet, G_IM_FMT_CI, w, h, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x<<2, y<<2, x+w<<2, y+h<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    // Draw a city
    x = 228;
    y = 64;
    w = 48;
    h = 48;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem11_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem11_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_city, G_IM_FMT_CI, w, h, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x<<2, y<<2, x+w<<2, y+h<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    // Draw the lamps
    x = 199;
    y = 169;
    w = 16;
    h = 32;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem1_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem1_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_lamp, G_IM_FMT_CI, w, h, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x<<2, y<<2, x+w<<2, y+h<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    x = 170;
    y = 176;
    w = 16;
    h = 32;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem2_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem1_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_lamp, G_IM_FMT_CI, w, h, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x<<2, y<<2, x+w<<2, y+h<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    // Draw a house
    x = 131;
    y = 162;
    w = 16;
    h = 32;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem3_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem3_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_house, G_IM_FMT_CI, w, h, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x<<2, y<<2, x+w<<2, y+h<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    // Draw the wheel
    x = 105;
    y = 146;
    w = 32;
    h = 48;
    gDPSetPrimColor(glistp++,0,0,255,255,255,bac_elem4_alpha); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem4_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_wheel, G_IM_FMT_CI, w, h, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x<<2, y<<2, x+w<<2, y+h<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    // Draw the mountains
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetPrimColor(glistp++,0,0,255,255,255,255); 
    gDPLoadTLUT_pal16(glistp++, 0, bac_elem2_tlut);
    x = 45;
    y = 133;
    w = 16;
    h = 16;
    
    for (i=0;i<3;i++)
    {
        for (j=0;j<8;j++)
        {
            if (x+w-16 > SCREEN_WD || x+w+16 < 0)
            {
                w += 32;
                continue;
            }
            if (y+h-16 > SCREEN_HT || y+h+16 < 0)
            {
                w += 32;
                continue;
            }
            gDPLoadTextureBlock_4b(glistp++, mountain_sprite[i][j], G_IM_FMT_CI, 32, 32, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, x+w-16 << 2, y+h-16 << 2, x+w+16 << 2, y+h+16 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5,  1 << 10, 1 << 10);
            w += 32;
        }
        w -= 32*j;
        h += 32;
    }
    
    gDPPipeSync(glistp++);
}


/*=================================
              DrawHUD
    Draws the Heads Up Display
=================================*/

void DrawHUD()
{
    int i, x, y, w, h;
    char conbuff[7];
    char conbuff2[7];
    u32 highscore = (global_save_highscores[3] << 24) | (global_save_highscores[2] << 16) | (global_save_highscores[1] << 8) | global_save_highscores[0];
    
    // Get some values for display if debug mode is on
    #if DEBUG_MODE == 1 
        int count=0;
        char conbuff3[5];
        char conbuff4[4];
        char conbuff5[4];

        for (i=0;i<MAXOBJECTS;i++)
            if (GameObjects[i].type != OBJ_NULL)
                count++;
        sprintf(conbuff4, "%03d", count);
        sprintf(conbuff5, "%03d", GameSpeed);
    #endif
    
    // Retrieve the highscore if in singleplayer mode or both players' scores
    if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0)
    {
        sprintf(conbuff, "%06d", GameScore);
        if (GameScore >= highscore)
            sprintf(conbuff2, "%06d", GameScore);
        else
            sprintf(conbuff2, "%06d", highscore);
    }
    else
    {
        sprintf(conbuff, "%06d", GameScore1);
        sprintf(conbuff2, "%06d", GameScore2);
    }
    
    // Show GAME OVER if Pyoro is dead
    if (instance_find(OBJ_PYORO) == -1)
    {
        global_text_alpha = 255;
        global_text_sprite = fnt_text_outlined;
        global_text_tlut = fnt_text_outlined_tlut;
        if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0)
            DrawText(SCREEN_WD/2, SCREEN_HT/2, "Game Over", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        else
        {
            if (GameScore1 > GameScore2)
                DrawText(SCREEN_WD/2, SCREEN_HT/2, "Player 1 Wins", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if (GameScore1 < GameScore2)
                DrawText(SCREEN_WD/2, SCREEN_HT/2, "Player 2 Wins", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else
                DrawText(SCREEN_WD/2, SCREEN_HT/2, "No winner", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        }
        global_text_tlut = fnt_text_tlut_white;
        global_text_sprite = fnt_text;
        global_text_alpha = 255;
    }
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    gDPPipeSync(glistp++);
    
    // Draw the border top and bottom
    x = 47;
    y = 40;
    w = 227;
    h = 3;
    gDPSetRenderMode(glistp++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetCycleType( glistp++, G_CYC_FILL );
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(255, 255, 255, 1) << 16 | GPACK_RGBA5551(255, 255, 255, 1)));
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    y +=166;
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    y = 44;
    h = 0;
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1)));
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    y +=161;
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    
    // Draw the border left and right
    x = 40;
    y = 46;
    w = 3;
    h = 156;
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(255, 255, 255, 1) << 16 | GPACK_RGBA5551(255, 255, 255, 1)));
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    x +=238;
    gDPFillRectangle(glistp++, x, y, x+w, y+164); // Hardware specific bug correction.
    x = 44;
    w = 0;
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1)));
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    x +=233;
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    
    // Draw the blue background
    x = 0;
    y = 0;
    w = SCREEN_WD;
    h = 39;
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(32, 32, 72, 1) << 16 | GPACK_RGBA5551(32, 32, 72, 1)));
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    y = 210;
    w = SCREEN_WD;
    h = SCREEN_HT-y;
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    x = 0;
    y = 39;
    w = 39;
    h = 210;
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    x = 282;
    w = SCREEN_WD-x;
    gDPFillRectangle(glistp++, x, y, x+w, y+h);
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE);
    gDPPipeSync(glistp++);
    
    // Draw the border corners
    x = 43;
    y = 43;
    w = 8;
    h = 8;
    gDPLoadTLUT_pal16(glistp++, 0, bac_bordercorner_tlut);
    gDPLoadTextureTile_4b(glistp++, bac_bordercorner, G_IM_FMT_CI, w, h, 0, 0, w-1, h-1, 0, G_TX_MIRROR, G_TX_MIRROR, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, x-w/2<<2, y-h/2<<2, x+w/2<<2, y+h/2<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    x +=232+4;
    gSPTextureRectangle(glistp++, x-w/2<<2, y-h/2<<2, (x+w/2<<2), (y+h/2<<2), G_TX_RENDERTILE, 8 << 5, 0 << 5, 1 << 10, 1 << 10 );
    y +=164;
    gSPTextureRectangle(glistp++, x-w/2<<2, y-h/2<<2, (x+w/2<<2), (y+h/2<<2), G_TX_RENDERTILE, 8 << 5, 8 << 5, 1 << 10, 1 << 10 );
    x -=232+4;
    gSPTextureRectangle(glistp++, x-w/2<<2, y-h/2<<2, (x+w/2<<2), (y+h/2<<2), G_TX_RENDERTILE, 0 << 5, 8 << 5, 1 << 10, 1 << 10 );
    
    // Draw the score
    if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0)
    {
        // Draw the current score
        gDPLoadTLUT_pal16(glistp++, 0, bac_score_tlut);
        gDPLoadTextureBlock_4b(glistp++, bac_score, G_IM_FMT_CI, 32, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, 81 << 2, 48 << 2, 81+32 << 2, 48+16 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5,  1 << 10, 1 << 10);
        
        gDPLoadTLUT_pal16(glistp++, 0, fnt_text_tlut_white);
        for (i=0;i<sizeof(conbuff)-1;i++)
        {
            char offset = conbuff[i] - '0';
            char pos = i*8;
            gDPLoadTextureTile_4b(glistp++, fnt_numbers, G_IM_FMT_CI, 8, 8, 0, offset*8, 8, 8 + (offset*8), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, 105+pos << 2, 46 << 2, 105+8+pos << 2, 46+8 << 2,  G_TX_RENDERTILE,  0 << 5, offset*8 << 5,  1 << 10, 1 << 10);
        }
   
        // Draw the Highscore
        gDPLoadTLUT_pal16(glistp++, 0, bac_score_tlut);
        gDPLoadTextureBlock_4b(glistp++, bac_hiscore, G_IM_FMT_CI, 32, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, 164 << 2, 48 << 2, 164+32 << 2, 40+16 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5,  1 << 10, 1 << 10);
        
        gDPLoadTLUT_pal16(glistp++, 0, fnt_text_tlut_white);
        for (i=0;i<sizeof(conbuff2)-1;i++)
        {
            char offset = conbuff2[i] - '0';
            char pos = i*8;
            gDPLoadTextureTile_4b(glistp++, fnt_numbers, G_IM_FMT_CI, 8, 8, 0, offset*8, 8, 8 + (offset*8), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, 164+31+pos << 2, 46 << 2, 164+31+8+pos << 2, 46+8 << 2,  G_TX_RENDERTILE,  0 << 5, offset*8 << 5,  1 << 10, 1 << 10);
        }
    }
    else
    {
        // Player 1 Score
        gDPLoadTLUT_pal16(glistp++, 0, bac_score_tlut);
        gDPLoadTextureBlock_4b(glistp++, bac_score_ply1, G_IM_FMT_CI, 32, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, 84 << 2, 48 << 2, 84+32 << 2, 48+16 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5,  1 << 10, 1 << 10);
        
        gDPLoadTLUT_pal16(glistp++, 0, fnt_text_tlut_white);
        for (i=0;i<sizeof(conbuff)-1;i++)
        {
            char offset = conbuff[i] - '0';
            char pos = i*8;
            gDPLoadTextureTile_4b(glistp++, fnt_numbers, G_IM_FMT_CI, 8, 8, 0, offset*8, 8, 8 + (offset*8), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, 108+pos << 2, 46 << 2, 108+8+pos << 2, 46+8 << 2,  G_TX_RENDERTILE,  0 << 5, offset*8 << 5,  1 << 10, 1 << 10);
        }
   
        // Player 2 Score
        gDPLoadTLUT_pal16(glistp++, 0, bac_score_tlut);
        gDPLoadTextureBlock_4b(glistp++, bac_score_ply2, G_IM_FMT_CI, 32, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, 167 << 2, 48 << 2, 167+32 << 2, 40+16 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5,  1 << 10, 1 << 10);
        
        gDPLoadTLUT_pal16(glistp++, 0, fnt_text_tlut_white);
        for (i=0;i<sizeof(conbuff2)-1;i++)
        {
            char offset = conbuff2[i] - '0';
            char pos = i*8;
            gDPLoadTextureTile_4b(glistp++, fnt_numbers, G_IM_FMT_CI, 8, 8, 0, offset*8, 8, 8 + (offset*8), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, 167+24+pos << 2, 46 << 2, 167+24+8+pos << 2, 46+8 << 2,  G_TX_RENDERTILE,  0 << 5, offset*8 << 5,  1 << 10, 1 << 10);
        }
    }
    
    // Draw the pause screen
    if (paused)
    {
        gDPPipeSync(glistp++);
        x = 45;
        y = 45;
        w = 232;
        h = 160;
        gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
        gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
        gDPSetPrimColor(glistp++,0,0,255,255,255,128); 
        gDPSetPrimDepth(glistp++, 0, 0);
        gDPLoadTLUT_pal16(glistp++, 0, bac_solid_tlut);
        gDPLoadTextureBlock_4b(glistp++, bac_solid, G_IM_FMT_CI, 16, 16, 0, G_TX_WRAP, G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x << 2, y << 2, x+w << 2, y+h << 2,  G_TX_RENDERTILE, 0 << 5, 0 << 5,  1 << 10, 1 << 10);

        gDPPipeSync(glistp++);
        
        gDPSetPrimColor(glistp++,0,0,255,255,255,255); 
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPSetRenderMode(glistp++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE);
        
        w = 36;
        h = 12;
        x = (float)SCREEN_WD/2 - (float)w/2;
        y = (float)SCREEN_HT/2 - (float)h/2 - 4;
        gDPLoadTLUT_pal16(glistp++, 0, bac_pause_tlut);
        for(i = 0; i < h; i++)
        {
            gDPLoadMultiTile_4b(glistp++, bac_pause, 0, G_TX_RENDERTILE, G_IM_FMT_CI, w, h, 0, i, w, i, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, x << 2, y + i << 2, x + w << 2, y + i+1 << 2,  G_TX_RENDERTILE,  0 << 5, i << 5,  1 << 10, 1 << 10);
        }
        
        x = x-13;
        y = y+22;
        w = 34;
        h = 8;
        gDPLoadTLUT_pal16(glistp++, 0, bac_pause_continue_tlut);
        gDPLoadMultiTile_4b(glistp++, bac_pause_continue, 0, G_TX_RENDERTILE, G_IM_FMT_CI, w, h, 0, 0, w, h, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x << 2, y << 2, x+w << 2, y+h << 2,  G_TX_RENDERTILE, 0 << 5, 0 << 5,  1 << 10, 1 << 10);

        x = x+47;
        w = 16;
        h = 8;
        gDPLoadTLUT_pal16(glistp++, 0, bac_pause_quit_tlut);
        gDPLoadMultiTile_4b(glistp++, bac_pause_quit, 0, G_TX_RENDERTILE, G_IM_FMT_CI, w, h, 0, 0, w, h, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x << 2, y << 2, x+w << 2, y+h << 2,  G_TX_RENDERTILE, 0 << 5, 0 << 5,  1 << 10, 1 << 10);

        y = y-1;
        w = 8;
        h = 9;
        if (pause_select == 0)
            x = x-47-8;
        else
            x = x-8;
            
        gDPLoadTLUT_pal16(glistp++, 0, bac_pause_selection_tlut);
        gDPLoadMultiTile_4b(glistp++, bac_pause_selection, 0, G_TX_RENDERTILE, G_IM_FMT_CI, w, h, 0, 0, w, h, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x << 2, y << 2, x+w << 2, y+h << 2,  G_TX_RENDERTILE, 0 << 5, 0 << 5,  1 << 10, 1 << 10);

    }
    
    // Show debug information
    #if DEBUG_MODE == 1 
        gDPLoadTLUT_pal16(glistp++, 0, fnt_text_tlut_white);
        glistafter = glistp;
        sprintf(conbuff3, "%4d", glistafter+22+37+18 - glistbefore);
        for (i=0;i<sizeof(conbuff3)-1;i++)
        {
            char offset = conbuff3[i] - '0';
            char pos = i*8;
            gDPLoadTextureTile_4b(glistp++, fnt_numbers, G_IM_FMT_CI, 8, 8, 0, offset*8, 8, 8 + (offset*8), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, 16+pos << 2, 16 << 2, 16+8+pos << 2, 16+8 << 2,  G_TX_RENDERTILE,  0 << 5, offset*8 << 5,  1 << 10, 1 << 10);
        }
        
        for (i=0;i<sizeof(conbuff4)-1;i++)
        {
            char offset = conbuff4[i] - '0';
            char pos = i*8;
            gDPLoadTextureTile_4b(glistp++, fnt_numbers, G_IM_FMT_CI, 8, 8, 0, offset*8, 8, 8 + (offset*8), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, 16+pos << 2, 32 << 2, 16+8+pos << 2, 32+8 << 2,  G_TX_RENDERTILE,  0 << 5, offset*8 << 5,  1 << 10, 1 << 10);
        }
        
        for (i=0;i<sizeof(conbuff5)-1;i++)
        {
            char offset = conbuff5[i] - '0';
            char pos = i*8;
            gDPLoadTextureTile_4b(glistp++, fnt_numbers, G_IM_FMT_CI, 8, 8, 0, offset*8, 8, 8 + (offset*8), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, 16+pos << 2, 48 << 2, 16+8+pos << 2, 48+8 << 2,  G_TX_RENDERTILE,  0 << 5, offset*8 << 5,  1 << 10, 1 << 10);
        }
    #endif
    gDPPipeSync(glistp++);
    
    // Draw screen fade
    if (fademode == FADE_OUT && fade_alpha != 0)
        fade_alpha -=5;
    if (fademode == FADE_IN && fade_alpha != 255)
        fade_alpha +=5;
        
    if (fade_alpha != 0)
    {
        x = 0;
        y = 0;
        w = SCREEN_WD;
        h = 8;
        gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
        gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
        gDPSetPrimDepth(glistp++, 0, 0);
        gDPSetPrimColor(glistp++,0,0,255,255,255,fade_alpha); 
        gDPLoadTLUT_pal16(glistp++, 0, bac_solid_tlut);
        gDPLoadTextureBlock_4b(glistp++, bac_solid, G_IM_FMT_CI, 16, 16, 0, G_TX_WRAP, G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, 0 << 2, 0 << 2, SCREEN_WD << 2, SCREEN_HT << 2,  G_TX_RENDERTILE, 0 << 5, 0 << 5,  1 << 10, 1 << 10);
    }
    gDPPipeSync(glistp++);
}