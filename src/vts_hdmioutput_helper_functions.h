/**
 *  If not stated otherwise in this file or this component's LICENSE
 *  file the following copyright and licenses apply:
 *
 *  Copyright 2024 RDK Management
 *
 *  Licensed under the Apache License, Version 2.0 (the License);
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an AS IS BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * @addtogroup HPK Hardware Porting Kit
 * @{
 * @par The Hardware Porting Kit
 * HPK is the next evolution of the well-defined Hardware Abstraction Layer
 * (HAL), but augmented with more comprehensive documentation and test suites
 * that OEM or SOC vendors can use to self-certify their ports before taking
 * them to RDKM for validation or to an operator for final integration and
 * deployment. The Hardware Porting Kit effectively enables an OEM and/or SOC
 * vendor to self-certify their own Video Accelerator devices, with minimal RDKM
 * assistance.
 *
 */

/**
 * @addtogroup HDMIOUTPUT HDMIOUTPUT Module
 * @{
 */

/**
 * @addtogroup HDMIOUTPUT_HALTEST HDMIOUTPUT HAL Tests
 * @{
 */

/**
 * @defgroup HDMIOUTPUT_HALTEST_HELPER_HEADER HDMIOUTPUT HAL Tests helper functions Header File
 * @{
 * @parblock
 *
 * ### Helper functions for VTS tests :
 *
 * Helper functions required for the module across all vendors.
 *
 * **Pre-Conditions:**  None @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
 * @file vts_hdmioutput_helper_functions.h
 *
 */

#ifndef __VTS_HDMIOUTPUT_HELPER_H__
#define __VTS_HDMIOUTPUT_HELPER_H__

#include <gtest/gtest.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/Status.h>
#include <utils/String16.h>
#include <stdio.h>
#include <vector>
#include <optional>
#include <ut.h>
#include <ut_log.h>
#include <ut_kvp_profile.h>
#include <ut_control_plane.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#include "com/rdk/hal/hdmioutput/IHDMIOutputEventListener.h"
#include "com/rdk/hal/hdmioutput/IHDMIOutputManager.h"
#include "com/rdk/hal/hdmioutput/IHDMIOutput.h"
#include "com/rdk/hal/hdmioutput/IHDMIOutputController.h"
#include "com/rdk/hal/hdmioutput/IHDMIOutputControllerListener.h"
#include "com/rdk/hal/hdmioutput/BnHDMIOutputEventListener.h"
#include "com/rdk/hal/hdmioutput/BnHDMIOutputControllerListener.h"
#include "com/rdk/hal/hdmioutput/AFD.h"
#include "com/rdk/hal/hdmioutput/ContentType.h"
#include "com/rdk/hal/hdmioutput/ScanInformation.h"

#define MAX_LENGTH 256

using android::sp;
using android::String16;
using android::defaultServiceManager;
using android::binder::Status;

using namespace com::rdk::hal;
using namespace com::rdk::hal::hdmioutput;

extern const ut_control_keyStringMapping_t bool_mapTable_hdmioutput[];
extern const ut_control_keyStringMapping_t statusHdmiOutputException_mapTable[];

std::string vecIntToString(std::vector<int32_t>& addrs);
std::string vecByteToHexString(const std::vector<uint8_t>& data);
HDROutputMode convertStringToHDROutputMode(const std::string &val); 
HDCPProtocolVersion convertStringToHDCPProtocolVersions(const std::string &val);
Colorimetry convertStringToColorimetry(const std::string &val); 
PixelFormat convertStringToPixelFormat(const std::string &val);
SPDSource convertStringToSPDSource(const std::string &val);
FreeSync convertStringToFreeSync(const std::string &val);
HDMIVersion convertStringToHDMIVersion(const std::string &val);
VIC convertStringToVIC(const std::string &val);
ExtendedColorimetry convertStringToExtendedColorimetry(const std::string &val);
AdditionalColorimetryExtension convertStringToAdditionalColorimetryExtension(const std::string &val);

// Profile reading helper functions
int32_t getProfileSupportedVIC(int resourceIndex, int vicIndex);
int32_t getProfileSupportedContentType(int resourceIndex, int typeIndex);
std::string getProfileSupportedHDRMode(int resourceIndex, int modeIndex);
int32_t getProfileSupportedAFD(int resourceIndex, int afdIndex);
int32_t getProfileSupportedScanInfo(int resourceIndex, int scanIndex);
int getProfileSupportedVICCount(int resourceIndex);
int getProfileSupportedContentTypeCount(int resourceIndex);
int getProfileSupportedHDRModeCount(int resourceIndex);
std::string getProfileSupportedSPDSource(int resourceIndex, int sourceIndex);

#endif //__VTS_HDMIOUTPUT_HELPER_H__

/** @} */ // End of HDMIOUTPUT_HALTEST_HELPER_HEADER
/** @} */ // End of HDMIOUTPUT_HALTEST
/** @} */ // End of HDMIOUTPUT
/** @} */ // End of HPK
