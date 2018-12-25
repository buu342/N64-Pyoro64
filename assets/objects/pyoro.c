/*========================================================
			           Pyoro Object
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include <nualstl_n.h>
#include "../../config.h"
#include "../sounds/definitions.h"
#include "../music/definitions.h"
#include "base.h"

// Sprites
#include "../sprites/spr_pyoro.h"
#include "../sprites/spr_pyoro_accessories.h"
#include "../sprites/spr_twirl.h"


/*=================================
           Custom Macros
=================================*/

#define WIDTH   16
#define HEIGHT  16

#define COL_X1 -WIDTH/2
#define COL_Y1 -HEIGHT/8
#define COL_X2 +WIDTH/2
#define COL_Y2 +HEIGHT/2

#define DIR_LEFT  0
#define DIR_RIGHT 1


/*=================================
        Variables Structure
=================================*/

#define self ((struct obj_vars *)(obj_base->vars))

struct obj_vars
{
    float x;
    float y;
    char direction;
    char state;
    char frame_num;
    char player;
    unsigned char * sprite;
    OSTime frame_timer;
    unsigned short *tlut;
    char frozen;
    
    float collision_bounds[5];
};


/*=================================
         Global Variables
=================================*/

static float stick1_x = 0;
static float stick1_y = 0;


/*=================================
             create_*
        Creates the object
=================================*/

void create_pyoro(BASE *obj_base, float x, float y)
{
    // Assign our variables struct
    obj_base->vars = (struct obj_vars *) malloc(sizeof(struct obj_vars));
    
    // Make sure it managed to allocate memory
    if (obj_base->vars == NULL)
    {
        obj_base->type = OBJ_NULL;
        return;
    }
    
    // Set the rest of the variables
    self->x = x;
    self->y = y;
    self->direction = DIR_RIGHT;
    self->sprite = spr_pyoro_walk1;
    self->state = STATE_IDLE;
    self->frame_timer = 0;
    self->frame_num = 0;
    self->player = 0;
    self->frozen = 0;
    self->tlut = tlut_copy(spr_pyoro_tlut_default, 5);
    
    // Make sure we managed to create our TLUT
    if (self->tlut == NULL)
    {
        free(obj_base->vars);
        obj_base->type = OBJ_NULL;
        return;
    }
    
    // Specify the collisions
    self->collision_bounds[0] = COL_RECTANGLE;
    self->collision_bounds[1] = x+COL_X1;
    self->collision_bounds[2] = y+COL_Y1;
    self->collision_bounds[3] = x+COL_X2;
    self->collision_bounds[4] = y+COL_Y2;
}


/*=================================
             update_*
        Updates the object
=================================*/

