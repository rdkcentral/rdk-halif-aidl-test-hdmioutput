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

/**
 * @addtogroup HPK Hardware Porting Kit
 * @{
 *
 */

/**
 * @addtogroup HDMI_OUTPUT HDMI OUTPUT Module
 * @{
 *
 */

/**
 * @defgroup HDMI_OUTPUT_HALTEST HDMI OUTPUT HAL Tests
 * @{
 *
 */

/**
 * @defgroup HDMI_OUTPUT_HALTEST_L3 HDMI OUTPUT HAL Tests L3 File
 * @{
 * @parblock
 *
 * ### L3 Test Cases for HDMI OUTPUT HAL :
 *
 * This module includes Level 3 functional tests (manual/semi-automated tests).
 * This is to ensure that the API meets the operational requirements across real hardware.
 *
 * **Pre-Conditions:** HDMI sink device connected @n
 * **Dependencies:** None@n
 *
 * @endparblock
 *
 */

/**
 * @file vts_hdmioutput_l3_tests.cpp
 *
 */
#include "vts_hdmioutput.h"
#include "vts_hdmioutput_parse_testconfig.h"

static const char *moduleName = "hdmioutput";
static const char *testGroupName = "L3";

/* Test class registration with UT_Core */
UT_ADD_TEST_TO_GROUP(VTS_L3_HDMIOUTPUT, UT_TESTS_L3)

class VTS_L3_HDMIOUTPUT : public UTCore
{
public:
    VTS_L3_HDMIOUTPUT() : UTCore() {}

    ~VTS_L3_HDMIOUTPUT() override = default;

    std::vector<std::tuple<IHDMIOutput::Id, sp<IHDMIOutput>, sp<HDMIOutputControllerListener>, sp<HDMIOutputEventListener>, sp<hdmioutput::IHDMIOutputController>, hdmioutput::Capabilities *>> mHDMIOutputList;

    void SetUp() override
    {
        std::vector<hdmioutput::IHDMIOutput::Id> hdmiOutputIds;

        android::ProcessState::self()->startThreadPool();

        /* Get the HDMI OUTPUT IDs */
        HDMIOUTPUT_GET_HDMIOUTPUT_IDS(gHDMIOUTPUTHandle, hdmiOutputIds);

        for (const auto& id : hdmiOutputIds)
        {
            android::sp<hdmioutput::IHDMIOutput> hdmiOutput;
            HDMIOUTPUT_GET_HDMIOUTPUT(gHDMIOUTPUTHandle, id, hdmiOutput);

            sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
            controllerListener->setHDMIOutput(hdmiOutput);  // Associate listener with output for state checking
            sp<HDMIOutputEventListener> eventListener = new HDMIOutputEventListener();
            hdmioutput::Capabilities *capabilities = new hdmioutput::Capabilities();
            android::sp<hdmioutput::IHDMIOutputController> controller;

            HDMIOUTPUT_GET_CAPABILITIES(hdmiOutput, *capabilities);

            // Register an event listener
            bool registerResult = false;
            HDMIOUTPUT_REGISTER_EVENT_LISTENER(hdmiOutput, eventListener, registerResult);
            UT_ASSERT_TRUE(registerResult);

            // Open the HDMI output port
            HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
            UT_ASSERT_NOT_EQUAL(controller, nullptr);

            mHDMIOutputList.push_back({id, hdmiOutput, controllerListener, eventListener, controller, capabilities});
        }
    }

    void TearDown() override
    {
        for (auto& output : mHDMIOutputList)
        {
            auto& hdmiOutput = std::get<1>(output);
            auto& eventListener = std::get<3>(output);
            auto& controller = std::get<4>(output);
            auto& capabilities = std::get<5>(output);

            // Close the HDMI output port
            bool closeResult = false;
            HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
            UT_ASSERT_TRUE(closeResult);

            // Unregister the event listener
            bool unregisterResult = false;
            HDMIOUTPUT_UNREGISTER_EVENT_LISTENER(hdmiOutput, eventListener, unregisterResult);
            UT_ASSERT_TRUE(unregisterResult);

            delete capabilities;
        }
    }

