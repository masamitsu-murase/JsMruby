#ifndef MRB_JS_OBJECT_H__
#define MRB_JS_OBJECT_H__

#include <vector>
#include <algorithm>
#include <cwctype>
#include <string>
#include <functional>

#include "plugin.h"
#include "npfunctions.h"

#include "mrb_js.h"

class MrbJsObject : public ScriptablePluginObjectBase
{
  public:
    static NPObject *Create(NPP npp, mrb_state *mrb, mrb_value value);

  public:
    MrbJsObject(NPP npp)
         : ScriptablePluginObjectBase(npp),
           m_mrb(NULL),
           m_value(mrb_nil_value())
    {
    }

    virtual ~MrbJsObject()
    {
    }

    virtual bool HasMethod(NPIdentifier name);
    virtual bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    virtual bool HasProperty(NPIdentifier name);
    virtual bool GetProperty(NPIdentifier name, NPVariant *result);
    virtual bool SetProperty(NPIdentifier name, const NPVariant *value);

    void Initialize(mrb_state *mrb, mrb_value value)
    {
        m_mrb = mrb;
        m_value = value;
    }

    mrb_value Value() const
    {
        return m_value;
    }

  private:
    mrb_state *m_mrb;
    mrb_value m_value;
};


class MrbProcObject : public ScriptablePluginObjectBase
{
  public:
    static NPObject *Create(NPP npp, mrb_state *mrb, mrb_value proc);

  public:
    MrbProcObject(NPP npp)
         : ScriptablePluginObjectBase(npp),
           m_mrb(NULL),
           m_proc(mrb_nil_value())
    {
    }

    virtual ~MrbProcObject()
    {
    }

    virtual bool HasMethod(NPIdentifier name);
    virtual bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);

    void Initialize(mrb_state *mrb, mrb_value proc)
    {
        m_mrb = mrb;
        m_proc = proc;
    }

  private:
    bool Call(const NPVariant *args, uint32_t argCount, NPVariant *result);

  private:
    mrb_state *m_mrb;
    mrb_value m_proc;
};

///////////////////////////////////////////////////////////////////
bool NPVARIANT_IS_MRB_JS_OBJECT(const NPVariant &variant);

#endif
