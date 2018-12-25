#ifndef N64_OBJECTS_PYORO2
#define N64_OBJECTS_PYORO2
    
    void  create_pyoro2(BASE *obj_base, float x, float y);
    void  update_pyoro2(BASE *obj_base);
    void* get_pyoro2(BASE *obj_base, u8 what);
    void  set_pyoro2(BASE *obj_base, u8 what, void* value);
    void  destroy_pyoro2(BASE *obj_base);
    void  draw_pyoro2(BASE *obj_base);
    
#endif