void update_pyoro(BASE *obj_base)
{
    int col_index;
    musHandle* sndHandle_player;
    
    // Stop updating if the character is frozen (used in the settings menu)
    if (self->frozen)
        return;
    
    // Select the sound handle depending if player 1 or 2
    if (self->player == 0)
        sndHandle_player = &sndHandle_pyoro1;
    else
        sndHandle_player = &sndHandle_pyoro2;
    
    // Transition the tlut if we reach the score needed
    if (GameScore >= DARK_MODE)
        tlut_transition(self->tlut, spr_pyoro_tlut_dark, 5);
    else if (GameScore >= BEIGE_MODE)
        tlut_transition(self->tlut, spr_pyoro_tlut_beige, 5);
    
    // If the player is dead, execute this code instead of what's below.
    if (self->state == STATE_DIE)
    {           
        self->sprite = spr_pyoro_die;
        self->y +=0.3;
        
        if (self->y > 220)
        {
            int index = instance_get_index(obj_base);
            if (index != -1)
                instance_destroy_index(index);
        }
        if (self->frame_timer < osGetTime())
        {
            self->frame_num++;
            self->frame_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(50000);
        }
        return;
    }
    
    // Save the control stick data
    stick1_x = 0;
    stick1_y = contdata[self->player].stick_y;
    if ((self->player == 0 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x00) || (self->player == 1 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x00))
        stick1_x = contdata[self->player].stick_x;
    else if ((self->player == 0 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x01) || (self->player == 1 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x10))
    {
        if (contdata[self->player].button & R_JPAD)
            stick1_x = 48;
        else if (contdata[self->player].button & L_JPAD)
            stick1_x = -48;
    }
    else if ((self->player == 0 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x02) || (self->player == 1 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x20))
    {
        if (contdata[self->player].button & R_CBUTTONS)
            stick1_x = 48;
        else if (contdata[self->player].button & L_CBUTTONS)
            stick1_x = -48;
    }
    else if ((self->player == 0 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x03) == 0x03) || (self->player == 1 && (global_save_settings[SAVE_SETTINGS_CONTROLS] & 0x30) == 0x30))
    {
        if (contdata[self->player].button & R_TRIG)
            stick1_x = 48;
        else if (contdata[self->player].button & L_TRIG)
            stick1_x = -48;
    }
    
    // Mark the character as idle or attacking depending if a tongue exists
    if (instance_find_withvalue(OBJ_TONGUE, GET_PLAYER, &(self->player)) == -1)
    {
        // If we are in the chew state, do a chew animation. Otherwise go to idle
        if (self->state == STATE_CHEW)
        {
            self->state = STATE_CHEW;
            
            // Toggle between chew2 and walk1 every 75000 microseconds, until we've chewed 5 times
            if (self->frame_timer < osGetTime())
            {
                if (self->sprite == spr_pyoro_walk1)
                    self->sprite = spr_pyoro_chew2;
                else
                    self->sprite = spr_pyoro_walk1;
                self->frame_num++;
                self->frame_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(75000);
            }
            else if (self->frame_num >= 5)
            {
                self->state = STATE_IDLE;
                self->sprite = spr_pyoro_walk1;
            }
        }
        else
        {
            self->state = STATE_IDLE;
            self->sprite = spr_pyoro_walk1;
            self->frame_num = 0;
        }
        
        // Using a deadzone of 32 units, allow the character to move on x
        if (stick1_x > 16 || stick1_x < -16)
        {
            // Set the state
            self->state = STATE_MOVE;
            self->frame_num = 0;
            
            // Prevent cheating by recalibrating the controller.
            if (stick1_x > 48)
                stick1_x = 48;
            if (stick1_x < -48)
                stick1_x = -48;
                
            // Specify the character's direction (for sprite manipulation)
            if (stick1_x > 0)
                self->direction = DIR_RIGHT;
            else
                self->direction = DIR_LEFT;
            
            // Check if there's a block near the player, and if so, let him move there
            if ((self->direction == DIR_LEFT && instance_collision_point(OBJ_BLOCK, self->x-5, self->y+12) != -1) || (self->direction == DIR_RIGHT && instance_collision_point(OBJ_BLOCK, self->x+5, self->y+12) != -1))
                self->x += (stick1_x/((-0.24*((float)GameSpeed))+32)); // 32 at 0 speed, 8 at 100
           
            // Don't let the player go further than the border
            if (self->x < 52)
                self->x = 52;
            if (self->x > 269)
                self->x = 269;
            
            // Toggle between walk1 and walk2 sprite every 75000 microseconds
            if (self->frame_timer < osGetTime())
            {
                *sndHandle_player = nuAuStlSndPlayerPlay(SND_PYORO_WALK);
                self->frame_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(75000);
                if (self->sprite == spr_pyoro_walk1)
                    self->sprite = spr_pyoro_walk2;
                else
                    self->sprite = spr_pyoro_walk1;
            }
        }
        
        // Allow the player to use their tongue
        if (PlayerIsAttacking(self->player, 0))
        {
            int index = instance_create(OBJ_TONGUE, self->x+6*(self->direction == DIR_RIGHT ? 1 : -1), self->y-2);
            if (index != -1)
            {
                instance_set(index, SET_DIR, &(self->direction));
                instance_set(index, SET_PLAYER, &(self->player));
                *sndHandle_player = nuAuStlSndPlayerPlay(SND_TONGUE_OUT);
            }
        }
    }
    else 
    {
        self->state = STATE_ATTACK;
        self->sprite = spr_pyoro_attack;
    }
    
    // Update the collision bounds
    self->collision_bounds[1] = self->x+COL_X1;
    self->collision_bounds[2] = self->y+COL_Y1;
    self->collision_bounds[3] = self->x+COL_X2;
    self->collision_bounds[4] = self->y+COL_Y2;
    
    // Check for a collision with a bean
    col_index = instance_collision(obj_base, OBJ_BEAN);
    if (col_index != -1)
    {
        int tongue_index;
        int colx, coly;
        
        // Play a sound and stop the music
        if ((DEBUG_MODE && !(contdata[self->player].button & Z_TRIG)) || !DEBUG_MODE)
        {
            char state = STATE_DIE;
            char otherplayer = (self->player + 1)%2;
            int otherplayer_index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &otherplayer);
            
            *sndHandle_player = nuAuStlSndPlayerPlay(SND_PYORO_DIE);
            
            // don't stop the music if there is another player alive
            if (otherplayer_index == -1 || *(char*)instance_get(otherplayer_index, GET_STATE) == STATE_DIE)
            {
                MusHandleStop(seqHandle1,0);
                MusHandleStop(seqHandle2,0);
                MusHandleStop(seqHandle3,0);
            }
        }
        
        // Create a puff and destroy the bean
        colx = *(float*)instance_get(col_index, GET_X);
        coly = *(float*)instance_get(col_index, GET_Y) + 4;
        instance_create(OBJ_PUFF, colx, coly);
        instance_destroy_index(col_index);
        
        if ((DEBUG_MODE && !(contdata[self->player].button & Z_TRIG)) || !DEBUG_MODE)
        {
            // Set the state to dead
            self->state = STATE_DIE;
            self->frame_num = 0;
            
            // Destroy the tongue if it exists
            tongue_index = instance_find_withvalue(OBJ_TONGUE, GET_PLAYER, &(self->player));
            if (tongue_index != -1)
                instance_destroy_index(tongue_index);
        }
    }
}