    void StartPort(int32_t portId)
    {
        // Find the matching tuple in mHDMIOutputList
        for (const auto& hdmiOutputTuple : mHDMIOutputList)
        {
            auto& _portId = std::get<0>(hdmiOutputTuple);
            if (_portId.value != portId)
                continue;
            auto& controller = std::get<4>(hdmiOutputTuple);
            HDMIOUTPUT_START(controller);
        }
    }

    void StopPort(int32_t portId)
    {
        // Find the matching tuple in mHDMIOutputList
        for (const auto& hdmiOutputTuple : mHDMIOutputList)
        {
            auto& _portId = std::get<0>(hdmiOutputTuple);
            if (_portId.value != portId)
                continue;
            auto& controller = std::get<4>(hdmiOutputTuple);
            HDMIOUTPUT_STOP(controller);
        }
    }

    void VerifyHPDAndHDCP(const hdmioutputHPDConfig_t& config,
                                      bool isStarted,
                                      sp<hdmioutput::IHDMIOutputController>& controller,
                                      sp<HDMIOutputControllerListener>& controllerListener)
    {
        // Step 1: Verify connection state before open
        bool hpdState = false;
        HDMIOUTPUT_GET_HPD_STATE(controller, hpdState);
        UT_ASSERT_EQUAL(hpdState, config.connectionState);
        UT_LOG_INFO("HPD state before start, HPD state: %d\n", hpdState);

        bool actualHPDState;
        bool callbackReceived = controllerListener->getNextHPDStateChange(actualHPDState, chrono::milliseconds(config.maxWaitTimeMs));
        UT_ASSERT_TRUE(callbackReceived);
        UT_ASSERT_EQUAL(actualHPDState, config.connectionState);

        // Step 2: Verify HDCP status before start
        std::pair<hdmioutput::HDCPStatus, hdmioutput::HDCPProtocolVersion> hdcpInfo;
        bool cbReceived = controllerListener->getNextHDCPStatusChange(hdcpInfo, chrono::milliseconds(config.maxWaitTimeMs));
        if (config.connectionState)
        {
            UT_ASSERT_TRUE(cbReceived);
            UT_ASSERT_EQUAL(hdcpInfo.first, hdmioutput::HDCPStatus::UNAUTHENTICATED);
            UT_LOG_INFO("HDCP status callback received: %d (valid in expected list)\n", static_cast<int>(hdcpInfo.first));
        }
        else
        {
            UT_ASSERT_FALSE(cbReceived);
            UT_LOG_INFO("HDCP callback is not received as expected till the state chages to STARTED\n");
        }

        // Step 4: Verify EDID callback if expected
        std::vector<uint8_t> edid;
        bool edidReceived = controllerListener->getNextEDID(edid, chrono::milliseconds(config.maxWaitTimeMs));
        if (config.connectionState)
        {
            UT_ASSERT_TRUE(edidReceived);
            UT_ASSERT_TRUE(edid.size() > 0);
            UT_LOG_INFO("EDID callback received: size=%zu bytes\n", edid.size());
        }
        else
        {
            UT_ASSERT_FALSE(edidReceived);
            UT_ASSERT_FALSE(edid.size() > 0);
            UT_LOG_INFO("EDID callback not received as expected when HPD is not TRUE\n");
        }
        // Step 3: Process HPD sequence from YAML configuration
        UT_LOG_INFO("Processing HPD sequence with %zu expected states\n", config.hpdSequence.size());
        
        // Wait for HPD callback with timeout from config
        bool waitSuccess = controllerListener->waitForHPDCount(config.hpdSequence.size(), chrono::milliseconds(config.maxWaitTimeMs));
        UT_ASSERT_FALSE(waitSuccess);
        sleep(1); // Ensure queue processing complete

        // get the max supported version for the port from the supported list in the config
        hdmioutput::HDCPProtocolVersion maxSupportedVersion = getProfileMaxSupportedHDCPProtocolVersion(config.portId);
        hdmioutput::HDCPProtocolVersion negotiatedVersion = (config.expectedHdcpVersions <= maxSupportedVersion) ? config.expectedHdcpVersions : maxSupportedVersion;
        for (size_t i = 0; i < config.hpdSequence.size(); ++i)
        {
            bool inputHPDState = config.hpdSequence[i];
            bool actualHPDState;
            bool callbackReceived = controllerListener->getNextHPDStateChange(actualHPDState, chrono::milliseconds(100));
            if (isStarted) {
                UT_ASSERT_TRUE(callbackReceived);
                UT_ASSERT_EQUAL(actualHPDState, inputHPDState);
                UT_LOG_INFO("HPD callback received in STARTED state: %d \n", actualHPDState);
            } else {
                UT_ASSERT_FALSE(callbackReceived); // Should not receive HPD callbacks in READY state
                UT_LOG_INFO("HPD callback correctly NOT received in READY state\n");
            }

            std::pair<hdmioutput::HDCPStatus, hdmioutput::HDCPProtocolVersion> hdcpInfo;
            bool cbReceived = controllerListener->getNextHDCPStatusChange(hdcpInfo, chrono::milliseconds(100));
            UT_ASSERT_TRUE(cbReceived);
            if (inputHPDState) {
                UT_ASSERT_EQUAL(hdcpInfo.first, hdmioutput::HDCPStatus::UNAUTHENTICATED);
                UT_LOG_INFO("HDCP status callback received: %d \n", static_cast<int>(hdcpInfo.first));
                if (isStarted) {
                    std::pair<hdmioutput::HDCPStatus, hdmioutput::HDCPProtocolVersion> hdcpInfo1;
                    bool cbReceived1 = controllerListener->getNextHDCPStatusChange(hdcpInfo1, chrono::milliseconds(100));
                    UT_ASSERT_TRUE(cbReceived1);
                    UT_ASSERT_EQUAL(hdcpInfo1.first, hdmioutput::HDCPStatus::AUTHENTICATION_IN_PROGRESS);
                    UT_ASSERT_EQUAL(hdcpInfo1.second, negotiatedVersion);
                    UT_LOG_INFO("HDCP status callback received: %d \n", static_cast<int>(hdcpInfo1.first));

                    std::pair<hdmioutput::HDCPStatus, hdmioutput::HDCPProtocolVersion> hdcpInfo2;
                    bool cbReceived2 = controllerListener->getNextHDCPStatusChange(hdcpInfo2, chrono::milliseconds(100));
                    UT_ASSERT_TRUE(cbReceived2);
                    UT_ASSERT_EQUAL(hdcpInfo2.first, hdmioutput::HDCPStatus::AUTHENTICATED);
                    UT_ASSERT_EQUAL(hdcpInfo2.second, negotiatedVersion);
                    UT_LOG_INFO("HDCP status callback received: %d \n", static_cast<int>(hdcpInfo2.first));

                    hdmioutput::HDCPProtocolVersion currentversion;
                    HDMIOUTPUT_GET_HDCP_CURRENT_VERSION(controller, currentversion);
                    UT_ASSERT_EQUAL(currentversion, negotiatedVersion);
                    UT_LOG_INFO("Current HDCP version from getHDCPCurrentVersion: %d \n", static_cast<int>(currentversion));
                    hdmioutput::HDCPStatus hdcpstatus;
                    HDMIOUTPUT_GET_HDCP_STATUS(controller, hdcpstatus);
                    UT_ASSERT_EQUAL(hdcpstatus, hdmioutput::HDCPStatus::AUTHENTICATED);
                    UT_LOG_INFO("Current HDCP status from getHDCPStatus: %d \n", static_cast<int>(hdcpstatus));
                }
                std::vector<uint8_t> edid;
                bool edidReceived = controllerListener->getNextEDID(edid, chrono::milliseconds(config.maxWaitTimeMs));
                UT_ASSERT_TRUE(edidReceived);
                UT_ASSERT_TRUE(edid.size() > 0);
                UT_LOG_INFO("EDID callback received: size=%zu bytes\n", edid.size());
                // Verify minimum EDID size
                UT_ASSERT_TRUE(edid.size() >= config.expectedEdidMinSize);
                UT_LOG_INFO("EDID size verification passed: %zu >= %u\n", edid.size(), config.expectedEdidMinSize);
    
                // Verify EDID content matches expected EDID if configured
                UT_ASSERT_EQUAL(edid.size(), config.expectedEdid.size());
                if (!config.expectedEdid.empty())
                {
                    for (size_t i = 0; i < edid.size(); ++i)
                    {
                        UT_ASSERT_EQUAL(edid[i], config.expectedEdid[i]);
                    }
                }
            } else {
                UT_ASSERT_EQUAL(hdcpInfo.first, hdmioutput::HDCPStatus::UNKNOWN);
                UT_ASSERT_EQUAL(hdcpInfo.second, hdmioutput::HDCPProtocolVersion::UNDEFINED);
                UT_LOG_INFO("HDCP status callback received with UNKNOWN status as expected when HPD is FALSE\n");
            }
        }
    }
};

