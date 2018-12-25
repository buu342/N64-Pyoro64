/*========================================================
			           Puff (small) Object
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include "../../config.h"
#include "base.h"

// Sprites
#include "../sprites/spr_puff.h"


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
    int x;
    int y;
    unsigned char * sprite;
    int frame;
    OSTime frame_timer;
    unsigned short *tlut;
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_puff(BASE *obj_base, float x, float y)
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
    self->sprite = spr_puff_1;
    self->frame = 0;
    self->frame_timer = 0;
    
    if (GameScore >= DARK_MODE)
        self->tlut = tlut_copy(spr_puff_tlut_dark, 4);
    else
        self->tlut = tlut_copy(spr_puff_tlut_default, 4);
        
    // Make sure we managed to create our TLUT
    if (self->tlut == NULL)
    {
        free(obj_base->vars);
        obj_base->type = OBJ_NULL;
        return;
    } 
}


/*=================================
             update_*
        Updates the object
=================================*/

void update_puff(BASE *obj_base)
{
    int index;
    // Change frame every 100000 microseconds
    if (self->frame_timer < osGetTime())
    {
        self->frame_timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(100000);
        self->frame++;
        switch (self->frame)
        {
            case 0:
                self->sprite = spr_puff_1;
                break;
            case 1:
                self->sprite = spr_puff_2;
                break;
            case 2:
                self->sprite = spr_puff_3;
                break;
            case 3:
                index = instance_get_index(obj_base);
                if (index != -1)
                {
                    instance_destroy_index(index);
                    
                    // Return to not execute any code below (since it'll be NULL)
                    return;
                }
                break;
        }
    }

    // Transition the tlut if we have enough points
    if (GameScore >= DARK_MODE)
        tlut_transition(self->tlut, spr_puff_tlut_dark, 4);
}


/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_puff(BASE *obj_base)
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

void draw_puff(BASE *obj_base)
{
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 20, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    gDPLoadTLUT_pal16( glistp++, 0, self->tlut);
    
    gDPLoadTextureTile_4b(glistp++, self->sprite, G_IM_FMT_CI, WIDTH, HEIGHT, 0, 0, WIDTH-1, HEIGHT-1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle( glistp++, ((int)self->x)-WIDTH/2<<2, ((int)self->y)-HEIGHT/2<<2, ((((int)self->x) + WIDTH/2)<<2)-1, ((((int)self->y) + HEIGHT/2)<<2)-1, G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
    
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}