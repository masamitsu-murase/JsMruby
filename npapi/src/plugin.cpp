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

//////////////////////////////////////////////////
//
// CPlugin class implementation
//
#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#endif

#include <vector>
#include <algorithm>
#include <cwctype>
#include <string>
#include <functional>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/variable.h>
#include <mruby/string.h>

#include "plugin.h"
#include "npfunctions.h"

static NPIdentifier mrb_load_id, mrb_send_id;

void ScriptablePluginObjectBase::Invalidate()
{
}

bool ScriptablePluginObjectBase::HasMethod(NPIdentifier name)
{
    return false;
}

bool ScriptablePluginObjectBase::Invoke(NPIdentifier name, const NPVariant *args,
                                        uint32_t argCount, NPVariant *result)
{
    return false;
}

bool ScriptablePluginObjectBase::InvokeDefault(const NPVariant *args,
                                               uint32_t argCount, NPVariant *result)
{
    return false;
}

bool ScriptablePluginObjectBase::HasProperty(NPIdentifier name)
{
    return false;
}

bool ScriptablePluginObjectBase::GetProperty(NPIdentifier name, NPVariant *result)
{
    return false;
}

bool ScriptablePluginObjectBase::SetProperty(NPIdentifier name,
                                             const NPVariant *value)
{
    return false;
}

bool ScriptablePluginObjectBase::RemoveProperty(NPIdentifier name)
{
    return false;
}

bool ScriptablePluginObjectBase::Enumerate(NPIdentifier **identifier,
                                           uint32_t *count)
{
    return false;
}

bool ScriptablePluginObjectBase::Construct(const NPVariant *args, uint32_t argCount,
                                           NPVariant *result)
{
    return false;
}

// static
void ScriptablePluginObjectBase::_Deallocate(NPObject *npobj)
{
    // Call the virtual destructor.
    delete (ScriptablePluginObjectBase *)npobj;
}

// static
void ScriptablePluginObjectBase::_Invalidate(NPObject *npobj)
{
    ((ScriptablePluginObjectBase *)npobj)->Invalidate();
}

// static
bool ScriptablePluginObjectBase::_HasMethod(NPObject *npobj, NPIdentifier name)
{
    return ((ScriptablePluginObjectBase *)npobj)->HasMethod(name);
}

// static
bool ScriptablePluginObjectBase::_Invoke(NPObject *npobj, NPIdentifier name,
                                         const NPVariant *args, uint32_t argCount,
                                         NPVariant *result)
{
    return ((ScriptablePluginObjectBase *)npobj)->Invoke(name, args, argCount,
                                                         result);
}

// static
bool ScriptablePluginObjectBase::_InvokeDefault(NPObject *npobj,
                                                const NPVariant *args,
                                                uint32_t argCount,
                                                NPVariant *result)
{
    return ((ScriptablePluginObjectBase *)npobj)->InvokeDefault(args, argCount,
                                                                result);
}

// static
bool ScriptablePluginObjectBase::_HasProperty(NPObject * npobj, NPIdentifier name)
{
    return ((ScriptablePluginObjectBase *)npobj)->HasProperty(name);
}

// static
bool ScriptablePluginObjectBase::_GetProperty(NPObject *npobj, NPIdentifier name,
                                              NPVariant *result)
{
    return ((ScriptablePluginObjectBase *)npobj)->GetProperty(name, result);
}

// static
bool ScriptablePluginObjectBase::_SetProperty(NPObject *npobj, NPIdentifier name,
                                              const NPVariant *value)
{
    return ((ScriptablePluginObjectBase *)npobj)->SetProperty(name, value);
}

// static
bool ScriptablePluginObjectBase::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    return ((ScriptablePluginObjectBase *)npobj)->RemoveProperty(name);
}

// static
bool ScriptablePluginObjectBase::_Enumerate(NPObject *npobj,
                                            NPIdentifier **identifier,
                                            uint32_t *count)
{
    return ((ScriptablePluginObjectBase *)npobj)->Enumerate(identifier, count);
}

// static
bool ScriptablePluginObjectBase::_Construct(NPObject *npobj, const NPVariant *args,
                                            uint32_t argCount, NPVariant *result)
{
    return ((ScriptablePluginObjectBase *)npobj)->Construct(args, argCount, result);
}

//////////////////////////////////////////////////////////////////////////////
bool ScriptablePluginObject::HasMethod(NPIdentifier name)
{
    if (name == mrb_load_id){
        return true;
    }else if (name == mrb_send_id){
        return true;
    }
    return false;
}