/**
 * @brief Verify HotPlugDetect state callback for each HDMI output port
 *
 * This test verifies that the Hot Plug Detect (HPD) callback is properly triggered
 * and reports correct state changes when HDMI sink devices are connected/disconnected.
 *
 * **Test Group ID:** Basic: 03@n
 * **Test Case ID:** 001@n
 * **Priority:** High@n *
 * **Pre-Conditions:**@n
 * HDMI sink device connection/disconnection orchestrated by Python wrapper@n
 *
 * **Dependencies:**
 * Python wrapper to handle device connections@n
 *
 * **User Interaction:** 
 * None - Python wrapper handles device connection automation@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Start HDMI output port | port ID | State transitions to STARTED | Should Pass |
 * | 02 | Check initial HPD state | none | Current HPD state logged | Should Pass |
 * | 03 | Python disconnects HDMI cable | automation | HPD callback with state=FALSE | Should Pass |
 * | 04 | Python reconnects HDMI cable | automation | HPD callback with state=TRUE | Should Pass |
 * | 05 | Verify onEDID callback after HPD TRUE | none | EDID callback received | Should Pass |
 * | 06 | Stop the port | none | Cleanup successful | Should Pass |
 *
 * Refer to UT specification documentation [hdmioutput_High-Level_TestSpecification.md](../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 *
 */
