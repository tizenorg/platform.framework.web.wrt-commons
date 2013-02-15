/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file    wrt_plugin_export.h
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   Header file for plugin export API
 */
#ifndef WRT_PLUGIN_EXPORT_H
#define WRT_PLUGIN_EXPORT_H

#include <stddef.h>

/**
 * Widget handle type
 */
typedef int widget_handle_t;

/**
 * Parameter which should be used during policy evaluation.
 */
typedef struct ace_param_s
{
    const char *name;
    const char *value;
} ace_param_t;

/**
 * List of additional parameters which should be used during policy evaluation.
 */
typedef struct ace_param_list_s
{
    size_t count;
    ace_param_t *param;
} ace_param_list_t;

/**
 * Contains list of device capabilities. Each device capability may have
 * associated list of function params.
 */
typedef struct ace_device_cap_s
{
    size_t devcapsCount;
    const char**      dev_cap_names;
    size_t paramsCount;
    ace_param_list_t* params;
} ace_device_cap_t;

/**
 * List of device capabilities which must be check.
 */
typedef struct ace_device_capabilities_s
{
    size_t count;
    const char **device_cap;
} ace_device_capabilities_t;

/**
 * List of api features that must be checked
 */
typedef struct ace_api_features_s
{
    size_t count;
    const char **api_feature;
} ace_api_features_t;

/**
 * Data from request will be used to evaluate policy file.
 */
typedef struct ace_request_s
{
    widget_handle_t widget_handle;
    const char*                 feature_api;
    const char*                 function_name;
    ace_device_capabilities_t device_capabilities;
    ace_param_list_t param_list;
} ace_request_t;

/**
 * Data from request will be used to evaluate policy file.
 */
typedef struct ace_request_2_s
{
    widget_handle_t widget_handle;
    ace_api_features_t api_features;
    const char*                 function_name;
    ace_device_cap_t device_capabilities;
} ace_request_2_t;

/**
 * info returned by plugin_api_check_access
 */
#define PLUGIN_API_ACCESS_GRANTED 1
#define PLUGIN_API_ACCESS_DENIED 0
#define PLUGIN_API_ACCESS_ERROR -1


typedef const void* java_script_context_t;

typedef struct js_object_properties_s
{
    size_t count;
    char** properties;
} js_object_properties_t;

typedef const void* js_class_template_t;
typedef void* js_object_ref_t;
typedef const void* js_value_ref_t;

typedef js_class_template_t (*js_class_template_getter) (void);
typedef void* (*js_class_constructor_cb_t)(js_class_template_t,
        js_object_ref_t, size_t, js_value_ref_t[],
        js_value_ref_t*);

typedef enum class_definition_type_e
{
    JS_CLASS,
    JS_FUNCTION,
    JS_INTERFACE
} class_definition_type_t;

typedef enum class_definition_iframe_behaviour_e
{
    //object should not be initalized in iframes
    //it is default one
    NONE,
    //object should be copied as reference to each iframe
    REFERENCE,
    //object should be created for each iframe and NOT inform plugin
    CREATE_INSTANCE,
} class_definition_iframe_behaviour_t;

typedef enum class_definition_iframe_notice_e
{
    //it is default one
    NONE_NOTICE,
    ALWAYS_NOTICE
} class_definition_iframe_notice_t;

typedef enum class_definition_iframe_overlay_e
{
    IGNORED,
    USE_OVERLAYED,
    OVERLAYED_BEFORE_ORIGINAL
} class_definition_iframe_overlay_t;

typedef void* js_object_instance_t;
//global_context - id
typedef void(*iframe_loaded_cb)(java_script_context_t global_context,
                                js_object_instance_t iframe,
                                js_object_instance_t object);

typedef void* (*js_function_impl)(void*);

typedef struct class_definition_options_s
{
    class_definition_type_t type;
    class_definition_iframe_behaviour_t iframe_option;
    class_definition_iframe_notice_t iframe_notice;
    class_definition_iframe_overlay_t iframe_overlay;
    iframe_loaded_cb cb;
    void * private_data;
    js_function_impl function;
} class_definition_options_t;


