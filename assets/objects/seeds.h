#ifndef N64_OBJECTS_SEEDS
#define N64_OBJECTS_SEEDS
    
    void  create_seeds(BASE *obj_base, float x, float y);
    void  update_seeds(BASE *obj_base);
    void* get_seeds(BASE *obj_base, u8 what);
    void  set_seeds(BASE *obj_base, u8 what, void* value);
    void  destroy_seeds(BASE *obj_base);
    void  draw_seeds(BASE *obj_base);
    
#endif