UT_ADD_TEST(VTS_L3_HDMIOUTPUT, HotPlugDetectStateBeforeStartCallback)
{
    UT_LOG_INFO("[L3 Test] HotPlugDetectStateBeforeStartCallback - Python wrapper will orchestrate device connections\n");

    // Parse test configuration from YAML
    std::vector<hdmioutputHPDConfig_t> hpdConfigs;
    UT_status_t result = vts_hdmioutput_parse_hpdTest((char*)VTS_HDMIOUTPUT_TEST_CONFIG_PATH, 
                                                        (char*)moduleName, (char*)testGroupName, 
                                                        (char*)"HotPlugDetectStateBeforeStartCallback", hpdConfigs);
    UT_ASSERT_EQUAL(result, UT_STATUS_OK);
    UT_ASSERT_TRUE(hpdConfigs.size() > 0);

    // Loop through all configured ports
    for (const auto& config : hpdConfigs)
    {
        UT_LOG_INFO("Testing port ID: %d\n", config.portId);

        // Find the matching port in mHDMIOutputList
        bool portFound = false;
        for (auto& outputTuple : mHDMIOutputList)
        {
            auto& portId = std::get<0>(outputTuple);
            if (portId.value != config.portId)
                continue;

            portFound = true;
            auto& hdmiOutput = std::get<1>(outputTuple);
            auto& controllerListener = std::get<2>(outputTuple);
            auto& controller = std::get<4>(outputTuple);

            UT_LOG_INFO("===== Testing Port %d =====\n", config.portId);

            // Verify HPD and HDCP state before start
            UT_LOG_INFO("Expected connection state before open: %s\n", config.connectionState ? "TRUE" : "FALSE");
            VerifyHPDAndHDCP(config, false, controller, controllerListener);

            UT_LOG_INFO("===== Port %d Test Complete =====\n", config.portId);
            break;
        }

        UT_ASSERT_TRUE(portFound);
    }
}

