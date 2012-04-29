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

wrt_commons_create_clean_db.sh

rm -rf /opt/apps/widget/system/*

# This directories contains test widgets and test keys. It shouldn't be removed.
#rm -rf /opt/apps/widget/user/*
#rm -rf /opt/apps/widget/data/*

#Removing of widget desktop icons
WIDGET_EXEC_PATH=/opt/apps/widget/exec
WIDGET_DESKTOP_PATH=/opt/share/install-info/application
WIDGET_ICON_PATH=/opt/share/icons/default/small
WIDGET_EXECS="${WIDGET_EXEC_PATH}/*";

for file in $WIDGET_EXECS; do
    widget_id=${file#${WIDGET_EXEC_PATH}/};

    widget_desktop_file="${WIDGET_DESKTOP_PATH}/org.tizen.${widget_id}.desktop";
    if [ -f ${widget_desktop_file} ]; then
        echo "rm -f $widget_desktop_file";
        rm -f $widget_desktop_file;
    fi

    widget_icon_file="${WIDGET_ICON_PATH}/${widget_id}.*"
    if [ -f ${widget_icon_file} ]; then
        echo "rm -f $widget_icon_file";
        rm -f $widget_icon_file;
    fi
done

rm -rf /opt/apps/widget/exec/*
touch /opt/apps/widget/plugin-installation-required

