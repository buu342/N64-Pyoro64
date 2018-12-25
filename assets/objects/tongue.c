/*========================================================
			           Tongue Object
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
#include "../sprites/spr_tongue.h"


/*=================================
           Custom Macros
=================================*/

#define WIDTH   16
#define HEIGHT  16

#define COL_X1 -WIDTH/2
#define COL_Y1 -0
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
    float x_origin;
    float y_origin;
    char direction;
    char player;
    int capture;
    float speed;
    unsigned short *tlut;
    
    float collision_bounds[5];
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_tongue(BASE *obj_base, float x, float y)
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
    self->x = x+8;
    self->y = y-8;
    self->x_origin = x;
    self->y_origin = y;
    self->direction = DIR_RIGHT;
    self->speed = 2+((float)GameSpeed/25);
    self->capture =-1;
    self->player = 0;
    if (GameScore >= DARK_MODE)
        self->tlut = tlut_copy(spr_tongue_tlut_dark, 4);
    else if (GameScore >= BEIGE_MODE)
        self->tlut = tlut_copy(spr_tongue_tlut_beige, 4);
    else
        self->tlut = tlut_copy(spr_tongue_tlut_default, 4);
    
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

void update_tongue(BASE *obj_base)
{
    int col_index = -1;
    musHandle* sndHandle_player;

    // Select the sound handle depending if player 1 or 2
    if (self->player == 0)
        sndHandle_player = &sndHandle_pyoro1;
    else
        sndHandle_player = &sndHandle_pyoro2;
    
    // Move the tongue (casts are to avoid rendering issues)
    if (self->direction == DIR_LEFT)
        self->x -= self->speed;
    else
        self->x += self->speed;
    self->y -= self->speed;
    
    // Update the collision bounds
    self->collision_bounds[1] = self->x+COL_X1;
    self->collision_bounds[2] = self->y+COL_Y1;
    self->collision_bounds[3] = self->x+COL_X2;
    self->collision_bounds[4] = self->y+COL_Y2;
    
    // Check for a collision with a bean if not retracting
    if (self->speed > 0)
    {
        col_index = instance_collision(obj_base, OBJ_BEAN);
        if (col_index != -1)
        {
            int score, index;
            u32* whichscore;
            char state;
        
            // Stop the tongue sound and play a sound saying we got a bean
            nuAuStlSndPlayerSndStop(*sndHandle_player, 1);
            *sndHandle_player = nuAuStlSndPlayerPlay(SND_BEAN_GET);
            
            // Check which score to get
            if (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && self->player == 0)
                whichscore = &GameScore1;
            else if (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && self->player == 1)
                whichscore = &GameScore2;
            else
                whichscore = &GameScore;
            
            score = (*whichscore);
            
            // Decide the score based on the height and set it
            if (self->y < 72)
                (*whichscore) += 1000;
            else if (self->y < 101)
                (*whichscore) += 300;
            else if (self->y < 129)
                (*whichscore) += 100;
            else if (self->y < 157)
                (*whichscore) += 50;
            else
                (*whichscore) += 10;
            
            // Create a score particle effect
            score = (*whichscore) - score;
            index = instance_create(OBJ_SCORES, self->x, self->y);
            if (index != -1)
                instance_set(index, SET_TYPE, &score);
            
            // Manipulate the bean
            state = STATE_DIE;
            instance_set(col_index, SET_X, &self->x);
            instance_set(col_index, SET_Y, &self->y);
            instance_set(col_index, SET_STATE, &state);
            self->speed = -(self->speed)*4;
            self->capture = col_index;
            
            // To avoid having to have each bean store a direction variable
            if (self->direction == DIR_RIGHT)
                instance_set(col_index, SET_SPEED, &self->speed);
            else
            {
                float speed_inv = -self->speed;
                instance_set(col_index, SET_SPEED, &speed_inv);
            }
            
            // Bean eating
            if (*(char*)instance_get(col_index, GET_TYPE) == 3 && bean3_destruction_y == 0)
            {
                // If we ate a flashing bean, set the destruction hitbox
                bean3_destruction_y = 206;
                bean3_destruction_player = self->player;
            }
            else if (*(char*)instance_get(col_index, GET_TYPE) == 2)
            {
                // If we ate a white bean, spawn an angel in the nearest empty spot to Pyoro
                char i=0;
                int nearest_x = -1;
                float distance_x = -1;
                for (i=0;i<=28;i++)
                {
                    if (instance_collision_point(OBJ_BLOCK, 49+8*i, 201) == -1)
                    {
                        int index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &(self->player));
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
                if (nearest_x != -1)
                {
                    sndHandle = nuAuStlSndPlayerPlay(SND_ANGEL_DOWN);
                    instance_create(OBJ_ANGEL, nearest_x, 28);
                }
            }
        }
    }
    
    // Retract the tongue if it hits a border/A is released
    if ((self->x >= 274 || self->x <= 46 || self->y <= 50 || !(PlayerIsAttacking(self->player, 1))) && self->speed > 0)
    {
        self->speed = -(self->speed)*4;
        nuAuStlSndPlayerSndStop(*sndHandle_player, 1);
        *sndHandle_player = nuAuStlSndPlayerPlay(SND_TONGUE_IN);
    }
    
    // Remove the tongue (and bean) when it returns to pyoro
    if (self->y >= self->y_origin)
    {
        int index;
        if (self->capture != -1 && instance_exists_index(self->capture))
        {
            // Destroy the bean
            instance_destroy_index(self->capture);
            self->capture = -1;
            
            // Set the chewing animation on Pyoro
            index = instance_find_withvalue(OBJ_PYORO, GET_PLAYER, &(self->player));
            if (index != -1)
            {
                char state = STATE_CHEW;
                instance_set(index, SET_STATE, &state);
            }
        }
        
        // Destroy self
        index = instance_get_index(obj_base);
        if (index != -1)
        {
            instance_destroy_index(index);
            
            // Return to not execute any code below (since it'll be NULL)
            return;
        }
    }
    
    // Transition the tlut if we have enough points
    if (GameScore >= DARK_MODE)
        tlut_transition(self->tlut, spr_tongue_tlut_dark, 4);
    else if (GameScore >= BEIGE_MODE)
        tlut_transition(self->tlut, spr_tongue_tlut_beige, 4);
}


/*=================================
               get_*
     Return a requested value
=================================*/

void* get_tongue(BASE *obj_base, u8 what)
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
    }
}


