/*========================================================
			           Leaf Object
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include "../../config.h"
#include "base.h"

// Sprites
#include "../sprites/spr_bean_leaf.h"
#include "../sprites/spr_bean.h"

#define DIR_LEFT  0
#define DIR_RIGHT 1

/*=================================
           Custom Macros
=================================*/

#define WIDTH   8
#define HEIGHT  8


/*=================================
        Variables Structure
=================================*/

#define self ((struct obj_vars *)(obj_base->vars))

struct obj_vars
{
    float x;
    float y;
    float hspeed;
    float vspeed;
    unsigned char * sprite;
    char frame;
    char type;
    OSTime frame_timer;
    OSTime creationtime;
    unsigned short *tlut;
    float collision_bounds[5];
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_leaf_small(BASE *obj_base, float x, float y)
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
    self->sprite = spr_bean_leaf_big1;
    self->frame = guRandom()%6;
    self->frame_timer = 0;
    self->type = 1;
    self->hspeed = 0;
    self->vspeed = 0.5;
    self->creationtime = osGetTime();
    
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
    self->collision_bounds[0] = COL_RECTANGLE;
    self->collision_bounds[1] = self->x-WIDTH/2;
    self->collision_bounds[2] = self->y-HEIGHT/2;
    self->collision_bounds[3] = self->x+WIDTH/2;
    self->collision_bounds[4] = self->y+HEIGHT/2;
}


/*=================================
             update_*
        Updates the object
=================================*/

void update_leaf_small(BASE *obj_base)
{
    int index;

    // Move the leaf
    self->x += self->hspeed;
    self->y += self->vspeed;
    
    // Decrease the speed due to air resistance
    if (self->hspeed != 0)
    {
        self->hspeed += (self->hspeed > 0) ? -0.1 : 0.1;
        if (self->hspeed < 0.1 && self->hspeed > -0.1)
            self->hspeed = 0;
    }

    if (self->vspeed < 0.5)
        self->vspeed += 0.1;
    
    // If out of bounds, destroy self
    index = instance_get_index(obj_base);
    if ((self->y > 220 || self->x < 46 || self->x > 278) && index != -1)
    {
        instance_destroy_index(index);
        
        // Return to not execute any code below (since it'll be NULL)
        return;
    }
    
    // Update the collision bounds
    self->collision_bounds[0] = COL_RECTANGLE;
    self->collision_bounds[1] = self->x-WIDTH/2;
    self->collision_bounds[2] = self->y-HEIGHT/2;
    self->collision_bounds[3] = self->x+WIDTH/2;
    self->collision_bounds[4] = self->y+HEIGHT/2;
    
    // Change frame every 100000 microseconds
    if (self->frame_timer < osGetTime())
    {
        self->frame_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(100000);
        self->frame = (self->frame + 1)%6;
        switch (self->frame)
        {
            case 0:
            case 1:
                self->sprite = spr_bean_leaf_small1;
                break;
            case 2:
            case 5:
                self->sprite = spr_bean_leaf_small2;
                break;
            case 3:
            case 4:
                self->sprite = spr_bean_leaf_small3;
                break;
        }
    }

    // Transition the tlut if we have enough points
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
               set_*
    Sets a variable on an object
=================================*/

void set_leaf_small(BASE *obj_base, u8 what, void* value)
{
    switch(what)
    {
        case SET_SPEED:
            self->hspeed = *(float*)value;
            self->vspeed = -abs(*(float*)value);
            
            // Temporarily disable collision checking to avoid while loop always checking the same leaf
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
            else if (self->type == 3)
            {
                free(self->tlut);
                if (GameScore >= DARK_MODE)
                    self->tlut = tlut_copy(spr_bean3_tlut_dark, 9);
                else if (GameScore >= BEIGE_MODE)
                    self->tlut = tlut_copy(spr_bean3_tlut_beige, 9);
                else
                    self->tlut = tlut_copy(spr_bean3_tlut, 9);
            }
            break;
    }
}


/*=================================
               get_*
     Return a requested value
=================================*/

void* get_leaf_small(BASE *obj_base, u8 what)
{
    switch(what)
    {
        case GET_X:
            return &self->x;
        case GET_Y:
            return &self->y;
        case GET_BOUND:
            return (void*)self->collision_bounds;
        case GET_TYPE:
            return &self->type;
        case GET_TIME:
            return &self->creationtime;
    }
}


/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_leaf_small(BASE *obj_base)
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

void draw_leaf_small(BASE *obj_base)
{
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 30, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    gDPLoadTLUT_pal16(glistp++, 0, self->tlut);
    
    gDPLoadTextureTile_4b(glistp++, self->sprite, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH-1, HEIGHT-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, ((int)self->x)-WIDTH/2<<2, ((int)self->y)-HEIGHT/2<<2, ((((int)self->x) + WIDTH/2)<<2)-1, ((((int)self->y) + HEIGHT/2)<<2)-1, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}