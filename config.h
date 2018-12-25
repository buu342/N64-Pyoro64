#ifndef N64_CONFIG_H
#define N64_CONFIG_H

    // TV defines
    #define NTSC    0
    #define PAL     1
    #define MPAL    2
    
    #define TV_TYPE PAL
    
    // Config defines
    #define DEBUG_MODE  0

    #define SCREEN_WD   320
    #define SCREEN_HT   240
    #define GLIST_LENGTH    7168
    
    #define MAXOBJECTS 150
    
    // Gameplay defines
    #define FADE_IN 0
    #define FADE_OUT 1
    
    #define MAX_LEAVES 40
        
    #define SUNSET_MODE 5000
    #define NIGHT_MODE  10000
    #define BEIGE_MODE  20000
    #define DARK_MODE   30000
    #define LIGHT_MODE  40000
    #define DISCO_MODE  50000
    
    // Text defines
    #define TEXT_HALIGN_LEFT 0
    #define TEXT_HALIGN_CENTER 1
    #define TEXT_HALIGN_RIGHT 2
    #define TEXT_VALIGN_TOP 0
    #define TEXT_VALIGN_MIDDLE 1
    #define TEXT_VALIGN_BOTTOM 2
    
    // Save defines
    #define SAVESLOT_UNLOCKS 0
    #define SAVESLOT_SETTINGS 1
    #define SAVESLOT_HIGHSCORES 2
    
    #define SAVE_UNLOCK_PYORO2 0
    
    #define SAVE_SETTINGS_PLAYER 0
    #define SAVE_SETTINGS_VOLUME 1
    #define SAVE_SETTINGS_CONTROLS 2
    
    
    // Return mode defines
    #define RETURN_LOGO  0
    #define RETURN_FADE  1
    #define RETURN_GAME1 2
    #define RETURN_GAME2 3
    
    
    /*=================================
              Global variables
    =================================*/
    
    extern u32 GameScore;
    extern u32 GameScore1;
    extern u32 GameScore2;
    extern u8 GameSpeed;
    extern u8 returnmode;
    
    extern volatile short int global_stage;
    extern char global_eeprom_loaded;
    extern char global_controlpak_loaded;
    
    extern unsigned char* global_text_sprite;
    extern unsigned short* global_text_tlut;
    extern char global_text_alpha;
    
    extern u8 global_save_unlocks[];
    extern u8 global_save_settings[];
    extern u8 global_save_highscores[];
    
    
    /*=================================
                  Buffers
    =================================*/
    
    Gfx glist[GLIST_LENGTH];
    Gfx* glistp;
    
    extern int ptr_buf[];
    extern int sfx_buf[];
    extern int ptr2_buf[];
    extern int tune1_buf[];
    extern int tune2_buf[];
    extern int tune3_buf[];
    
    extern NUContData contdata[];
    extern u8 contPattern;
    
    
    /*=================================
                Prototypes
    =================================*/
    
    void RCPInit(Gfx *glistp);
    void Rom2Ram(void *from_addr, void *to_addr, s32 seq_size);
    void ClearBackground(u8 r, u8 g, u8 b);
    void DrawText(int x, int y, char* text, char halign, char valign);
    void SetDefaultSave();
    int getPt(int n1 , int n2 , float perc);
    int getBezierY(int x, int y1, int y2, int y3, int y4);
    
    int max(int a, int b);
    float ceilfloor(float n);
    #define CheckBit(var,pos) ((var) & (1<<(pos)))
    
    #if (TV_TYPE == PAL)
        #define OS_USEC_TO_CYCLES_CORRECT(x) OS_USEC_TO_CYCLES(x)
    #else
        #define OS_USEC_TO_CYCLES_CORRECT(x) OS_USEC_TO_CYCLES(((float)x)*((float)0.8))
    #endif
    
    void tlut_transition(u16 *change, u16 *changeto, char num);
    u16* tlut_copy(unsigned short* src, int len);
    void tlut_edit(u16 *tlut, u16 change, u16 changeto, char num);
    
    char PlayerIsAttacking(char player, char holding);
    
    /*=================================
            Sound code segments
    =================================*/
    
    // Code segment start and end
    extern char _codeSegmentStart[];
    extern char _codeSegmentEnd[];

    // Segment arrays
    extern u8 _bank_sfx_wbankSegmentRomStart[];
    extern u8 _bank_sfx_pbankSegmentRomStart[];
    extern u8 _bank_sfx_pbankSegmentRomEnd[];
    extern u8 _bank_sfx_sfxSegmentRomStart[];
    extern u8 _bank_sfx_sfxSegmentRomEnd[];
    
    extern u8 _bank_instr_wbankSegmentRomStart[];
    extern u8 _bank_instr_pbankSegmentRomStart[];
    extern u8 _bank_instr_pbankSegmentRomEnd[];
    extern u8 _sng_main1_1SegmentRomStart[];
    extern u8 _sng_main1_1SegmentRomEnd[];
    extern u8 _sng_main1_2SegmentRomStart[];
    extern u8 _sng_main1_2SegmentRomEnd[];
    extern u8 _sng_main1_3SegmentRomStart[];
    extern u8 _sng_main1_3SegmentRomEnd[];
    extern u8 _sng_main2SegmentRomStart[];
    extern u8 _sng_main2SegmentRomEnd[];
    extern u8 _sng_main3SegmentRomStart[];
    extern u8 _sng_main3SegmentRomEnd[];
    extern u8 _sng_gameoverSegmentRomStart[];
    extern u8 _sng_gameoverSegmentRomEnd[];
    extern u8 _sng_menuSegmentRomStart[];
    extern u8 _sng_menuSegmentRomEnd[];
    
    // Segment macros
    #define WBANK_SFX_START	    _bank_sfx_wbankSegmentRomStart
    #define PBANK_SFX_START	    _bank_sfx_pbankSegmentRomStart
    #define PBANK_SFX_END	    _bank_sfx_pbankSegmentRomEnd
    #define BANK_SFX_START	    _bank_sfx_sfxSegmentRomStart
    #define BANK_SFX_END	    _bank_sfx_sfxSegmentRomEnd
    
    #define WBANK_INSTR_START	_bank_instr_wbankSegmentRomStart
    #define PBANK_INSTR_START	_bank_instr_pbankSegmentRomStart
    #define PBANK_INSTR_END	    _bank_instr_pbankSegmentRomEnd
    #define SNG_MAIN1_1_START   _sng_main1_1SegmentRomStart
    #define SNG_MAIN1_1_END     _sng_main1_1SegmentRomEnd
    #define SNG_MAIN1_2_START   _sng_main1_2SegmentRomStart
    #define SNG_MAIN1_2_END     _sng_main1_2SegmentRomEnd
    #define SNG_MAIN1_3_START   _sng_main1_3SegmentRomStart
    #define SNG_MAIN1_3_END     _sng_main1_3SegmentRomEnd
    #define SNG_MAIN2_START     _sng_main2SegmentRomStart
    #define SNG_MAIN2_END       _sng_main2SegmentRomEnd
    #define SNG_MAIN3_START     _sng_main3SegmentRomStart
    #define SNG_MAIN3_END       _sng_main3SegmentRomEnd
    #define SNG_GAMEOVER_START  _sng_gameoverSegmentRomStart
    #define SNG_GAMEOVER_END    _sng_gameoverSegmentRomEnd
    #define SNG_MENU_START      _sng_menuSegmentRomStart
    #define SNG_MENU_END        _sng_menuSegmentRomEnd

#endif