/*========================================================
			         Seeds Object
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include <nualstl_n.h>
#include "../../config.h"
#include "../sprites/bac_solid.h"
#include "../sounds/definitions.h"
#include "base.h"


/*=================================
           Custom Macros
=================================*/

#define WIDTH   4
#define HEIGHT  4

#define NUM_SEEDS 3

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
    char player;
    float collision_bounds[5];
    float particles[NUM_SEEDS*2];
    float yspeed;
    char direction;
    char life;
};


/*=================================
             create_*
        Creates the object
=================================*/

void create_seeds(BASE *obj_base, float x, float y)
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
    self->direction = DIR_RIGHT;
        
    // Specify the collisions
    self->collision_bounds[0] = COL_LINE;
    self->collision_bounds[1] = x+7;
    self->collision_bounds[2] = y-7;

    // Calculate how much we need to add to x+y to make a line that hits a border
    i = 274-x;
    if (y-i <= 50)
        i = y-50;

    // Add the calculation to the other collision bound (aka the other point of the line)
    self->collision_bounds[3] = x+i;
    self->collision_bounds[4] = y-i;

    // Effect stuff
    self->life = 0;
    self->yspeed = -1;
    for (i=0; i<NUM_SEEDS*2; i+=2)
    {
        self->particles[i] = x+9;
        self->particles[i+1] = y-7;
    }
}


/*=================================
             update_*
        Updates the object
=================================*/

