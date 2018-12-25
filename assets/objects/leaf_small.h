#ifndef N64_OBJECTS_LEAF_SMALL
#define N64_OBJECTS_LEAF_SMALL
    
    void create_leaf_small(BASE *obj_base, float x, float y);
    void update_leaf_small(BASE *obj_base);
    void set_leaf_small(BASE *obj_base, u8 what, void* value);
    void* get_leaf_small(BASE *obj_base, u8 what);
    void destroy_leaf_small(BASE *obj_base);
    void draw_leaf_small(BASE *obj_base);
    
#endif