/*=================================
               set_*
    Sets a variable on an object
=================================*/

void set_tongue(BASE *obj_base, u8 what, void* value)
{
    switch(what)
    {
        case SET_X:
            self->x = *(float*) value;
            break;
        case SET_Y:
            self->y = *(float*) value;
            break;
        case SET_DIR:
            self->direction = *(char*) value;
            if (self->direction == DIR_LEFT)
                self->x = self->x-16;
            break;
        case SET_PLAYER:
            self->player = *(char*) value;
            break;
    }
}


/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_tongue(BASE *obj_base)
{
    // Destroy the bean if killed
    if (self->capture != -1 && instance_exists_index(self->capture))
        instance_destroy_index(self->capture);
        
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

void draw_tongue(BASE *obj_base)
{
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 70 + 30*self->player, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    gDPLoadTLUT_pal16( glistp++, 0, self->tlut);
    
    if (self->direction == DIR_LEFT)
    {
        int xpos = self->x_origin-4-2;
        int ypos = self->y_origin-6;
        int drawx = self->x+2;
        int drawy = ypos - (xpos - drawx);
        int i;
        
        // Draw tongue segments
        gDPLoadTextureTile_4b(glistp++, spr_tongue_body, G_IM_FMT_CI, 8, 8, 0, 0, 8, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        for (i=0;i<(((float)(xpos - self->x))/7);i++)
            gSPTextureRectangle(glistp++, xpos-(7*i)<<2, ypos-(7*i)<<2, 8+(xpos-(7*i))<<2, 8+(ypos-(7*i))<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gDPSetPrimDepth(glistp++, 60 + 30*self->player, 0);    
        // Draw the actual tongue
        gDPLoadTextureTile_4b(glistp++, spr_tongue, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH-1, HEIGHT-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, drawx-WIDTH/2<<2, drawy-HEIGHT/2<<2, drawx+WIDTH/2<<2, drawy+HEIGHT/2<<2, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    }
    else // Flip the texture
    {
        int xpos = self->x_origin-3-2;
        int ypos = self->y_origin-3;
        int drawx = self->x+2;
        int drawy = ypos - (drawx-xpos);
        int i;
        
        // Draw tongue segments
        gDPLoadTextureTile_4b(glistp++, spr_tongue_body, G_IM_FMT_CI, 8, 8, 0, 0, 8, 8, 0, G_TX_MIRROR, G_TX_NOMIRROR, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        for (i=0;i<(((float)(self->x - xpos))/7)-1;i++)
            gSPTextureRectangle(glistp++, xpos+1+(7*i)<<2, ypos-(7*i)<<2, 8+(xpos+1+(7*i))<<2, 8+(ypos-(7*i))<<2, G_TX_RENDERTILE, 8 << 5, 0 << 5, 1 << 10, 1 << 10 );
        gDPSetPrimDepth(glistp++, 60 + 30*self->player, 0);    
        // Draw the actual tongue
        gDPLoadTextureTile_4b(glistp++, spr_tongue, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, 0, G_TX_MIRROR, G_TX_NOMIRROR, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(glistp++, drawx-WIDTH/2<<2, drawy+9-HEIGHT/2<<2, drawx + WIDTH/2<<2, drawy+9 + HEIGHT/2<<2, G_TX_RENDERTILE, 16 << 5, 0 << 5, 1 << 10, 1 << 10 );
    }

    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}