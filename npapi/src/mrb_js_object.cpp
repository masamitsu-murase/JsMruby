
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/variable.h>
#include <mruby/string.h>

#include "mrb_js_object.h"

//////////////////////////////////////////////////////////////////////////////
static NPIdentifier mrb_call_id;

bool MrbProcObject::HasMethod(NPIdentifier name)
{
    if (name == mrb_call_id){
        return true;
    }
    return false;
}

bool MrbProcObject::Invoke(NPIdentifier name, const NPVariant *args,
                           uint32_t argCount, NPVariant *result)
{
    if (name == mrb_call_id){
        return Call(args, argCount, result);
    }

    return false;
}

bool MrbProcObject::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return Call(args, argCount, result);
}

bool MrbProcObject::Call(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    if (mrb_nil_p(m_proc)){
        return false;
    }

    std::vector< mrb_value > argv(argCount);
    for (uint32_t i=0; i<argCount; i++){
        if (!convert_js_to_mrb(mNpp, args[i], m_mrb, &argv[i])){
            return false;
        }
    }

    mrb_funcall_argv(m_mrb, m_proc, mrb_intern(m_mrb, "call"), argCount, &argv[0]);
    return true;
}

static NPObject *AllocateMrbProcObject(NPP npp, NPClass *aClass)
{
    return new MrbProcObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(MrbProcObject, AllocateMrbProcObject);

NPObject *MrbProcObject::Create(NPP npp, mrb_state *mrb, mrb_value proc)
{
    if (!mrb_call_id){
        mrb_call_id = NPN_GetStringIdentifier("call");
    }

    MrbProcObject *obj = static_cast< MrbProcObject* >(NPN_CreateObject(npp, GET_NPOBJECT_CLASS(MrbProcObject)));
    NPN_RetainObject(obj);
    obj->Initialize(mrb, proc);
    return obj;
}

//////////////////////////////////////////////////////////////////////////////
bool MrbJsObject::HasMethod(NPIdentifier name)
{
    NPUTF8 *str = NPN_UTF8FromIdentifier(name);
    if (!str){
        return false;
    }

    std::string method_name(str);
    NPN_MemFree(str);

    mrb_sym method_id = mrb_intern(m_mrb, method_name.c_str());
    return (mrb_respond_to(m_mrb, m_value, method_id) != FALSE);
}

bool MrbJsObject::Invoke(NPIdentifier name, const NPVariant *args,
                         uint32_t argCount, NPVariant *result)
{
    NPUTF8 *str = NPN_UTF8FromIdentifier(name);
    if (!str){
        return false;
    }

    std::string method_name(str);
    NPN_MemFree(str);

    mrb_sym method_id = mrb_intern(m_mrb, method_name.c_str());
    if (!mrb_respond_to(m_mrb, m_value, method_id)){
        return false;
    }

    std::vector< mrb_value > argv(argCount);
    for (uint32_t i=0; i<argCount; i++){
        if (!convert_js_to_mrb(mNpp, args[i], m_mrb, &argv[i])){
            return false;
        }
    }

    mrb_value ret = mrb_funcall_argv(m_mrb, m_value, mrb_intern(m_mrb, method_name.c_str()), argCount, &argv[0]);
    return convert_mrb_to_js(m_mrb, ret, mNpp, result);
}

bool MrbJsObject::HasProperty(NPIdentifier name)
{
    return false;


    // TODO
    NPUTF8 *str = NPN_UTF8FromIdentifier(name);
    if (!str){
        return false;
    }

    std::string method_name(str);
    NPN_MemFree(str);

    mrb_sym method_id = mrb_intern(m_mrb, method_name.c_str());
    return mrb_respond_to(m_mrb, m_value, method_id) != FALSE;
}

bool MrbJsObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    return false;


    // TODO
    NPUTF8 *str = NPN_UTF8FromIdentifier(name);
    if (!str){
        return false;
    }

    std::string method_name(str);
    NPN_MemFree(str);

    mrb_sym method_id = mrb_intern(m_mrb, method_name.c_str());
    if (!mrb_respond_to(m_mrb, m_value, method_id)){
        return false;
    }

    mrb_value ret = mrb_funcall_argv(m_mrb, m_value, mrb_intern(m_mrb, method_name.c_str()), 0, NULL);
    return convert_mrb_to_js(m_mrb, ret, mNpp, result);
}

bool MrbJsObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
    return false;


    // TODO
    NPUTF8 *str = NPN_UTF8FromIdentifier(name);
    if (!str){
        return false;
    }

    std::string method_name(str);
    NPN_MemFree(str);

    method_name += "=";

    mrb_sym method_id = mrb_intern(m_mrb, method_name.c_str());
    if (!mrb_respond_to(m_mrb, m_value, method_id)){
        return false;
    }

    mrb_value arg;
    if (!convert_js_to_mrb(mNpp, *value, m_mrb, &arg)){
        return false;
    }

    mrb_funcall_argv(m_mrb, m_value, mrb_intern(m_mrb, method_name.c_str()), 1, &arg);
    return true;
}

static NPObject *AllocateMrbJsObject(NPP npp, NPClass *aClass)
{
    return new MrbJsObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(MrbJsObject, AllocateMrbJsObject);

NPObject *MrbJsObject::Create(NPP npp, mrb_state *mrb, mrb_value value)
{
    MrbJsObject *obj = static_cast< MrbJsObject* >(NPN_CreateObject(npp, GET_NPOBJECT_CLASS(MrbJsObject)));
    NPN_RetainObject(obj);
    obj->Initialize(mrb, value);
    return obj;
}

// TODO
bool NPVARIANT_IS_MRB_JS_OBJECT(const NPVariant &variant)
{
    return (NPVARIANT_IS_OBJECT(variant) && NPVARIANT_TO_OBJECT(variant)->_class == GET_NPOBJECT_CLASS(MrbJsObject));
}