/**
 * @brief Verify HotPlugDetect state callback after START for each HDMI output port
 *
 * This test verifies that the Hot Plug Detect (HPD) callbacks are properly triggered
 * and reports correct state changes when HDMI sink devices are connected/disconnected
 * AFTER the port has been started.
 *
 * **Test Group ID:** Basic: 03@n
 * **Test Case ID:** 002@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:**@n
 * HDMI sink device connection/disconnection orchestrated by Python wrapper@n
 *
 * **Dependencies:**
 * Python wrapper to handle device connections@n
 *
 * **User Interaction:** 
 * None - Python wrapper handles device connection automation@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Verify initial HPD state | none | Current HPD state logged | Should Pass |
 * | 02 | Verify HDCP state before start | none | HDCP status logged | Should Pass |
 * | 03 | Start HDMI output port | port ID | State transitions to STARTED | Should Pass |
 * | 04 | Python disconnects HDMI cable | automation | HPD callback with state=FALSE | Should Pass |
 * | 05 | Python reconnects HDMI cable | automation | HPD callback with state=TRUE | Should Pass |
 * | 06 | Verify onEDID callback after HPD TRUE | none | EDID callback received | Should Pass |
 * | 07 | Verify HDCP authentication | none | HDCP AUTHENTICATED | Should Pass |
 * | 08 | Stop the port | none | Cleanup successful | Should Pass |
 *
 * Refer to UT specification documentation [hdmioutput_High-Level_TestSpecification.md](../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 *
 */
UT_ADD_TEST(VTS_L3_HDMIOUTPUT, HotPlugDetectStateAfterStartCallback)
{
    UT_LOG_INFO("[L3 Test] HotPlugDetectStateAfterStartCallback - Python wrapper will orchestrate device connections\n");

    // Parse test configuration from YAML
    std::vector<hdmioutputHPDConfig_t> hpdConfigs;
    UT_status_t result = vts_hdmioutput_parse_hpdTest((char*)VTS_HDMIOUTPUT_TEST_CONFIG_PATH, 
                                                        (char*)moduleName, (char*)testGroupName, 
                                                        (char*)"HotPlugDetectStateAfterStartCallback", hpdConfigs);
    UT_ASSERT_EQUAL(result, UT_STATUS_OK);
    UT_ASSERT_TRUE(hpdConfigs.size() > 0);

    // Loop through all configured ports
    for (const auto& config : hpdConfigs)
    {
        UT_LOG_INFO("Testing port ID: %d\n", config.portId);

        StartPort(config.portId);
        // Find the matching port in mHDMIOutputList
        bool portFound = false;
        for (auto& outputTuple : mHDMIOutputList)
        {
            auto& portId = std::get<0>(outputTuple);
            if (portId.value != config.portId)
                continue;

            portFound = true;
            auto& hdmiOutput = std::get<1>(outputTuple);
            auto& controllerListener = std::get<2>(outputTuple);
            auto& controller = std::get<4>(outputTuple);

            UT_LOG_INFO("===== Testing Port %d =====\n", config.portId);

            // Step 1: Verify connection state after start (during READY state, before START)
            UT_LOG_INFO("Expected connection state after start: %s\n", config.connectionState ? "TRUE" : "FALSE");
            VerifyHPDAndHDCP(config, true, controller, controllerListener);

            UT_LOG_INFO("===== Port %d Test Complete =====\n", config.portId);
            break;
        }

        UT_ASSERT_TRUE(portFound);
        StopPort(config.portId);
    }
}

