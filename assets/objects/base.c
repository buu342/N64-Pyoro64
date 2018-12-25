/*========================================================
			 Base Object to derive all from
========================================================*/
/*=================================
            Libraries
=================================*/
#include <nusys.h>
#include "../../config.h"
#include "base.h"
#include "objects.h"


/*=================================
         Global Variables
=================================*/

BASE GameObjects[MAXOBJECTS];


/*=================================
          instance_create
         Creates an object
=================================*/

int instance_create(char type, float x, float y)
{
    int i=0;
    
    // Cycle through all the objects until we find an empty slot
    for (i=0;i<MAXOBJECTS;i++)
        if (GameObjects[i].type == OBJ_NULL)
            break;
    
    // Specify this slot as taken
    GameObjects[i].type = type;

    // Assign the correct functions based on the type
    switch (type)
    {
        case OBJ_PYORO:
            GameObjects[i].Create   = create_pyoro;
            GameObjects[i].Update   = update_pyoro;
            GameObjects[i].Get      = get_pyoro;
            GameObjects[i].Set      = set_pyoro;
            GameObjects[i].Destroy  = destroy_pyoro;
            GameObjects[i].Draw     = draw_pyoro;
            break;        
        case OBJ_PYORO2:
            GameObjects[i].Create   = create_pyoro2;
            GameObjects[i].Update   = update_pyoro2;
            GameObjects[i].Get      = get_pyoro2;
            GameObjects[i].Set      = set_pyoro2;
            GameObjects[i].Destroy  = destroy_pyoro2;
            GameObjects[i].Draw     = draw_pyoro2;
            break;
        case OBJ_BLOCK:
            GameObjects[i].Create   = create_block;
            GameObjects[i].Update   = update_block;
            GameObjects[i].Get      = get_block;
            GameObjects[i].Set      = set_block;
            GameObjects[i].Destroy  = destroy_block;
            GameObjects[i].Draw     = draw_block;
            break;
        case OBJ_BEAN:
            GameObjects[i].Create   = create_bean;
            GameObjects[i].Update   = update_bean;
            GameObjects[i].Get      = get_bean;
            GameObjects[i].Set      = set_bean;
            GameObjects[i].Destroy  = destroy_bean;
            GameObjects[i].Draw     = draw_bean;
            break;
        case OBJ_PUFF:
            GameObjects[i].Create   = create_puff;
            GameObjects[i].Update   = update_puff;
            GameObjects[i].Get      = get_dummy;
            GameObjects[i].Set      = set_dummy;
            GameObjects[i].Destroy  = destroy_puff;
            GameObjects[i].Draw     = draw_puff;
            break;
        case OBJ_TONGUE:
            GameObjects[i].Create   = create_tongue;
            GameObjects[i].Update   = update_tongue;
            GameObjects[i].Get      = get_tongue;
            GameObjects[i].Set      = set_tongue;
            GameObjects[i].Destroy  = destroy_tongue;
            GameObjects[i].Draw     = draw_tongue;
            break;
        case OBJ_SCORES:
            GameObjects[i].Create   = create_scores;
            GameObjects[i].Update   = update_scores;
            GameObjects[i].Get      = get_dummy;
            GameObjects[i].Set      = set_scores;
            GameObjects[i].Destroy  = destroy_scores;
            GameObjects[i].Draw     = draw_scores;
            break;
        case OBJ_ANGEL:
            GameObjects[i].Create   = create_angel;
            GameObjects[i].Update   = update_angel;
            GameObjects[i].Get      = get_angel;
            GameObjects[i].Set      = set_angel;
            GameObjects[i].Destroy  = destroy_angel;
            GameObjects[i].Draw     = draw_angel;
            break;
        case OBJ_PARTICLE:
            GameObjects[i].Create   = create_particle;
            GameObjects[i].Update   = update_particle;
            GameObjects[i].Get      = get_dummy;
            GameObjects[i].Set      = set_dummy;
            GameObjects[i].Destroy  = destroy_particle;
            GameObjects[i].Draw     = draw_particle;
            break;
        case OBJ_SEEDS:
            GameObjects[i].Create   = create_seeds;
            GameObjects[i].Update   = update_seeds;
            GameObjects[i].Get      = get_seeds;
            GameObjects[i].Set      = set_seeds;
            GameObjects[i].Destroy  = destroy_seeds;
            GameObjects[i].Draw     = draw_seeds;
            break;
        case OBJ_PUFF_SMALL:
            GameObjects[i].Create   = create_puff_small;
            GameObjects[i].Update   = update_puff_small;
            GameObjects[i].Get      = get_dummy;
            GameObjects[i].Set      = set_puff_small;
            GameObjects[i].Destroy  = destroy_puff_small;
            GameObjects[i].Draw     = draw_puff_small;
            break;
        case OBJ_LEAF:
            GameObjects[i].Create   = create_leaf;
            GameObjects[i].Update   = update_leaf;
            GameObjects[i].Get      = get_leaf;
            GameObjects[i].Set      = set_leaf;
            GameObjects[i].Destroy  = destroy_leaf;
            GameObjects[i].Draw     = draw_leaf;
            break;        
        case OBJ_LEAF_SMALL:
            GameObjects[i].Create   = create_leaf_small;
            GameObjects[i].Update   = update_leaf_small;
            GameObjects[i].Get      = get_leaf_small;
            GameObjects[i].Set      = set_leaf_small;
            GameObjects[i].Destroy  = destroy_leaf_small;
            GameObjects[i].Draw     = draw_leaf_small;
            break;
    }
    
    // Execute that type's create function
    GameObjects[i].Create(&GameObjects[i], x, y);
    
    // Return the index, in case we wish to manipulate this object after
    if (GameObjects[i].type == OBJ_NULL)
        return -1;
    else
        return i;
}


