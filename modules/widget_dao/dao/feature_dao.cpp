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
 * This file contains the definition of feature dao class.
 *
 * @file    widget_dao.cpp
 * @author  Jaroslaw Osmanski (j.osmanski@samsung.com)
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of feature configuration.
 */

#include <dpl/wrt-dao-rw/feature_dao.h>
#include <dpl/foreach.h>
#include <dpl/db/orm.h>
#include <orm_generator_wrt.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>

namespace WrtDB {
namespace FeatureDAO {

FeatureHandle RegisterFeature(const PluginMetafileData::Feature &feature,
                              const DbPluginHandle pluginHandle)
{
    Try
    {
        LogDebug("Registering Feature " << feature.m_name);
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        if (FeatureDAOReadOnly::isFeatureInstalled(feature.m_name)) {
            LogError(" >> Feature " << feature.m_name <<
                     " is already registered.");
            transaction.Commit();
            return -1;
        }

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        //register feature
        {
            LogInfo("    |-- Registering feature " << feature.m_name);

            FeaturesList::Row row;
            row.Set_FeatureName(DPL::FromUTF8String(feature.m_name));
            row.Set_PluginPropertiesId(pluginHandle);

            WRT_DB_INSERT(insert, FeaturesList, &WrtDatabase::interface())
            insert->Values(row);
            insert->Execute();
        }

        FeatureHandle featureHandle =
                FeatureDAOReadOnly(feature.m_name).GetFeatureHandle();

        //register device capabilities
        // Device Capabilities is unused in current version
        FOREACH(itdev, feature.m_deviceCapabilities)
        {
            int deviceCapID;

            if (FeatureDAOReadOnly::isDeviceCapabilityInstalled(*itdev)) {
                LogInfo("    |    |--DeviceCap " << *itdev <<
                        " already installed!");

                WRT_DB_SELECT(select, DeviceCapabilities, &WrtDatabase::interface())

                select->Where(Equals<DeviceCapabilities::DeviceCapName>(
                                  DPL::FromUTF8String(*itdev)));

                deviceCapID =
                    select->GetSingleValue<DeviceCapabilities::DeviceCapID>();
            } else {
                LogInfo("    |    |--Register DeviceCap: " << *itdev);

                DeviceCapabilities::Row row;
                row.Set_DeviceCapName(DPL::FromUTF8String(*itdev));

                WRT_DB_INSERT(insert, DeviceCapabilities, &WrtDatabase::interface())
                insert->Values(row);
                deviceCapID = insert->Execute();
            }

            FeatureDeviceCapProxy::Row row;
            row.Set_FeatureUUID(featureHandle);
            row.Set_DeviceCapID(deviceCapID);

            WRT_DB_INSERT(insert, FeatureDeviceCapProxy, &WrtDatabase::interface())
            insert->Values(row);
            insert->Execute();
        }

        transaction.Commit();

        return featureHandle;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during Registering Feature");
    }
}

} // namespace FeatureDAO
} // namespace WrtDB
