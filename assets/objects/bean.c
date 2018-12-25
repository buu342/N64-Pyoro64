/*========================================================
			           Bean Object
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include <nualstl_n.h>
#include "../../config.h"
#include "../sounds/definitions.h"
#include "base.h"

// Sprites
#include "../sprites/spr_bean.h"


/*=================================
           Custom Macros
=================================*/

#define WIDTH   16
#define HEIGHT  16


/*=================================
        Variables Structure
=================================*/

#define self ((struct obj_vars *)(obj_base->vars))

struct obj_vars
{
    float  x;
    float  y;
    float  speed;
    unsigned char *sprite;
    char   state;
    char   type;
    int    frame;
    OSTime frame_timer;
    OSTime tlut_flash_timer;
    char   tlut_num;
    unsigned short *tlut;
    
    float collision_bounds[5];
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_bean(BASE *obj_base, float x, float y)
{
    // Make some calculations for the speed.
    int min, max;
    min = 100000*(((float)GameSpeed)*0.01 + 0.5);
    max = 100000*(((float)GameSpeed)*0.0125 + 0.75);
    
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
    self->sprite = spr_bean_1;
    self->state = STATE_IDLE;
    self->type = 1;
    self->frame = 0;
    self->frame_timer = 0;
    self->tlut_flash_timer = 0;
    self->tlut_num = 0;
    self->speed = ((float)(min + guRandom() % (max+1 - min)))/100000; // Range of 0.5-0.75 at 0 speed, 1.5-2 at 100. 
    if (GameScore >= DARK_MODE)
        self->tlut = tlut_copy(spr_bean1_tlut_dark, 9);
    else if (GameScore >= BEIGE_MODE)
        self->tlut = tlut_copy(spr_bean1_tlut_beige, 9);
    else
        self->tlut = tlut_copy(spr_bean1_tlut, 9);
    
    // Make sure we managed to create our TLUT
    if (self->tlut == NULL)
    {
        free(obj_base->vars);
        obj_base->type = OBJ_NULL;
        return;
    } 
    
    // Specify the collisions
    self->collision_bounds[0] = COL_RECTANGLE;
    self->collision_bounds[1] = self->x-5;
    self->collision_bounds[2] = self->y;
    self->collision_bounds[3] = self->x+1;
    self->collision_bounds[4] = self->y+8;
}


/*=================================
             update_*
        Updates the object
=================================*/

void update_bean(BASE *obj_base)
{
    // Make the bean fall with gravity
    if (self->state != STATE_DIE)
        self->y += self->speed;
    else
    {
        // Move the sprite towards the player
        self->y += abs(self->speed);
        self->x += self->speed;
    }
    
    // Check if a flashing bean was recently caught
    if (bean3_destruction_y != 0 && self->y > bean3_destruction_y && self->state != STATE_DIE)
    {
        int index = instance_get_index(obj_base);
        int score_obj;
        int score = 50;
        u32 *whichscore;

        // Check which score to get
        if (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && bean3_destruction_player == 0)
            whichscore = &GameScore1;
        else if (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && bean3_destruction_player == 1)
            whichscore = &GameScore2;
        else
            whichscore = &GameScore;
        
        *whichscore += 50;
        instance_create(OBJ_PUFF, self->x, self->y);
        score_obj = instance_create(OBJ_SCORES, self->x, self->y);
        if (score_obj != -1)
            instance_set(score_obj, SET_TYPE, &score);
        sndHandle = nuAuStlSndPlayerPlay(SND_BEAN_POP);
        if (index != -1)
            instance_destroy_index(index);
            
        // Return to not execute any code below (since it'll be NULL)
        return;
    }
    
    // Change frame every 100000 microseconds (depending on speed)
    if (self->frame_timer < osGetTime())
    {
        self->frame_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(100000/(self->speed));
        self->frame = (self->frame + 1)%6;
        switch (self->frame)
        {
            case 0:
            case 1:
                self->sprite = spr_bean_1;
                break;
            case 2:
            case 5:
                self->sprite = spr_bean_2;
                break;
            case 3:
            case 4:
                self->sprite = spr_bean_3;
                break;
        }
    }
    
    // Set the collision bounds
    if (self->state != STATE_DIE)
    {
        self->collision_bounds[1] = self->x-3;
        // Make the y hitbox bigger in Pyoro 2 so it's easier to hit
        if (instance_find(OBJ_PYORO2) != -1)
            self->collision_bounds[2] = self->y-6;
        else
            self->collision_bounds[2] = self->y;
        self->collision_bounds[3] = self->x+2;
        self->collision_bounds[4] = self->y+8;
    }
    
    // If caught, make sure it isn't visible when below pyoro
    if (self->state == STATE_DIE)
    {
        int check_index = instance_find(OBJ_PYORO);
        if (check_index != -1 && 189 < self->y)
        {
            int index = instance_get_index(obj_base);
            if (index != -1)
                instance_destroy_index(index);
            return;
        }
    }
    
    // Check for Collisions with blocks/out of bounds
    if (self->y > 194)
    {
        int index = instance_get_index(obj_base);
        int col_index, closest;
        
        // If out of bounds, destroy self
        if (self->y > 220 && index != -1)
        {
            instance_destroy_index(index);
            
            // Return to not execute any code below (since it'll be NULL)
            return;
        }
        
        // Otherwise, if we're in block territory, check for collisions
        if (self->y < 208)
        {
            // Check if colliding with a block
            col_index = instance_collision(obj_base, OBJ_BLOCK);
               
            // If we found an object, destroy self and the closest block
            if (col_index != -1)
            {
                // Find the closest block (to ensure we remove the proper block)
                closest = instance_get_nearest(obj_base, OBJ_BLOCK);
                sndHandle = nuAuStlSndPlayerPlay(SND_BLOCK_DIE);
                instance_create(OBJ_PUFF, self->x, self->y);
                instance_destroy_index(closest);
                instance_destroy_index(index);
                
                // Return to not execute any code below (since it'll be NULL)
                return;
            }
        }
    }
    
    // Make the bean flash if it's type 3
    if (self->type == 3 && self->tlut_flash_timer < osGetTime())
    {
        self->tlut_flash_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(50000);
        free(self->tlut);
        if (GameScore >= DARK_MODE)
        {
            if (self->tlut_num == 0)
                self->tlut = tlut_copy(spr_bean1_tlut_dark, 9);
            else if (self->tlut_num == 1)
                self->tlut = tlut_copy(spr_bean2_tlut_dark, 9);
            else if (self->tlut_num == 2)
                self->tlut = tlut_copy(spr_bean3_tlut_dark, 9);
        }
        else if (GameScore >= BEIGE_MODE)
        {
            if (self->tlut_num == 0)
                self->tlut = tlut_copy(spr_bean1_tlut_beige, 9);
            else if (self->tlut_num == 1)
                self->tlut = tlut_copy(spr_bean2_tlut_beige, 9);
            else if (self->tlut_num == 2)
                self->tlut = tlut_copy(spr_bean3_tlut_beige, 9);
        }
        else
        {
            if (self->tlut_num == 0)
                self->tlut = tlut_copy(spr_bean1_tlut, 9);
            else if (self->tlut_num == 1)
                self->tlut = tlut_copy(spr_bean2_tlut, 9);
            else if (self->tlut_num == 2)
                self->tlut = tlut_copy(spr_bean3_tlut, 9);
        }
        self->tlut_num = (self->tlut_num + 1)%3;
    }
    
    // Transition the tlut if we reach the correct amount of points
    if (GameScore >= DARK_MODE)
    {
        if (self->type == 1)
            tlut_transition(self->tlut, spr_bean1_tlut_dark, 9);
        else if (self->type == 2)
            tlut_transition(self->tlut, spr_bean2_tlut_dark, 9);
    }
    else if (GameScore >= BEIGE_MODE)
    {
        if (self->type == 1)
            tlut_transition(self->tlut, spr_bean1_tlut_beige, 9);
        else if (self->type == 2)
            tlut_transition(self->tlut, spr_bean2_tlut_beige, 9);
    }
}


/*=================================
               get_*
     Return a requested value
=================================*/

void* get_bean(BASE *obj_base, u8 what)
{
    switch(what)
    {
        case GET_X:
            return &self->x;
        case GET_Y:
            return &self->y;
        case GET_STATE:
            return &self->state;
        case GET_BOUND:
            return (void*)self->collision_bounds;
        case GET_TYPE:
            return &self->type;
    }
}


/*=================================
               set_*
     Return a requested value
=================================*/

void set_bean(BASE *obj_base, u8 what, void* value)
{
    switch(what)
    {
        case SET_X:
            self->x = *(float*) value;
            break;
        case SET_Y:
            self->y = *(float*) value;
            break;
        case SET_SPEED:
            self->speed = *(float*) value;
            break;
        case SET_STATE:
            self->state = *(char*) value;
            if (self->state == STATE_DIE)
                self->collision_bounds[0] = COL_NONE;
            break;
        case SET_TYPE:
            self->type = *(char*) value;
            if (self->type == 2)
            {
                free(self->tlut);
                if (GameScore >= DARK_MODE)
                    self->tlut = tlut_copy(spr_bean2_tlut_dark, 9);
                else if (GameScore >= BEIGE_MODE)
                    self->tlut = tlut_copy(spr_bean2_tlut_beige, 9);
                else
                    self->tlut = tlut_copy(spr_bean2_tlut, 9);
            }
            break;
    }
}


/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_bean(BASE *obj_base)
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

void draw_bean(BASE *obj_base)
{
    // Don't draw when caught and below Pyoro
    if (self->state == STATE_DIE && self->y > 188)
        return;
        
    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 40, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    
    gDPLoadTLUT_pal16( glistp++, 0, self->tlut);
    gDPLoadTextureTile_4b(glistp++, self->sprite, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH-1, HEIGHT-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle( glistp++, ((int)self->x)-WIDTH/2<<2, ((int)self->y)-HEIGHT/2<<2, ((((int)self->x) + WIDTH/2)<<2), ((((int)self->y) + HEIGHT/2)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}
