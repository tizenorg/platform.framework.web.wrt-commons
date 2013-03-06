#!/bin/bash

# Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
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

set -e

trap 'echo "Script failed"; exit 1' ERR

WRT_DB=/opt/dbspace/.wrt.db
WRT_DB_BCK=/tmp/wrt.db_backup
WIDGET_INSTALL_PATH=/opt/usr/apps

case $1 in
    start)
        echo "start"
        cp $WRT_DB $WRT_DB_BCK
        wrt_commons_create_clean_db.sh

        #Widgets
        INS_ALL_WIDGETEXT="insert into WidgetExtendedInfo(app_id, installed_path)"
        INS_ALL_WIDGET="insert into WidgetInfo(app_id, tizen_appid)"

        sqlite3 $WRT_DB "${INS_ALL_WIDGET} VALUES(1, 'tizenid201')";
        sqlite3 $WRT_DB "${INS_ALL_WIDGET} VALUES(2, 'tizenid202')";
        sqlite3 $WRT_DB "${INS_ALL_WIDGETEXT} VALUES(1, '/opt/share/widget/tests/localization/widget1')";
        sqlite3 $WRT_DB "${INS_ALL_WIDGETEXT} VALUES(2, '/opt/share/widget/tests/localization/widget2')";
        exit 0
        ;;
    stop)
        echo "stop";
        cp $WRT_DB_BCK $WRT_DB
        exit 0
        ;;
    *)
        echo "nothing to do"
        exit 1
        ;;
esac
