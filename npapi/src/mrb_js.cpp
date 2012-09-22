
#include <vector>
#include <algorithm>
#include <cwctype>
#include <string>
#include <functional>
#include <sstream>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/variable.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/khash.h>
#include <mruby/hash.h>
#include <mruby/data.h>
#include <mruby/class.h>

#include "plugin.h"
#include "npfunctions.h"

#include "mrb_js.h"
#include "mrb_js_object.h"


KHASH_DECLARE(ht, mrb_value, mrb_value, 1);

////////////////////////////////////////////////////////////////////////////////////////
static void mrb_js_func_free(mrb_state *mrb, void *ptr)
{
    NPObject *func = (NPObject *)ptr;
    NPN_ReleaseObject(func);
}
static struct mrb_data_type mrb_js_func_type = { "MrbJsFunc", mrb_js_func_free };

static mrb_value mrb_js_func_call(mrb_state *mrb, mrb_value self)
{
    NPP npp = MRB_UD_NPP(mrb);

    NPObject *func = (NPObject *)DATA_PTR(self);
    if (!func){
        return mrb_nil_value();
    }

    mrb_value *values;
    int len;
    mrb_get_args(mrb, "*", &values, &len);

    NPVariant empty;
    NULL_TO_NPVARIANT(empty);
    std::vector< NPVariant > args(len + 1, empty);
    bool success = true;
    for (int i=0; i<len; i++){
        if (!convert_mrb_to_js(mrb, values[i], npp, &args[i + 1])){
            success = false;
            break;
        }
    }

    mrb_value ret = mrb_nil_value();
    if (success){
        NPObject *window;
        NPN_GetValue(npp, NPNVWindowNPObject, &window);

        NPVariant result;
        OBJECT_TO_NPVARIANT(window, args[0]);
        NPN_Invoke(npp, func, NPN_GetStringIdentifier("call"), &args[0], args.size(), &result);
        convert_js_to_mrb(npp, result, mrb, &ret);
    }

    for (int i=0; i<len+1; i++){
        NPN_ReleaseVariantValue(&args[i]);
    }

    return ret;
}

static mrb_value mrb_js_func_class_new(mrb_state *mrb, NPObject *func)
{
    NPN_RetainObject(func);
    return mrb_obj_value(Data_Wrap_Struct(mrb, MRB_UD_JS_FUNC_CLASS(mrb), &mrb_js_func_type, func));
}

static void mrb_init_js_func(mrb_state *mrb)
{
    struct RClass *js;
    js = MRB_UD_JS_FUNC_CLASS(mrb) = mrb_define_class(mrb, "JsFunc", mrb->object_class);
    MRB_SET_INSTANCE_TT(js, MRB_TT_DATA);

    mrb_define_method(mrb, js, "call", mrb_js_func_call, ARGS_ANY());
}

////////////////////////////////////////////////////////////////////////////////////////
static void mrb_js_obj_free(mrb_state *mrb, void *ptr)
{
    NPObject *obj = (NPObject *)ptr;
    NPN_ReleaseObject(obj);
}
static struct mrb_data_type mrb_js_obj_type = { "MrbObjFunc", mrb_js_obj_free };

static mrb_value mrb_js_obj_method_missing(mrb_state *mrb, mrb_value self)
{
    NPP npp = MRB_UD_NPP(mrb);

    mrb_value name_sym, *args;
    int len;
    mrb_get_args(mrb, "o*", &name_sym, &args, &len);
    if (mrb_type(name_sym) != MRB_TT_SYMBOL){
        return mrb_nil_value();
    }

    bool success = true;
    NPVariant empty;
    NULL_TO_NPVARIANT(empty);
    std::vector< NPVariant > var_args(len, empty);
    for (int i=0; i<len; i++){
        if (!convert_mrb_to_js(mrb, args[i], npp, &var_args[i])){
            success = false;
            break;
        }
    }

    mrb_value ret = mrb_nil_value();
    if (success){
        int name_len;
        const char *name_char = mrb_sym2name_len(mrb, SYM2ID(name_sym), &name_len);
        std::string name(name_char, name_len);
        NPIdentifier name_id = NPN_GetStringIdentifier(name.c_str());

        NPObject *obj = (NPObject *)DATA_PTR(self);
        if (NPN_HasMethod(npp, obj, name_id)){
            NPVariant result;
            NPN_Invoke(npp, obj, name_id, &var_args[0], len, &result);
            convert_js_to_mrb(npp, result, mrb, &ret);
        }else if (NPN_HasProperty(npp, obj, name_id)){
            NPVariant result;
            NPN_GetProperty(npp, obj, name_id, &result);
            convert_js_to_mrb(npp, result, mrb, &ret);
        }else if (name.size() > 1 && name[name.size() - 1] == '=' && var_args.size() == 1){
            name.resize(name.size() - 1);
            name_id = NPN_GetStringIdentifier(name.c_str());
            if (NPN_HasProperty(npp, obj, name_id)){
                NPN_SetProperty(npp, obj, name_id, &var_args[0]);
            }
        }
    }

    for (int i=0; i<len; i++){
        NPN_ReleaseVariantValue(&var_args[i]);
    }

    return ret;
}

