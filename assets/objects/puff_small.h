#ifndef N64_OBJECTS_PUFF_SMALL
#define N64_OBJECTS_PUFF_SMALL
    
    void create_puff_small(BASE *obj_base, float x, float y);
    void update_puff_small(BASE *obj_base);
    void set_puff_small(BASE *obj_base, u8 what, void* value);
    void destroy_puff_small(BASE *obj_base);
    void draw_puff_small(BASE *obj_base);
    
#endif