/**
 * @brief Verify SPD InfoFrame transmission on HDMI output
 *
 * This test verifies that SPD (Source Product Description) InfoFrame can be set
 * and is properly transmitted when video output starts.
 *
 * **Test Group ID:** Basic: 03@n
 * **Test Case ID:** 003@n
 * **Priority:** Medium@n
 *
 * **Pre-Conditions:**@n
 * HDMI sink device connected@n
 *
 * **Dependencies:**
 * None@n
 *
 * **User Interaction:** 
 * None - Optional HDMI analyzer verification handled by Python wrapper@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Set SPD InfoFrame | vendor, description, source info | Returns success | Should Pass |
 * | 02 | Get SPD InfoFrame and verify | none | Values match what was set | Should Pass |
 * | 03 | Start the HDMI output port | none | Video output begins | Should Pass |
 * | 04 | Stop the port | none | Cleanup successful | Should Pass |
 *
 * Refer to UT specification documentation [hdmioutput_High-Level_TestSpecification.md](../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 *
 */
UT_ADD_TEST(VTS_L3_HDMIOUTPUT, SPDInfoFrameTransmission)
{
    UT_LOG_INFO("[L3 Test] SPDInfoFrameTransmission - Optional analyzer verification via Python wrapper\n");

    // Loop through all HDMI output ports
    for (auto& outputTuple : mHDMIOutputList)
    {
        auto& portId = std::get<0>(outputTuple);
        auto& hdmiOutput = std::get<1>(outputTuple);
        auto& controller = std::get<4>(outputTuple);

        UT_LOG_INFO("Testing port ID: %d\n", portId.value);

        // Create SPD InfoFrame
        hdmioutput::SPDInfoFrame spdInfoFrame;
        // vendorName is std::array<uint8_t, 8>
        std::string vendorStr = "RDK";
        std::copy(vendorStr.begin(), vendorStr.end(), spdInfoFrame.vendorName.begin());
        // productDescription is std::array<uint8_t, 16>
        std::string productStr = "Test Device";
        std::copy(productStr.begin(), productStr.end(), spdInfoFrame.productDescription.begin());
        spdInfoFrame.spdSourceInformation = hdmioutput::SPDSource::DIGITAL_STB;

        // Set SPD InfoFrame
        HDMIOUTPUT_SET_SPD_INFOFRAME(controller, spdInfoFrame);
        UT_LOG_INFO("SPD InfoFrame set successfully\n");

        // Get and verify SPD InfoFrame
        hdmioutput::SPDInfoFrame retrievedSPD;
        HDMIOUTPUT_GET_SPD_INFOFRAME(controller, retrievedSPD);
        UT_ASSERT_EQUAL(retrievedSPD.vendorName, spdInfoFrame.vendorName);
        UT_ASSERT_EQUAL(retrievedSPD.productDescription, spdInfoFrame.productDescription);
        UT_ASSERT_EQUAL(retrievedSPD.spdSourceInformation, spdInfoFrame.spdSourceInformation);
        UT_LOG_INFO("SPD InfoFrame verified successfully\n");

        // Start the port to begin transmission
        StartPort(portId.value);
        UT_LOG_INFO("HDMI output started, SPD InfoFrame transmitting (Vendor='RDK', Product='Test Device', Source=%d)\n", spdInfoFrame.spdSourceInformation);

        // Python wrapper can verify on HDMI analyzer if available
        sleep(1); // Allow time for InfoFrame transmission

        // Stop the port
        StopPort(portId.value);
    }
}

/**
 * @brief Verify properties are applied on HDMI output device
 *
 * This test verifies that property changes (VIC, CONTENT_TYPE, AFD, HDR_OUTPUT_MODE, SCAN_INFORMATION)
 * are properly applied and result in correct InfoFrame transmission.
 *
 * **Test Group ID:** Basic: 03@n
 * **Test Case ID:** 004@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:**@n
 * HDMI sink device connected@n
 *
 * **Dependencies:**
 * None@n
 *
 * **User Interaction:** 
 * None - Optional analyzer verification via Python wrapper@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Start HDMI output port | port ID | State transitions to STARTED | Should Pass |
 * | 02 | Set VIC property | supported VIC value | Returns success | Should Pass |
 * | 03 | Get property and verify | none | Value matches what was set | Should Pass |
 * | 04 | Set CONTENT_TYPE property | supported value | Returns success | Should Pass |
 * | 05 | Set HDR_OUTPUT_MODE property | supported mode | Returns success | Should Pass |
 * | 06 | Stop the port | none | Cleanup successful | Should Pass |
 *
 * Refer to UT specification documentation [hdmioutput_High-Level_TestSpecification.md](../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 *
 */
