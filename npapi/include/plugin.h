/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or 
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#ifdef _WINDOWS
#include <windows.h>
#endif

#include "npapi.h"
#include "npruntime.h"

#include "mrb_js.h"

class CPlugin
{
  private:
    NPP m_pNPInstance;

    NPWindow *m_Window;

    NPStream *m_pNPStream;
    NPBool m_bInitialized;

    NPObject *m_pScriptableObject;

  public:
    CPlugin(NPP pNPInstance);
    ~CPlugin();

    NPBool init(NPWindow* pNPWindow);
    void shut();
    NPBool isInitialized();

    int16_t handleEvent(void* event);

    NPObject *GetScriptableObject();
};

#define DECLARE_NPOBJECT_CLASS_WITH_BASE(_class, ctor)                          \
NPClass s##_class##_NPClass = {                                                 \
    NP_CLASS_STRUCT_VERSION_CTOR,                                               \
    ctor,                                                                       \
    ScriptablePluginObjectBase::_Deallocate,                                    \
    ScriptablePluginObjectBase::_Invalidate,                                    \
    ScriptablePluginObjectBase::_HasMethod,                                     \
    ScriptablePluginObjectBase::_Invoke,                                        \
    ScriptablePluginObjectBase::_InvokeDefault,                                 \
    ScriptablePluginObjectBase::_HasProperty,                                   \
    ScriptablePluginObjectBase::_GetProperty,                                   \
    ScriptablePluginObjectBase::_SetProperty,                                   \
    ScriptablePluginObjectBase::_RemoveProperty,                                \
    ScriptablePluginObjectBase::_Enumerate,                                     \
    ScriptablePluginObjectBase::_Construct                                      \
}

#define GET_NPOBJECT_CLASS(_class) &s##_class##_NPClass

// Helper class that can be used to map calls to the NPObject hooks
// into virtual methods on instances of classes that derive from this
// class.
class ScriptablePluginObjectBase : public NPObject
{
  public:
    ScriptablePluginObjectBase(NPP npp)
         : mNpp(npp)
    {
    }

    virtual ~ScriptablePluginObjectBase()
    {
    }

    // Virtual NPObject hooks called through this base class. Override
    // as you see fit.
    virtual void Invalidate();
    virtual bool HasMethod(NPIdentifier name);
    virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                        uint32_t argCount, NPVariant *result);
    virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                               NPVariant *result);
    virtual bool HasProperty(NPIdentifier name);
    virtual bool GetProperty(NPIdentifier name, NPVariant *result);
    virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
    virtual bool RemoveProperty(NPIdentifier name);
    virtual bool Enumerate(NPIdentifier **identifier, uint32_t *count);
    virtual bool Construct(const NPVariant *args, uint32_t argCount,
                           NPVariant *result);

  public:
    static void _Deallocate(NPObject *npobj);
    static void _Invalidate(NPObject *npobj);
    static bool _HasMethod(NPObject *npobj, NPIdentifier name);
    static bool _Invoke(NPObject *npobj, NPIdentifier name,
                        const NPVariant *args, uint32_t argCount,
                        NPVariant *result);
    static bool _InvokeDefault(NPObject *npobj, const NPVariant *args,
                               uint32_t argCount, NPVariant *result);
    static bool _HasProperty(NPObject * npobj, NPIdentifier name);
    static bool _GetProperty(NPObject *npobj, NPIdentifier name,
                             NPVariant *result);
    static bool _SetProperty(NPObject *npobj, NPIdentifier name,
                             const NPVariant *value);
    static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
    static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier,
                           uint32_t *count);
    static bool _Construct(NPObject *npobj, const NPVariant *args,
                           uint32_t argCount, NPVariant *result);

  protected:
    NPP mNpp;
};


class ScriptablePluginObject : public ScriptablePluginObjectBase
{
  public:
    ScriptablePluginObject(NPP npp)
         : ScriptablePluginObjectBase(npp)
    {
        m_mrb = mrb_open();
        mrb_init_js(m_mrb, npp);
    }
    virtual ~ScriptablePluginObject()
    {
        mrb_close_js(m_mrb);
        mrb_close(m_mrb);
    }

    virtual bool HasMethod(NPIdentifier name);
    virtual bool HasProperty(NPIdentifier name);
    virtual bool GetProperty(NPIdentifier name, NPVariant *result);
    virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                        uint32_t argCount, NPVariant *result);
    virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                               NPVariant *result);

  private:
    void MrbRun(const NPVariant *args, uint32_t argCount, NPVariant *result);
    void MrbCall(const NPVariant *args, uint32_t argCount, NPVariant *result);

  private:
    mrb_state *m_mrb;
};

#endif // __PLUGIN_H__
