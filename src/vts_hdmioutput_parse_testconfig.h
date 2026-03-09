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
* vts_hdmioutput_parse_testconfig.h
*
* Parse test configuration file for HDMI Output L3 tests
*/

#ifndef __VTS_HDMIOUTPUT_PARSE_TESTCONFIG_H__
#define __VTS_HDMIOUTPUT_PARSE_TESTCONFIG_H__

#include <string>
#include <vector>

#include <ut.h>
#include <ut_log.h>
#include "ut_kvp_profile.h"
#include "ut_control_plane.h"

#include "com/rdk/hal/hdmioutput/HDCPStatus.h"
#include "com/rdk/hal/hdmioutput/HDCPProtocolVersion.h"

using namespace std;
using namespace com::rdk::hal::hdmioutput;

// Map tables for parsing enum values from YAML
extern const ut_control_keyStringMapping_t hdcpProtocolVersion_mapTable[];
extern const ut_control_keyStringMapping_t hdcpStatus_mapTable[];

#define VTS_HDMIOUTPUT_TEST_CONFIG_PATH "./test_configuration/vts_l3_HdmiOutput.yaml"

typedef struct hdmioutputHPDConfig
{
    int32_t portId;
    bool connectionState;
    std::vector<bool> hpdSequence;
    hdmioutput::HDCPProtocolVersion expectedHdcpVersions;
    bool expectEdidAfterHpd;
    uint32_t expectedEdidMinSize;
    std::vector<uint8_t> expectedEdid;
    uint32_t maxWaitTimeMs;
} hdmioutputHPDConfig_t;

typedef struct hdmioutputSPDConfig
{
    int32_t portId;
    std::string vendorName;
    std::string productDescription;
    std::string sourceInformation;
    uint32_t maxWaitTimeMs;
} hdmioutputSPDConfig_t;

typedef struct hdmioutputPropertyTestItem
{
    std::string propertyName;
    std::vector<int32_t> values;
} hdmioutputPropertyTestItem_t;

typedef struct hdmioutputPropertyConfig
{
    int32_t portId;
    std::vector<hdmioutputPropertyTestItem_t> testProperties;
    uint32_t maxWaitTimeMs;
} hdmioutputPropertyConfig_t;

typedef struct hdmioutputEDIDConfig
{
    int32_t portId;
    uint32_t expectedEdidMinSize;
    std::vector<uint8_t> expectedEdid;
    uint32_t maxWaitTimeMs;
} hdmioutputEDIDConfig_t;

UT_status_t vts_hdmioutput_parse_hpdTest(char *configurationFile, const char *module, const char *testGroup,
                                          const char *testName, std::vector<hdmioutputHPDConfig_t>& hpdConfigs);

UT_status_t vts_hdmioutput_parse_spdTest(char *configurationFile, const char *module, const char *testGroup,
                                          const char *testName, std::vector<hdmioutputSPDConfig_t>& spdConfigs);

UT_status_t vts_hdmioutput_parse_propertyTest(char *configurationFile, const char *module, const char *testGroup,
                                               const char *testName, std::vector<hdmioutputPropertyConfig_t>& propertyConfigs);

#endif // __VTS_HDMIOUTPUT_PARSE_TESTCONFIG_H__
