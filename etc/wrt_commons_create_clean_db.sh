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

source /etc/tizen-platform.conf

DB_PATH=$TZ_SYS_DB/
DB_USER_PATH=$TZ_USER_DB/

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
    chown 0:db_wrt $dbpath.$name.db
    chown 0:db_wrt $dbpath.$name.db-journal
    chmod 660 $dbpath.$name.db
    chmod 660 $dbpath.$name.db-journal

    pkill -9 security-serv

    # The Tizen 3.0 smack domains have not been estableshed yet,
    # which requires all files to be set for floor.  Once the
    # tizen security team is defined a domain structure then 
    # the following can be reenabled using the appropriate names
    ## restore smack label
    #if [ -n "$DB_LABEL" ]
    #then
    #    chsmack -a "$DB_LABEL" $dbpath.$name.db
    #fi
    ## restore smack label
    #if [ -n "$JOURNAL_LABEL" ]
    #then
    #    chsmack -a "$JOURNAL_LABEL" $dbpath.$name.db-journal
    #fi
}

for name in wrt
do
    create_db $name $DB_PATH
done

for name in wrt_custom_handler wrt_i18n
do
    create_db $name $DB_USER_PATH
done