static mrb_value mrb_js_obj_get(mrb_state *mrb, mrb_value klass)
{
    mrb_value str;
    mrb_get_args(mrb, "S", &str);

    NPObject *window;
    NPN_GetValue(MRB_UD_NPP(mrb), NPNVWindowNPObject, &window);

    NPUTF8 *s = (NPUTF8 *)NPN_MemAlloc(RSTRING_LEN(str));
    std::copy(RSTRING_PTR(str), RSTRING_END(str), s);
    NPString evaluated_str = { s, RSTRING_LEN(str) };

    NPVariant result;
    std::string js_str(RSTRING_PTR(str), RSTRING_LEN(str));
    NPN_Evaluate(MRB_UD_NPP(mrb), window, &evaluated_str, &result);
    NPN_ReleaseObject(window);
    NPN_MemFree(s);

    mrb_value ret;
    if (!convert_js_to_mrb(MRB_UD_NPP(mrb), result, mrb, &ret)){
        return mrb_nil_value();
    }

    return ret;
}

static mrb_value mrb_js_obj_class_new(mrb_state *mrb, NPObject *obj)
{
    NPN_RetainObject(obj);
    return mrb_obj_value(Data_Wrap_Struct(mrb, MRB_UD_JS_OBJ_CLASS(mrb), &mrb_js_obj_type, obj));
}

static void mrb_init_js_obj(mrb_state *mrb)
{
    struct RClass *obj;
    obj = MRB_UD_JS_OBJ_CLASS(mrb) = mrb_define_class(mrb, "JsObj", mrb->object_class);
    MRB_SET_INSTANCE_TT(obj, MRB_TT_DATA);

    mrb_define_class_method(mrb, obj, "get", mrb_js_obj_get, ARGS_REQ(1));
    mrb_define_method(mrb, obj, "method_missing", mrb_js_obj_method_missing, ARGS_ANY());
}

////////////////////////////////////////////////////////////////////////////////////////
bool convert_js_to_mrb_array(NPP npp, const NPVariant &variant, mrb_state *mrb, mrb_value *result)
{
    NPVariant length;
    NPN_GetProperty(npp, NPVARIANT_TO_OBJECT(variant), NPN_GetStringIdentifier("length"), &length);

    int len;
    if (NPVARIANT_IS_INT32(length)){
        len = NPVARIANT_TO_INT32(length);
    }else if (NPVARIANT_IS_DOUBLE(length)){
        len = static_cast< int >(NPVARIANT_TO_DOUBLE(length));  // Why double??
    }else{
        NPN_ReleaseVariantValue(&length);
        return false;
    }
    NPN_ReleaseVariantValue(&length);

    mrb_value ret_array = mrb_ary_new_capa(mrb, len);
    for (int32_t i=0; i<len; i++){
        NPVariant item;
        std::ostringstream oss;
        oss << i;
        NPN_GetProperty(npp, NPVARIANT_TO_OBJECT(variant), NPN_GetStringIdentifier(oss.str().c_str()), &item);

        mrb_value mrb_item;
        if (!convert_js_to_mrb(npp, item, mrb, &mrb_item)){
            NPN_ReleaseVariantValue(&item);
            return false;
        }
        NPN_ReleaseVariantValue(&item);
        mrb_ary_push(mrb, ret_array, mrb_item);
    }
    *result = ret_array;
    return true;
}

