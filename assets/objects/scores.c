/*========================================================
			           Scores Object
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include "../../config.h"
#include "base.h"

// Sprites
#include "../sprites/spr_scores.h"


/*=================================
        Variables Structure
=================================*/

#define self ((struct obj_vars *)(obj_base->vars))

struct obj_vars
{
    float x;
    float y;
    
    unsigned int type;
    unsigned short *tlut;
    OSTime timer;
    OSTime lifetime;
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_scores(BASE *obj_base, float x, float y)
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
    self->tlut = spr_scores_tlut_white;
    self->type = 10;
    self->lifetime = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(1000000);
    self->timer = osGetTime();
}


/*=================================
             update_*
        Updates the object
=================================*/

void update_scores(BASE *obj_base)
{
    // Set the TLUT according to the type
    switch(self->type)
    {
        case 10:
            if (GameScore >= DARK_MODE)
                self->tlut = spr_scores_tlut_white;
            else if (GameScore >= BEIGE_MODE)
                self->tlut = spr_scores_tlut_red_beige;
            else
                self->tlut = spr_scores_tlut_red;
            break;
        case 50:
            if (GameScore >= DARK_MODE)
                self->tlut = spr_scores_tlut_white;
            else if (GameScore >= BEIGE_MODE)
                self->tlut = spr_scores_tlut_blue_beige;
            else
                self->tlut = spr_scores_tlut_blue;
            break;
        case 100:
            self->tlut = spr_scores_tlut_white;
            break;
        case 300:
            if (self->timer < osGetTime())
            {
                if (GameScore >= DARK_MODE)
                {
                    if (self->tlut == spr_scores_tlut_black)
                        self->tlut = spr_scores_tlut_white;
                    else
                        self->tlut = spr_scores_tlut_black;
                }
                else if (GameScore >= BEIGE_MODE)
                {
                    if (self->tlut == spr_scores_tlut_blue_beige)
                        self->tlut = spr_scores_tlut_gold_beige;
                    else
                        self->tlut = spr_scores_tlut_blue_beige;
                }
                else
                {
                    if (self->tlut == spr_scores_tlut_blue)
                        self->tlut = spr_scores_tlut_gold;
                    else
                        self->tlut = spr_scores_tlut_blue;
                }
                self->timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(50000);
            }
            break;
        case 1000:
            if (self->timer < osGetTime())
            {
                if (GameScore >= DARK_MODE)
                {
                    if (self->tlut == spr_scores_tlut_black)
                        self->tlut = spr_scores_tlut_white;
                    else
                        self->tlut = spr_scores_tlut_black;
                }
                else if (GameScore >= BEIGE_MODE)
                {
                    if (self->tlut == spr_scores_tlut_white)
                        self->tlut = spr_scores_tlut_gold_beige;
                    else
                        self->tlut = spr_scores_tlut_white;
                }
                else
                {
                    if (self->tlut == spr_scores_tlut_white)
                        self->tlut = spr_scores_tlut_gold;
                    else
                        self->tlut = spr_scores_tlut_white;
                }
                self->timer = osGetTime() + OS_USEC_TO_CYCLES_CORRECT(50000);
            }
            break;
    }

    // Destroy the instance after 1 second
    if (self->lifetime < osGetTime())
    {
        int index = instance_get_index(obj_base);
        if (index != -1)
        {
            instance_destroy_index(index);
            
            // Return to not execute any code below (since it'll be NULL)
            return;
        }
    }
}


/*=================================
               set_*
     Return a requested value
=================================*/

void set_scores(BASE *obj_base, u8 what, void* value)
{
    switch(what)
    {
        case SET_TYPE:
            self->type = *(int*) value;
            break;
    }
}


/*=================================
             destroy_*
        Destroy the object
=================================*/

void destroy_scores(BASE *obj_base)
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

void draw_scores(BASE *obj_base)
{
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 10, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    
    gDPSetTextureLUT( glistp++, G_TT_RGBA16 );
    gDPLoadTLUT_pal16( glistp++, 0, self->tlut);
    
    switch(self->type)
    {
        case 10:
            gDPLoadTextureTile_4b(glistp++, spr_scores_10, G_IM_FMT_CI, 8, 8, 0, 0, 8, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle( glistp++, ((int)self->x)-4<<2, ((int)self->y)-4<<2, ((((int)self->x) + 4)<<2), ((((int)self->y) + 4)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
            break;
        case 50:
            gDPLoadTextureTile_4b(glistp++, spr_scores_50, G_IM_FMT_CI, 8, 8, 0, 0, 8, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle( glistp++, ((int)self->x)-4<<2, ((int)self->y)-4<<2, ((((int)self->x) + 4)<<2), ((((int)self->y) + 4)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
            break;
        case 100:
            gDPLoadTextureTile_4b(glistp++, spr_scores_100, G_IM_FMT_CI, 16, 8, 0, 0, 16, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle( glistp++, ((int)self->x)-8<<2, ((int)self->y)-4<<2, ((((int)self->x) + 8)<<2), ((((int)self->y) + 4)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
            break;
        case 300:
            gDPLoadTextureTile_4b(glistp++, spr_scores_300, G_IM_FMT_CI, 16, 8, 0, 0, 16, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle( glistp++, ((int)self->x)-8<<2, ((int)self->y)-4<<2, ((((int)self->x) + 8)<<2), ((((int)self->y) + 4)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
            break;
        case 1000:
            gDPLoadTextureTile_4b(glistp++, spr_scores_1000, G_IM_FMT_CI, 16, 8, 0, 0, 16, 8, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle( glistp++, ((int)self->x)-8<<2, ((int)self->y)-4<<2, ((((int)self->x) + 8)<<2), ((((int)self->y) + 4)<<2), G_TX_RENDERTILE, 0 << 5, 0 << 5, 1 << 10, 1 << 10 );
            break;
    }
    gDPPipeSync(glistp++);
    gDPSetTextureLUT(glistp++, G_TT_NONE);
}