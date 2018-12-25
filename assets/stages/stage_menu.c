/*========================================================
                    Main menu stage
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

#include "../sprites/bac_solid.h"
#include "../sprites/bac_menu_back.h"
#include "../sprites/bac_pause_selection.h"
#include "../sprites/fnt_text.h"
#include "../sprites/fnt_text_outlined.h"
#include "../sprites/spr_block.h"
#include "../sprites/spr_buttonicon_1.h"
#include "../sprites/spr_buttonicon_2.h"
#include "../sprites/spr_logo.h"
#include "../sprites/spr_logo_eyes.h"
#include "../sprites/spr_menu_button.h"
#include "../sprites/spr_menu_button_select.h"
#include "../sprites/spr_menu_button_unlock.h"
#include "../sprites/spr_menu_cursor.h"
#include "../sprites/spr_menu_slider.h"
#include "../sprites/spr_mouth.h"
#include "../sprites/spr_pyoro.h"
#include "../sprites/spr_pyoro_accessories.h"
#include "../sprites/spr_tongue.h"


/*=================================
         Global Variables
=================================*/

#define MENUSTATE_SHOWLOGO 0
#define MENUSTATE_STARTMOVE 1
#define MENUSTATE_SECONDMOVE 2
#define MENUSTATE_READYMAIN 3
#define MENUSTATE_GAMESTART 4
#define MENUSTATE_GOTOCREDITS 5
#define MENUSTATE_READYCREDITS 6
#define MENUSTATE_RETURNCREDITS 7
#define MENUSTATE_GOTOSETTINGS 8
#define MENUSTATE_READYSETTINGS 9
#define MENUSTATE_RETURNSETTINGS 10
#define MENUSTATE_GOTOPLAYER 11
#define MENUSTATE_RETURNPLAYER 12
#define MENUSTATE_READYPLAYER 13
#define MENUSTATE_GOTORESET 14
#define MENUSTATE_READYRESET 15
#define MENUSTATE_RETURNRESET 16
#define MENUSTATE_GOTOCONTROLS 17
#define MENUSTATE_READYCONTROLS 18
#define MENUSTATE_RETURNCONTROLS 19

#define SELECT_PYORO1 0
#define SELECT_PYORO2 1
#define SELECT_MAINPLAYER 2
#define SELECT_MAINSETTINGS 3
#define SELECT_MAINCREDITS 4
#define SELECT_CREDITSBACK 5
#define SELECT_SETTINGSBACK 6
#define SELECT_SETTINGSSOUND 7
#define SELECT_SETTINGSMUSIC 8
#define SELECT_SETTINGSCONTROLS 9
#define SELECT_SETTINGSRESET 10
#define SELECT_RESETYES 11
#define SELECT_RESETNO 12
#define SELECT_CONTROLSBACK 13
#define SELECT_CONTROLSP1MOVE 14
#define SELECT_CONTROLSP1ATTACK 15
#define SELECT_CONTROLSP1TEST 16
#define SELECT_CONTROLSP2MOVE 17
#define SELECT_CONTROLSP2ATTACK 18
#define SELECT_CONTROLSP2TEST 19

// Logo stuff
static int logo_x;
static int logo_y;
static char logo_eyes;
static OSTime eyes_timer;

// Button stuff
static int button_pyoro1_x;
static int button_pyoro1_y;
static int button_pyoro2_x;
static int button_pyoro2_y;
static int button_main_x;
static int button_main_y;
static int button_credits_x;
static int button_credits_y;
static int button_settings_x;
static int button_settings_y;
static int button_reset_x;
static int button_reset_y;
static int button_controls_x;
static int button_controls_y;

// Menu stuff
static char background_alpha;
static char menu_state;
static OSTime menu_state_timer;
static OSTime stick_timer;
static OSTime stick2_timer;
static char selected;
static char selectedp2;
static short* selected_tlut;
static char selected_transition;
static char returnmessage[23];
static char backfadealpha;
static char frontfadealpha;

// Moving pyoro on button
static float pyoro1_speed;
static signed char pyoro1_movestotongue;
static OSTime pyoro1_timer;
static float pyoro1_posx;
static float pyoro1_tonguex;
static char pyoro1_textwave;

// Moving pyoro2 on button
static float pyoro2_speed;
static signed char pyoro2_movestoattack;
static u8 pyoro2_justattacked;
static OSTime pyoro2_timer;
static float pyoro2_posx;
static char pyoro2_textwave;

// Debug mode stuff
#if (DEBUG_MODE == 1)
    #include "../sprites/fnt_numbers.h"
    Gfx * glistbefore;
    Gfx * glistafter;
#endif


/*=================================
             Prototypes
=================================*/

static void DrawBackground();
static void DrawLogo();
static void DrawButtonPyoro1();
static void DrawButtonPyoro2();
static void DrawSlider(int x, int y, char type);
static void DrawSmallButton(int x, int y, char id);
static void DrawFade(char fade_alpha);
#if (DEBUG_MODE == 1)
    static void DrawDebugInfo();
#endif


/*=================================
          stage_menu_init
        Initialize the stage
=================================*/

void stage_menu_init(void)
{
    // Initialize the variables
    if (returnmode == RETURN_LOGO)
    {
        logo_x = SCREEN_WD/2;
        logo_y = SCREEN_HT/2;
        button_pyoro1_x = -108;
        button_pyoro1_y = 84;
        button_pyoro2_x = SCREEN_WD+108;
        button_pyoro2_y = 84;
        button_main_x = SCREEN_WD/2;
        button_main_y = SCREEN_HT + 35;
        menu_state_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(700000);
        selected = SELECT_PYORO1;
        background_alpha = 0;
        backfadealpha = 0;
        frontfadealpha = 0;
    }
    else if (returnmode == RETURN_FADE)
    {
        logo_x = SCREEN_WD/2;
        logo_y = 64;
        button_pyoro1_x = 36;
        button_pyoro1_y = 84;
        button_pyoro2_x = SCREEN_WD - 144;
        button_pyoro2_y = 84;
        button_main_x = SCREEN_WD/2;
        button_main_y = 193;
        menu_state_timer = osGetTime();
        selected = SELECT_PYORO1;
        background_alpha = 255;
        backfadealpha = 0;
        frontfadealpha = 255;
    }
    else if (returnmode == RETURN_GAME1)
    {
        logo_x = SCREEN_WD/2;
        logo_y = 64;
        button_pyoro1_x = SCREEN_WD/2-54;
        button_pyoro1_y = SCREEN_HT/2-64;
        button_pyoro2_x = SCREEN_WD - 144;
        button_pyoro2_y = 84;
        button_main_x = SCREEN_WD/2;
        button_main_y = 193;
        menu_state_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
        selected = SELECT_PYORO1;
        background_alpha = 255;
        backfadealpha = 255;
        frontfadealpha = 0;
    }
    else if (returnmode == RETURN_GAME2)
    {
        logo_x = SCREEN_WD/2;
        logo_y = 64;
        button_pyoro1_x = 36;
        button_pyoro1_y = 84;
        button_pyoro2_x = SCREEN_WD/2-54;
        button_pyoro2_y = SCREEN_HT/2-64;
        button_main_x = SCREEN_WD/2;
        button_main_y = 193;
        menu_state_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
        selected = SELECT_PYORO2;
        background_alpha = 255;
        backfadealpha = 255;
        frontfadealpha = 0;
    }
  
    menu_state = MENUSTATE_SHOWLOGO;
    logo_eyes = 0;
    button_credits_x = SCREEN_WD + 76;
    button_credits_y = 193;
    button_settings_x = SCREEN_WD/2;
    button_settings_y = SCREEN_HT+32;
    button_reset_x = SCREEN_WD/2;
    button_reset_y = SCREEN_HT+SCREEN_HT/2;
    button_controls_x = SCREEN_WD + SCREEN_WD/2;
    button_controls_y = 32;
    eyes_timer = 0;
    stick_timer = 0;
    stick2_timer = 0;
    selectedp2 = SELECT_CONTROLSP2MOVE;
    selected_tlut = tlut_copy(spr_menu_button_select_tlut_bright, 3);
    selected_transition = 0;
    
    pyoro1_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(500000);
    pyoro1_movestotongue = 5;
    pyoro1_speed = 0;
    pyoro1_posx = 0;
    pyoro1_tonguex = 0;
    pyoro1_textwave = 0;    
    
    pyoro2_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(300000);
    pyoro2_movestoattack = 6;
    pyoro2_speed = 0;
    pyoro2_posx = 0;
    pyoro2_textwave = 0;
    pyoro2_justattacked = 0;
    
    // Read the song from the ROM
    Rom2Ram((void *)SNG_MENU_START, (void *)tune1_buf, SNG_MENU_END-SNG_MENU_START);
    
    // Play the song
    MusPtrBankSetSingle(ptr2_buf);
    seqHandle1 = MusStartSong(tune1_buf);
    MusHandleSetVolume(seqHandle1, 48);
    
    // If returning to the menu after losing, display a message
    if (returnmode == RETURN_GAME1 || returnmode == RETURN_GAME2)
    {
        char strings[5][3][23] = {
            {"Keep trying", "Take a break", "Ouch"},
            {"So far so good", "Not bad at all", "Hang in there!"},
            {"Nice work!", "Impressive!", "Well done!"},
            {"That was really cool!", "Fantastic!", "Awesome work!"},
            {"Amazing!", "Congratulations!", "Incredible!"}
        };
        int message = guRandom()%3;
        int group;
        
        // Choose the group depending on your score last game
        if (GameScore < 5000)
            group = 0;
        else if (GameScore >= 5000 && GameScore < 20000) 
            group = 1;
        else if (GameScore >= 20000 && GameScore < 30000) 
            group = 2;
        else if (GameScore >= 30000 && GameScore < 50000) 
            group = 3;
        else
            group = 4;
            
        // Copy the string to display it
        strcpy(returnmessage, strings[group][message]);
    }
    else
        returnmessage[0] = '\0';
}


/*=================================
        stage_menu_update
   Update variables and objects
=================================*/

