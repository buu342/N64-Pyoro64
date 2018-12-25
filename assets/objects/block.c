/*========================================================
			           Block Object
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include "../../config.h"
#include "base.h"

// Sprites
#include "../sprites/spr_block.h"


/*=================================
           Custom Macros
=================================*/

#define WIDTH   8
#define HEIGHT  8

#define COL_X1 -WIDTH/2
#define COL_Y1 -HEIGHT/2
#define COL_X2 +WIDTH/2
#define COL_Y2 +HEIGHT/2


/*=================================
        Variables Structure
=================================*/

#define self ((struct obj_vars *)(obj_base->vars))


struct obj_vars
{
    float x;
    float y;
    float speed;
    unsigned short *tlut;
    
    float collision_bounds[5];
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_block(BASE *obj_base, float x, float y)
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
    self->speed = 0;
    
    if (GameScore >= DARK_MODE)
        self->tlut = tlut_copy(spr_block_tlut_dark, 5);
    else if (GameScore >= BEIGE_MODE)
        self->tlut = tlut_copy(spr_block_tlut_beige, 5);
    else
        self->tlut = tlut_copy(spr_block_tlut, 5);
    
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

void update_block(BASE *obj_base)
{
    // Move the block if it has speed
    if (self->y < 201)
    {
        self->collision_bounds[0] = COL_NONE;
        self->y = self->y + self->speed;
    }
    else if (self->speed > 0)
    {
        self->y = 201;
        self->speed = 0;
        self->collision_bounds[0] = COL_RECTANGLE;
        self->collision_bounds[1] = self->x+COL_X1;
        self->collision_bounds[2] = self->y+COL_Y1;
        self->collision_bounds[3] = self->x+COL_X2;
        self->collision_bounds[4] = self->y+COL_Y2;
    }
    
    // Transition the tlut if we reach the proper score
    if (GameScore >= DARK_MODE)
        tlut_transition(self->tlut, spr_block_tlut_dark, 5);
    else if (GameScore >= BEIGE_MODE)
        tlut_transition(self->tlut, spr_block_tlut_beige, 5);
}


/*=================================
               get_*
     Return a requested value
=================================*/

void* get_block(BASE *obj_base, u8 what)
{
    switch(what)
    {
        case GET_X:
            return &self->x;
        case GET_Y:
            return &self->y;
        case GET_BOUND:
            return (void*)self->collision_bounds;
    }
}


/*=================================
               set_*
    Sets a variable on an object
=================================*/

void set_block(BASE *obj_base, u8 what, void* value)
{
    switch(what)
    {
        case SET_X:
            self->x = *(float*)value;
            self->collision_bounds[1] = self->x+COL_X1;
            self->collision_bounds[2] = self->y+COL_Y1;
            self->collision_bounds[3] = self->x+COL_X2;
            self->collision_bounds[4] = self->y+COL_Y2;
            break;
        case SET_SPEED:
            self->speed = *(float*)value;
            break;
    }
}

/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_block(BASE *obj_base)
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

void draw_block(BASE *obj_base)
{
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 100, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    gDPLoadTLUT_pal16( glistp++, 0, self->tlut);
    
    gDPLoadTextureTile_4b(glistp++, spr_block, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH-1, HEIGHT-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle( glistp++, ((int)self->x)-WIDTH/2<<2, ((int)self->y)-HEIGHT/2<<2, ((((int)self->x) + WIDTH/2)<<2), ((((int)self->y) + HEIGHT/2)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}