/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
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
 * @defgroup HDMIOUTPUT_HALTEST_CLASS HDMIOUTPUT HAL Tests L1 Test Cases
 * @{
 * @parblock
 *
 * ### Common functions/macros required for the HDMIOUTPUT module :
 *
 * Common functions/macros required for the HDMIOUTPUT module .
 *
 * **Pre-Conditions:**  None @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
 * @file vts_hdmioutput.h
 *
 */


#include <gtest/gtest.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/Status.h>
#include <utils/String16.h>
#include <stdio.h>
#include <vector>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>
#include <utility>
#include <ut.h>
#include <ut_kvp_profile.h>
#include <ut_log.h>

#include "com/rdk/hal/hdmioutput/IHDMIOutputEventListener.h"
#include "com/rdk/hal/hdmioutput/IHDMIOutputManager.h"
#include "com/rdk/hal/hdmioutput/IHDMIOutput.h"
#include "com/rdk/hal/hdmioutput/IHDMIOutputController.h"
#include "com/rdk/hal/hdmioutput/IHDMIOutputControllerListener.h"
#include "com/rdk/hal/hdmioutput/BnHDMIOutputEventListener.h"
#include "com/rdk/hal/hdmioutput/BnHDMIOutputControllerListener.h"
#include "vts_hdmioutput_helper_functions.h"


using android::sp;
using android::String16;
using android::defaultServiceManager;
using android::binder::Status;

using namespace com::rdk::hal;
using namespace com::rdk::hal::hdmioutput;

extern sp<IHDMIOutputManager> gHDMIOUTPUTHandle;

#define HDMIOUTPUT_GET_PLATCAPABILITIES(hdmioutputHandle, platCap1) \
    do { \
        UT_LOG_INFO("Calling getCapabilities(OUT: platCap1)\n");\
        ::android::binder::Status status = hdmioutputHandle->getCapabilities(&platCap1);\
        UT_LOG_INFO("Result getCapabilities(OUT: nativeFrameRate:[%f], freeSync:[%d]), status:[isOK:%s, exception:%s]\n",\
                    platCap1.nativeFrameRate, static_cast<int32_t>(platCap1.freeSync),\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_HDMIOUTPUT_IDS(hdmioutputHandle, hdmiOutputIds) \
    do { \
        UT_LOG_INFO("Calling getHDMIOutputIds(OUT: hdmiOutputIds)\n");\
        ::android::binder::Status status = hdmioutputHandle->getHDMIOutputIds(&hdmiOutputIds);\
        UT_LOG_INFO("Result getHDMIOutputIds(OUT: hdmiOutputIds count:[%zu]), status:[isOK:%s, exception:%s]\n",\
                    hdmiOutputIds.size(),\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_HDMIOUTPUT(hdmioutputHandle, hdmiOutputId, hdmiOutput) \
    do { \
        UT_LOG_INFO("Calling getHDMIOutput(IN: hdmiOutputId:[%d], OUT: IHDMIOutput)\n", hdmiOutputId.value);\
        android::binder::Status status = hdmioutputHandle->getHDMIOutput(hdmiOutputId, &hdmiOutput);\
        UT_LOG_INFO("Result getHDMIOutput(IN: hdmiOutputId:[%d], OUT: IHDMIOutput[%p]), status:[isOK:%s]\n", \
                    hdmiOutputId.value, hdmiOutput.get(), status.isOk() ? "true" : "false");\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_HDMIOUTPUT_NEG(hdmioutputHandle, hdmiOutputId, hdmiOutput, status) \
    do { \
        UT_LOG_INFO("Calling getHDMIOutput(IN: hdmiOutputId:[%d], OUT: IHDMIOutput)\n", hdmiOutputId.value);\
        status = hdmioutputHandle->getHDMIOutput(hdmiOutputId, &hdmiOutput);\
        UT_LOG_INFO("Result getHDMIOutput(IN: hdmiOutputId:[%d], OUT: IHDMIOutput[%p]), status:[isOK:%s, exception:%s]\n", \
                    hdmiOutputId.value, hdmiOutput.get(), \
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()), \
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_GET_CAPABILITIES(hdmiOutput, caps1) \
    do { \
        UT_LOG_INFO("Calling getCapabilities(IN: none, OUT: capabilities)\n");\
        ::android::binder::Status status = hdmiOutput->getCapabilities(&caps1);\
        UT_LOG_INFO("Result getCapabilities(OUT: capabilities, status:[isOK:%s, exception:%s])\n", \
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()), \
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_PROPERTY(hdmiOutput, property, propValue) \
    do { \
        UT_LOG_INFO("Calling getProperty(IN: property:[%d], OUT:_aidl_return[])\n", static_cast<int32_t>(property));\
        ::std::optional<::com::rdk::hal::PropertyValue> optionalValue;\
        ::android::binder::Status status = hdmiOutput->getProperty(property, &optionalValue);\
        if (optionalValue.has_value()) {\
            propValue = optionalValue.value();\
        }\
        UT_LOG_INFO("Result getProperty(IN: property:[%d], OUT:_aidl_return[has value:%s]), status:[isOK:%s]\n",\
                    static_cast<int32_t>(property),\
                    (optionalValue.has_value() ? "true" : "false"),\
                    (status.isOk() ? "true" : "false"));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_PROPERTY_NEG(hdmiOutput, property, propValue, status) \
    do { \
        UT_LOG_INFO("Calling getProperty(IN: property:[%d], OUT: propertyValue)\n", static_cast<int32_t>(property));\
        status = hdmiOutput->getProperty(property, &propValue);\
        UT_LOG_INFO("Result getProperty(IN: property:[%d], OUT: propertyValue [has value: %s]), status:[isOK:%s, exception:%s]\n",\
                    static_cast<int32_t>(property),\
                    (propValue.has_value() ? "true" : "false"),\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_GET_STATE(hdmiOutput, state) \
    do { \
        UT_LOG_INFO("Calling getState(IN: none, OUT:_aidl_return)");\
        ::android::binder::Status status = hdmiOutput->getState(&state);\
        UT_LOG_INFO("Result getState(OUT:_aidl_return:[%d]), status:[isOK:%s, exception:%s]",\
                    static_cast<int>(state),\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_STATE_NEG(hdmiOutput, statePtr, status) \
    do { \
        UT_LOG_INFO("Calling getState(IN: none, OUT:_aidl_return)");\
        status = hdmiOutput->getState(statePtr);\
        UT_LOG_INFO("Result getState(OUT:_aidl_return:[%p]), status:[isOK:%s, exception:%s]",\
                    statePtr,\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_OPEN(hdmiOutput, validListener, tempController) \
    do { \
        UT_LOG_INFO("Calling open(IN: hdmiOutputControllerListener, OUT: IHDMIOutputController pointer)\n");\
            android::binder::Status status = hdmiOutput->open(validListener, &tempController);\
            UT_LOG_INFO("Result open(IN: hdmiOutputControllerListener, OUT: IHDMIOutputController pointer[0x%X]), status:[isOK:%s, exception:None]\n", \
                     tempController.get(), status.isOk() ? "true" : "false");\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_OPEN_NEG(hdmiOutput, listener, tempController, status) \
    do { \
        UT_LOG_INFO("Calling open(IN: hdmiOutputControllerListener, OUT: IHDMIOutputController pointer)\n");\
        status = hdmiOutput->open(listener, &tempController);\
        UT_LOG_INFO("Result open(IN: hdmiOutputControllerListener, OUT: IHDMIOutputController pointer[%p]), status:[isOK:%s, exception:%s]\n", \
                     tempController.get(), \
                     UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                     UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_CLOSE(hdmiOutput, tempController, aidl_return) \
    do { \
        UT_LOG_INFO("Calling close(IN:hdmiOutputController, OUT:_aidl_return)\n");\
        ::android::binder::Status status = hdmiOutput->close(tempController, &aidl_return);\
        UT_LOG_INFO("Result close(IN:hdmiOutputController, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_CLOSE_NEG(hdmiOutput, tempController, aidl_return, status) \
    do { \
        UT_LOG_INFO("Calling close(IN:hdmiOutputController, OUT:_aidl_return)\n");\
        status = hdmiOutput->close(tempController, &aidl_return);\
        UT_LOG_INFO("Result close(IN:hdmiOutputController, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_REGISTER_EVENT_LISTENER(hdmiOutput, eventListener, aidl_return) \
    do { \
        UT_LOG_INFO("Calling registerEventListener(IN: hdmiOutputEventListener, OUT:_aidl_return)\n");\
        ::android::binder::Status status = hdmiOutput->registerEventListener(eventListener, &aidl_return);\
        UT_LOG_INFO("Result registerEventListener(IN: hdmiOutputEventListener, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_REGISTER_EVENT_LISTENER_NEG(hdmiOutput, eventListener, aidl_return, status) \
    do { \
        UT_LOG_INFO("Calling registerEventListener(IN: hdmiOutputEventListener, OUT:_aidl_return)\n");\
        status = hdmiOutput->registerEventListener(eventListener, &aidl_return);\
        UT_LOG_INFO("Result registerEventListener(IN: hdmiOutputEventListener, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_UNREGISTER_EVENT_LISTENER(hdmiOutput, eventListener, aidl_return) \
    do { \
        UT_LOG_INFO("Calling unregisterEventListener(IN: hdmiOutputEventListener, OUT:_aidl_return)\n");\
        ::android::binder::Status status = hdmiOutput->unregisterEventListener(eventListener, &aidl_return);\
        UT_LOG_INFO("Result unregisterEventListener(IN: hdmiOutputEventListener, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_UNREGISTER_EVENT_LISTENER_NEG(hdmiOutput, eventListener, aidl_return, status) \
    do { \
        UT_LOG_INFO("Calling unregisterEventListener(IN: hdmiOutputEventListener, OUT:_aidl_return)\n");\
        status = hdmiOutput->unregisterEventListener(eventListener, &aidl_return);\
        UT_LOG_INFO("Result unregisterEventListener(IN: hdmiOutputEventListener, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_START(controller) \
    do { \
        UT_LOG_INFO("Calling start()\n");\
        ::android::binder::Status status = controller->start();\
        UT_LOG_INFO("Result start(), status:[isOK:%s, exception:%s]\n",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_START_NEG(controller, status) \
    do { \
        UT_LOG_INFO("Calling start()\n");\
        status = controller->start();\
        UT_LOG_INFO("Result start(), status:[isOK:%s, exception:%s]\n",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_STOP(controller) \
    do { \
        UT_LOG_INFO("Calling stop()\n");\
        ::android::binder::Status status = controller->stop();\
        UT_LOG_INFO("Result stop(), status:[isOK:%s, exception:%s]\n",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_STOP_NEG(controller, status) \
    do { \
        UT_LOG_INFO("Calling stop()\n");\
        status = controller->stop();\
        UT_LOG_INFO("Result stop(), status:[isOK:%s, exception:%s]\n",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_GET_HPD_STATE(controller, hpdState) \
    do { \
        UT_LOG_INFO("Calling getHotPlugDetectState(OUT:_aidl_return)\n");\
        ::android::binder::Status status = controller->getHotPlugDetectState(&hpdState);\
        UT_LOG_INFO("Result getHotPlugDetectState(OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    hpdState ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_HPD_STATE_NEG(controller, hpdState, status) \
    do { \
        UT_LOG_INFO("Calling getHotPlugDetectState(OUT:_aidl_return)\n");\
        status = controller->getHotPlugDetectState(hpdState);\
        UT_LOG_INFO("Result getHotPlugDetectState(OUT:_aidl_return []), status:[isOK:%s, exception:%s]\n",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_SET_PROPERTY(controller, property, propValue, aidl_return) \
    do { \
        UT_LOG_INFO("Calling setProperty(IN: property:[%d], propertyValue, OUT:_aidl_return)\n", static_cast<int32_t>(property));\
        ::android::binder::Status status = controller->setProperty(property, propValue, &aidl_return);\
        UT_LOG_INFO("Result setProperty(IN: property:[%d], propertyValue, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    static_cast<int32_t>(property),\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_SET_PROPERTY_NEG(controller, property, propValue, aidl_return, status) \
    do { \
        UT_LOG_INFO("Calling setProperty(IN: property:[%d], propertyValue, OUT:_aidl_return)\n", static_cast<int32_t>(property));\
        status = controller->setProperty(property, propValue, &aidl_return);\
        UT_LOG_INFO("Result setProperty(IN: property:[%d], propertyValue, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    static_cast<int32_t>(property),\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_SET_PROPERTYMULTI(controller, propertyKVList, aidl_return) \
    do { \
        UT_LOG_INFO("Calling setPropertyMulti(IN:propertyKVList with size:[%zu], OUT:_aidl_return)\n", propertyKVList.size());\
        ::android::binder::Status status = controller->setPropertyMulti(propertyKVList, &aidl_return);\
        UT_LOG_INFO("Result setPropertyMulti(IN:propertyKVList, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_SET_PROPERTYMULTI_NEG(controller, propertyKVList, aidl_return, status) \
    do { \
        UT_LOG_INFO("Calling setPropertyMulti(IN:propertyKVList with size:[%zu], OUT:_aidl_return)\n", propertyKVList.size());\
        status = controller->setPropertyMulti(propertyKVList, &aidl_return);\
        UT_LOG_INFO("Result setPropertyMulti(IN:propertyKVList, OUT:_aidl_return [%s]), status:[isOK:%s, exception:%s]\n",\
                    aidl_return ? "true" : "false",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
    } while (0)

#define HDMIOUTPUT_GET_HDCP_CURRENT_VERSION(controller, hdcpVersion) \
    do { \
        UT_LOG_INFO("Calling getHDCPCurrentVersion(OUT:_aidl_return)\n");\
        ::android::binder::Status status = controller->getHDCPCurrentVersion(&hdcpVersion);\
        UT_LOG_INFO("Result getHDCPCurrentVersion(OUT:_aidl_return [%d]), status:[isOK:%s, exception:%s]\n",\
                    static_cast<int32_t>(hdcpVersion),\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_HDCP_RECEIVER_VERSION(controller, hdcpVersion) \
    do { \
        UT_LOG_INFO("Calling getHDCPReceiverVersion(OUT:_aidl_return)\n");\
        ::android::binder::Status status = controller->getHDCPReceiverVersion(&hdcpVersion);\
        UT_LOG_INFO("Result getHDCPReceiverVersion(OUT:_aidl_return [%d]), status:[isOK:%s, exception:%s]\n",\
                    static_cast<int32_t>(hdcpVersion),\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_HDCP_STATUS(controller, hdcpStatus) \
    do { \
        UT_LOG_INFO("Calling getHDCPStatus(OUT:_aidl_return)\n");\
        ::android::binder::Status status = controller->getHDCPStatus(&hdcpStatus);\
        UT_LOG_INFO("Result getHDCPStatus(OUT:_aidl_return [%d]), status:[isOK:%s, exception:%s]\n",\
                    static_cast<int32_t>(hdcpStatus),\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_SET_SPD_INFOFRAME(controller, spdInfoFrame) \
    do { \
        UT_LOG_INFO("Calling setSPDInfoFrame(IN: spdInfoFrame)\n");\
        ::android::binder::Status status = controller->setSPDInfoFrame(spdInfoFrame);\
        UT_LOG_INFO("Result setSPDInfoFrame(IN: spdInfoFrame), status:[isOK:%s, exception:%s]\n",\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
    } while (0)

#define HDMIOUTPUT_GET_SPD_INFOFRAME(controller, spdInfoFrame) \
    do { \
        UT_LOG_INFO("Calling getSPDInfoFrame(OUT: spdInfoFrame)\n");\
        ::std::optional<::com::rdk::hal::hdmioutput::SPDInfoFrame> optionalSPD; \
        ::android::binder::Status status = controller->getSPDInfoFrame(&optionalSPD);\
        UT_LOG_INFO("Result getSPDInfoFrame(OUT: spdInfoFrame [has value:%s]), status:[isOK:%s, exception:%s]\n",\
                    (optionalSPD.has_value() ? "true" : "false"),\
                    UT_Control_GetMapString(bool_mapTable_hdmioutput, status.isOk()),\
                    UT_Control_GetMapString(statusHdmiOutputException_mapTable, status.exceptionCode()));\
        UT_ASSERT_TRUE(status.isOk());\
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
        if (optionalSPD.has_value()) { \
            spdInfoFrame = optionalSPD.value(); \
        } \
    } while (0)

android::sp<com::rdk::hal::hdmioutput::IHDMIOutput> getHDMIOutput(int32_t i);

class HDMIOutputEventListener
        : public com::rdk::hal::hdmioutput::BnHDMIOutputEventListener {
public:
    HDMIOutputEventListener() = default;
    virtual ~HDMIOutputEventListener() = default;

    ::android::binder::Status onStateChanged(::com::rdk::hal::hdmioutput::State oldState,
                                             ::com::rdk::hal::hdmioutput::State newState) override;

    // Event retrieval method
    bool getNextStateChanges(std::pair<::com::rdk::hal::hdmioutput::State, ::com::rdk::hal::hdmioutput::State>& stateChanges);
    
    // Utility methods
    void reset();
    bool hasStateChanged() const;

private:
    mutable std::mutex mStateChangeMutex;
    std::queue<std::pair<::com::rdk::hal::hdmioutput::State, ::com::rdk::hal::hdmioutput::State>> mStateChangeQueue;
};

class HDMIOutputControllerListener
        : public com::rdk::hal::hdmioutput::BnHDMIOutputControllerListener {
public:
    HDMIOutputControllerListener() = default;
    virtual ~HDMIOutputControllerListener() = default;

    ::android::binder::Status onHotPlugDetectStateChanged(bool state) override;
    ::android::binder::Status onFrameRateChanged() override;
    ::android::binder::Status onHDCPStatusChanged(
            ::com::rdk::hal::hdmioutput::HDCPStatus hdcpStatus,
            ::com::rdk::hal::hdmioutput::HDCPProtocolVersion hdcpProtocolVersion) override;
    ::android::binder::Status onEDID(const ::std::vector<uint8_t>& edid) override;

    // Event retrieval methods
    bool getNextHPDStateChange(bool& state);
    bool getNextFrameRateChange();
    bool getNextHDCPStatusChange(std::pair<::com::rdk::hal::hdmioutput::HDCPStatus, ::com::rdk::hal::hdmioutput::HDCPProtocolVersion>& hdcpInfo);
    bool getNextEDID(::std::vector<uint8_t>& edid);
    
    // Utility methods
    void reset();
    bool hasHPDStateChanged() const;
    bool hasFrameRateChanged() const;
    bool hasHDCPStatusChanged() const;
    bool hasEDIDReceived() const;

private:
    mutable std::mutex mHPDStateMutex;
    std::queue<bool> mHPDStateQueue;
    
    mutable std::mutex mFrameRateMutex;
    std::queue<bool> mFrameRateQueue;
    
    mutable std::mutex mHDCPStatusMutex;
    std::queue<std::pair<::com::rdk::hal::hdmioutput::HDCPStatus, ::com::rdk::hal::hdmioutput::HDCPProtocolVersion>> mHDCPStatusQueue;
    
    mutable std::mutex mEDIDMutex;
    std::queue<::std::vector<uint8_t>> mEDIDQueue;
    
    // Legacy single-value storage for backward compatibility
    bool m_hpdStateChanged{false};
    bool m_hpdState{false};
    bool m_frameRateChanged{false};
    bool m_hdcpStatusChanged{false};
    ::com::rdk::hal::hdmioutput::HDCPStatus m_hdcpStatus;
    ::com::rdk::hal::hdmioutput::HDCPProtocolVersion m_hdcpProtocolVersion;
    bool m_edidReceived{false};
    ::std::vector<uint8_t> m_edid;
};

/**
* @} // End of HDMIOUTPUT_HALTEST_CLASS
* @} // End of HDMIOUTPUT_HALTEST
* @} // End of HDMIOUTPUT
* @} // End of HPK
*/
/* End of vts_hdmioutput.h */