/*=================================
        instance_update_all
  Runs everyone's update function
=================================*/

void instance_update_all()
{
    int i=0;
    
    // Cycle through all non-null objects and run their update function
    for (i=0;i<MAXOBJECTS;i++)
        if (GameObjects[i].type != OBJ_NULL)
            GameObjects[i].Update(&GameObjects[i]);
}


/*=================================
        instance_draw_all
   Runs everyone's draw function
=================================*/

void instance_draw_all()
{ 
    int i=0;
    
    // Cycle through all non-null objects and run their draw function
    for (i=0;i<MAXOBJECTS;i++)
        if (GameObjects[i].type != OBJ_NULL)
            GameObjects[i].Draw(&GameObjects[i]);
}


/*=================================
        instance_destroy_all
  Runs everyone's destroy function
=================================*/

void instance_destroy_all()
{ 
    int i=0;
    
    // Cycle through all non-null objects and run their destroy function
    for (i=0;i<MAXOBJECTS;i++)
        if (GameObjects[i].type != OBJ_NULL)
            GameObjects[i].Destroy(&GameObjects[i]);
}

/*=================================
 Dummy Functions if you don't wish
 to use Get/Set (prevent crashes if
    an object gets overwritten)
=================================*/

char dummy = -1;
void* get_dummy(BASE *obj_base, u8 what)
{
    return (char*)&dummy;
}

void  set_dummy(BASE *obj_base, u8 what, void* value){}


/*=================================
        instance_get_index
Return the index of this object in
      the GameObjects array
=================================*/

int instance_get_index(BASE *obj)
{
    int i=0;
    
    // Cycle through all objects until the addresses compare
    for (i=0;i<MAXOBJECTS;i++)
        if (&GameObjects[i] == obj)
            return i;
            
    // Return -1 if we didn't find it
    return -1;
}


/*=================================
       instance_get_nearest
  Return the index of the closest
        object with a type
=================================*/