void stage_menu_update(void)
{
    int i;
    
    // Get controller input
    nuContDataGetExAll(contdata);
    
    // Generate a new random seed (to keep the main game different)
    guRandom();
    
    // Make the selection tlut flash
    selected_transition = (selected_transition+1)%30;
    if (selected_transition < 15)
        tlut_transition(selected_tlut, spr_menu_button_select_tlut_dark, 1);
    else
        tlut_transition(selected_tlut, spr_menu_button_select_tlut_bright, 1);
    
    // Make Pyoro do things
    {
        // Decide what part of the animation we're on
        if (pyoro1_timer < osGetTime() && pyoro1_timer != 0)
        {
            if (pyoro1_movestotongue > 0)
            {
                if (pyoro1_speed > 0)
                    pyoro1_speed = -1;
                else 
                    pyoro1_speed = 1;
                pyoro1_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000+guRandom()%100000);
            }
            else if (pyoro1_movestotongue == 0)
            {
                pyoro1_tonguex = pyoro1_posx;
                pyoro1_timer = 0;
            }
            else if (pyoro1_movestotongue == -3)
            {
                pyoro1_movestotongue = 3+guRandom()%8;
                if (pyoro1_movestotongue%2 == 0)
                    pyoro1_movestotongue++;
            }
            pyoro1_movestotongue--;
        }
        
        // Move pyoro
        if (pyoro1_speed > 0.1)
        {
            pyoro1_speed -=0.04;
            pyoro1_posx +=pyoro1_speed;
        }
        else if (pyoro1_speed < -0.1)
        {
            pyoro1_speed +=0.04;
            pyoro1_posx +=pyoro1_speed;
        }
        
        // Move the tongue
        if (pyoro1_movestotongue == -1)
        {
            pyoro1_tonguex +=1.5;
            if (pyoro1_tonguex > 31)
            {
                pyoro1_movestotongue--;
            }
        }
        else if (pyoro1_movestotongue == -2)
        {
            pyoro1_tonguex -=3;
            if (pyoro1_tonguex < 12)
            {
                pyoro1_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(750000);
                if (guRandom()%5 == 0)
                    pyoro1_movestotongue = 0;
                else
                    pyoro1_movestotongue--;
            }
        }
        
        // Make the text wave
        if ((pyoro1_movestotongue == -2) || pyoro1_textwave != 0)
        {
            pyoro1_textwave++;
        }
        
        // Stop the text wave
        if (pyoro1_textwave == 14)
            pyoro1_textwave = 0;
    }
    
    // Make Pyoro2 do things
    {
        // Decide what part of the animation we're on
        if (pyoro2_timer < osGetTime() && pyoro2_timer != 0)
        {
            if (pyoro2_movestoattack > 0)
            {
                if (pyoro2_speed < 0 && !pyoro2_justattacked)
                    pyoro2_speed = 1;
                else 
                    pyoro2_speed = -1;
                pyoro2_justattacked = 0;
                pyoro2_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000+guRandom()%200000);
            }
            else if (pyoro2_movestoattack == 0)
            {
                pyoro2_speed = -0.01;
                pyoro2_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
            }
            else if (pyoro2_movestoattack == -1)
            {
                pyoro2_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(20000);
            }
            else if (pyoro2_movestoattack < -2 && pyoro2_movestoattack > -6)
            {
                pyoro2_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(20000);
            }
            else if (pyoro2_movestoattack == -6)
            {
                pyoro2_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
            }
            else if (pyoro2_movestoattack == -7)
            {
                pyoro2_speed = -0.01;
                pyoro2_justattacked = 1;
                pyoro2_movestoattack = 3+guRandom()%8;
                if (pyoro2_movestoattack%2 == 0)
                    pyoro2_movestoattack++;
            }
            
            if (pyoro2_movestoattack == -6 && guRandom()%5 == 0)
                pyoro2_movestoattack = -1;
            else
                pyoro2_movestoattack--;
        }
        
        // Move pyoro
        if (pyoro2_speed > 0.1)
        {
            pyoro2_speed -=0.04;
            pyoro2_posx +=pyoro2_speed;
        }
        else if (pyoro2_speed < -0.1)
        {
            pyoro2_speed +=0.04;
            pyoro2_posx +=pyoro2_speed;
        }
        
        // Make the text wave
        if ((pyoro2_movestoattack < -2 && pyoro2_movestoattack > -6) || pyoro2_textwave != 0)
        {
            pyoro2_textwave++;
        }
        
        // Stop the text wave
        if (pyoro2_textwave == 14)
            pyoro2_textwave = 0;
    }

    // Control the selection
    switch (menu_state)
    {
        case MENUSTATE_READYMAIN:
        {
            char oldselected = selected;
            
            // Choose what to do depending on what is currently selected
            if (selected == SELECT_PYORO1 && (contdata[0].trigger & R_CBUTTONS || contdata[0].trigger & R_JPAD || (contdata->stick_x > 16 && stick_timer < osGetTime())))
                selected = SELECT_PYORO2;
            else if (selected == SELECT_PYORO1 && (contdata[0].trigger & D_CBUTTONS || contdata[0].trigger & D_JPAD || (contdata->stick_y < -16 && stick_timer < osGetTime())))
                selected = SELECT_MAINPLAYER;
            else if (selected == SELECT_PYORO2 && (contdata[0].trigger & L_CBUTTONS || contdata[0].trigger & L_JPAD || (contdata->stick_x < -16 && stick_timer < osGetTime())))
                selected = SELECT_PYORO1;
            else if (selected == SELECT_PYORO2 && (contdata[0].trigger & D_CBUTTONS || contdata[0].trigger & D_JPAD || (contdata->stick_y < -16 && stick_timer < osGetTime())))
                selected = SELECT_MAINCREDITS;
            else if ((selected == SELECT_MAINPLAYER || selected == SELECT_MAINSETTINGS) && (contdata[0].trigger & U_CBUTTONS || contdata[0].trigger & U_JPAD || (contdata->stick_y > 16 && stick_timer < osGetTime())))
                selected = SELECT_PYORO1;
            else if (selected == SELECT_MAINCREDITS && (contdata[0].trigger & U_CBUTTONS || contdata[0].trigger & U_JPAD || (contdata->stick_y > 16 && stick_timer < osGetTime())))
                selected = SELECT_PYORO2;
            else if ((selected == SELECT_MAINPLAYER || selected == SELECT_MAINSETTINGS) && (contdata[0].trigger & R_CBUTTONS || contdata[0].trigger & R_JPAD || (contdata->stick_x > 16 && stick_timer < osGetTime())))
                selected++;
            else if ((selected == SELECT_MAINSETTINGS || selected == SELECT_MAINCREDITS) && (contdata[0].trigger & L_CBUTTONS || contdata[0].trigger & L_JPAD || (contdata->stick_x < -16 && stick_timer < osGetTime())))
                selected--;
            
            // Play a sound and make the stick wait
            if (oldselected != selected)
            {
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                if (contdata->stick_x > 16 || contdata->stick_x < -16)
                    stick_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }
            
            // Decide what to do if the player pressed A/Start
            if (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON)
            {
                if (selected == SELECT_PYORO1)
                {
                    menu_state = MENUSTATE_GAMESTART;
                    sndHandle = nuAuStlSndPlayerPlay(SND_GAME_START);
                    menu_state_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
                }
                else if (selected == SELECT_PYORO2)
                {
                    if (global_save_unlocks[SAVE_UNLOCK_PYORO2] == 1)
                    {
                        menu_state = MENUSTATE_GAMESTART;
                        sndHandle = nuAuStlSndPlayerPlay(SND_GAME_START);
                        menu_state_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
                    }
                    else
                        sndHandle = nuAuStlSndPlayerPlay(SND_MENU_BACK);
                    
                }
                else if (selected == SELECT_MAINPLAYER)
                {
                    if (CheckBit(contPattern,1))
                    {
                        if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0)
                            global_save_settings[SAVE_SETTINGS_PLAYER] = 1;
                        else
                            global_save_settings[SAVE_SETTINGS_PLAYER] = 0;
                            
                        if (global_eeprom_loaded == 1)
                            nuEepromWrite(SAVESLOT_SETTINGS, global_save_settings, 1);
                        sndHandle = nuAuStlSndPlayerPlay(SND_MENU_SELECT);
                    }
                    else
                        sndHandle = nuAuStlSndPlayerPlay(SND_MENU_BACK);
                    
                }
                else if (selected == SELECT_MAINSETTINGS)
                {
                    menu_state = MENUSTATE_GOTOSETTINGS;
                    sndHandle = nuAuStlSndPlayerPlay(SND_MENU_SELECT);
                }
                else if (selected == SELECT_MAINCREDITS)
                {
                    menu_state = MENUSTATE_GOTOCREDITS;
                    sndHandle = nuAuStlSndPlayerPlay(SND_MENU_SELECT);
                }
                else
                {
                    sndHandle = nuAuStlSndPlayerPlay(SND_MENU_SELECT);
                }
            }
            break;
        }
        case MENUSTATE_READYCREDITS:
        {
            if (contdata[0].trigger & A_BUTTON || contdata[0].trigger & B_BUTTON || contdata[0].trigger & START_BUTTON)
            {
                menu_state = MENUSTATE_RETURNCREDITS;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_BACK);
            }
            break;
        }
        case MENUSTATE_READYSETTINGS:
        {
            char oldselected = selected;
            
            // Choose what to do depending on what is currently selected
            if (selected < SELECT_SETTINGSRESET && (contdata[0].trigger & D_CBUTTONS || contdata[0].trigger & D_JPAD || (contdata->stick_y < -16 && stick_timer < osGetTime())))
                selected++;
            else if (selected > SELECT_SETTINGSBACK && (contdata[0].trigger & U_CBUTTONS || contdata[0].trigger & U_JPAD || (contdata->stick_y > 16 && stick_timer < osGetTime())))
                selected--;
            
            // Back button
            if ((selected == SELECT_SETTINGSBACK && (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON)) || contdata[0].trigger & B_BUTTON)
            {
                menu_state = MENUSTATE_RETURNSETTINGS;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_BACK);
                nuEepromWrite(SAVESLOT_SETTINGS, global_save_settings, 1);
            }
            
            // Reset button
            if (selected == SELECT_SETTINGSRESET && (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON))
            {
                menu_state = MENUSTATE_GOTORESET;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_SELECT);
            }
            
            // Controls button
            if (selected == SELECT_SETTINGSCONTROLS && (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON))
            {
                menu_state = MENUSTATE_GOTOCONTROLS;
                
                // Create some objects for the player to test his controls
                GameScore = 0;
                if (CheckBit(contPattern,1))
                {
                    int index = instance_create(OBJ_PYORO, button_controls_x+16, button_controls_y+184-12); // Player 2
                    char value = 1;
                    instance_create(OBJ_PYORO, button_controls_x-16, button_controls_y+184-12); // Player 1
                    if (index != -1)
                        instance_set(index, SET_PLAYER, &value);
                }
                else
                    instance_create(OBJ_PYORO, button_controls_x, button_controls_y+184-12);
                    
                for (i=-64;i<=64;i+=8)
                    instance_create(OBJ_BLOCK,  button_controls_x+i,  button_controls_y+184);
                    
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_SELECT);
            }
            
            // Sound Volume slider
            if (selected == SELECT_SETTINGSSOUND && (contdata[0].trigger & R_CBUTTONS || contdata[0].trigger & R_JPAD || (contdata->stick_x > 16 && stick_timer < osGetTime())))
            {
                if ((global_save_settings[SAVE_SETTINGS_VOLUME] & 0x0F) > 0)
                    global_save_settings[SAVE_SETTINGS_VOLUME]--;
                nuAuStlSndPlayerSetMasterVol(0x7FFF*((float)(15 - (global_save_settings[SAVE_SETTINGS_VOLUME] & 0x0F))/15));
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                stick_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }
            else if (selected == SELECT_SETTINGSSOUND && (contdata[0].trigger & L_CBUTTONS || contdata[0].trigger & L_JPAD || (contdata->stick_x < -16 && stick_timer < osGetTime())))
            {
                if ((global_save_settings[SAVE_SETTINGS_VOLUME] & 0x0F) < 15)
                    global_save_settings[SAVE_SETTINGS_VOLUME]++;
                nuAuStlSndPlayerSetMasterVol(0x7FFF*((float)(15 - (global_save_settings[SAVE_SETTINGS_VOLUME] & 0x0F))/15));
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                stick_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }
            
            // Music Volume slider
            if (selected == SELECT_SETTINGSMUSIC && (contdata[0].trigger & R_CBUTTONS || contdata[0].trigger & R_JPAD || (contdata->stick_x > 16 && stick_timer < osGetTime())))
            {
                if ((global_save_settings[SAVE_SETTINGS_VOLUME] >> 4) > 0)
                    global_save_settings[SAVE_SETTINGS_VOLUME] -= 0x10;
                MusSetMasterVolume(MUSFLAG_SONGS, 0x7FFF*((float)(15 - (global_save_settings[SAVE_SETTINGS_VOLUME] >> 4))/15));
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                stick_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }
            else if (selected == SELECT_SETTINGSMUSIC && (contdata[0].trigger & L_CBUTTONS || contdata[0].trigger & L_JPAD || (contdata->stick_x < -16 && stick_timer < osGetTime())))
            {
                if ((global_save_settings[SAVE_SETTINGS_VOLUME] >> 4) < 15)
                    global_save_settings[SAVE_SETTINGS_VOLUME] += 0x10;
                MusSetMasterVolume(MUSFLAG_SONGS, 0x7FFF*((float)(15 - (global_save_settings[SAVE_SETTINGS_VOLUME] >> 4))/15));
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                stick_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }

            // Play a sound and make the stick wait
            if (oldselected != selected)
            {
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                if (contdata->stick_y > 16 || contdata->stick_y < -16)
                    stick_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }
            break;
        }
        case MENUSTATE_READYRESET:
        {
            char oldselected = selected;
            
            // Choose what to do depending on what is currently selected
            if (selected == SELECT_RESETYES && (contdata[0].trigger & R_CBUTTONS || contdata[0].trigger & R_JPAD || (contdata->stick_x > 16 && stick_timer < osGetTime())))
                selected = SELECT_RESETNO;
            else if (selected == SELECT_RESETNO && (contdata[0].trigger & L_CBUTTONS || contdata[0].trigger & L_JPAD || (contdata->stick_x < -16 && stick_timer < osGetTime())))
                selected = SELECT_RESETYES;
        
            // Decide what to do if the player pressed A/Start
            if (selected == SELECT_RESETYES && (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON))
            {
                menu_state = MENUSTATE_RETURNRESET;
                SetDefaultSave();
                nuAuStlSndPlayerSetMasterVol(0x7FFF);
                MusSetMasterVolume(MUSFLAG_SONGS, 0x7FFF);
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_SELECT);
            }
            else if ((selected == SELECT_RESETNO && (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON)) || contdata[0].trigger & B_BUTTON )
            {
                menu_state = MENUSTATE_RETURNRESET;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_BACK);
            }
            
            // Play a sound and make the stick wait
            if (oldselected != selected)
            {
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                if (contdata->stick_y > 16 || contdata->stick_y < -16)
                    stick_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }
            break;
        }
        case MENUSTATE_READYCONTROLS:
        {
            char oldselected = selected;
            char oldselectedp2 = selectedp2;
            
            // Update the objects
            instance_update_all();
            
            // Player 1
            if (selected < SELECT_CONTROLSP1TEST && (contdata[0].trigger & D_CBUTTONS || contdata[0].trigger & D_JPAD || (contdata->stick_y < -16 && stick_timer < osGetTime())))
            {
                selected++;
                if (selected == SELECT_CONTROLSP1TEST)
                {
                    char value = 0;
                    int index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &value);
                    value = 0;
                    instance_set(index, SET_FROZEN, &value);
                }
            }
            else if (selected > SELECT_CONTROLSBACK && (contdata[0].trigger & U_CBUTTONS || contdata[0].trigger & U_JPAD || (contdata->stick_y > 16 && stick_timer < osGetTime())))
            {
                if (selected == SELECT_CONTROLSP1TEST)
                {
                    char value = 0;
                    int index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &value);
                    value = 1;
                    instance_set(index, SET_FROZEN, &value);
                }      
                selected--;          
            }
                
            // Player 2
            if (CheckBit(contPattern,1))
            {
                if (selectedp2 < SELECT_CONTROLSP2TEST && (contdata[1].trigger & D_CBUTTONS || contdata[1].trigger & D_JPAD || (contdata[1].stick_y < -16 && stick2_timer < osGetTime())))
                {
                    selectedp2++;
                    if (selectedp2 == SELECT_CONTROLSP2TEST)
                    {
                        char value = 1;
                        int index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &value);
                        value = 0;
                        instance_set(index, SET_FROZEN, &value);
                    }
                }   
                else if (selectedp2 > SELECT_CONTROLSP2MOVE && (contdata[1].trigger & U_CBUTTONS || contdata[1].trigger & U_JPAD || (contdata[1].stick_y > 16 && stick2_timer < osGetTime())))
                {
                    if (selectedp2 == SELECT_CONTROLSP2TEST)
                    {
                        char value = 1;
                        int index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &value);
                        value = 1;
                        instance_set(index, SET_FROZEN, &value);
                    }      
                    selectedp2--;          
                }
            }
            
            // Choose what to do depending on what is currently selected (player 1)
            if ((selected == SELECT_CONTROLSBACK && (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON)) || ((contdata[0].trigger & B_BUTTON) && selected != SELECT_CONTROLSP1TEST))
            {
                menu_state = MENUSTATE_RETURNCONTROLS;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_BACK);
            }
            else if (selected == SELECT_CONTROLSP1MOVE && (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON))
            {
                if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x00)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x01;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x01)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x01;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x02 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) != 0x0C)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x01;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x02)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] -= 0x02;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x03)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] -= 0x03;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
            }
            else if (selected == SELECT_CONTROLSP1ATTACK && (contdata[0].trigger & A_BUTTON || contdata[0].trigger & START_BUTTON))
            {
                if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x00)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x04;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x04)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x04;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x08 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) != 0x03)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x04;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x08)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] -= 0x08;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x0C)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] -= 0x0C;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
            }
            
            // Choose what to do depending on what is currently selected (player 2)
            if (selectedp2 == SELECT_CONTROLSP2MOVE && (contdata[1].trigger & A_BUTTON || contdata[1].trigger & START_BUTTON))
            {
                if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x00)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x10;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x10)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x10;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x20 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) != 0xC0)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x10;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x20)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] -= 0x20;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x30)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] -= 0x30;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
            }
            else if (selectedp2 == SELECT_CONTROLSP2ATTACK && (contdata[1].trigger & A_BUTTON || contdata[1].trigger & START_BUTTON))
            {
                if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0x00)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x40;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0x40)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x40;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0x80 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) != 0x30)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] += 0x40;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0x80)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] -= 0x80;
                else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0xC0)
                    global_save_settings[SAVE_SETTINGS_CONTROLS] -= 0xC0;
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
            }
                        
            // Play a sound and make the stick wait (player 1)
            if (oldselected != selected)
            {
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                if (contdata->stick_y > 16 || contdata->stick_y < -16)
                    stick_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }
            
            // Play a sound and make the stick wait (player 2)
            if (CheckBit(contPattern,1) && oldselectedp2 != selectedp2)
            {
                sndHandle = nuAuStlSndPlayerPlay(SND_MENU_CHANGE);
                if (contdata[1].stick_y > 16 || contdata[1].stick_y < -16)
                    stick2_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(200000);
            }
            break;
        }
    } 
    
    // Decide what animations to do in the main screen
    if (menu_state == MENUSTATE_SHOWLOGO && menu_state_timer < osGetTime())
        menu_state = MENUSTATE_STARTMOVE;
    else if (menu_state == MENUSTATE_STARTMOVE && logo_y > 64 && logo_y != 64)
        logo_y = logo_y - (float)logo_y/25;
    else if (menu_state == MENUSTATE_STARTMOVE)
    {
        logo_y = 64;
        menu_state = MENUSTATE_SECONDMOVE;
        eyes_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(3000000);
    }
    
    // Move stuff
    switch (menu_state)
    {
        case MENUSTATE_SECONDMOVE:
        case MENUSTATE_READYMAIN:
        case MENUSTATE_GAMESTART:
        {
            // Fade background in/out
            if (menu_state != MENUSTATE_SHOWLOGO && menu_state != MENUSTATE_GAMESTART && background_alpha < 255)
                background_alpha +=5;
                
            if (menu_state != MENUSTATE_SHOWLOGO && menu_state != MENUSTATE_GAMESTART && returnmode == RETURN_FADE && frontfadealpha >= 5)
                frontfadealpha -=5;
                
            if (menu_state != MENUSTATE_SHOWLOGO && menu_state != MENUSTATE_GAMESTART && (returnmode == RETURN_GAME1 || returnmode == RETURN_GAME2) && backfadealpha >= 15)
                backfadealpha -=15;
            
            if (menu_state == MENUSTATE_GAMESTART)
            {
                if (backfadealpha < 255)
                    backfadealpha +=15;
                
                if (menu_state_timer < osGetTime())
                {
                    frontfadealpha +=15;
                    MusHandleSetVolume(seqHandle1, ((float)(255-frontfadealpha)/255)*48);
                    
                    // Prevent crashing
                    if (frontfadealpha >= 225)
                        MusHandleStop(seqHandle1, 0);
                        
                    // Go to the next stage
                    if (frontfadealpha == 255)
                    {
                        free(selected_tlut);
                        if (selected == SELECT_PYORO1)
                            global_stage = STAGE_GAME1;
                        else
                            global_stage = STAGE_GAME2;
                        return;
                    }
                }
            }
            
            // Move the Pyoro 1 + 2 Button
            if (menu_state > MENUSTATE_SHOWLOGO && menu_state != MENUSTATE_GAMESTART) 
            {
                if (returnmode == RETURN_LOGO)
                {
                    float i = (float)(66  - button_pyoro1_x)/20;
                    button_pyoro1_x += i;
                    if (button_pyoro1_x > 36)
                    {
                        button_pyoro1_x = 36;
                        menu_state = MENUSTATE_READYMAIN;
                    }
                    button_pyoro2_x = SCREEN_WD - (button_pyoro1_x+108);
                }
                else if (returnmode == RETURN_GAME1)
                {
                    float i = (float)(26 - button_pyoro1_x)/20;
                    float j = (float)(105 - button_pyoro1_y)/20;
                    button_pyoro1_x += i;
                    button_pyoro1_y += j;
                    
                    if (button_pyoro1_y > 84)
                        button_pyoro1_y = 84;
                        
                    if (button_pyoro1_x < 36)
                    {
                        button_pyoro1_x = 36;
                        menu_state = MENUSTATE_READYMAIN;
                    }             
                }
                else if (returnmode == RETURN_GAME2)
                {
                    float i = (float)(SCREEN_WD - 105 - button_pyoro2_x)/20;
                    float j = (float)(105 - button_pyoro2_y)/20;
                    button_pyoro2_x += i;
                    button_pyoro2_y += j;
                    
                    if (button_pyoro2_y > 84)
                        button_pyoro2_y = 84;
                        
                    if (button_pyoro2_x > SCREEN_WD - 144)
                    {
                        button_pyoro2_x = SCREEN_WD - 144;
                        menu_state = MENUSTATE_READYMAIN;
                    }             
                }
                else if (returnmode == RETURN_FADE && frontfadealpha == 0)
                    menu_state = MENUSTATE_READYMAIN;
            }
            if (menu_state == MENUSTATE_GAMESTART) 
            {
                int* button_to_move_x;
                int* button_to_move_y;
                float i;
                float j;
                
                // Choose what variable to modify depending on what's selected
                if (selected == SELECT_PYORO1)
                {
                    button_to_move_x = &button_pyoro1_x;
                    button_to_move_y = &button_pyoro1_y;
                }
                else
                {
                    button_to_move_x = &button_pyoro2_x;
                    button_to_move_y = &button_pyoro2_y;
                }
                
                // Move the button on x and y
                i = ((float)((SCREEN_WD/2) - ((*button_to_move_x)+54)))/5;
                j = ((float)((SCREEN_HT/2) - ((*button_to_move_y)+50)))/10;
                *button_to_move_x += i;
                *button_to_move_y += j;
            }
            
            // Move the small buttons
            if (menu_state > MENUSTATE_SHOWLOGO && button_main_y > 193) 
            {
                float i = (float)(SCREEN_HT+50  - button_main_y)/30;
                button_main_y -= i;
                if (button_main_y < 193)
                {
                    button_main_y = 193;
                }
            }
            break;
        }
        case MENUSTATE_GOTOCREDITS:
        {
            logo_x -= 20;
            button_pyoro1_x -= 20;
            button_pyoro2_x -= 20;
            button_main_x -= 20;
            button_credits_x -= 20;
            if (logo_x < -(SCREEN_WD/2))
            {
                menu_state = MENUSTATE_READYCREDITS;
                selected = SELECT_CREDITSBACK;
            }
            break;
        }
        case MENUSTATE_RETURNCREDITS:
        {
            logo_x += 20;
            button_pyoro1_x += 20;
            button_pyoro2_x += 20;
            button_main_x += 20;
            button_credits_x += 20;
            if (logo_x == SCREEN_WD/2)
            {
                menu_state = MENUSTATE_READYMAIN;
                selected = SELECT_MAINCREDITS;
            }
            break;
        }
        case MENUSTATE_GOTOSETTINGS:
        {
            logo_y -= 20;
            button_pyoro1_y -= 20;
            button_pyoro2_y -= 20;
            button_main_y -= 20;
            button_settings_y -= 20;
            if (logo_y < -SCREEN_HT+64)
            {
                menu_state = MENUSTATE_READYSETTINGS;
                selected = SELECT_SETTINGSBACK;
            }
            break;
        }
        case MENUSTATE_RETURNSETTINGS:
        {
            logo_y += 20;
            button_pyoro1_y += 20;
            button_pyoro2_y += 20;
            button_main_y += 20;
            button_settings_y += 20;
            if (logo_y == 64)
            {
                menu_state = MENUSTATE_READYMAIN;
                selected = SELECT_MAINSETTINGS;
            }
            break;
        }
        case MENUSTATE_GOTORESET:
        {
            button_settings_y -= 20;
            button_reset_y -= 20;
            if (button_settings_y <= -SCREEN_HT-12)
            {
                menu_state = MENUSTATE_READYRESET;
                selected = SELECT_RESETNO;
            }
            break;
        }
        case MENUSTATE_RETURNRESET:
        {
            button_settings_y += 20;
            button_reset_y += 20;
            if (button_settings_y >= 12)
            {
                menu_state = MENUSTATE_READYSETTINGS;
                selected = SELECT_SETTINGSRESET;
            }
            break;
        }
        case MENUSTATE_GOTOCONTROLS:
        {
            button_settings_x -= 20;
            button_controls_x -= 20;
            for (i=0;i<MAXOBJECTS;i++)
                if (GameObjects[i].type != OBJ_NULL)
                {
                    float xpos = (*(float*)GameObjects[i].Get(&GameObjects[i], GET_X)) - 20;
                    GameObjects[i].Set(&GameObjects[i], SET_X, &xpos);
                }

            if (button_settings_x <= -SCREEN_WD/2)
            {
                char value = 0;
                int index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &value);
                value = 1;
                instance_set(index, SET_FROZEN, &value);
                menu_state = MENUSTATE_READYCONTROLS;
                selected = SELECT_CONTROLSBACK;
                selectedp2 = SELECT_CONTROLSP2MOVE;
                if (CheckBit(contPattern, 1))
                {
                    value = 1;
                    index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &value);
                    value = 1;
                    instance_set(index, SET_FROZEN, &value);
                }
            }
            break;
        }
        case MENUSTATE_RETURNCONTROLS:
        {
            button_settings_x += 20;
            button_controls_x += 20;
            for (i=0;i<MAXOBJECTS;i++)
                if (GameObjects[i].type != OBJ_NULL)
                {
                    float xpos = (*(float*)GameObjects[i].Get(&GameObjects[i], GET_X)) + 20;
                    GameObjects[i].Set(&GameObjects[i], SET_X, &xpos);
                }
            if (button_settings_x >= SCREEN_WD/2)
            {
                menu_state = MENUSTATE_READYSETTINGS;
                selected = SELECT_SETTINGSCONTROLS;
                instance_destroy_all();
            }
            break;
        }
    }
    
    // Change the logo's eyes
    if (menu_state > MENUSTATE_STARTMOVE && eyes_timer < osGetTime())
    {
        if (logo_eyes == 0 || logo_eyes == 2 || logo_eyes == 4 || logo_eyes == 6 || logo_eyes == 8 || logo_eyes == 10 || logo_eyes == 12)
        {
            logo_eyes++;
            eyes_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(50000);
        }
        else if (logo_eyes == 1 || logo_eyes == 5)
        {
            logo_eyes++;
            eyes_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
        }
        else if (logo_eyes == 3 || logo_eyes == 7 || logo_eyes == 9 || logo_eyes == 11 || logo_eyes == 13 || logo_eyes == 14)
        {
            logo_eyes++;
            eyes_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(100000);
        }
        else if (logo_eyes == 15)
        {
            logo_eyes=0;
            eyes_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(7000000);
        }
    }
}


