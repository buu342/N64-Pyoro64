#ifndef N64_OBJECTS_PYORO
#define N64_OBJECTS_PYORO
    
    void  create_pyoro(BASE *obj_base, float x, float y);
    void  update_pyoro(BASE *obj_base);
    void* get_pyoro(BASE *obj_base, u8 what);
    void  set_pyoro(BASE *obj_base, u8 what, void* value);
    void  destroy_pyoro(BASE *obj_base);
    void  draw_pyoro(BASE *obj_base);
    
#endif