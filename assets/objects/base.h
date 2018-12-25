#ifndef N64_OBJECTS_BASE
#define N64_OBJECTS_BASE
    
    typedef struct _base BASE;
    extern BASE GameObjects[MAXOBJECTS];
    extern int bean3_destruction_y;
    extern char bean3_destruction_player;
    
    struct _base
    {
        u8 type;
        void *vars;
        
        void(*Create)(BASE *obj_base, float x, float y);
        void(*Destroy)(BASE *obj_base);
        void(*Update)(BASE *obj_base);
        void*(*Get)(BASE *obj_base, u8 what);
        void(*Set)(BASE *obj_base, u8 what, void* value);
        void(*Draw)(BASE *obj_base);
    };
    
    int  instance_create(char type, float x, float y);
    void instance_update_all();
    void instance_draw_all();
    void instance_destroy_all();
    void* get_dummy(BASE *obj_base, u8 what);
    void  set_dummy(BASE *obj_base, u8 what, void* value);
    
    // Helper functions
    int instance_get_index(BASE *obj);
    int instance_get_nearest(BASE *obj, u8 type);
    void instance_destroy_index(int index);
    int instance_find(u8 type);
    int instance_find_withvalue(u8 type, u8 what, void* value);
    char instance_exists_index(int index);
    int instance_count(u8 type);
    
    void instance_set(int index, u8 what, void* value);
    void* instance_get(int index, u8 what);
    
    int instance_collision(BASE *obj, u8 type);
    int instance_collision_point(u8 type, float x, float y);
    
    
    // Get Values
    #define GET_X      0
    #define GET_Y      1
    #define GET_BOUND  2
    #define GET_STATE  3
    #define GET_TYPE   4
    #define GET_PLAYER 5
    #define GET_FROZEN 6
    #define GET_TIME   7
    #define GET_HEALTH 8
    
    // Set Values
    #define SET_X      0
    #define SET_Y      1
    #define SET_DIR    2
    #define SET_STATE  3
    #define SET_SPEED  4
    #define SET_TYPE   5
    #define SET_PITCH  6
    #define SET_PLAYER 7
    #define SET_FROZEN 8
    #define SET_HEALTH 9
    
    // Collision Types
    #define COL_NONE      0
    #define COL_POINT     1
    #define COL_RECTANGLE 2
    #define COL_CIRCLE    3
    #define COL_LINE      4
    
    // States
    #define STATE_IDLE   0
    #define STATE_MOVE   1
    #define STATE_ATTACK 2
    #define STATE_CHEW   3
    #define STATE_DIE    4
    #define STATE_IGNORE 5
    
    // Object Type definitions
    #define OBJ_NULL       0
    #define OBJ_PYORO      1
    #define OBJ_BLOCK      2
    #define OBJ_BEAN       3
    #define OBJ_PUFF       4
    #define OBJ_TONGUE     5
    #define OBJ_SCORES     6
    #define OBJ_ANGEL      7
    #define OBJ_PARTICLE   8
    #define OBJ_PYORO2     9
    #define OBJ_SEEDS      10
    #define OBJ_PUFF_SMALL 11
    #define OBJ_LEAF       12
    #define OBJ_LEAF_SMALL 13
    
#endif