bool convert_js_to_mrb_hash(NPP npp, const NPVariant &variant, mrb_state *mrb, mrb_value *result)
{
    NPIdentifier *id_list = NULL;
    uint32_t count;
    NPN_Enumerate(npp, NPVARIANT_TO_OBJECT(variant), &id_list, &count);

    mrb_value ret_hash = mrb_hash_new_capa(mrb, count);
    for (uint32_t i=0; i<count; i++){
        NPVariant item;
        NPN_GetProperty(npp, NPVARIANT_TO_OBJECT(variant), id_list[i], &item);

        mrb_value mrb_item;
        if (!convert_js_to_mrb(npp, item, mrb, &mrb_item)){
            NPN_MemFree(id_list);
            NPN_ReleaseVariantValue(&item);
            return false;
        }
        NPN_ReleaseVariantValue(&item);

        NPUTF8 *key = NPN_UTF8FromIdentifier(id_list[i]);
        mrb_hash_set(mrb, ret_hash, mrb_str_new2(mrb, key), mrb_item);
        NPN_MemFree(key);
    }

    NPN_MemFree(id_list);
    *result = ret_hash;
    return true;
}

bool convert_js_to_mrb_function(NPP npp, const NPVariant &variant, mrb_state *mrb, mrb_value *result)
{
    NPObject *func = NPVARIANT_TO_OBJECT(variant);
    *result = mrb_js_func_class_new(mrb, func);

    return true;
}

bool convert_js_to_mrb_object(NPP npp, const NPVariant &variant, mrb_state *mrb, mrb_value *result)
{
    if (NPVARIANT_IS_MRB_JS_OBJECT(variant)){
        MrbJsObject *obj = static_cast< MrbJsObject* >(NPVARIANT_TO_OBJECT(variant));
        *result = obj->Value();
        return true;
    }

    NPObject *window;
    NPN_GetValue(npp, NPNVWindowNPObject, &window);

    NPVariant object;
    NPN_GetProperty(npp, window, NPN_GetStringIdentifier("Object"), &object);
    NPN_ReleaseObject(window);

    NPVariant prototype;
    NPN_GetProperty(npp, NPVARIANT_TO_OBJECT(object), NPN_GetStringIdentifier("prototype"), &prototype);
    NPN_ReleaseVariantValue(&object);

    NPVariant to_string;
    NPN_GetProperty(npp, NPVARIANT_TO_OBJECT(prototype), NPN_GetStringIdentifier("toString"), &to_string);
    NPN_ReleaseVariantValue(&prototype);

    NPVariant type_string;
    NPN_Invoke(npp, NPVARIANT_TO_OBJECT(to_string), NPN_GetStringIdentifier("call"), &variant, 1, &type_string);
    NPN_ReleaseVariantValue(&to_string);

    if (!NPVARIANT_IS_STRING(type_string)){
        NPN_ReleaseVariantValue(&type_string);
        return false;
    }

    std::string type(NPVARIANT_TO_STRING(type_string).UTF8Characters, NPVARIANT_TO_STRING(type_string).UTF8Length);
    NPN_ReleaseVariantValue(&type_string);
    if (type == "[object Array]"){
        return convert_js_to_mrb_array(npp, variant, mrb, result);
    }else if (type == "[object Object]"){
        return convert_js_to_mrb_hash(npp, variant, mrb, result);
    }else if (type == "[object Function]"){
        return convert_js_to_mrb_function(npp, variant, mrb, result);
    }else{
        *result = mrb_js_obj_class_new(mrb, NPVARIANT_TO_OBJECT(variant));
        return true;
    }

    return true;
}

