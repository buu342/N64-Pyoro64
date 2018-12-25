#ifndef N64_OBJECTS_LEAF
#define N64_OBJECTS_LEAF
    
    void create_leaf(BASE *obj_base, float x, float y);
    void update_leaf(BASE *obj_base);
    void set_leaf(BASE *obj_base, u8 what, void* value);
    void* get_leaf(BASE *obj_base, u8 what);
    void destroy_leaf(BASE *obj_base);
    void draw_leaf(BASE *obj_base);
    
#endif