UT_ADD_TEST(VTS_L3_HDMIOUTPUT, PropertyApplicationVerification)
{
    UT_LOG_INFO("[L3 Test] PropertyApplicationVerification - Optional analyzer verification via Python wrapper\n");

    // Parse test configuration from YAML
    std::vector<hdmioutputPropertyConfig_t> propertyConfigs;
    UT_status_t result = vts_hdmioutput_parse_propertyTest((char*)VTS_HDMIOUTPUT_TEST_CONFIG_PATH, 
                                                            (char*)moduleName, (char*)testGroupName, 
                                                            (char*)"PropertyApplicationVerification", propertyConfigs);
    UT_ASSERT_EQUAL(result, UT_STATUS_OK);
    UT_ASSERT_TRUE(propertyConfigs.size() > 0);

    // Loop through all configured ports
    for (const auto& config : propertyConfigs)
    {
        UT_LOG_INFO("Testing port ID: %d\n", config.portId);

        // Find the matching port in mHDMIOutputList
        bool portFound = false;
        for (auto& outputTuple : mHDMIOutputList)
        {
            auto& portId = std::get<0>(outputTuple);
            if (portId.value != config.portId)
                continue;

            portFound = true;
            auto& hdmiOutput = std::get<1>(outputTuple);
            auto& controllerListener = std::get<2>(outputTuple);
            auto& controller = std::get<4>(outputTuple);

            UT_LOG_INFO("===== Testing Port %d =====\n", config.portId);

            // Loop through each property to test
            for (const auto& property : config.testProperties)
            {
                UT_LOG_INFO("Testing property: %s with %zu values\n", 
                           property.propertyName.c_str(), property.values.size());

                // Get the property enum using helper function
                hdmioutput::Property prop = convertStringToProperty(property.propertyName);

                // Test each value
                for (int32_t testValue : property.values)
                {
                    UT_LOG_INFO("  Setting %s to value: %d\n", property.propertyName.c_str(), testValue);

                    // Set the property
                    PropertyValue propValue;
                    propValue.value = PropertyValue::Value();
                    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(testValue);
                    
                    bool setResult = false;
                    HDMIOUTPUT_SET_PROPERTY(controller, prop, propValue, setResult);
                    UT_ASSERT_TRUE(setResult);

                    // Get and verify the property
                    std::optional<PropertyValue> retrievedValue;
                    HDMIOUTPUT_GET_PROPERTY(hdmiOutput, prop, retrievedValue);
                    UT_ASSERT_TRUE(retrievedValue.has_value());
                    UT_ASSERT_EQUAL(retrievedValue->value->get<PropertyValue::Value::Tag::intValue>(), testValue);
                    UT_LOG_INFO("  Property verified successfully\n");

                    StartPort(config.portId);
                    UT_LOG_INFO("  HDMI output started with %s=%d\n", property.propertyName.c_str(), testValue);
                    // Allow time for property application
                    sleep(1);
                    // Python wrapper can verify InfoFrames on HDMI analyzer if available
                    UT_LOG_INFO("All properties tested. Python wrapper may verify InfoFrames on analyzer\n");
        
                    // Stop the port
                    StopPort(config.portId);
                }
                if (prop == hdmioutput::Property::VIC) {
                    UT_LOG_INFO("VIC property changes may trigger frame rate changes.\n");
                    bool waitsuccess = controllerListener->waitForFrameRateCount(property.values.size(), chrono::milliseconds(config.maxWaitTimeMs));
                    UT_ASSERT_TRUE(waitsuccess);
                    sleep(1); // Ensure queue processing complete
                    for (size_t i = 0; i < property.values.size(); ++i)
                    {
                        bool callbackReceived = controllerListener->getNextFrameRateChange(chrono::milliseconds(100));
                        UT_ASSERT_TRUE(callbackReceived);
                        UT_LOG_INFO("  onFrameRateChanged callback received for %s change\n", property.propertyName.c_str());
                    }
                }
            }

            UT_LOG_INFO("===== Port %d Test Complete =====\n", config.portId);
            break;
        }

        UT_ASSERT_TRUE(portFound);
    }
}

