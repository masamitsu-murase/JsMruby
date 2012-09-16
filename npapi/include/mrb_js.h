#ifndef MRB_JS_H__
#define MRB_JS_H__

#include <mruby.h>

#include "plugin.h"

struct MrbJsUd
{
    NPP npp;
    struct RClass *js_func_class;
    struct RClass *js_obj_class;
};

#define MRB_UD(mrb) ((MrbJsUd *)((mrb)->ud))
#define MRB_UD_NPP(mrb) (MRB_UD(mrb)->npp)
#define MRB_UD_JS_FUNC_CLASS(mrb) (MRB_UD(mrb)->js_func_class)
#define MRB_UD_JS_OBJ_CLASS(mrb) (MRB_UD(mrb)->js_obj_class)

bool convert_js_to_mrb(NPP npp, const NPVariant &variant, mrb_state *mrb, mrb_value *result);
bool convert_js_to_mrb_object(NPP npp, const NPVariant &variant, mrb_state *mrb, mrb_value *result);

bool convert_mrb_to_js(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result);
bool convert_mrb_to_js_array(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result);
bool convert_mrb_to_js_string(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result);
bool convert_mrb_to_js(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result);

void mrb_init_js(mrb_state *mrb, NPP npp);
void mrb_close_js(mrb_state *mrb);


#endif