/*=================================
         stage_menu_draw
          Draw the stage
=================================*/

void stage_menu_draw(void)
{
    // Assign our glist pointer to our glist array for ease of access
    glistp = glist;
    #if (DEBUG_MODE == 1)
        glistbefore = glistp;
    #endif
    
    // Initialize the RCP
    RCPInit(glistp);

    // Wipe the background with a color
    ClearBackground(0, 0, 0);
    
    // Draw the background
    DrawBackground();
    
    // Draw the menu
    if (menu_state >= MENUSTATE_GOTOCREDITS && menu_state <= MENUSTATE_RETURNCREDITS)
    {
        DrawSmallButton(button_credits_x, button_credits_y, SELECT_CREDITSBACK);
        global_text_sprite = fnt_text_outlined;
        global_text_tlut = fnt_text_outlined_tlut;
        DrawText(button_credits_x+104, button_credits_y-90, "Programmer\nBuu342\n\nBeta testers\nRocky\nDarklink623\nGCaldL\n\nPyoro assets created by Nintendo\n\nHuge thanks to the N64Brew\ncommunity for being awesome, kind,\n and incredibly helpful!\nI'd love to name all the great folk\nthere but 240 pixels isn't enough\nto fit them all :(", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        global_text_tlut = fnt_text_tlut_white;
        global_text_sprite = fnt_text;
    }
    if ((menu_state >= MENUSTATE_GOTOSETTINGS && menu_state <= MENUSTATE_RETURNSETTINGS) || (menu_state >= MENUSTATE_GOTORESET && menu_state <= MENUSTATE_RETURNCONTROLS && menu_state != MENUSTATE_READYRESET && menu_state != MENUSTATE_READYCONTROLS))
    {
        DrawSmallButton(button_settings_x, button_settings_y, SELECT_SETTINGSBACK);
        DrawSlider(button_settings_x, button_settings_y+45+31, SELECT_SETTINGSSOUND);
        DrawSlider(button_settings_x, button_settings_y+45*2+31, SELECT_SETTINGSMUSIC);
        DrawSmallButton(button_settings_x, button_settings_y+45*3, SELECT_SETTINGSCONTROLS);
        DrawSmallButton(button_settings_x, button_settings_y+45*4, SELECT_SETTINGSRESET);
    }
    if (menu_state >= MENUSTATE_GOTORESET && menu_state <= MENUSTATE_RETURNRESET)
    {
        global_text_sprite = fnt_text_outlined;
        global_text_tlut = fnt_text_outlined_tlut;
        DrawText(button_reset_x, button_reset_y+11, "Reset your save?\nYou cannot undo this!", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        global_text_tlut = fnt_text_tlut_white;
        global_text_sprite = fnt_text;
        DrawSmallButton(button_reset_x-48, button_reset_y+43, SELECT_RESETYES);
        DrawSmallButton(button_reset_x+48, button_reset_y+43, SELECT_RESETNO);    
    }
    if (menu_state >= MENUSTATE_GOTOCONTROLS && menu_state <= MENUSTATE_RETURNCONTROLS)
    {
        DrawSmallButton(button_controls_x, button_controls_y-16, SELECT_CONTROLSBACK);
        DrawSmallButton(button_controls_x-96, button_controls_y+38, SELECT_CONTROLSP1MOVE);
        DrawSmallButton(button_controls_x-96, button_controls_y+76, SELECT_CONTROLSP1ATTACK);
        DrawSmallButton(button_controls_x-96, button_controls_y+114, SELECT_CONTROLSP1TEST);
        DrawSmallButton(button_controls_x+96, button_controls_y+38, SELECT_CONTROLSP2MOVE);
        DrawSmallButton(button_controls_x+96, button_controls_y+76, SELECT_CONTROLSP2ATTACK);
        DrawSmallButton(button_controls_x+96, button_controls_y+114, SELECT_CONTROLSP2TEST);
        global_text_sprite = fnt_text_outlined;
        global_text_tlut = fnt_text_outlined_tlut;
        DrawText(button_controls_x-96, button_controls_y+28, "Player 1", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        DrawText(button_controls_x+96, button_controls_y+28, "Player 2", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        DrawText(button_controls_x, button_controls_y+56, "Movement", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        DrawText(button_controls_x, button_controls_y+56+38, "Attack", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        DrawText(button_controls_x, button_controls_y+56+76, "Test controls", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        global_text_tlut = fnt_text_tlut_white;
        global_text_sprite = fnt_text;
        instance_draw_all();
    }
    
    if (menu_state != MENUSTATE_READYCREDITS && menu_state != MENUSTATE_READYSETTINGS && menu_state <= MENUSTATE_RETURNSETTINGS)
    {
        // Main menu buttons
        DrawSmallButton(button_main_x - 86, button_main_y, SELECT_MAINPLAYER);
        DrawSmallButton(button_main_x     , button_main_y, SELECT_MAINSETTINGS);
        DrawSmallButton(button_main_x + 86, button_main_y, SELECT_MAINCREDITS);
        
        // Pyoro64 Logo
        DrawLogo();
        
        // Since we're using XLU Surf, we need to swap the order of the Pyoro buttons to render things properly
        if (selected == SELECT_PYORO1)
        {
            DrawButtonPyoro2();
            DrawFade(backfadealpha);
            DrawButtonPyoro1();
        }
        else
        {
            DrawButtonPyoro1();
            DrawFade(backfadealpha);
            DrawButtonPyoro2();
        }
        
        // Draw some text if we lost a game recently
        if (returnmessage[0] != '\0' && menu_state == MENUSTATE_SHOWLOGO)
        {
            global_text_sprite = fnt_text;
            global_text_tlut = fnt_text_tlut_white;
            global_text_alpha = 255;
            DrawText(SCREEN_WD/2, SCREEN_HT/2+50, returnmessage, TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
        }
        
        DrawFade(frontfadealpha);
    }
    
    #if (DEBUG_MODE == 1)
        DrawDebugInfo();
    #endif
    
    // Syncronize the RCP and CPU
    gDPFullSync(glistp++);
    
    // Specify that our display list has ended
    gSPEndDisplayList(glistp++);
    
    // Start the display task
    nuGfxTaskStart(glist, (s32)(glistp - glist) * sizeof(Gfx), NU_GFX_UCODE_F3DEX, NU_SC_SWAPBUFFER);
}


/*=================================
          DrawBackground
Draw a tiling, scrolling background
=================================*/

void DrawBackground()
{
    static float time=0;
    time += 0.35;
    
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetPrimColor(glistp++,0,0,255,255,255,background_alpha);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    
    gDPLoadTLUT_pal16(glistp++, 0, bac_menu_back_tlut);
    gDPLoadTextureBlock_4b(glistp++, bac_menu_back, G_IM_FMT_CI, 16, 16, 0, G_TX_WRAP, G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, 0 << 2, 0 << 2, SCREEN_WD << 2, SCREEN_HT << 2,  G_TX_RENDERTILE, ((int)(16-time))%16 << 5, ((int)(16-time))%16 << 5,  1 << 10, 1 << 10);
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}


/*=================================
             DrawLogo
        Draw the Pyoro logo
=================================*/

void DrawLogo()
{
    int i, j;
    int offsetx, offsety;
    int x = logo_x;
    int y = logo_y;
    unsigned int * sprite[2][6] = {
        {spr_logo_1,  spr_logo_2,  spr_logo_3,  spr_logo_4,  spr_logo_5,  spr_logo_6 },
        {spr_logo_7,  spr_logo_8,  spr_logo_9,  spr_logo_10, spr_logo_11, spr_logo_12}
    };
    
    offsetx = -80;
    offsety = -32;
    
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
    gDPSetPrimColor(glistp++,0,0,255,255,255,255);

    for (i=0;i<2;i++)
    {
        for (j=0;j<6;j++)
        {
            if (x+offsetx-16 > SCREEN_WD || x+offsetx+16 < 0)
            {
                offsetx += 32;
                continue;
            }
            if (y+offsety-16 > SCREEN_HT || y+offsety+16 < 0)
            {
                offsetx += 32;
                continue;
            }
            gDPLoadTextureBlock(glistp++, sprite[i][j], G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, x+offsetx-16 << 2, y+offsety-16 << 2, x+offsetx+16 << 2, y+offsety+16 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5,  1 << 10, 1 << 10);
            offsetx += 32;
        }
        offsetx -= 32*j;
        offsety += 32;
    }
    
    
    // Draw the 64
    sprite[0][0] = spr_logo_64_1;
    sprite[0][1] = spr_logo_64_2;
    offsetx = 68;
    offsety = -32;
    for (i=0;i<2;i++)
    {
        gDPLoadTextureBlock(glistp++, sprite[0][i], G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, x+offsetx-16 << 2, y+offsety-16 << 2, x+offsetx+16 << 2, y+offsety+16 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5,  1 << 10, 1 << 10);
        offsetx += 32;
    }
    
    gDPPipeSync(glistp++);
    
    // Draw the eyes
    if (logo_eyes != 0)
    {
        char* sprite;
        if (logo_eyes == 1 || logo_eyes == 15)
            sprite = spr_logo_eyes1;
        else if (logo_eyes == 2 || logo_eyes == 6 || logo_eyes == 10 || logo_eyes == 14)
            sprite = spr_logo_eyes2;
        else if (logo_eyes == 3 || logo_eyes == 5 || logo_eyes == 7 || logo_eyes == 9 || logo_eyes == 11 || logo_eyes == 13)
            sprite = spr_logo_eyes3;
        else if (logo_eyes == 4 || logo_eyes == 8 || logo_eyes == 12)
            sprite = spr_logo_eyes4;
        gDPSetTextureLUT(glistp++, G_TT_RGBA16);
        gDPLoadTLUT_pal16(glistp++, 0, spr_logo_eyes_tlut);
        gDPLoadMultiTile_4b(glistp++, sprite, 0, G_TX_RENDERTILE, G_IM_FMT_CI, 20, 16, 0, 0, 20 - 1, 16, 0, G_TX_CLAMP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, x-70-10 << 2, y-27-8 << 2, x-70+10 << 2, y-27+8 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5,  1 << 10, 1 << 10);   
        gDPPipeSync(glistp++);
        gDPSetTextureLUT(glistp++, G_TT_NONE);
    }
}


/*=================================
          DrawButtonPyoro1
      Draw the Pyoro 1 Button
=================================*/

void DrawButtonPyoro1()
{
    int i, j;
    int offsetx, offsety;
    char conbuff1[7];
    char conbuff2[2];
    int score;
    unsigned char* pyoro_sprite;

    unsigned char * sprite[4][4] = {
        {spr_menu_button_unlock_1,  spr_menu_button_unlock_2,  spr_menu_button_unlock_3,  spr_menu_button_unlock_4 },
        {spr_menu_button_unlock_5,  spr_menu_button_unlock_6,  spr_menu_button_unlock_7,  spr_menu_button_unlock_8 },
        {spr_menu_button_unlock_9,  spr_menu_button_unlock_10, spr_menu_button_unlock_11, spr_menu_button_unlock_12},
        {spr_menu_button_unlock_13, spr_menu_button_unlock_14, spr_menu_button_unlock_15, spr_menu_button_unlock_16}
    };
    unsigned short * sprite2[3][3] = {
        {spr_buttonicon_1_1, spr_buttonicon_1_2, spr_buttonicon_1_3 },
        {spr_buttonicon_1_4, spr_buttonicon_1_5, spr_buttonicon_1_6 },
        {spr_buttonicon_1_7, spr_buttonicon_1_8, spr_buttonicon_1_9 }
    };
    
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
    gDPSetPrimColor(glistp++,0,0,255,255,255,255);
        
    gDPLoadTLUT_pal16(glistp++, 0, spr_menu_button_unlock_tlut_unlocked);
    offsetx = -64;
    offsety = -64;
    for (i=0;i<4;i++)
    {
        for (j=0;j<4;j++)
        {
            if (button_pyoro1_x+64+offsetx > SCREEN_WD || button_pyoro1_x+64+offsetx + 32 < 0)
            {
                offsetx += 32;
                continue;
            }
            if (button_pyoro1_y+64+offsety > SCREEN_HT || button_pyoro1_y+offsety+64+32 < 0)
            {
                offsetx += 32;
                continue;
            }
            gDPLoadTextureBlock_4b(glistp++, sprite[i][j], G_IM_FMT_CI, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, button_pyoro1_x+64+offsetx << 2, button_pyoro1_y+64+offsety << 2, button_pyoro1_x+64+offsetx + 32 << 2, button_pyoro1_y+offsety+64+32 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
            offsetx += 32;
        }
        offsetx -= 32*j;
        offsety += 32;
    }
    
    gDPPipeSync(glistp++);
    
    if (selected == SELECT_PYORO1 && menu_state == MENUSTATE_READYMAIN)
    {
        gDPLoadTLUT_pal16(glistp++, 0, selected_tlut);
        
        gDPLoadMultiTile_4b(glistp++, spr_menu_button_select, 0, G_TX_RENDERTILE, G_IM_FMT_CI, 16, 16, 0, 0, 16, 16, 0, G_TX_MIRROR, G_TX_MIRROR, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, button_pyoro1_x-4<<2, button_pyoro1_y-4<<2, button_pyoro1_x+12<<2, button_pyoro1_y+12<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, button_pyoro1_x+108-12<<2, button_pyoro1_y-4<<2, button_pyoro1_x+108+4<<2, button_pyoro1_y+12<<2, G_TX_RENDERTILE, 16 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, button_pyoro1_x-4<<2, button_pyoro1_y+101-17<<2, button_pyoro1_x+12<<2, button_pyoro1_y+101-1<<2, G_TX_RENDERTILE, 0 << 5, 16 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, button_pyoro1_x+108-12<<2, button_pyoro1_y+101-17<<2, button_pyoro1_x+108+4<<2, button_pyoro1_y+101-1<<2, G_TX_RENDERTILE, 16 << 5, 16 << 5, 1 << 10, 1 << 10 );
    }
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE); 

    offsetx = -54;
    offsety = -54;
    for (i=0;i<3;i++)
    {
        for (j=0;j<3;j++)
        {
            if (button_pyoro1_x+64+offsetx > SCREEN_WD || button_pyoro1_x+64+offsetx + 32 < 0)
            {
                offsetx += 32;
                continue;
            }
            if (button_pyoro1_y+64+offsety > SCREEN_HT || button_pyoro1_y+offsety+64+32 < 0)
            {
                offsetx += 32;
                continue;
            }
            gDPLoadTextureBlock(glistp++, sprite2[i][j], G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, button_pyoro1_x+64+offsetx << 2, button_pyoro1_y+64+offsety << 2, button_pyoro1_x+64+offsetx + 32 << 2, button_pyoro1_y+offsety+64+32 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
            offsetx += 32;
        }
        offsetx -= 32*j;
        offsety += 32;
    }
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16); 
        
    // Draw pyoro
    if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0)
    {
        if (pyoro1_speed > 0.3 || pyoro1_speed < -0.3 || pyoro1_movestotongue == 0)
            pyoro_sprite = spr_pyoro_walk1;
        else if (pyoro1_movestotongue < 0 && pyoro1_movestotongue > -3)
        {
            pyoro_sprite = spr_pyoro_attack;
            gDPLoadTLUT_pal16(glistp++, 0, spr_tongue_tlut_default);
            if (pyoro1_tonguex > 17)
            {
                gDPLoadTextureTile_4b(glistp++, spr_tongue_body, G_IM_FMT_CI, 8, 8, 0, 0, 8, 8, 0, G_TX_MIRROR, G_TX_NOMIRROR, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
                gSPScisTextureRectangle(glistp++, button_pyoro1_x+51-4 << 2, button_pyoro1_y+66-4 << 2, button_pyoro1_x+51+4 << 2, button_pyoro1_y+66+4 << 2,  G_TX_RENDERTILE,  8 << 5, 0 << 5, 1 << 10, 1 << 10);
                if (pyoro1_tonguex > 24)
                    gSPScisTextureRectangle(glistp++, button_pyoro1_x+58-4 << 2, button_pyoro1_y+59-4 << 2, button_pyoro1_x+58+4 << 2, button_pyoro1_y+59+4 << 2,  G_TX_RENDERTILE,  8 << 5, 0 << 5, 1 << 10, 1 << 10);
            }
            gDPLoadTextureBlock_4b(glistp++, spr_tongue, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, button_pyoro1_x+38-8+(int)pyoro1_tonguex << 2, button_pyoro1_y+68-8-((int)pyoro1_tonguex-11) << 2, button_pyoro1_x+38+8+(int)pyoro1_tonguex << 2, button_pyoro1_y+68+8-((int)pyoro1_tonguex-11) << 2,  G_TX_RENDERTILE,  16 << 5, 0 << 5, 1 << 10, 1 << 10);   
        }
        else
            pyoro_sprite = spr_pyoro_walk2;
        
        // Draw Pyoro
        gDPLoadTLUT_pal16(glistp++, 0, spr_pyoro_tlut_default);
        gDPLoadTextureBlock_4b(glistp++, pyoro_sprite, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        if (pyoro1_speed > 0)
        {
            gSPScisTextureRectangle(glistp++, button_pyoro1_x+32-8+(int)pyoro1_posx << 2, button_pyoro1_y+70-8 << 2, button_pyoro1_x+32+8+(int)pyoro1_posx << 2, button_pyoro1_y+70+8 << 2,  G_TX_RENDERTILE,  16 << 5, 0 << 5, 1 << 10, 1 << 10);
        }
        else
            gSPScisTextureRectangle(glistp++, button_pyoro1_x+32-8+(int)pyoro1_posx << 2, button_pyoro1_y+70-8 << 2, button_pyoro1_x+32+8+(int)pyoro1_posx << 2, button_pyoro1_y+70+8 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
    }
    else
    {
        gDPLoadTLUT_pal16(glistp++, 0, spr_pyoro_tlut_default);
        gDPLoadTextureBlock_4b(glistp++, spr_pyoro_walk2, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, button_pyoro1_x+32-8 << 2, button_pyoro1_y+70-8 << 2, button_pyoro1_x+32+8 << 2, button_pyoro1_y+70+8 << 2,  G_TX_RENDERTILE,  16 << 5, 0 << 5, 1 << 10, 1 << 10);
        gDPLoadTLUT_pal16(glistp++, 0, spr_pyoro_tlut_green);
        gDPLoadTextureTile_4b(glistp++, spr_pyoro_hair2_1, G_IM_FMT_CI, 8, 8, 0, 0, 8-1, 8-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, button_pyoro1_x+86-4 << 2, button_pyoro1_y+65-4 << 2, button_pyoro1_x+86+4 << 2, button_pyoro1_y+65+4 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
        gDPLoadTextureBlock_4b(glistp++, spr_pyoro_walk2, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, button_pyoro1_x+80-8 << 2, button_pyoro1_y+70-8 << 2, button_pyoro1_x+80+8 << 2, button_pyoro1_y+70+8 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
    }
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE); 
    
    // Draw the highscore
    score = (global_save_highscores[3] << 24) | (global_save_highscores[2] << 16) | (global_save_highscores[1] << 8) | global_save_highscores[0];
    sprintf(conbuff1, "%06d", score);
    conbuff1[6] = '\0';
    conbuff2[1] = '\0';
    global_text_alpha = 255;
    global_text_tlut = fnt_text_tlut_black;
    for (i=0;i<6;i++)
    {
        float textup;
        if (i < 3)
            textup = sinf((float)pyoro1_textwave/4+(i)%6)*5;
        else
            textup = sinf((float)pyoro1_textwave/4-(i+2)%6)*5;
            
        if (pyoro1_textwave == 0 || textup < 0 || global_save_settings[SAVE_SETTINGS_PLAYER] == 1)
            textup = 0;
        conbuff2[0] = conbuff1[i];
        DrawText(button_pyoro1_x+45+8*i, button_pyoro1_y+37-textup, conbuff2, 0, 0);
    }
    global_text_tlut = fnt_text_tlut_white;
    global_text_alpha = 255;
}


/*=================================
          DrawButtonPyoro2
      Draw the Pyoro 2 Button
=================================*/

void DrawButtonPyoro2()
{
    int i, j;
    int offsetx, offsety;
    char conbuff1[7];
    char conbuff2[2];
    int score;
    unsigned char* pyoro_sprite;
    unsigned char* mouth_sprite = NULL;
    char* accessory_index;
    int accessory_x;
    int accessory_y;
    unsigned char * sprite[4][4] = {
        {spr_menu_button_unlock_1,  spr_menu_button_unlock_2,  spr_menu_button_unlock_3,  spr_menu_button_unlock_4 },
        {spr_menu_button_unlock_5,  spr_menu_button_unlock_6,  spr_menu_button_unlock_7,  spr_menu_button_unlock_8 },
        {spr_menu_button_unlock_9,  spr_menu_button_unlock_10, spr_menu_button_unlock_11, spr_menu_button_unlock_12},
        {spr_menu_button_unlock_13, spr_menu_button_unlock_14, spr_menu_button_unlock_15, spr_menu_button_unlock_16}
    };
    
    unsigned short * sprite2[3][3] = {
        {spr_buttonicon_2_1, spr_buttonicon_2_2, spr_buttonicon_2_3 },
        {spr_buttonicon_2_4, spr_buttonicon_2_5, spr_buttonicon_2_6 },
        {spr_buttonicon_2_7, spr_buttonicon_2_8, spr_buttonicon_2_9 }
    };
        
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
    gDPSetPrimColor(glistp++,0,0,255,255,255,255);
    gDPLoadTLUT_pal16(glistp++, 0, spr_menu_button_unlock_tlut_unlocked);
    
    if (global_save_unlocks[SAVE_UNLOCK_PYORO2] == 0)
        gDPLoadTLUT_pal16(glistp++, 0, spr_menu_button_unlock_tlut_locked);
        
    offsetx = -64;
    offsety = -64;
    for (i=0;i<4;i++)
    {
        for (j=0;j<4;j++)
        {
            if (button_pyoro2_x+64+offsetx > SCREEN_WD || button_pyoro2_x+64+offsetx + 32 < 0)
            {
                offsetx += 32;
                continue;
            }
            if (button_pyoro2_y+64+offsety > SCREEN_HT || button_pyoro2_y+offsety+64+32 < 0)
            {
                offsetx += 32;
                continue;
            }
            gDPLoadTextureBlock_4b(glistp++, sprite[i][j], G_IM_FMT_CI, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, button_pyoro2_x+64+offsetx << 2, button_pyoro2_y+64+offsety << 2, button_pyoro2_x+64+offsetx + 32 << 2, button_pyoro2_y+offsety+64+32 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
            offsetx += 32;
        }
        offsetx -= 32*j;
        offsety += 32;
    }
    
    gDPPipeSync(glistp++);
    
    if (selected == SELECT_PYORO2 && menu_state == MENUSTATE_READYMAIN)
    {
        gDPLoadTLUT_pal16(glistp++, 0, selected_tlut);
        
        gDPLoadMultiTile_4b(glistp++, spr_menu_button_select, 0, G_TX_RENDERTILE, G_IM_FMT_CI, 16, 16, 0, 0, 16, 16, 0, G_TX_MIRROR, G_TX_MIRROR, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, button_pyoro2_x-4<<2, button_pyoro2_y-4<<2, button_pyoro2_x+12<<2, button_pyoro2_y+12<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, button_pyoro2_x+108-12<<2, button_pyoro2_y-4<<2, button_pyoro2_x+108+4<<2, button_pyoro2_y+12<<2, G_TX_RENDERTILE, 16 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, button_pyoro2_x-4<<2, button_pyoro2_y+101-17<<2, button_pyoro2_x+12<<2, button_pyoro2_y+101-1<<2, G_TX_RENDERTILE, 0 << 5, 16 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, button_pyoro2_x+108-12<<2, button_pyoro2_y+101-17<<2, button_pyoro2_x+108+4<<2, button_pyoro2_y+101-1<<2, G_TX_RENDERTILE, 16 << 5, 16 << 5, 1 << 10, 1 << 10 );
    }
    
    gDPPipeSync(glistp++);    
    gDPSetTextureLUT(glistp++, G_TT_NONE);

    // Only draw from here if we've unlocked Pyoro2
    if (global_save_unlocks[SAVE_UNLOCK_PYORO2] == 0)
        return;
        
    offsetx = -54;
    offsety = -54;
    for (i=0;i<3;i++)
    {
        for (j=0;j<3;j++)
        {
            if (button_pyoro2_x+64+offsetx > SCREEN_WD || button_pyoro2_x+64+offsetx + 32 < 0)
            {
                offsetx += 32;
                continue;
            }
            if (button_pyoro2_y+64+offsety > SCREEN_HT || button_pyoro2_y+offsety+64+32 < 0)
            {
                offsetx += 32;
                continue;
            }
            gDPLoadTextureBlock(glistp++, sprite2[i][j], G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, button_pyoro2_x+64+offsetx << 2, button_pyoro2_y+64+offsety << 2, button_pyoro2_x+64+offsetx + 32 << 2, button_pyoro2_y+offsety+64+32 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
            offsetx += 32;
        }
        offsetx -= 32*j;
        offsety += 32;
    }
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16); 
    
    // Draw pyoro
    gDPLoadTLUT_pal16(glistp++, 0, spr_pyoro_tlut_yellow);
    if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0)
    {
        // Decide pyoro and his mouth's sprite
        if (pyoro2_speed > 0.3 || pyoro2_speed < -0.3)
            pyoro_sprite = spr_pyoro_walk1;
        else if (pyoro2_movestoattack < 0)
        {
            switch (pyoro2_movestoattack)
            {
                case -1:
                    pyoro_sprite = spr_pyoro_walk1;
                    break;
                case -2:
                    pyoro_sprite = spr_pyoro_spit1;
                    mouth_sprite = spr_mouth_spit1;
                    break;
                case -3:
                    pyoro_sprite = spr_pyoro_spit2;
                    mouth_sprite = spr_mouth_spit2;
                    break;
                case -4:
                    pyoro_sprite = spr_pyoro_spit3;
                    mouth_sprite = spr_mouth_spit3;
                    break;
                case -5:
                    pyoro_sprite = spr_pyoro_spit4;
                    break;
                case -6:
                case -7:
                    pyoro_sprite = spr_pyoro_walk1;
                    break;
            }

            // Draw the mouth
            if (mouth_sprite != NULL)
            {
                gDPLoadTextureTile_4b(glistp++, mouth_sprite, G_IM_FMT_CI, 8, 16, 0, 0, 8-1, 16-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(glistp++, button_pyoro2_x+80+(int)pyoro2_posx-4-11 << 2, button_pyoro2_y+70-8-8 << 2, button_pyoro2_x+80+(int)pyoro2_posx+4-11 << 2, button_pyoro2_y+70+8-8 << 2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10);
            }
        }
        else
            pyoro_sprite = spr_pyoro_walk2;
            
        // Decide the hair sprite
        if (pyoro_sprite == spr_pyoro_walk1 || pyoro_sprite == spr_pyoro_chew2 || pyoro_sprite == spr_pyoro_spit4)
        {
            accessory_index = spr_pyoro_hair1_2;
            accessory_x = 8;
            accessory_y = -3;
        }
        else if (pyoro_sprite == spr_pyoro_walk2)
        {
            accessory_index = spr_pyoro_hair1_1;
            accessory_x = 8;
            accessory_y = -3;
        }
        else if (pyoro_sprite == spr_pyoro_spit1)
        {
            accessory_index = spr_pyoro_hair1_1;
            accessory_x = 5;
            accessory_y = -6;
        }
        else if (pyoro_sprite == spr_pyoro_spit2)
        {
            accessory_index = spr_pyoro_hair1_1;
            accessory_x = 5;
            accessory_y = -5;
        }
        else if (pyoro_sprite == spr_pyoro_spit3)
        {
            accessory_index = spr_pyoro_hair1_2;
            accessory_x = 5;
            accessory_y = -4;
        }
        else
        {
            accessory_index = spr_pyoro_hair1_1;
            accessory_x = 0;
            accessory_y = 0;
        }
        
        
        // Draw Pyoro
        if (pyoro2_speed > 0)
        {
            gDPLoadTextureTile_4b(glistp++, accessory_index, G_IM_FMT_CI, 8, 8, 0, 0, 8-1, 8-1, 0, G_TX_MIRROR, G_TX_WRAP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, button_pyoro2_x+64+(int)pyoro2_posx+accessory_x-4 << 2, button_pyoro2_y+70+accessory_y-4 << 2, button_pyoro2_x+64+(int)pyoro2_posx+accessory_x+4 << 2, button_pyoro2_y+70+accessory_y+4 << 2,  G_TX_RENDERTILE,  8 << 5, 0 << 5, 1 << 10, 1 << 10);
        
            gDPLoadTextureBlock_4b(glistp++, pyoro_sprite, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, button_pyoro2_x+80-8+(int)pyoro2_posx << 2, button_pyoro2_y+70-8 << 2, button_pyoro2_x+80+8+(int)pyoro2_posx << 2, button_pyoro2_y+70+8 << 2,  G_TX_RENDERTILE,  16 << 5, 0 << 5, 1 << 10, 1 << 10);
        }
        else
        {
            gDPLoadTextureTile_4b(glistp++, accessory_index, G_IM_FMT_CI, 8, 8, 0, 0, 8-1, 8-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, button_pyoro2_x+80+(int)pyoro2_posx+accessory_x-4 << 2, button_pyoro2_y+70+accessory_y-4 << 2, button_pyoro2_x+80+(int)pyoro2_posx+accessory_x+4 << 2, button_pyoro2_y+70+accessory_y+4 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);

            gDPLoadTextureBlock_4b(glistp++, pyoro_sprite, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(glistp++, button_pyoro2_x+80-8+(int)pyoro2_posx << 2, button_pyoro2_y+70-8 << 2, button_pyoro2_x+80+8+(int)pyoro2_posx << 2, button_pyoro2_y+70+8 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
        }
    }
    else
    {
        gDPLoadTextureTile_4b(glistp++, spr_pyoro_hair1_2, G_IM_FMT_CI, 8, 8, 0, 0, 8-1, 8-1, 0, G_TX_MIRROR, G_TX_WRAP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, button_pyoro2_x+24-4 << 2, button_pyoro2_y+68-4 << 2, button_pyoro2_x+24+4 << 2, button_pyoro2_y+68+4 << 2,  G_TX_RENDERTILE,  8 << 5, 0 << 5, 1 << 10, 1 << 10);
        gDPLoadTextureBlock_4b(glistp++, spr_pyoro_walk2, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, button_pyoro2_x+32-8 << 2, button_pyoro2_y+70-8 << 2, button_pyoro2_x+32+8 << 2, button_pyoro2_y+70+8 << 2,  G_TX_RENDERTILE,  16 << 5, 0 << 5, 1 << 10, 1 << 10);
        gDPLoadTLUT_pal16(glistp++, 0, spr_pyoro_tlut_blue);
        gDPLoadTextureTile_4b(glistp++, spr_pyoro_hair3, G_IM_FMT_CI, 8, 8, 0, 0, 8-1, 8-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, button_pyoro2_x+88-4 << 2, button_pyoro2_y+67-4 << 2, button_pyoro2_x+88+4 << 2, button_pyoro2_y+67+4 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
        gDPLoadTextureBlock_4b(glistp++, spr_pyoro_walk2, G_IM_FMT_CI, 16, 16, 0, G_TX_MIRROR, G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, button_pyoro2_x+80-8 << 2, button_pyoro2_y+70-8 << 2, button_pyoro2_x+80+8 << 2, button_pyoro2_y+70+8 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
    }
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE); 
    
    // Draw the highscore
    score = (global_save_highscores[7] << 24) | (global_save_highscores[6] << 16) | (global_save_highscores[5] << 8) | global_save_highscores[4];
    sprintf(conbuff1, "%06d", score);
    conbuff1[6] = '\0';
    conbuff2[1] = '\0';
    global_text_alpha = 255;
    global_text_tlut = fnt_text_tlut_black;
    for (i=0;i<6;i++)
    {
        float textup;
        textup = cosf((float)pyoro2_textwave/2-i)*5;
        
        if (pyoro2_textwave == 0 || (i <= 3 && pyoro2_textwave > 9) || textup < 0 || global_save_settings[SAVE_SETTINGS_PLAYER] == 1)
            textup = 0;
        conbuff2[0] = conbuff1[i];
        DrawText(button_pyoro2_x+45+8*i, button_pyoro2_y+37-textup, conbuff2, 0, 0);
    }
    global_text_tlut = fnt_text_tlut_white;
    global_text_alpha = 255;
}


/*=================================
          DrawSmallButton
        Draw a small button
=================================*/

void DrawSmallButton(int x, int y, char id)
{
    int i;
    int w = 108;
    
    // Don't draw if outside the screen
    if (x-38 > SCREEN_WD || x+38 < 0)
        return;
    
    if (y > SCREEN_HT || y+35 < 0)
        return;
    
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
    gDPSetPrimColor(glistp++,0,0,255,255,255,255); 
    gDPLoadTLUT_pal16(glistp++, 0, spr_menu_button_tlut);
    
    for(i = 0; i < 35; i++)
    {
        gDPLoadMultiTile_4b(glistp++, spr_menu_button, 0, G_TX_RENDERTILE, G_IM_FMT_CI, 76, 35, 0, i, 76 - 1, i, 0, G_TX_CLAMP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, x-38 << 2, y + i << 2, x+38 << 2, y + i+1 << 2,  G_TX_RENDERTILE,  0 << 5, i << 5,  1 << 10, 1 << 10);
    }
    if ((selected == id || (CheckBit(contPattern,1) && selectedp2 == id)) && (menu_state == MENUSTATE_READYMAIN || menu_state == MENUSTATE_READYCREDITS || menu_state == MENUSTATE_READYSETTINGS || menu_state == MENUSTATE_READYPLAYER || menu_state == MENUSTATE_READYRESET || menu_state == MENUSTATE_READYCONTROLS))
    {
        gDPLoadTLUT_pal16(glistp++, 0, selected_tlut);
        
        gDPLoadMultiTile_4b(glistp++, spr_menu_button_select, 0, G_TX_RENDERTILE, G_IM_FMT_CI, 16, 16, 0, 0, 16, 16, 0, G_TX_MIRROR, G_TX_MIRROR, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x-4-38<<2, y-4<<2, x+12-38<<2, y+12<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, x-4+30<<2, y-4<<2, x+12+30<<2, y+12<<2, G_TX_RENDERTILE, 16 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, x-4-38<<2, y-4+23<<2, x+12-38<<2, y+12+23<<2, G_TX_RENDERTILE, 0 << 5, 16 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, x-4+30<<2, y-4+23<<2, x+12+30<<2, y+12+23<<2, G_TX_RENDERTILE, 16 << 5, 16 << 5, 1 << 10, 1 << 10 );
    }
    gDPPipeSync(glistp++);

    // Draw text on top
    global_text_alpha = 255;
    switch (id)
    {
        case SELECT_MAINPLAYER: 
        {
            if (global_save_settings[SAVE_SETTINGS_PLAYER] == 0) 
            {
                if (!(CheckBit(contPattern,1)))
                    global_text_tlut = fnt_text_tlut_gray;
                DrawText(x,y+16,"2 Players", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); 
                global_text_tlut = fnt_text_tlut_white;
            }
            else
                DrawText(x-1,y+16,"1 Player", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); 
            break;
        }
        case SELECT_MAINSETTINGS: DrawText(x,y+16,"Settings", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_MAINCREDITS: DrawText(x-1,y+16,"Credits", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_CREDITSBACK: DrawText(x,y+16,"Back", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_SETTINGSBACK: DrawText(x,y+16,"Back", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_SETTINGSCONTROLS: DrawText(x,y+16,"Controls", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_SETTINGSRESET: DrawText(x,y+16,"Reset", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_RESETYES: DrawText(x,y+16,"Yes", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_RESETNO: DrawText(x,y+16,"No", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_CONTROLSBACK: DrawText(x,y+16,"Back", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE); break;
        case SELECT_CONTROLSP1MOVE: 
        {
            if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x00)
                DrawText(x,y+16,"Stick", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x01)
                DrawText(x,y+16,"D-Pad", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x02)
                DrawText(x,y+16,"C-Pad", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x03)
                DrawText(x,y+16,"R and L", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            break;
        }
        case SELECT_CONTROLSP1ATTACK: 
        {
            if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x00)
                DrawText(x,y+16,"A", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x04)
                DrawText(x,y+16,"B", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x08)
                DrawText(x,y+16,"Z", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x0C) == 0x0C)
                DrawText(x,y+16,"R and L", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            break;
        }
        case SELECT_CONTROLSP2MOVE: 
        {
            if (!(CheckBit(contPattern,1)))
                global_text_tlut = fnt_text_tlut_gray;
            
            if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x00)
                DrawText(x,y+16,"Stick", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x10)
                DrawText(x,y+16,"D-Pad", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x20)
                DrawText(x,y+16,"C-Pad", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x30)
                DrawText(x,y+16,"R and L", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            
            global_text_tlut = fnt_text_tlut_white;
            break;
        }
        case SELECT_CONTROLSP2ATTACK:
        {
            if (!(CheckBit(contPattern,1)))
                global_text_tlut = fnt_text_tlut_gray;
            
            if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0x00)
                DrawText(x,y+16,"A", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0x40)
                DrawText(x,y+16,"B", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0x80)
                DrawText(x,y+16,"Z", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            else if ((global_save_settings[SAVE_SETTINGS_CONTROLS] & 0xC0) == 0xC0)
                DrawText(x,y+16,"R and L", TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
            
            global_text_tlut = fnt_text_tlut_white;
            break;
        }
    }
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}


/*=================================
             DrawSlider
      Draw the volume sliders
=================================*/

void DrawSlider(int x, int y, char id)
{
    int i;
    char* sprite[4] = {spr_menu_slider_1, spr_menu_slider_2, spr_menu_slider_3, spr_menu_slider_4};
    int volume;
    
    // Get the volume for each slider
    if (id == SELECT_SETTINGSSOUND)
        volume = global_save_settings[SAVE_SETTINGS_VOLUME] & 0x0F;
    else
        volume = global_save_settings[SAVE_SETTINGS_VOLUME] >> 4;
    
    // Draw the slider
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
    gDPLoadTLUT_pal16(glistp++, 0, spr_menu_slider_tlut);
    for (i=0;i<4;i++)
    {
        gDPLoadTextureBlock_4b(glistp++, sprite[i], G_IM_FMT_CI, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPScisTextureRectangle(glistp++, x+i*32-64 << 2, y-16 << 2, x+i*32-32 << 2, y+16 << 2, G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
    }
    
    // Draw the triangle
    gDPLoadTLUT_pal16(glistp++, 0, bac_pause_selection_tlut);
    gDPLoadMultiTile_4b(glistp++, bac_pause_selection, 0, G_TX_RENDERTILE, G_IM_FMT_CI, 8, 9, 0, 0, 8, 9, 0, G_TX_WRAP, G_TX_WRAP, 0, 0, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangleFlip(glistp++, x-56-4+(int)((0x0F-volume)*7.4) << 2, y-23 << 2, x-56+5+(int)((0x0F-volume)*7.4) << 2, y-23+8 << 2, G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
    
    // Draw the selection brackets
    if (selected == id && menu_state == MENUSTATE_READYSETTINGS)
    {
        gDPLoadTLUT_pal16(glistp++, 0, selected_tlut);
        gDPLoadMultiTile_4b(glistp++, spr_menu_button_select, 0, G_TX_RENDERTILE, G_IM_FMT_CI, 16, 16, 0, 0, 16, 16, 0, G_TX_MIRROR, G_TX_MIRROR, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, x-4-64<<2, y-4-31<<2, x+12-64<<2, y+12-31<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, x-4+56<<2, y-4-31<<2, x+12+56<<2, y+12-31<<2, G_TX_RENDERTILE, 16 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, x-4-64<<2, y-4-8<<2, x+12-64<<2, y+12-8<<2, G_TX_RENDERTILE, 0 << 5, 16 << 5, 1 << 10, 1 << 10 );
        gSPTextureRectangle(glistp++, x-4+56<<2, y-4-8<<2, x+12+56<<2, y+12-8<<2, G_TX_RENDERTILE, 16 << 5, 16 << 5, 1 << 10, 1 << 10 );
    }
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
    
    // Draw the text on top of the slider
    global_text_sprite = fnt_text_outlined;
    global_text_tlut = fnt_text_outlined_tlut;
    if (id == SELECT_SETTINGSSOUND)
        DrawText(x,y-27,"Sound",TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
    else
        DrawText(x,y-27,"Music",TEXT_HALIGN_CENTER, TEXT_VALIGN_MIDDLE);
    global_text_tlut = fnt_text_tlut_white;
    global_text_sprite = fnt_text;
}


void DrawFade(char fade_alpha)
{
    if (fade_alpha != 0)
    {
        int x = 0;
        int y = 0;
        int w = SCREEN_WD;
        int h = 8;
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
        gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF);
        gDPSetDepthSource(glistp++, G_ZS_PRIM);
        gDPSetTexturePersp(glistp++, G_TP_NONE);
        gDPSetTextureLUT(glistp++, G_TT_RGBA16);
        gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
        gDPSetPrimColor(glistp++,0,0,255,255,255,fade_alpha); 
        gDPLoadTLUT_pal16(glistp++, 0, bac_solid_tlut);
        gDPLoadTextureBlock_4b(glistp++, bac_solid, G_IM_FMT_CI, 16, 16, 0, G_TX_WRAP, G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, 0 << 2, 0 << 2, SCREEN_WD << 2, SCREEN_HT << 2,  G_TX_RENDERTILE, 0 << 5, 0 << 5,  1 << 10, 1 << 10);
        gDPPipeSync(glistp++);
        gDPSetTextureLUT(glistp++, G_TT_NONE);
    }
}

/*=================================
           DrawDebugInfo
 Show how full the display list is
=================================*/

#if (DEBUG_MODE == 1)
    void DrawDebugInfo()
    {
        int i;
        char conbuff[5];
    
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF);
        gDPSetDepthSource(glistp++, G_ZS_PRIM);
        gDPSetTexturePersp(glistp++, G_TP_NONE);
        gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    
        // Show debug information
        gDPLoadTLUT_pal16(glistp++, 0, fnt_text_tlut_white);
        glistafter = glistp;
        sprintf(conbuff, "%4d", glistafter - glistbefore);
        for (i=0;i<sizeof(conbuff)-1;i++)
        {
            char offset = conbuff[i] - '0';
            char pos = i*8;
            gDPLoadTextureTile_4b(glistp++, fnt_numbers, G_IM_FMT_CI, 8, 8, 0, offset*8, 8, 8 + (offset*8), 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, 16+pos << 2, 16 << 2, 16+8+pos << 2, 16+8 << 2,  G_TX_RENDERTILE,  0 << 5, offset*8 << 5,  1 << 10, 1 << 10);
        }
        gDPPipeSync(glistp++);        
        gDPSetTextureLUT(glistp++, G_TT_NONE);
    }
#endif