void update_seeds(BASE *obj_base)
{
    int index;
    char i;

    // Check for collisions
    if (self->collision_bounds[0] != COL_NONE)
    {
        u32* whichscore;
        int score = 0;
        int beanskilled = 0;
        float beanskilledpos[2*10]; // Can store the position of up to 10 beans
        char j;
        
        // Check for collisions with leaves
        do
        {
            index = instance_collision(obj_base, OBJ_LEAF);
            if (index != -1)
            {
                char health = (*(char*)instance_get(index, GET_HEALTH))-1;
                float speed = (((float)(10+guRandom()%10))/10)*((self->direction == DIR_RIGHT) ? 1 : -1);
                instance_set(index, SET_SPEED, &speed);
                instance_set(index, SET_HEALTH, &health);
            }
        }
        while (index != -1);
        
        // Check for collisions with small leaves
        do
        {
            index = instance_collision(obj_base, OBJ_LEAF_SMALL);
            if (index != -1)
            {
                float speed = (((float)(10+guRandom()%10))/10)*((self->direction == DIR_RIGHT) ? 1 : -1);
                instance_set(index, SET_SPEED, &speed);
            }
        }
        while (index != -1);
        
        // Get the x y position of all the beans in the line of fire
        do
        {
            index = instance_collision(obj_base, OBJ_BEAN);
            if (index != -1)
            {
                char type = *(char*)instance_get(index, GET_TYPE);
                beanskilledpos[beanskilled*2] = *(float*)instance_get(index, GET_X);
                beanskilledpos[1+beanskilled*2] = *(float*)instance_get(index, GET_Y);
                
                // Figure out if we destroyed a special bean
                if (type == 3 && bean3_destruction_y == 0)
                {
                    // If we destroyed a flashing bean, set the destruction hitbox
                    bean3_destruction_y = 206;
                    bean3_destruction_player = self->player;
                }
                else if (type == 2)
                {
                    // If we destroyed a white bean, spawn an angel in the nearest empty spot to Pyoro
                    int nearest_x = -1;
                    float distance_x = -1;
                    for (i=0;i<=28;i++)
                    {
                        if (instance_collision_point(OBJ_BLOCK, 49+8*i, 201) == -1)
                        {
                            int index = instance_find_withvalue(OBJ_PYORO2, GET_PLAYER, &(self->player));
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
                    
                    // If we found a spot, create an angel and play a sound
                    if (nearest_x != -1)
                    {
                        sndHandle = nuAuStlSndPlayerPlay(SND_ANGEL_DOWN);
                        instance_create(OBJ_ANGEL, nearest_x, 28);
                    }
                }
                
                // Create some leaves
                for (j=0;j<2;j++)
                {
                    int leaf_index = instance_create(OBJ_LEAF, beanskilledpos[beanskilled*2]+(-4+guRandom()%8), beanskilledpos[1+beanskilled*2]-(guRandom()%6));
                    if (leaf_index != -1)
                    {
                        float speed = (((float)(10+guRandom()%15))/10)*((self->direction == DIR_RIGHT) ? 1 : -1);
                        instance_set(leaf_index, SET_TYPE, &type);
                        instance_set(leaf_index, SET_SPEED, &speed);
                    }
                }
                instance_destroy_index(index);
                beanskilled++;
            }
        }
        while (index != -1 && beanskilled < 10);
        
        // Calculate the score based on how many beans were destroyed
        switch(beanskilled)
        {
            case 0:
                break;
            case 1:
                score = 50;
                sndHandle = nuAuStlSndPlayerPlay(SND_BEAN_DIE1);
                break;
            case 2:
                score = 100;
                sndHandle = nuAuStlSndPlayerPlay(SND_BEAN_DIE2);
                break;
            case 3:
                score = 300;
                sndHandle = nuAuStlSndPlayerPlay(SND_BEAN_DIE3);
                break;
            default:
                score = 1000;
                sndHandle = nuAuStlSndPlayerPlay(SND_BEAN_DIE4);
                break;
        }
        
        // Pick which score to give
        if (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && self->player == 0)
            whichscore = &GameScore1;
        else if (global_save_settings[SAVE_SETTINGS_PLAYER] == 1 && self->player == 1)
            whichscore = &GameScore2;
        else
            whichscore = &GameScore;
        
        // Increase said score
        (*whichscore) += score*beanskilled;
        
        // Create some effects
        for (i=0;i<beanskilled;i++)
        {
            int score_obj;
            
            // Create smoke puffs
            instance_create(OBJ_PUFF, beanskilledpos[i*2], beanskilledpos[1+i*2]);
            for (j=0; j<3; j++)
            {
                int puff_obj = instance_create(OBJ_PUFF_SMALL, beanskilledpos[i*2]+(8-guRandom()%16), beanskilledpos[1+i*2]+(8-guRandom()%16));
                if (puff_obj != -1)
                    instance_set(puff_obj, SET_DIR, &self->direction);
            }
            
            // Create a score particle
            score_obj = instance_create(OBJ_SCORES, beanskilledpos[i*2], beanskilledpos[1+i*2]);
            if (score_obj != -1)
                instance_set(score_obj, SET_TYPE, &score);
        }

        // Stop checking collisions with this seeds object
        self->collision_bounds[0] = COL_NONE;
    }
    
    // Move the seeds effect
    for(i=0; i<NUM_SEEDS*2; i+=2)
    {
        self->particles[i] += ((self->direction == DIR_LEFT) ? -1 : 1)*(1+guRandom()%30)/4;
        self->particles[i+1] += self->yspeed*(1+guRandom()%30)/4;
    }
    self->yspeed += 0.10;
    
    // Destroy the seeds after some time has passed
    self->life++;
    if (self->life >= 10)
    {
        int index = instance_get_index(obj_base);
        if (index != -1)
            instance_destroy_index(index);
    }
}


/*=================================
               get_*
     Return a requested value
=================================*/

void* get_seeds(BASE *obj_base, u8 what)
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

void set_seeds(BASE *obj_base, u8 what, void* value)
{
    int i;
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
            {
                // Reposition collision bounds and other stuff since we're facing the other direction
                i = self->x-46;
                if (self->y-i <= 50)
                    i = self->y-50;
                self->collision_bounds[1] = self->x-7;
                self->collision_bounds[3] = self->x+7-i;
                self->collision_bounds[4] = self->y-7-i;
            
                for (i=0; i<NUM_SEEDS*2; i+=2)
                {
                    self->particles[i] = self->x-9;
                    self->particles[i+1] = self->y-7;
                }
            }
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

void destroy_seeds(BASE *obj_base)
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

void draw_seeds(BASE *obj_base)
{
    int i;
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 100, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPSetTextureLUT(glistp++, G_TT_RGBA16);
    gDPSetCombineMode(glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM );
    gDPLoadTLUT_pal16(glistp++, 0, bac_solid_tlut_white);
    gDPLoadTextureBlock_4b(glistp++, bac_solid, G_IM_FMT_CI, 16, 16, 0, G_TX_WRAP, G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
    for(i=0;i<NUM_SEEDS*2; i+=2)
    {
        u8 col = 5*(guRandom()%51);
        if (GameScore >= BEIGE_MODE)
            col = 0;
        gDPSetPrimColor(glistp++,0,0,255,255-col/2,255-col,255); 
        gSPTextureRectangle(glistp++, (int)self->particles[i] << 2, (int)self->particles[i+1] << 2, (int)self->particles[i]+1 << 2, (int)self->particles[i+1]+1 << 2,  G_TX_RENDERTILE, 0 << 5, 0 << 5,  1 << 10, 1 << 10);
    }
    gDPPipeSync(glistp++);
    gDPSetTextureLUT( glistp++, G_TT_NONE );
}