bool convert_js_to_mrb(NPP npp, const NPVariant &variant, mrb_state *mrb, mrb_value *result)
{
    switch(variant.type){
      case NPVariantType_Void:
      case NPVariantType_Null:
        *result = mrb_nil_value();
        return true;
      case NPVariantType_Bool:
        if (NPVARIANT_TO_BOOLEAN(variant)){
            *result = mrb_true_value();
        }else{
            *result = mrb_false_value();
        }
        return true;
      case NPVariantType_Int32:
        *result = mrb_fixnum_value(NPVARIANT_TO_INT32(variant));
        return true;
      case NPVariantType_Double:
        *result = mrb_float_value(NPVARIANT_TO_DOUBLE(variant));
        return true;
      case NPVariantType_String:
        *result = mrb_str_new(mrb, NPVARIANT_TO_STRING(variant).UTF8Characters,
                              NPVARIANT_TO_STRING(variant).UTF8Length);
        return true;
      case NPVariantType_Object:
        return convert_js_to_mrb_object(npp, variant, mrb, result);
      default:
        break;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////
bool convert_mrb_to_js_hash(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result)
{
    NPObject *window;
    NPN_GetValue(npp, NPNVWindowNPObject, &window);

    NPVariant hash;
    NPN_Invoke(npp, window, NPN_GetStringIdentifier("Object"), NULL, 0, &hash);
    NPN_ReleaseObject(window);

    khash_t(ht) *h = RHASH_TBL(value);
    if (!h){
        *result = hash;
        return true;
    }

    NPObject *hash_obj = NPVARIANT_TO_OBJECT(hash);
    for (khiter_t k=kh_begin(h); k!=kh_end(h); k++){
        if (kh_exist(h, k)) {
            mrb_value key = mrb_funcall(mrb, kh_key(h, k), "to_s", 0);
            mrb_value val = kh_value(h, k);

            NPVariant var;
            if (!convert_mrb_to_js(mrb, val, npp, &var)){
                NPN_ReleaseVariantValue(&hash);
                return false;
            }

            std::string name(RSTRING_PTR(key), RSTRING_LEN(key));
            NPN_SetProperty(npp, hash_obj, NPN_GetStringIdentifier(name.c_str()), &var);
        }
    }
    *result = hash;

    return true;
}

bool convert_mrb_to_js_array(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result)
{
    NPObject *window;
    NPN_GetValue(npp, NPNVWindowNPObject, &window);

    NPVariant array;
    NPN_Invoke(npp, window, NPN_GetStringIdentifier("Array"), NULL, 0, &array);
    NPN_ReleaseObject(window);

    NPIdentifier push = NPN_GetStringIdentifier("push");
    mrb_value *ptr = RARRAY_PTR(value);
    for (int i=0, len=RARRAY_LEN(value); i<len; i++){
        NPVariant var;
        if (!convert_mrb_to_js(mrb, ptr[i], npp, &var)){
            NPN_ReleaseVariantValue(&array);
            return false;
        }
        NPVariant ret;
        NPN_Invoke(npp, NPVARIANT_TO_OBJECT(array), push, &var, 1, &ret);
        NPN_ReleaseVariantValue(&ret);
    }
    *result = array;

    return true;
}

bool convert_mrb_to_js_string(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result)
{
    NPUTF8 *ptr = (NPUTF8 *)NPN_MemAlloc(RSTRING_LEN(value));
    if (!ptr){
        return false;
    }

    std::copy(RSTRING_PTR(value), RSTRING_END(value), ptr);
    STRINGN_TO_NPVARIANT(ptr, RSTRING_LEN(value), *result);

    return true;
}

bool convert_mrb_to_js_data(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result)
{
    if (DATA_TYPE(value) == &mrb_js_func_type){
        NPObject *func = (NPObject *)DATA_PTR(value);
        NPN_RetainObject(func);
        OBJECT_TO_NPVARIANT(func, *result);
        return true;
    }else if (DATA_TYPE(value) == &mrb_js_obj_type){
        NPObject *obj = (NPObject *)DATA_PTR(value);
        NPN_RetainObject(obj);
        OBJECT_TO_NPVARIANT(obj, *result);
        return true;
    }
    return false;
}

bool convert_mrb_to_js(mrb_state *mrb, mrb_value value, NPP npp, NPVariant *result)
{
    switch(mrb_type(value)){
      case MRB_TT_FALSE:
        if (mrb_nil_p(value)){
            NULL_TO_NPVARIANT(*result);
        }else{
            BOOLEAN_TO_NPVARIANT(false, *result);
        }
        return true;
      case MRB_TT_TRUE:
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
      case MRB_TT_FIXNUM:
        INT32_TO_NPVARIANT(mrb_fixnum(value), *result);
        return true;
      case MRB_TT_FLOAT:
        DOUBLE_TO_NPVARIANT(mrb_float(value), *result);
        return true;
      case MRB_TT_ARRAY:
        return convert_mrb_to_js_array(mrb, value, npp, result);
      case MRB_TT_HASH:
        return convert_mrb_to_js_hash(mrb, value, npp, result);
      case MRB_TT_STRING:
        return convert_mrb_to_js_string(mrb, value, npp, result);
      case MRB_TT_DATA:
        return convert_mrb_to_js_data(mrb, value, npp, result);
      case MRB_TT_PROC:
        OBJECT_TO_NPVARIANT(MrbProcObject::Create(npp, mrb, value), *result);
        return true;
      default:
        OBJECT_TO_NPVARIANT(MrbJsObject::Create(npp, mrb, value), *result);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////
void mrb_init_js(mrb_state *mrb, NPP npp)
{
    mrb->ud = mrb_malloc(mrb, sizeof(MrbJsUd));
    MRB_UD_NPP(mrb) = npp;
    mrb_init_js_func(mrb);
    mrb_init_js_obj(mrb);
}

void mrb_close_js(mrb_state *mrb)
{
    mrb_free(mrb, mrb->ud);
}
