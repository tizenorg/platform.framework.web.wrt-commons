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

rm -rf /opt/apps/widget/system/*

#Removing of widget desktop icons
WIDGET_EXEC_PATH=/opt/apps/
WIDGET_DESKTOP_PATH=/opt/share/applications/
WRT_DB=/opt/dbspace/.wrt.db
PLUGINS_INSTALLATION_REQUIRED_PATH=/opt/apps/widget/
PLUGINS_INSTALLATION_REQUIRED=plugin-installation-required

if [ -f ${WRT_DB} ]
then
    PKG_NAME_SET=$(sqlite3 $WRT_DB 'select pkgname from WidgetInfo;')
    for pkgname in $PKG_NAME_SET
    do
        rm -rf ${WIDGET_EXEC_PATH}${pkgname}
        widget_desktop_file="${WIDGET_DESKTOP_PATH}${pkgname}.desktop";
        if [ -f ${widget_desktop_file} ]; then
            rm -f $widget_desktop_file;
        fi
    done
else
    echo "${WRT_DB} doesn't exist"
fi

wrt_commons_create_clean_db.sh
if [ -e ${PLUGINS_INSTALLATION_REQUIRED_PATH} ] && [ -d ${PLUGINS_INSTALLATION_REQUIRED_PATH} ]
then
    touch ${PLUGINS_INSTALLATION_REQUIRED_PATH}${PLUGINS_INSTALLATION_REQUIRED}
fi
