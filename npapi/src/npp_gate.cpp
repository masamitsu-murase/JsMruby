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

////////////////////////////////////////////////////////////
//
// Implementation of plugin entry points (NPP_*)
// most are just empty stubs for this particular plugin 
//
#include "plugin.h"

char*
NPP_GetMIMEDescription(void)
{
  return "";
}



NPError NPP_Initialize(void)
{
  return NPERR_NO_ERROR;
}

void NPP_Shutdown(void)
{
}

// here the plugin creates an instance of our CPlugin object which 
// will be associated with this newly created plugin instance and 
// will do all the neccessary job
NPError NPP_New(NPMIMEType pluginType,
                NPP instance,
                uint16_t mode,
                int16_t argc,
                char* argn[],
                char* argv[],
                NPSavedData* saved)
{   
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    CPlugin *pPlugin = new CPlugin(instance);
    if (!pPlugin){
        return NPERR_OUT_OF_MEMORY_ERROR;
    }

    instance->pdata = pPlugin;
    return NPERR_NO_ERROR;
}

// here is the place to clean up and destroy the CPlugin object
NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    CPlugin *pPlugin = static_cast< CPlugin* >(instance->pdata);
    if (pPlugin){
        pPlugin->shut();
        delete pPlugin;
    }
    return NPERR_NO_ERROR;
}

// during this call we know when the plugin window is ready or
// is about to be destroyed so we can do some gui specific
// initialization and shutdown
NPError NPP_SetWindow(NPP instance, NPWindow* pNPWindow)
{    
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    if (!pNPWindow){
        return NPERR_GENERIC_ERROR;
    }

    CPlugin *pPlugin = static_cast< CPlugin* >(instance->pdata);

    if (!pPlugin){
        return NPERR_GENERIC_ERROR;
    }

    // window just created
    if (!pPlugin->isInitialized() && pNPWindow->window){
        if (!pPlugin->init(pNPWindow)){
            delete pPlugin;
            pPlugin = NULL;
            return NPERR_MODULE_LOAD_FAILED_ERROR;
        }
    }

    // window goes away
    if (!(pNPWindow->window) && pPlugin->isInitialized()){
        return NPERR_NO_ERROR;
    }

    // window resized
    if (pPlugin->isInitialized() && pNPWindow->window){
        return NPERR_NO_ERROR;
    }

    // this should not happen, nothing to do
    if (!(pNPWindow->window) && !pPlugin->isInitialized()){
        return NPERR_NO_ERROR;
    }

    return NPERR_NO_ERROR;
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to 
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError	NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    CPlugin * pPlugin = static_cast< CPlugin* >(instance->pdata);
    if(!pPlugin){
        return NPERR_GENERIC_ERROR;
    }

    switch(variable){
      case NPPVpluginNameString:
        *(static_cast< char** >(value)) = "NPRuntimeTest";
        break;
      case NPPVpluginDescriptionString:
        *(static_cast< char** >(value)) = "NPRuntime scriptability API test plugin";
        break;
      case NPPVpluginScriptableNPObject:
        *(static_cast< NPObject** >(value)) = pPlugin->GetScriptableObject();
        break;
      default:
        return NPERR_GENERIC_ERROR;
    }

    return NPERR_NO_ERROR;
}

NPError NPP_NewStream(NPP instance,
                      NPMIMEType type,
                      NPStream* stream, 
                      NPBool seekable,
                      uint16_t* stype)
{
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    return NPERR_NO_ERROR;
}

int32_t NPP_WriteReady(NPP instance, NPStream *stream)
{
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    return 0x0FFFFFFF;
}

int32_t NPP_Write(NPP instance, NPStream *stream, int32_t offset, int32_t len, void *buffer)
{
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    return len;
}

NPError NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    return NPERR_NO_ERROR;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
    if (!instance){
        return;
    }
}

void NPP_Print(NPP instance, NPPrint* printInfo)
{
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    if (!instance){
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    return NPERR_NO_ERROR;
}

int16_t	NPP_HandleEvent(NPP instance, void* event)
{
    if (!instance){
        return 0;
    }

    CPlugin *pPlugin = static_cast< CPlugin* >(instance->pdata);
    if (!pPlugin){
        return 0;
    }

    return pPlugin->handleEvent(event);
}

NPObject *NPP_GetScriptableInstance(NPP instance)
{
    if(!instance){
        return 0;
    }

    CPlugin *pPlugin = static_cast< CPlugin* >(instance->pdata);
    if (!pPlugin){
        return 0;
    }
    return pPlugin->GetScriptableObject();
}
