#!/bin/sh
# Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

WRT_DB=/opt/dbspace/.wrt.db
WRT_DB_BCK=/tmp/wrt.db_backup

if [ "x$1" == "xstart" ]; then
    echo start;
    cp $WRT_DB $WRT_DB_BCK
    wrt_commons_create_clean_db.sh;

    #simple plugins
    INS_MIN_PLUGINPROP="insert into PluginProperties(PluginPropertiesId, InstallationState, PluginLibraryName"
    INS_ALL_PLUGINPROP="insert into PluginProperties(PluginPropertiesId, InstallationState, PluginLibraryName, PluginLibraryPath)"
    INS_PLUGIN_OBJECTS="insert into PluginImplementedObjects(PluginObject, PluginPropertiesId)"

    sqlite3 $WRT_DB "${INS_MIN_PLUGINPROP}) VALUES(1, 1, 'plugin1')";
    sqlite3 $WRT_DB "${INS_MIN_PLUGINPROP}, PluginLibraryPath) VALUES(2, 1, 'plugin2', 'path_to_plugin2')";
    sqlite3 $WRT_DB "${INS_MIN_PLUGINPROP}) VALUES(3, 1, 'plugin3')";
    sqlite3 $WRT_DB "${INS_ALL_PLUGINPROP} VALUES(4, 1, 'p4', 'path_to_p4')";
    sqlite3 $WRT_DB "${INS_ALL_PLUGINPROP} VALUES(5, 1, 'p5', 'path_to_p5')";

    #plugin dependendencies
    #sqlite3 $WRT_DB "${INS_PLUGIN_OBJECTS} VALUES(1, )";
    sqlite3 $WRT_DB "${INS_PLUGIN_OBJECTS} VALUES('', 2)";
    sqlite3 $WRT_DB "${INS_PLUGIN_OBJECTS} VALUES('Plugin_3_Object_A', 3)";
    sqlite3 $WRT_DB "${INS_PLUGIN_OBJECTS} VALUES('Plugin_4_Object_A', 4)";
    sqlite3 $WRT_DB "${INS_PLUGIN_OBJECTS} VALUES('Plugin_4_Object_B', 4)";
    sqlite3 $WRT_DB "${INS_PLUGIN_OBJECTS} VALUES('Plugin_4_Object_C', 4)";
    sqlite3 $WRT_DB "${INS_PLUGIN_OBJECTS} VALUES('Plugin_5_Object_A', 5)";


    #simple features
    INS_ALL_FEATURESLIST="insert into FeaturesList(FeatureUUID, FeatureName, PluginPropertiesId)"
    sqlite3 $WRT_DB "${INS_ALL_FEATURESLIST} VALUES(1, 'feature1', 1)";
    sqlite3 $WRT_DB "${INS_ALL_FEATURESLIST} VALUES(2, 'feature2', 4)";
    sqlite3 $WRT_DB "${INS_ALL_FEATURESLIST} VALUES(3, 'feature3', 4)";
    sqlite3 $WRT_DB "${INS_ALL_FEATURESLIST} VALUES(4, 'feature4', 4)";

    #device capabilities
    INS_ALL_DEVICECAPS="insert into DeviceCapabilities(DeviceCapID, DeviceCapName, DeviceCapDefaultValue)"
    sqlite3 $WRT_DB "${INS_ALL_DEVICECAPS} VALUES(1, 'devicecap1', 1)";
    sqlite3 $WRT_DB "${INS_ALL_DEVICECAPS} VALUES(2, 'devicecap2', 2)";
    sqlite3 $WRT_DB "${INS_ALL_DEVICECAPS} VALUES(3, 'devicecap3', 3)";
    sqlite3 $WRT_DB "${INS_ALL_DEVICECAPS} VALUES(4, 'devicecap4', 4)";

    #Widgets
    INS_ALL_WIDGETEXT="insert into WidgetExtendedInfo(app_id, share_href, signature_type)"
    INS_ALL_WIDGET="insert into WidgetInfo(app_id, widget_id, widget_version, widget_width, widget_height, author_name, author_email, author_href, base_folder, webkit_plugins_required, recognized, wac_signed, distributor_signed, min_version, tizen_appid)"
    INS_ALL_WIDGET_LOC="insert into LocalizedWidgetInfo(app_id, widget_locale, widget_name, widget_shortname, widget_description, widget_license, widget_license_file, widget_license_href)"
    INS_ALL_WIDGET_ICONS="insert into WidgetIcon(app_id, icon_src, icon_width, icon_height)"
    INS_ALL_WIDGET_LOC_ICONS="insert into WidgetLocalizedIcon(app_id, icon_id, widget_locale)"
    INS_ALL_WIDGET_STARTFILE="insert into WidgetStartFile(app_id, src)"
    INS_ALL_WIDGET_LOC_STARTFILE="insert into WidgetLocalizedStartFile(app_id, start_file_id, widget_locale, type, encoding)"
    INS_ALL_WIDGET_DEFPREF="insert into WidgetDefaultPreference(app_id, key_name, key_value, readonly)"
    INS_ALL_WIDGET_PREF="insert into WidgetPreference(tizen_appid, key_name, key_value, readonly)"
    INS_ALL_WIDGET_FEATURE="insert into WidgetFeature(app_id, name, required)"
    INS_ALL_WIDGET_FEATURE_PARAM="insert into FeatureParam(widget_feature_id, name, value)"
    INS_ALL_WIDGET_WINMODES="insert into WidgetWindowModes(app_id, window_mode)"
    INS_ALL_WIDGET_WARP="insert into WidgetWARPInfo(app_id, iri, subdomain_access)"
    INS_ALL_WIDGET_CERT="insert into WidgetCertificateFingerprint(app_id, owner, chainid, type, md5_fingerprint, sha1_fingerprint, common_name)"
    INS_ALL_WIDGET_POWDERLEV="insert into PowderLevels(app_id, category, level)"
    INS_ALL_WIDGET_POWDERLEV_CONT="insert into PowderLevelContexts(levelid, context)"


    sqlite3 $WRT_DB "${INS_ALL_WIDGET} VALUES(2000, 'w_id_2000', '1.0.0', 100, 200, 'a_name_2000', 'a_email_2000', 'a_href_2000', 'basef_2000', 1, 1, 1, 1, '1.0', 'tizenid201')";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET} VALUES(2001, 'w_id_2001', '2.0.0', 100, 200, 'a_name_2001', 'a_email_2001', 'a_href_2001', 'basef_2001', 1, 1, 1, 1, '0.5', 'tizenid202')";
    sqlite3 $WRT_DB "insert into WidgetInfo(app_id, back_supported, tizen_appid) VALUES(2002, 0, 'tizenid203')";
    sqlite3 $WRT_DB "insert into WidgetInfo(app_id, back_supported, tizen_appid) VALUES(2003, 0, 'tizenid204')"; # for properties tests

    sqlite3 $WRT_DB "${INS_ALL_WIDGETEXT} VALUES(2000, 'share_href_2000', 0)";
    sqlite3 $WRT_DB "${INS_ALL_WIDGETEXT} VALUES(2001, 'share_href_2001', 0)";
    sqlite3 $WRT_DB "insert into WidgetExtendedInfo(app_id) VALUES(2002)";
    sqlite3 $WRT_DB "insert into WidgetExtendedInfo(app_id) VALUES(2003)";

    sqlite3 $WRT_DB "${INS_ALL_WIDGET_LOC} VALUES(2000, 'en', 'w_name_2000_en', 'w_shortname_2000_en', 'w_desc_2000_en', 'w_lic_2000_en', 'w_licf_2000_en', 'w_lic_href_2000_en')";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_LOC} VALUES(2000, 'pl', 'w_name_2000_pl', 'w_shortname_2000_pl', 'w_desc_2000_pl', 'w_lic_2000_pl', 'w_licf_2000_pl', 'w_lic_href_2000_pl')";
    sqlite3 $WRT_DB "insert into LocalizedWidgetInfo(app_id, widget_locale) VALUES(2002, 'en')";
    sqlite3 $WRT_DB "insert into LocalizedWidgetInfo(app_id, widget_locale) VALUES(2003, 'en')";

    sqlite3 $WRT_DB "${INS_ALL_WIDGET_ICONS} VALUES(2000, 'icon_src_2000', 50, 50)";
    sqlite3 $WRT_DB "insert into WidgetIcon(app_id, icon_src) VALUES(2002, 'icon_src_2002')";

    sqlite3 $WRT_DB "${INS_ALL_WIDGET_LOC_ICONS} VALUES(2000, 1, 'en')";

    sqlite3 $WRT_DB "${INS_ALL_WIDGET_STARTFILE} VALUES(2000, 'start_file_2000')";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_STARTFILE} VALUES(2001, 'start_file_2001')";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_STARTFILE} VALUES(2002, 'start_file_2002')";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_STARTFILE} VALUES(2003, 'start_file_2003')";

    sqlite3 $WRT_DB "${INS_ALL_WIDGET_LOC_STARTFILE} VALUES(2000, 1, 'en', '', '')";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_LOC_STARTFILE} VALUES(2001, 2, 'en', '', '')";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_LOC_STARTFILE} VALUES(2002, 3, 'en', '', '')";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_LOC_STARTFILE} VALUES(2003, 4, 'en', '', '')";

    #widget properties
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_PREF} VALUES('tizenid201', 'key1_for_2000', 'value_for_key1_2000', 0)";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_PREF} VALUES('tizenid201', 'key2_for_2000', 'value_for_key2_2000', 0)";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_PREF} VALUES('tizenid202', 'key1_for_2001', 'value1_for_key_2001', 1)";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_PREF} VALUES('tizenid203', 'key1_for_2002', 'value1_for_key_2002', 0)";
    sqlite3 $WRT_DB "${INS_ALL_WIDGET_PREF} VALUES('tizenid203', 'key2_for_2002', 'value2_for_key_2002', 1)";

    #create if not exists and fix autoincrement value
    sqlite3 $WRT_DB "INSERT INTO WidgetInfo(tizen_appid) VALUES('temp')";
    sqlite3 $WRT_DB "DELETE FROM WidgetInfo WHERE tizen_appid = 'temp'";
    sqlite3 $WRT_DB "UPDATE sqlite_sequence SET seq = 2004 WHERE name = 'WidgetInfo'";

    exit $?

elif [ "x$1" == "xstop" ]; then
    echo stop;
    cp $WRT_DB_BCK $WRT_DB
    exit $?
else
    exit 1
fi
