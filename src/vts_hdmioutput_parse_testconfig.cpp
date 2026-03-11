/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2026 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @brief
* vts_hdmioutput_parse_testconfig.cpp
*
* Parse test configuration file for HDMI Output L3 tests
*/

#include "vts_hdmioutput_helper_functions.h"
#include "vts_hdmioutput_parse_testconfig.h"

// Map tables for enum conversion (defined in vts_hdmioutput_helper_functions.cpp)
extern const ut_control_keyStringMapping_t hdcpProtocolVersion_mapTable[];
extern const ut_control_keyStringMapping_t hdcpStatus_mapTable[];

static void* hdmioutputkvp_create_instance(char *fileName)
{
    ut_kvp_instance_t* instance;
    ut_kvp_status_t result;

    instance = ut_kvp_createInstance();

    if(!instance)
    {
        UT_LOG_ERROR("[HDMIOUTPUT]Failed to create ut kvp instance\n");
        return nullptr;
    }

    result = ut_kvp_open(instance, fileName);

    if(result != UT_KVP_STATUS_SUCCESS)
    {
        UT_LOG_ERROR("[HDMIOUTPUT]Failed to open test configuration file\n");
        ut_kvp_destroyInstance(instance);
        return nullptr;
    }

    return (ut_kvp_instance_t*)instance;
}

static void hdmioutputkvp_destroy_instance(void *instance)
{
    ut_kvp_destroyInstance((ut_kvp_instance_t*)instance);
}

UT_status_t vts_hdmioutput_parse_hpdTest(char *configurationFile, const char *module, const char *testGroup,
                                          const char *testName, std::vector<hdmioutputHPDConfig_t>& hpdConfigs)
{
    char key[2 * UT_KVP_MAX_ELEMENT_SIZE];
    char subkey[2 * UT_KVP_MAX_ELEMENT_SIZE];
    char subValue[UT_KVP_MAX_ELEMENT_SIZE];

    void *testconfig = hdmioutputkvp_create_instance(configurationFile);
    ut_kvp_instance_t* kvpTestInstance = (ut_kvp_instance_t*)testconfig;

    if (!kvpTestInstance)
        return UT_STATUS_FAILURE;

    const std::string testListString = string(module) + "." + string(testGroup) + "." + string(testName);
    char *testList = const_cast<char*>(testListString.c_str());

    // ports array
    snprintf(key, sizeof(key), "%s.ports", testList);
    int numPorts = ut_kvp_getListCount(kvpTestInstance, key);
    for (int i = 0; i < numPorts; ++i)
    {
        hdmioutputHPDConfig_t config;
        
        // id
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.id", testList, i);
        config.portId = ut_kvp_getUInt32Field(kvpTestInstance, subkey);

        // connection_state
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.connection_state", testList, i);
        config.connectionState = ut_kvp_getBoolField(kvpTestInstance, subkey);

        // hpd_sequence array
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.hpd_sequence", testList, i);
        int numStates = ut_kvp_getListCount(kvpTestInstance, subkey);
        config.hpdSequence.clear();
        for (int j = 0; j < numStates; ++j)
        {
            char elemkey[2 * UT_KVP_MAX_ELEMENT_SIZE];
            snprintf(elemkey, sizeof(elemkey), "%s.ports.%d.hpd_sequence.%d", testList, i, j);
            config.hpdSequence.push_back(ut_kvp_getBoolField(kvpTestInstance, elemkey));
        }

        // expected_hdcp_version
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.expected_hdcp_version", testList, i);
        ut_kvp_getStringField(kvpTestInstance, subkey, subValue, sizeof(subValue));
        config.expectedHdcpVersions = (hdmioutput::HDCPProtocolVersion)UT_Control_GetMapValue(hdcpProtocolVersion_mapTable, subValue, (int32_t)hdmioutput::HDCPProtocolVersion::UNDEFINED);

        // expect_edid_after_hpd
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.expect_edid_after_hpd", testList, i);
        config.expectEdidAfterHpd = ut_kvp_getBoolField(kvpTestInstance, subkey);

        // expected_edid_min_size
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.expected_edid_min_size", testList, i);
        config.expectedEdidMinSize = ut_kvp_getUInt32Field(kvpTestInstance, subkey);

        // expected_edid array
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.expected_edid", testList, i);
        int numEdidBytes = ut_kvp_getListCount(kvpTestInstance, subkey);
        config.expectedEdid.clear();
        for (int j = 0; j < numEdidBytes; ++j)
        {
            char elemkey[2 * UT_KVP_MAX_ELEMENT_SIZE];
            snprintf(elemkey, sizeof(elemkey), "%s.ports.%d.expected_edid.%d", testList, i, j);
            uint32_t byteValue = ut_kvp_getUInt32Field(kvpTestInstance, elemkey);
            config.expectedEdid.push_back(static_cast<uint8_t>(byteValue));
        }

        // max_wait_time_ms
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.max_wait_time_ms", testList, i);
        config.maxWaitTimeMs = ut_kvp_getUInt32Field(kvpTestInstance, subkey);

        hpdConfigs.push_back(config);
    }
    
    hdmioutputkvp_destroy_instance(kvpTestInstance);
    return UT_STATUS_OK;
}