/*=================================
               get_*
     Return a requested value
=================================*/

void* get_pyoro(BASE *obj_base, u8 what)
{
    switch(what)
    {
        case GET_X:
            return &(self->x);
        case GET_Y:
            return &(self->y);
        case GET_BOUND:
            return (void*)self->collision_bounds;
        case GET_PLAYER:
            return &(self->player);
        case GET_STATE:
            return &(self->state);
        case GET_FROZEN:
            return &(self->frozen);
    }
}


/*=================================
               set_*
    Sets a variable on an object
=================================*/

void set_pyoro(BASE *obj_base, u8 what, void* value)
{
    switch(what)
    {
        case SET_X:
            self->x = *(float*)value;
            break;
        case SET_STATE:
            self->state = *(char*)value;
            self->frame_num = 0;
            break;
        case SET_PLAYER:
            self->player = *(char*)value;
            if (self->player == 1)
            {
                free(self->tlut);
                self->tlut = tlut_copy(spr_pyoro_tlut_green, 5);
            }
            break;
        case SET_FROZEN:
            self->frozen = *(char*)value;
            break;
    }
}



/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_pyoro(BASE *obj_base)
{
    // Free the memory allocated for the tlut
    free(self->tlut);

    // Free the memory used by our variables struct
    free(obj_base->vars);
    
    // Mark the slot in GameObjects as empty
    obj_base->type = OBJ_NULL;
}


/*=================================
              draw_*
         Draws the object
=================================*/