int instance_get_nearest(BASE *obj, u8 type)
{
    int caller_index, check_index, closest_index = -1;
    float call_x, call_y, closest_distance = -1;
   
    // Get the index of the calling object
    for (caller_index=0;caller_index<MAXOBJECTS+1;caller_index++)
    {
        // If we hit the MAXOBJECTS limit, return fail
        if (caller_index == MAXOBJECTS)
            return -1;
        
        // Search through all objects until we find the calling object's index
        if (&GameObjects[caller_index] == obj)
            break;
    }
    
    // Assign the caller variables (to make our code less chunky below)
    call_x = *(float*)GameObjects[caller_index].Get(&GameObjects[caller_index], GET_X);
    call_y = *(float*)GameObjects[caller_index].Get(&GameObjects[caller_index], GET_Y);
    
    // Cycle through all objects and check their distance
    for (check_index=0;check_index<MAXOBJECTS;check_index++)
    {
        if (GameObjects[check_index].type == type)
        {
            float check_x = *(float*)GameObjects[check_index].Get(&GameObjects[check_index], GET_X);
            float check_y = *(float*)GameObjects[check_index].Get(&GameObjects[check_index], GET_Y);
            float distance = (check_x - call_x)*(check_x - call_x) + (check_y - call_y)*(check_y - call_y);
            if (closest_distance == -1 || distance < closest_distance)
            {
                closest_index = check_index;
                closest_distance = distance;
            }
        }
    }
    return closest_index;
}


/*=================================
       instance_destroy_index
  Run the destroy function of the
  object with the specified index
=================================*/

void instance_destroy_index(int index)
{
    GameObjects[index].Destroy(&GameObjects[index]);
}


/*=================================
        instance_collision
  Returns the index of the first
 object found colliding, or -1 if
      there was no collision
=================================*/

int instance_collision(BASE *obj, u8 type)
{
    int caller_index, check_index;
    float call_x, call_y;
    float* check_bounds;
   
    // Get the index of the calling object
    for (caller_index=0;caller_index<MAXOBJECTS+1;caller_index++)
    {
        // If we hit the MAXOBJECTS limit, return fail
        if (caller_index == MAXOBJECTS)
            return -1;
        
        // Search through all objects until we find the calling object's index
        if (&GameObjects[caller_index] == obj)
            break;
    }
    
    // Assign the caller variables (to make our code less chunky below)
    call_x = *(float*)GameObjects[caller_index].Get(&GameObjects[caller_index], GET_X);
    call_y = *(float*)GameObjects[caller_index].Get(&GameObjects[caller_index], GET_Y);
    
    // Check collisions with all objects of the requested type
    for (check_index=0;check_index<MAXOBJECTS;check_index++)
    {
        // Check for collisions
        if (GameObjects[check_index].type == type)
        {
            float check_x = *(float*)GameObjects[check_index].Get(&GameObjects[check_index], GET_X);
            float check_y = *(float*)GameObjects[check_index].Get(&GameObjects[check_index], GET_Y);
            float* call_bounds = (float*)GameObjects[caller_index].Get(&GameObjects[caller_index], GET_BOUND);
            check_bounds = (float*)GameObjects[check_index].Get(&GameObjects[check_index], GET_BOUND);
            
            // Select a collision based off what both objects have as collision types.
            switch ((int)call_bounds[0])
            {
                case COL_RECTANGLE:
                    if (check_bounds[0] == COL_RECTANGLE)
                    {
                        // Game specific cheat. I know that I won't have objects larger than 64 pixels, so I can skip checking if the distance is > 64. 
                        // I avoid using Sqrt as its unnecessary calculations, 4096 = 64^2 
                        if ((float)((check_x - call_x)*(check_x - call_x) + (check_y - call_y)*(check_y - call_y)) < 4096)
                        {
                           if (!((call_bounds[3]<check_bounds[1])||(check_bounds[3]<call_bounds[1])||(call_bounds[4]<check_bounds[2])||(check_bounds[4]<call_bounds[2])))
                                return check_index;
                        }
                    }
                    break;
                case COL_LINE:
                    if (check_bounds[0] == COL_RECTANGLE)
                    {
                        // Liang–Barsky algorithm implementation
                        // Create some temporary variables
                        float minX = call_bounds[1];
                        float maxX = call_bounds[3];
                        float minY;
                        float maxY;
                        float dx;

                        // Swap the min/max if they're incorrect
                        if (call_bounds[1] > call_bounds[3])
                        {
                            minX = call_bounds[3];
                            maxX = call_bounds[1];
                        }

                        // Find the intersection of the segment's and rectangle's x-projections
                        if (maxX > check_bounds[3])
                            maxX = check_bounds[3];
                        if (minX < check_bounds[1])
                            minX = check_bounds[1];

                        // If their projections do not intersect then stop checking this object
                        if (minX > maxX) 
                            continue;

                        // Find corresponding min and max Y for min and max X we found before
                        minY = call_bounds[2];
                        maxY = call_bounds[3];
                        dx = call_bounds[3] - call_bounds[1];
                        
                        if (dx != 0)
                        {
                            float a = (call_bounds[4] - call_bounds[2]) / dx;
                            float b = call_bounds[2] - a * call_bounds[1];
                            minY = a * minX + b;
                            maxY = a * maxX + b;
                        }

                        // Swap the min/max if they're incorrect
                        if (minY > maxY)
                        {
                            float tmp = maxY;
                            maxY = minY;
                            minY = tmp;
                        }

                        // Find the intersection of the segment's and rectangle's y-projections
                        if (maxY > check_bounds[4])
                            maxY = check_bounds[4];
                        if (minY < check_bounds[2])
                            minY = check_bounds[2];

                        // If their projections do not intersect then stop checking this object
                        if (minY > maxY)
                            continue;
                        
                        // There was a collision!
                        return check_index;
                    }
                    break;
            }
        }
    }
    return -1;
}


