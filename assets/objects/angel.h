#ifndef N64_OBJECTS_ANGEL
#define N64_OBJECTS_ANGEL
    
    void  create_angel(BASE *obj_base, float x, float y);
    void  update_angel(BASE *obj_base);
    void* get_angel(BASE *obj_base, u8 what);
    void  set_angel(BASE *obj_base, u8 what, void* value);
    void  destroy_angel(BASE *obj_base);
    void  draw_angel(BASE *obj_base);
    
#endif