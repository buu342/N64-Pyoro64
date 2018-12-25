#ifndef N64_OBJECTS_BLOCK
#define N64_OBJECTS_BLOCK
    
    void  create_block(BASE *obj_base, float x, float y);
    void  update_block(BASE *obj_base);
    void* get_block(BASE *obj_base, u8 what);
    void  set_block(BASE *obj_base, u8 what, void* value);
    void  destroy_block(BASE *obj_base);
    void  draw_block(BASE *obj_base);
    
#endif