/*=================================
     instance_collision_point
Returns the index of the object at
      a specific coordinate
=================================*/

int instance_collision_point(u8 type, float x, float y)
{
    int check_index;
    for (check_index=0;check_index<MAXOBJECTS+1;check_index++)
    {
        // If we didn't find the checking object, return fail
        if (check_index == MAXOBJECTS)
            return -1;
            
        // Otherwise, check for collisions
        if (GameObjects[check_index].type == type)
        {
            float* check_bounds = (float*)GameObjects[check_index].Get(&GameObjects[check_index], GET_BOUND);
            switch ((int)check_bounds[0])
            {
                case COL_RECTANGLE:
                    if (check_bounds[0] == COL_RECTANGLE)
                    {
                        if (x >= check_bounds[1] && x <= check_bounds[3] && y >= check_bounds[2] && y <= check_bounds[4])
                            return check_index;
                    }
                    break;
            }
        }
    }
}


/*=================================
           instance_find
Returns the index of an instance if
         it exists, or -1
=================================*/

int instance_find(u8 type)
{
    int i=0;
    for (i=0;i<MAXOBJECTS;i++)
        if (GameObjects[i].type == type)
            return i;
    return -1;
}


/*=================================
      instance_find_withvalue
Returns the index of an instance if
it exists with a specific value set
               or -1
=================================*/

int instance_find_withvalue(u8 type, u8 what, void* value)
{
    int i=0;
    for (i=0;i<MAXOBJECTS;i++)
        if (GameObjects[i].type == type)
        {
            switch (what)
            {
                case GET_PLAYER:
                    if (*(char*)GameObjects[i].Get(&GameObjects[i], what) == *(char*)value)
                        return i;
                    break;
            }
        }
    return -1;
}


/*=================================
       instance_exists_index
Return 1 if the given index is not
             OBJ_NULL
=================================*/

char instance_exists_index(int index)
{
    return (GameObjects[index].type != OBJ_NULL);
}


/*=================================
           instance_set
  Executes the set function of an
              object
=================================*/

void instance_set(int index, u8 what, void* value)
{
    GameObjects[index].Set(&GameObjects[index], what, value);
}


/*=================================
           instance_get
  Executes the get function of an
   object and returns its value
=================================*/

void* instance_get(int index, u8 what)
{
    return GameObjects[index].Get(&GameObjects[index], what);
}


/*=================================
          instance_count
  Return the number of a specific
              object
=================================*/

int instance_count(u8 type)
{
    int i;
    int count = 0;
    
    for (i=0;i<MAXOBJECTS;i++)
        if (GameObjects[i].type == type)
            count++;
            
    return count;
}
