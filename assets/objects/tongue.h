#ifndef N64_OBJECTS_TONGUE
#define N64_OBJECTS_TONGUE
    
    void  create_tongue(BASE *obj_base, float x, float y);
    void  update_tongue(BASE *obj_base);
    void* get_tongue(BASE *obj_base, u8 what);
    void  set_tongue(BASE *obj_base, u8 what, void* value);
    void  destroy_tongue(BASE *obj_base);
    void  draw_tongue(BASE *obj_base);
    
#endif