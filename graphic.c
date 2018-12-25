/*========================================================
				Initialize the graphics
========================================================*/
/*=================================
            Libraries
=================================*/

#include <nusys.h>
#include "config.h"


/*=================================
         Global Variables
=================================*/

// Initialize the game camera
static Vp viewport = {
  SCREEN_WD * 2, SCREEN_HT * 2, G_MAXZ / 2, 0,
  SCREEN_WD * 2, SCREEN_HT * 2, G_MAXZ / 2, 0,
};

// Initialize the RSP
Gfx rspinit_dl[] = {
    gsSPViewport(&viewport),
    gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
                            G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                            G_TEXTURE_GEN_LINEAR | G_LOD),
    gsSPTexture(0, 0, 0, 0, G_OFF),
    gsSPEndDisplayList(),
};

// Initialize the RDP
Gfx rdpinit_dl[] = {
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPPipelineMode(G_PM_1PRIMITIVE),
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
    gsDPSetColorDither(G_CD_DISABLE),
    gsDPSetAlphaDither(G_AD_DISABLE),
    gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetTextureConvert(G_TC_FILT),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};


/*=================================
              RCPInit
 Prepare the RCP for incoming work
=================================*/

void RCPInit(Gfx *glistp)
{
    // Set the segment register
    gSPSegment(glistp++, 0, 0);
    
    // Execute our RSP Display List
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(rspinit_dl));
    
    // Execute our RDP Display List
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(rdpinit_dl));
}