void draw_pyoro(BASE *obj_base)
{
    char* accessory_index;
    int accessory_x;
    int accessory_y;
    
    // Choose what sprite the hair has depending on pyoro's sprite
    if (self->player == 1)
    {
        if (self->sprite == spr_pyoro_walk1 || self->sprite == spr_pyoro_chew2)
        {
            accessory_index = spr_pyoro_hair2_1;
            accessory_x = 6;
            accessory_y = -6;
        }
        else if (self->sprite == spr_pyoro_walk2)
        {
            accessory_index = spr_pyoro_hair2_2;
            accessory_x = 6;
            accessory_y = -5;
        }
        else if (self->sprite == spr_pyoro_attack)
        {
            accessory_index = spr_pyoro_hair2_2;
            accessory_x = 4;
            accessory_y = -7;
        }
        else
        {
            accessory_index = spr_pyoro_hair1_1;
            accessory_x = 0;
            accessory_y = 0;
        }
    }
    
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 60 + 30*self->player, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    gDPLoadTLUT_pal16( glistp++, 0, self->tlut);
    if (self->direction == DIR_LEFT)
    {
        if (self->player == 1)
        {
            gDPLoadTextureTile_4b(glistp++, accessory_index, G_IM_FMT_CI, 8, 8, 0, 0, 8-1, 8-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, (int)self->x+accessory_x-4 << 2, (int)self->y+accessory_y-4 << 2, (int)self->x+accessory_x+4 << 2, (int)self->y+accessory_y+4 << 2,  G_TX_RENDERTILE,  0 << 5, 0 << 5, 1 << 10, 1 << 10);
        }
        gDPSetPrimDepth(glistp++, 50 + 30*self->player, 0);
        gDPLoadTextureTile_4b(glistp++, self->sprite, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH-1, HEIGHT-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, ((int)self->x)-WIDTH/2<<2, ((int)self->y)-HEIGHT/2<<2, ((((int)self->x) + WIDTH/2)<<2), ((((int)self->y) + HEIGHT/2)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    }
    else // Flip the texture
    {
        if (self->player == 1)
        {
            gDPLoadTextureTile_4b(glistp++, accessory_index, G_IM_FMT_CI, 8, 8, 0, 0, 8-1, 8-1, 0, G_TX_MIRROR, G_TX_WRAP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(glistp++, (int)self->x-accessory_x+1-4 << 2, (int)self->y+accessory_y-4 << 2, (int)self->x-accessory_x+1+4 << 2, (int)self->y+accessory_y+4 << 2,  G_TX_RENDERTILE,  8 << 5, 0 << 5, 1 << 10, 1 << 10);
        }
        gDPSetPrimDepth(glistp++, 50 + 30*self->player, 0);
        gDPLoadTextureTile_4b(glistp++, self->sprite, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH-1, HEIGHT-1, 0, G_TX_MIRROR, G_TX_NOMIRROR, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, ((int)self->x+1)-WIDTH/2<<2, ((int)self->y)-HEIGHT/2<<2, ((((int)self->x+1) + WIDTH/2)<<2), ((((int)self->y) + HEIGHT/2)<<2), G_TX_RENDERTILE, 16 << 5, 0 << 5, 1 << 10, 1 << 10 );
    }
    
    // Draw the twirl around the head if dead
    if (self->state == STATE_DIE)
    {
        unsigned char* twirl_sprite;
        switch ((self->frame_num%6)+1)
        {
            case 1: twirl_sprite = spr_twirl1; break;
            case 2: twirl_sprite = spr_twirl2; break;
            case 3: twirl_sprite = spr_twirl3; break;
            case 4: twirl_sprite = spr_twirl4; break;
            case 5: twirl_sprite = spr_twirl5; break;
            case 6: twirl_sprite = spr_twirl6; break;
        }
        gDPSetPrimDepth(glistp++, 40 + 30*self->player, 0);
        gDPLoadTLUT_pal16(glistp++, 0, spr_twirl_tlut);
        if (GameScore >= BEIGE_MODE)
            gDPLoadTLUT_pal16(glistp++, 0, spr_twirl_tlut_white);
        gDPLoadTextureTile_4b(glistp++, twirl_sprite, G_IM_FMT_CI, 16, 8, 0, 0, 16, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, ((int)self->x)-8<<2, ((int)self->y-4)-HEIGHT/2<<2, ((((int)self->x) + 8)<<2), ((((int)self->y-4) - HEIGHT/2)+8<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    }
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}