bool ScriptablePluginObject::HasProperty(NPIdentifier name)
{
    return false;
}

bool ScriptablePluginObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    VOID_TO_NPVARIANT(*result);
    return true;
}

bool ScriptablePluginObject::Invoke(NPIdentifier name, const NPVariant *args,
                                    uint32_t argCount, NPVariant *result)
{
    if (name == mrb_load_id){
        MrbRun(args, argCount, result);
        return true;
    }else if (name == mrb_send_id){
        MrbCall(args, argCount, result);
        return true;
    }

    return false;
}

bool ScriptablePluginObject::InvokeDefault(const NPVariant *args, uint32_t argCount,
                                           NPVariant *result)
{
    VOID_TO_NPVARIANT(*result);
    return false;
}

void ScriptablePluginObject::MrbRun(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    if (argCount != 1 || !NPVARIANT_IS_STRING(args[0])){
        BOOLEAN_TO_NPVARIANT(false, *result);
        return;
    }

    mrb_state *mrb = m_mrb;
    NPString npstr = NPVARIANT_TO_STRING(args[0]);
    std::string str(npstr.UTF8Characters, npstr.UTF8Length);

    mrb_value mrb_result = mrb_load_string_cxt(mrb, str.c_str(), NULL);

    NPVariant ret;
    if (convert_mrb_to_js(mrb, mrb_result, mNpp, &ret)){
        *result = ret;
    }else{
        BOOLEAN_TO_NPVARIANT(false, *result);
    }
}

void ScriptablePluginObject::MrbCall(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    if (argCount < 1 || !NPVARIANT_IS_STRING(args[0])){
        BOOLEAN_TO_NPVARIANT(false, *result);
        return;
    }

    std::vector< mrb_value > argv(argCount - 1);
    for (uint32_t i=1; i<argCount; i++){
        if (!convert_js_to_mrb(mNpp, args[i], m_mrb, &argv[i-1])){
            BOOLEAN_TO_NPVARIANT(false, *result);
            return;
        }
    }

    NPString npstr = NPVARIANT_TO_STRING(args[0]);
    std::string str(npstr.UTF8Characters, npstr.UTF8Length);
    mrb_value mrb_result = mrb_funcall_argv(m_mrb, mrb_top_self(m_mrb),
                                            mrb_intern2(m_mrb, npstr.UTF8Characters, npstr.UTF8Length),
                                            argv.size(), &argv[0]);

    NPVariant ret;
    if (convert_mrb_to_js(m_mrb, mrb_result, mNpp, &ret)){
        *result = ret;
    }else{
        BOOLEAN_TO_NPVARIANT(false, *result);
    }
}

static NPObject *AllocateScriptablePluginObject(NPP npp, NPClass *aClass)
{
    return new ScriptablePluginObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(ScriptablePluginObject, AllocateScriptablePluginObject);


//////////////////////////////////////////////////////////////////////////////
CPlugin::CPlugin(NPP pNPInstance)
     : m_pNPInstance(pNPInstance),
       m_pNPStream(NULL),
       m_bInitialized(false),
       m_pScriptableObject(NULL)
{
    if (!mrb_load_id){
        mrb_load_id = NPN_GetStringIdentifier("load");
    }
    if (!mrb_send_id){
        mrb_send_id = NPN_GetStringIdentifier("send");
    }
}

CPlugin::~CPlugin()
{
    if (m_pScriptableObject){
        NPN_ReleaseObject(m_pScriptableObject);
    }
}

NPBool CPlugin::init(NPWindow* pNPWindow)
{
    if (!pNPWindow){
        return false;
    }

    m_Window = pNPWindow;

    m_bInitialized = true;
    return true;
}

void CPlugin::shut()
{
    m_bInitialized = false;
}

NPBool CPlugin::isInitialized()
{
    return m_bInitialized;
}

int16_t CPlugin::handleEvent(void* event)
{
    return 0;
}

NPObject *CPlugin::GetScriptableObject()
{
    if (!m_pScriptableObject){
        m_pScriptableObject = NPN_CreateObject(m_pNPInstance, GET_NPOBJECT_CLASS(ScriptablePluginObject));
    }

    if (m_pScriptableObject){
        NPN_RetainObject(m_pScriptableObject);
    }

    return m_pScriptableObject;
}
