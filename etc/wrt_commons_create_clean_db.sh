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

DB_PATH=/opt/dbspace/
DB_USER_PATH=/opt/usr/dbspace/

function create_db {
    name=$1
    dbpath=$2
    # extract smack label before removal
    DB_LABEL=""
    rm -f $dbpath.$name.db
    # extract smack label before removal
    JOURNAL_LABEL=""
    rm -f $dbpath.$name.db-journal

    SQL=".read /usr/share/wrt-engine/"$name"_db.sql"
    sqlite3 $dbpath.$name.db "$SQL"
    touch $dbpath.$name.db-journal
    chown 0:6026 $dbpath.$name.db
    chown 0:6026 $dbpath.$name.db-journal
    chmod 660 $dbpath.$name.db
    chmod 660 $dbpath.$name.db-journal

    pkill -9 security-serv

    # restore smack label
    # restore smack label
}

for name in wrt
do
    create_db $name $DB_PATH
done

for name in wrt_custom_handler
do
    create_db $name $DB_USER_PATH
done