UT_status_t vts_hdmioutput_parse_spdTest(char *configurationFile, const char *module, const char *testGroup,
                                          const char *testName, std::vector<hdmioutputSPDConfig_t>& spdConfigs)
{
    char key[2 * UT_KVP_MAX_ELEMENT_SIZE];
    char subkey[2 * UT_KVP_MAX_ELEMENT_SIZE];
    char subValue[UT_KVP_MAX_ELEMENT_SIZE];

    void *testconfig = hdmioutputkvp_create_instance(configurationFile);
    ut_kvp_instance_t* kvpTestInstance = (ut_kvp_instance_t*)testconfig;

    if (!kvpTestInstance)
        return UT_STATUS_FAILURE;

    const std::string testListString = string(module) + "." + string(testGroup) + "." + string(testName);
    char *testList = const_cast<char*>(testListString.c_str());

    // ports array
    snprintf(key, sizeof(key), "%s.ports", testList);
    int numPorts = ut_kvp_getListCount(kvpTestInstance, key);
    for (int i = 0; i < numPorts; ++i)
    {
        hdmioutputSPDConfig_t config;
        
        // id
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.id", testList, i);
        config.portId = ut_kvp_getUInt32Field(kvpTestInstance, subkey);

        // spd_info.vendor_name
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.spd_info.vendor_name", testList, i);
        ut_kvp_getStringField(kvpTestInstance, subkey, subValue, sizeof(subValue));
        config.vendorName = std::string(subValue);

        // spd_info.product_description
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.spd_info.product_description", testList, i);
        ut_kvp_getStringField(kvpTestInstance, subkey, subValue, sizeof(subValue));
        config.productDescription = std::string(subValue);

        // spd_info.source_information
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.spd_info.source_information", testList, i);
        ut_kvp_getStringField(kvpTestInstance, subkey, subValue, sizeof(subValue));
        config.sourceInformation = std::string(subValue);

        // max_wait_time_ms
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.max_wait_time_ms", testList, i);
        config.maxWaitTimeMs = ut_kvp_getUInt32Field(kvpTestInstance, subkey);

        spdConfigs.push_back(config);
    }
    
    hdmioutputkvp_destroy_instance(kvpTestInstance);
    return UT_STATUS_OK;
}

UT_status_t vts_hdmioutput_parse_propertyTest(char *configurationFile, const char *module, const char *testGroup,
                                               const char *testName, std::vector<hdmioutputPropertyConfig_t>& propertyConfigs)
{
    char key[2 * UT_KVP_MAX_ELEMENT_SIZE];
    char subkey[2 * UT_KVP_MAX_ELEMENT_SIZE];
    char subValue[UT_KVP_MAX_ELEMENT_SIZE];

    void *testconfig = hdmioutputkvp_create_instance(configurationFile);
    ut_kvp_instance_t* kvpTestInstance = (ut_kvp_instance_t*)testconfig;

    if (!kvpTestInstance)
        return UT_STATUS_FAILURE;

    const std::string testListString = string(module) + "." + string(testGroup) + "." + string(testName);
    char *testList = const_cast<char*>(testListString.c_str());

    // ports array
    snprintf(key, sizeof(key), "%s.ports", testList);
    int numPorts = ut_kvp_getListCount(kvpTestInstance, key);
    for (int i = 0; i < numPorts; ++i)
    {
        hdmioutputPropertyConfig_t config;
        
        // id
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.id", testList, i);
        config.portId = ut_kvp_getUInt32Field(kvpTestInstance, subkey);

        // test_properties array
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.test_properties", testList, i);
        int numProperties = ut_kvp_getListCount(kvpTestInstance, subkey);
        config.testProperties.clear();
        for (int j = 0; j < numProperties; ++j)
        {
            hdmioutputPropertyTestItem_t property;
            char elemkey[2 * UT_KVP_MAX_ELEMENT_SIZE];
            
            // property
            snprintf(elemkey, sizeof(elemkey), "%s.ports.%d.test_properties.%d.property", testList, i, j);
            ut_kvp_getStringField(kvpTestInstance, elemkey, subValue, sizeof(subValue));
            property.propertyName = std::string(subValue);

            // value array
            snprintf(elemkey, sizeof(elemkey), "%s.ports.%d.test_properties.%d.value", testList, i, j);
            int numValues = ut_kvp_getListCount(kvpTestInstance, elemkey);
            property.values.clear();
            for (int k = 0; k < numValues; ++k)
            {
                char valuekey[2 * UT_KVP_MAX_ELEMENT_SIZE];
                snprintf(valuekey, sizeof(valuekey), "%s.ports.%d.test_properties.%d.value.%d", testList, i, j, k);
                property.values.push_back(ut_kvp_getUInt32Field(kvpTestInstance, valuekey));
            }

            config.testProperties.push_back(property);
        }

        // max_wait_time_ms
        snprintf(subkey, sizeof(subkey), "%s.ports.%d.max_wait_time_ms", testList, i);
        config.maxWaitTimeMs = ut_kvp_getUInt32Field(kvpTestInstance, subkey);

        propertyConfigs.push_back(config);
    }
    
    hdmioutputkvp_destroy_instance(kvpTestInstance);
    return UT_STATUS_OK;
}