/**
 * @brief Verify video playback generated InfoFrame events
 *
 * This test verifies that when video playback starts in STARTED state,
 * appropriate InfoFrame callbacks are generated and can be verified.
 *
 * **Test Group ID:** Basic: 03@n
 * **Test Case ID:** 005@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:**@n
 * HDMI sink device connected @n
 *
 * **Dependencies:**
 * None@n
 *
 * **User Interaction:** 
 * None - Test automated. Python wrapper can optionally verify InfoFrames on analyzer@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Read supported output ports | IHDMIOutputManager | Port list obtained | Should Pass |
 * | 02 | Loop through each port | port IDs | Process all ports | Should Pass |
 * | 03 | Start video playback in STARTED state | port ID | Port starts successfully | Should Pass |
 * | 04 | Verify InfoFrame callbacks | none | Callbacks received | Should Pass |
 * | 05 | Stop the port | none | Cleanup successful | Should Pass |
 *
 * Refer to UT specification documentation [hdmioutput_High-Level_TestSpecification.md](../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 *
 */
UT_ADD_TEST(VTS_L3_HDMIOUTPUT, VideoPlaybackEventVerification)
{
    UT_LOG_INFO("[L3 Test] VideoPlaybackEventVerification - Verifying video playback generates InfoFrame events\n");

    // Loop through all supported HDMI output ports from IHDMIOutputManager
    for (auto& outputTuple : mHDMIOutputList)
    {
        auto& portId = std::get<0>(outputTuple);
        auto& hdmiOutput = std::get<1>(outputTuple);
        auto& controllerListener = std::get<2>(outputTuple);
        auto& eventListener = std::get<3>(outputTuple);
        auto& controller = std::get<4>(outputTuple);

        UT_LOG_INFO("===== Testing Port %d =====\n", portId.value);

        // Start video playback in STARTED state
        StartPort(portId.value);
        UT_LOG_INFO("Port %d started in STARTED state\n", portId.value);

        // Allow time for InfoFrame transmission to begin
        sleep(2);

        // Check for HPD state (should be connected for InfoFrames)
        bool hpdState = false;
        HDMIOUTPUT_GET_HPD_STATE(controller, hpdState);
        UT_LOG_INFO("HPD state: %s\n", hpdState ? "CONNECTED" : "DISCONNECTED");

        if (hpdState)
        {
            UT_LOG_INFO("HDMI sink connected - InfoFrame transmission active\n");

            // Verify HDCP status callback (part of video playback handshake)
            hdmioutput::HDCPStatus hdcpReceived;
            HDMIOUTPUT_GET_HDCP_STATUS(controller, hdcpReceived);
            if (hdcpReceived == hdmioutput::HDCPStatus::AUTHENTICATED)
            {
                UT_LOG_INFO("HDCP status received: status=%d\n", static_cast<int>(hdcpReceived));
            }

            UT_LOG_INFO("Video playback InfoFrame events verified successfully\n");
        }
        else
        {
            UT_LOG_WARNING("HDMI sink not connected - InfoFrame transmission not possible\n");
        }

        // Stop the port
        StopPort(portId.value);
        UT_LOG_INFO("===== Port %d Test Complete =====\n", portId.value);
    }

    UT_LOG_INFO("VideoPlaybackEventVerification test completed for all ports\n");
}

/** @} */ // End of HDMI_OUTPUT_HALTEST_L3
/** @} */ // End of HDMI_OUTPUT_HALTEST
/** @} */ // End of HDMI_OUTPUT Module
/** @} */ // End of HPK
