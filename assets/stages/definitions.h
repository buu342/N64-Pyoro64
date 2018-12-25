/*========================================================
				Stages Functions for linking
========================================================*/

#ifndef N64_STAGES_H
#define N64_STAGES_H

    #define STAGE_DISCLAIMER 0
    #define STAGE_MENU  1
    #define STAGE_GAME1 2
    #define STAGE_GAME2 3

    void stage_disclaimer_init(void);
    void stage_disclaimer_update(void);
    void stage_disclaimer_draw(void);
    
    void stage_menu_init(void);
    void stage_menu_update(void);
    void stage_menu_draw(void);
    
    void stage_game1_init(void);
    void stage_game1_update(void);
    void stage_game1_draw(void);
    
    void stage_game2_init(void);
    void stage_game2_update(void);
    void stage_game2_draw(void);
        
#endif