/*
 * list of device caps
 */
typedef struct devcaps_s
{
    char** deviceCaps;
    size_t devCapsCount;
} devcaps_t;

/*
 * mapping from a feature to corresponding list of device capabilities
 */
typedef struct feature_devcaps_s
{
    char* feature_name;
    devcaps_t devCaps;
} feature_devcaps_t;

/*
 * list of feature_devcaps_t structs
 */
typedef struct feature_mapping_s
{
    feature_devcaps_t* features;
    size_t featuresCount;
} feature_mapping_t;


typedef feature_mapping_t* pfeature_mapping_t;

typedef pfeature_mapping_t (*features_getter)(void);

typedef const devcaps_t* (*devcaps_getter)(pfeature_mapping_t /*features*/,
                                           const char* /*featureName*/);
typedef void (*deinitializer)(pfeature_mapping_t /*features*/);

typedef struct feature_mapping_interface_s
{
    features_getter featGetter;  /* returns a list of api features */
    devcaps_getter dcGetter;     /*
                                  * for a given api feature returns a list of
                                  * corresponding device capabilities
                                  */

    deinitializer release;       /* as memory ownership of features is
                                  * transfered to callee you have to call
                                  * the release function ptr on features
                                  */
} feature_mapping_interface_t;


/*
 * This is a structure describing a JS entity template (a class, an interface
 * or function), object name and it's parent class name (parent_name). JS
 * entity will be bind to a parent class name (parent_name.js_entity_name).
 * @param parent_name - parent name (ie Widget.Device)
 * @param object_name - object name (DeviceStatus)
 * @param interface_name - interface name (e.g. Widget)
 * @param js_class_template_getter_fun - js_class_template required to create
 *          JS object
 * @param js_class_consturctor_cb - constructor to call to when instance of
 *          certain interface is created
 * @param private_data private data for object creator if required (usually
 *          NULL)
 */
typedef struct js_entity_definition_s
{
    const char *parent_name;
    const char *object_name;
    const char *interface_name;
    js_class_template_getter js_class_template_getter_fun;
    js_class_constructor_cb_t js_class_constructor_cb;
    //class options may be null - default
    class_definition_options_t* class_options;
} js_entity_definition_t;

typedef const js_entity_definition_t *js_entity_definition_ptr_t;

/**
 * Plugin export names
 */
#define PLUGIN_WIDGET_START_PROC        on_widget_start
#define PLUGIN_WIDGET_INIT_PROC         on_widget_init
#define PLUGIN_WIDGET_STOP_PROC         on_widget_stop
#define PLUGIN_FRAME_LOAD_PROC          on_frame_load
#define PLUGIN_FRAME_UNLOAD_PROC        on_frame_unload
#define PLUGIN_CLASS_MAP                class_map
#define PLUGIN_GET_CLASS_PROC_MAP       get_widget_class_map

#define PLUGIN_WIDGET_START_PROC_NAME   "on_widget_start"
#define PLUGIN_WIDGET_INIT_PROC_NAME    "on_widget_init"
#define PLUGIN_WIDGET_STOP_PROC_NAME    "on_widget_stop"
#define PLUGIN_FRAME_LOAD_PROC_NAME     "on_frame_load"
#define PLUGIN_FRAME_UNLOAD_PROC_NAME   "on_frame_unload"
#define PLUGIN_CLASS_MAP_NAME           "class_map"
#define PLUGIN_GET_CLASS_MAP_PROC_NAME  "get_widget_class_map"

/**
 * Plugin export typedefs
 */
typedef void (*on_widget_start_proc)(int widgetId);

typedef void (*on_widget_init_proc)(feature_mapping_interface_t *interface);

/**
 * FIXME: Add documentation
 */
typedef void (*on_widget_stop_proc)(int widgetId);

typedef void (*on_frame_load_proc)(java_script_context_t context);

typedef void (*on_frame_unload_proc)(java_script_context_t context);

typedef const js_entity_definition_t* (*get_widget_entity_map_proc)();

#endif // WRT_PLUGIN_EXPORT_H
