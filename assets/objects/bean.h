#ifndef N64_OBJECTS_BEAN
#define N64_OBJECTS_BEAN
    
    void  create_bean(BASE *obj_base, float x, float y);
    void  update_bean(BASE *obj_base);
    void* get_bean(BASE *obj_base, u8 what);
    void  set_bean(BASE *obj_base, u8 what, void* value);
    void  destroy_bean(BASE *obj_base);
    void  draw_bean(BASE *obj_base);
    
#endif