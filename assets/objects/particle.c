/*========================================================
			         Particle Object
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include "../../config.h"
#include "../sprites/bac_solid.h"
#include "base.h"


/*=================================
           Custom Macros
=================================*/

#define WIDTH   4
#define HEIGHT  4

#define JUMPNUM 5
#define JUMPAMT 10


/*=================================
        Variables Structure
=================================*/

#define self ((struct obj_vars *)(obj_base->vars))


struct obj_vars
{
    float x;
    float y;
    OSTime nextjump;
    float oldy[JUMPNUM];
    
    u16 color;
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_particle(BASE *obj_base, float x, float y)
{
    int i=0;
    
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
    self->color = 0x01;
    self->nextjump = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(100000);
    
    self->oldy[0] = y;
    for (i=1;i<JUMPNUM;i++)
        self->oldy[i] = -1;
}


/*=================================
             update_*
        Updates the object
=================================*/

void update_particle(BASE *obj_base)
{
    int i;
    
    // Jump the particle effect upwards
    if (self->nextjump < osGetTime())
    {
        self->nextjump = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(100000);
        self->y -= JUMPAMT;
        for(i=0;i<JUMPNUM;i++)
        {
            if (self->oldy[i] == -1)
            {
                self->oldy[i] = self->y + (i*JUMPAMT);
                return;
            }
            self->oldy[i] -= JUMPAMT;
        }
        
        if (self->oldy[JUMPNUM-1] < 44)
        {
            int index = instance_get_index(obj_base);
            if (index != -1)
                instance_destroy_index(index);
        }
    }
}


/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_particle(BASE *obj_base)
{
    // Free the memory used by our variables struct
    free(obj_base->vars);
    
    // Mark the slot in GameObjects as empty
    obj_base->type = OBJ_NULL;
}


/*=================================
              draw_*
         Draws the object
=================================*/

void draw_particle(BASE *obj_base)
{
    int i;
    
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 120, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
    gDPLoadTLUT_pal16(glistp++, 0, bac_solid_tlut_white);
    gDPLoadTextureBlock_4b(glistp++, bac_solid, G_IM_FMT_CI, 16, 16, 0, G_TX_WRAP, G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
    for(i=0;i<JUMPNUM;i++)
    {
        if (self->oldy[i] > 44)
        {
            char brightness = 255-(((float)i)/((float)JUMPNUM))*255;
            gDPSetPrimColor(glistp++,0,0,255,255,255,brightness); 
            gSPTextureRectangle(glistp++, (int)self->x-WIDTH/2 << 2, (int)self->oldy[i]-HEIGHT/2 << 2, (int)self->x+WIDTH/2 << 2, (int)self->oldy[i]+HEIGHT/2 << 2,  G_TX_RENDERTILE, 0 << 5, 0 << 5,  1 << 10, 1 << 10);
        }
    }
    gDPPipeSync(glistp++);
    gDPSetTextureLUT( glistp++, G_TT_NONE );
}