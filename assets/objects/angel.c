/*========================================================
			           Angel Object
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
#include "../sprites/spr_angel.h"


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
    float x;
    float y;
    float speed;
    char  pitch;
    int   parent;
    
    unsigned char * sprite;
    OSTime frame_timer;
    unsigned short *tlut;
    
    float collision_bounds[5];
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_angel(BASE *obj_base, float x, float y)
{
    // Assign our variables struct
    obj_base->vars = (struct obj_vars *) malloc(sizeof(struct obj_vars));
    
    // Make sure it managed to allocate memory
    if (obj_base->vars == NULL)
    {
        obj_base->type = OBJ_NULL;
        return;
    }
    
    // Set some variables
    self->x = x;
    self->y = y;
    self->speed = 3 + 4*((float)GameSpeed/100);
    self->parent = instance_create(OBJ_BLOCK, self->x, self->y + 12);
    
    // Make sure we managed to create a block
    if (self->parent == -1)
    {
        free(obj_base->vars);
        obj_base->type = OBJ_NULL;
        return;
    }
    
    // Set more variables
    self->sprite = spr_angel_1;
    self->frame_timer = 0;
    self->pitch = 0;
    if (GameScore >= DARK_MODE)
        self->tlut = tlut_copy(spr_angel_tlut_dark, 4);
    else
        self->tlut = tlut_copy(spr_angel_tlut_default, 4);
        
    // Make sure we managed to create our TLUT
    if (self->tlut == NULL)
    {
        instance_destroy_index(self->parent);
        free(obj_base->vars);
        obj_base->type = OBJ_NULL;
        return;
    } 
    
    // Make the collision box specific for checking if an angel is occupying a location in tongue.c
    self->collision_bounds[0] = COL_RECTANGLE;
    self->collision_bounds[1] = x-WIDTH/4;
    self->collision_bounds[2] = 0;
    self->collision_bounds[3] = x+WIDTH/4;
    self->collision_bounds[4] = 20;
}


/*=================================
             update_*
        Updates the object
=================================*/

void update_angel(BASE *obj_base)
{
    // Move the angel
    if (self->y < 201-12 && self->speed > 0)
    {
        self->speed = ((float)(200-self->y))/(-0.085*((float)GameSpeed)+15);
        if (self->parent != -1 && instance_exists_index(self->parent))
            instance_set(self->parent, SET_SPEED, &self->speed);
        self->y = self->y + self->speed;
    }
    else
    {
        if (self->speed > 0)
        {
            self->y = 201 - HEIGHT;
            sndHandle = nuAuStlSndPlayerPlay(SND_ANGEL_REPAIR);
            MusHandleSetFreqOffset(sndHandle, self->pitch);
            self->parent = -1;
        }
        self->speed = -4.0 - 3.0*((float)GameSpeed/100);
        self->y = self->y + self->speed;
        if (self->y < 20)
        {
            int index = instance_get_index(obj_base);
            if (index != -1)
                instance_destroy_index(index);
                
            // Return to not execute any code below (since it'll be NULL)
            return;
        }
    }
    
    // Change frame depending on speed
    if (self->frame_timer < osGetTime())
    {
        if (self->speed < 0)
            self->frame_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(50000*abs(8+self->speed));
        else
            self->frame_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(50000 + (200-self->y)*1000 );
        if (self->sprite == spr_angel_1)
            self->sprite = spr_angel_2;
        else
            self->sprite = spr_angel_1;
    }
   
   // Transition the tlut if we have enough points
    if (GameScore >= DARK_MODE)
        tlut_transition(self->tlut, spr_angel_tlut_dark, 4);
}


/*=================================
               get_*
     Return a requested value
=================================*/

void* get_angel(BASE *obj_base, u8 what)
{
    switch(what)
    {
        case GET_BOUND:
            return (void*)self->collision_bounds;
        default:
            break;
    }
}


/*=================================
               set_*
    Sets a variable on an object
=================================*/

void set_angel(BASE *obj_base, u8 what, void* value)
{
    switch(what)
    {
        case SET_PITCH:
            self->pitch = *(char*) value*4;
            if (self->pitch == 8)
                self->pitch--;
            if (self->pitch >= 16)
                self->pitch = 12 + (self->pitch+4)%8;
            break;
        default:
            break;
    }
}


/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_angel(BASE *obj_base)
{
    // Destroy the parented block if it exists
    if (self->parent != -1 && instance_exists_index(self->parent))
        instance_destroy_index(self->parent);
        
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

void draw_angel(BASE *obj_base)
{
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 30, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    gDPLoadTLUT_pal16( glistp++, 0, self->tlut);
    
    gDPLoadTextureTile_4b(glistp++, self->sprite, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH-1, HEIGHT-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle( glistp++, ((int)self->x)-WIDTH/2<<2, ((int)self->y)-HEIGHT/2<<2, ((((int)self->x) + WIDTH/2)<<2), ((((int)self->y) + HEIGHT/2)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}