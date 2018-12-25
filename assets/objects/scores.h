#ifndef N64_OBJECTS_SCORES
#define N64_OBJECTS_SCORES
    
    void  create_scores(BASE *obj_base, float x, float y);
    void  update_scores(BASE *obj_base);
    void  set_scores(BASE *obj_base, u8 what, void* value);
    void  destroy_scores(BASE *obj_base);
    void  draw_scores(BASE *obj_base);
    
#endif