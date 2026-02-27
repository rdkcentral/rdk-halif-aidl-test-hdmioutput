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
 * @defgroup HDMIOUTPUT_HALTEST_L2 HDMIOUTPUT HAL Tests L2 Test Cases
 * @{
 * @parblock
 *
 * ### L2 Tests for HDMIOUTPUT HAL :
 *
 * L2 test cases validate state transitions, event callbacks, property operations,
 * and advanced HDMI output functionality.
 *
 * **Important:** All L2 tests are designed to work WITHOUT external HDMI devices connected.
 * L2 tests validate HAL API functionality and internal state management independently
 * of physical device connectivity. L3 tests will validate behavior with actual HDMI devices.
 *
 * **Pre-Conditions:**  HDMI Output service must be running @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
 * @file vts_hdmioutput_L2_tests.cpp
 *
 */

#include "vts_hdmioutput.h"
#include <thread>
#include "com/rdk/hal/PropertyValue.h"

extern sp<IHDMIOutputManager> gHDMIOUTPUTHandle;

/* Test class registration with UT_Core */
UT_ADD_TEST_TO_GROUP(VTS_L2_HDMIOUTPUT, UT_TESTS_L2)

class VTS_L2_HDMIOUTPUT : public UTCore
{
public:
    VTS_L2_HDMIOUTPUT() : UTCore() {}

    ~VTS_L2_HDMIOUTPUT() override = default;

    void SetUp() override
    {
        android::ProcessState::self()->startThreadPool();
    }

    void TearDown() override
    {
    }
};

/**
 * @brief Test L2-01: Validate state machine transitions
 *
 * This test validates the state transitions of HDMI output during open, start, stop, and close operations.
 * It verifies that the state machine transitions correctly and that state change callbacks are fired.
 *
 * **Test Group ID:** 02@n
 * **Test Case ID:** 001@n
 *
 * **Pre-conditions:** No external HDMI device required. Test should work without any device connected.
 *
 * **Test Procedure:**
 * Refer to HDMI Output specification documentation [hdmioutput_High-Level_TestSpecification](../../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L2_HDMIOUTPUT, StateTransitions)
{
    UT_LOG_INFO("Starting Test: L2-01 State Machine Transitions");
    
    // Get count of HDMI output resources
    uint32_t resourceCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources");
    UT_ASSERT_TRUE(resourceCount > 0);
    UT_LOG_INFO("Number of HDMI output resources: %u", resourceCount);
    
    // Loop through all HDMI output resources
    for (uint32_t i = 0; i < resourceCount; i++) {
        UT_LOG_INFO("Testing HDMI output resource %u", i);
        
        // Get HDMI output instance
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(i);
        UT_ASSERT_NOT_NULL(hdmiOutput.get());
    
        // Create listeners
        android::sp<HDMIOutputEventListener> eventListener = new HDMIOutputEventListener();
        android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
        
        // Register event listener
        bool registerResult = false;
        HDMIOUTPUT_REGISTER_EVENT_LISTENER(hdmiOutput, eventListener, registerResult);
        UT_ASSERT_TRUE(registerResult);
        UT_LOG_INFO("Event listener registered successfully");
        
        // Reset event listener
        eventListener->reset();
        
        // Test 1: CLOSED → READY (during open)
        UT_LOG_INFO("Test Step 1: Opening HDMI output (CLOSED → READY)");
        ::com::rdk::hal::hdmioutput::State initialState;
        HDMIOUTPUT_GET_STATE(hdmiOutput, initialState);
        UT_ASSERT_EQUAL(initialState, ::com::rdk::hal::hdmioutput::State::CLOSED);
        
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutputController> controller;
        HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
        UT_ASSERT_NOT_NULL(controller.get());
        
        // Verify state is READY
        ::com::rdk::hal::hdmioutput::State currentState;
        HDMIOUTPUT_GET_STATE(hdmiOutput, currentState);
        UT_ASSERT_EQUAL(currentState, ::com::rdk::hal::hdmioutput::State::READY);
        UT_LOG_INFO("State after open: READY");
        
        // Verify state change callback was fired
        UT_ASSERT_TRUE(eventListener->hasStateChanged());
        std::pair<::com::rdk::hal::hdmioutput::State, ::com::rdk::hal::hdmioutput::State> stateChanges;
        UT_ASSERT_TRUE(eventListener->getNextStateChanges(stateChanges));
        UT_ASSERT_EQUAL(stateChanges.first, ::com::rdk::hal::hdmioutput::State::CLOSED);
        UT_ASSERT_EQUAL(stateChanges.second, ::com::rdk::hal::hdmioutput::State::OPENING);
        UT_ASSERT_TRUE(eventListener->getNextStateChanges(stateChanges));
        UT_ASSERT_EQUAL(stateChanges.first, ::com::rdk::hal::hdmioutput::State::OPENING);
        UT_ASSERT_EQUAL(stateChanges.second, ::com::rdk::hal::hdmioutput::State::READY);
        UT_LOG_INFO("State change callback verified: CLOSED → READY");
        
        // Test 2: READY → STARTED (during start)
        UT_LOG_INFO("Test Step 2: Starting HDMI output (READY → STARTED)");
        eventListener->reset();
        
        HDMIOUTPUT_START(controller);
        
        // Wait for state to become STARTED
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        HDMIOUTPUT_GET_STATE(hdmiOutput, currentState);
        UT_ASSERT_EQUAL(currentState, ::com::rdk::hal::hdmioutput::State::STARTED);
        UT_LOG_INFO("State after start: STARTED");
        
        // Verify state change callback
        UT_ASSERT_TRUE(eventListener->hasStateChanged());
        UT_ASSERT_TRUE(eventListener->getNextStateChanges(stateChanges));
        UT_ASSERT_EQUAL(stateChanges.first, ::com::rdk::hal::hdmioutput::State::READY);
        UT_ASSERT_EQUAL(stateChanges.second, ::com::rdk::hal::hdmioutput::State::STARTING);
        UT_ASSERT_TRUE(eventListener->getNextStateChanges(stateChanges));
        UT_ASSERT_EQUAL(stateChanges.first, ::com::rdk::hal::hdmioutput::State::STARTING);
        UT_ASSERT_EQUAL(stateChanges.second, ::com::rdk::hal::hdmioutput::State::STARTED);
        UT_LOG_INFO("State change callback verified: READY → STARTED");
        
        // Test 3: STARTED → READY (during stop)
        UT_LOG_INFO("Test Step 3: Stopping HDMI output (STARTED → READY)");
        eventListener->reset();
        
        HDMIOUTPUT_STOP(controller);
        
        // Wait for state to become READY
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        HDMIOUTPUT_GET_STATE(hdmiOutput, currentState);
        UT_ASSERT_EQUAL(currentState, ::com::rdk::hal::hdmioutput::State::READY);
        UT_LOG_INFO("State after stop: READY");
        
        // Verify state change callback
        UT_ASSERT_TRUE(eventListener->hasStateChanged());
        UT_ASSERT_TRUE(eventListener->getNextStateChanges(stateChanges));
        UT_ASSERT_EQUAL(stateChanges.first, ::com::rdk::hal::hdmioutput::State::STARTED);
        UT_ASSERT_EQUAL(stateChanges.second, ::com::rdk::hal::hdmioutput::State::STOPPING);
        UT_ASSERT_TRUE(eventListener->getNextStateChanges(stateChanges));
        UT_ASSERT_EQUAL(stateChanges.first, ::com::rdk::hal::hdmioutput::State::STOPPING);
        UT_ASSERT_EQUAL(stateChanges.second, ::com::rdk::hal::hdmioutput::State::READY);
        UT_LOG_INFO("State change callback verified: STARTED → READY");
        
        // Test 4: READY → CLOSED (during close)
        UT_LOG_INFO("Test Step 4: Closing HDMI output (READY → CLOSED)");
        eventListener->reset();
        
        bool closeResult = false;
        HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
        UT_ASSERT_TRUE(closeResult);
        
        // Wait for state to become CLOSED
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        HDMIOUTPUT_GET_STATE(hdmiOutput, currentState);
        UT_ASSERT_EQUAL(currentState, ::com::rdk::hal::hdmioutput::State::CLOSED);
        UT_LOG_INFO("State after close: CLOSED");
        
        // Verify state change callback
        UT_ASSERT_TRUE(eventListener->hasStateChanged());
        UT_ASSERT_TRUE(eventListener->getNextStateChanges(stateChanges));
        UT_ASSERT_EQUAL(stateChanges.first, ::com::rdk::hal::hdmioutput::State::READY);
        UT_ASSERT_EQUAL(stateChanges.second, ::com::rdk::hal::hdmioutput::State::CLOSING);
        UT_ASSERT_TRUE(eventListener->getNextStateChanges(stateChanges));
        UT_ASSERT_EQUAL(stateChanges.first, ::com::rdk::hal::hdmioutput::State::CLOSING);
        UT_ASSERT_EQUAL(stateChanges.second, ::com::rdk::hal::hdmioutput::State::CLOSED);
        UT_LOG_INFO("State change callback verified: READY → CLOSED");
    
        // Unregister event listener
        bool unregisterResult = false;
        HDMIOUTPUT_UNREGISTER_EVENT_LISTENER(hdmiOutput, eventListener, unregisterResult);
        UT_LOG_INFO("Event listener unregistered successfully");
        
        UT_LOG_INFO("Completed testing HDMI output resource %u", i);
    }
    
    UT_LOG_INFO("Test L2-01 State Machine Transitions: PASSED");
    return;
}

/**
 * @brief Test L2-02: Verify HPD state when no device is connected
 *
 * This test verifies the Hot Plug Detect (HPD) state when no HDMI device is connected.
 * It expects HPD to be FALSE and no EDID callback.
 *
 * **Test Group ID:** 02@n
 * **Test Case ID:** 002@n
 *
 * **Pre-conditions:** No external HDMI device should be connected for this test.
 *
 * **Test Procedure:**
 * Refer to HDMI Output specification documentation [hdmioutput_High-Level_TestSpecification](../../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L2_HDMIOUTPUT, HPDStateTransition)
{
    UT_LOG_INFO("Starting Test: L2-02 HPD State Verification (No Device)");
    
    // Get count of HDMI output resources
    uint32_t resourceCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources");
    UT_ASSERT_TRUE(resourceCount > 0);
    UT_LOG_INFO("Number of HDMI output resources: %u", resourceCount);
    
    // Loop through all HDMI output resources
    for (uint32_t i = 0; i < resourceCount; i++) {
        UT_LOG_INFO("Testing HDMI output resource %u", i);
        
        // Get HDMI output instance
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(i);
        UT_ASSERT_NOT_NULL(hdmiOutput.get());
    
        // Create controller listener
        android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
        controllerListener->reset();
        
        // Open HDMI output
        UT_LOG_INFO("Test Step 1: Opening HDMI output");
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutputController> controller;
        HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
        UT_ASSERT_NOT_NULL(controller.get());
        
        // Wait a bit for any callbacks
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        // Get HPD state
        UT_LOG_INFO("Test Step 2: Getting HPD state");
        bool hpdState = true;
        HDMIOUTPUT_GET_HPD_STATE(controller, hpdState);
        UT_ASSERT_FALSE(hpdState);
        UT_LOG_INFO("HPD state: FALSE (as expected - no device connected)");
        
        // Verify HPD callback received and reports FALSE
        UT_LOG_INFO("Test Step 3: Verifying HPD callback");
        UT_ASSERT_TRUE(controllerListener->hasHPDStateChanged());
        if (controllerListener->hasHPDStateChanged()) {
            bool callbackHPDState;
            UT_ASSERT_TRUE(controllerListener->getNextHPDStateChange(callbackHPDState));
            UT_ASSERT_FALSE(callbackHPDState);
            UT_LOG_INFO("HPD callback received with state: FALSE");
        } else {
            UT_LOG_INFO("No HPD callback received (acceptable if no state change occurred)");
        }
        
        // Verify no EDID callback received
        UT_LOG_INFO("Test Step 4: Verifying no EDID callback");
        UT_ASSERT_FALSE(controllerListener->hasEDIDReceived());
        UT_LOG_INFO("No EDID callback received (as expected - no device connected)");
        
        // Close HDMI output
        UT_LOG_INFO("Test Step 5: Closing HDMI output");
        bool closeResult = false;
        HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
        UT_ASSERT_TRUE(closeResult);
        
        UT_LOG_INFO("Completed testing HDMI output resource %u", i);
    }
    
    UT_LOG_INFO("Test L2-02 HPD State Verification: PASSED");
    return;
}

/**
 * @brief Test L2-03: Validate individual property set/get operations
 *
 * This test validates setting and getting individual HDMI output properties.
 * It loops through all properties and tests all supported values from profile.
 *
 * **Test Group ID:** 02@n
 * **Test Case ID:** 003@n
 *
 * **Pre-conditions:** No external HDMI device required. Properties can be set/get without device connected.
 *
 * **Test Procedure:**
 * Refer to HDMI Output specification documentation [hdmioutput_High-Level_TestSpecification](../../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L2_HDMIOUTPUT, SetGetIndividualProperty)
{
    UT_LOG_INFO("Starting Test: L2-03 Individual Property Validation");
    
    // Get count of HDMI output resources
    uint32_t resourceCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources");
    UT_ASSERT_TRUE(resourceCount > 0);
    UT_LOG_INFO("Number of HDMI output resources: %u", resourceCount);
    
    // Loop through all HDMI output resources
    for (uint32_t i = 0; i < resourceCount; i++) {
        UT_LOG_INFO("Testing HDMI output resource %u", i);
        
        // Get HDMI output instance
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(i);
        UT_ASSERT_NOT_NULL(hdmiOutput.get());
    
        // Create controller listener
        android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
        
        // Open HDMI output
        UT_LOG_INFO("Opening HDMI output resource %u", i);
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutputController> controller;
        HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
        UT_ASSERT_NOT_NULL(controller.get());
        
        // Start HDMI output
        UT_LOG_INFO("Starting HDMI output resource %u", i);
        HDMIOUTPUT_START(controller);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Get all properties from Property enum
        constexpr auto allProperties = android::internal::enum_values<::com::rdk::hal::hdmioutput::Property>;
        
        // Loop through all properties from enum
        for (const auto& property : allProperties) {
            // Skip RESOURCE_ID and METRIC_xxx as they are not settable properties
            if (property == ::com::rdk::hal::hdmioutput::Property::RESOURCE_ID ||
                property == ::com::rdk::hal::hdmioutput::Property::METRIC_xxx) {
                continue;
            }
            
            int valueCount = 0;
            std::string propertyName = ::com::rdk::hal::hdmioutput::toString(property);
            
            // Get count of supported values based on property
            switch (property) {
                case ::com::rdk::hal::hdmioutput::Property::VIC:
                    valueCount = getProfileSupportedVICCount(i);
                    break;
                case ::com::rdk::hal::hdmioutput::Property::CONTENT_TYPE:
                    valueCount = getProfileSupportedContentTypeCount(i);
                    break;
                case ::com::rdk::hal::hdmioutput::Property::AFD:
                    valueCount = UT_KVP_PROFILE_GET_LIST_COUNT(("hdmioutput/resources/" + std::to_string(i) + "/supportedAFD").c_str());
                    break;
                case ::com::rdk::hal::hdmioutput::Property::HDR_OUTPUT_MODE:
                    valueCount = getProfileSupportedHDRModeCount(i);
                    break;
                case ::com::rdk::hal::hdmioutput::Property::SCAN_INFORMATION:
                    valueCount = UT_KVP_PROFILE_GET_LIST_COUNT(("hdmioutput/resources/" + std::to_string(i) + "/supportedScanInformation").c_str());
                    break;
                default:
                    continue; // Skip any unknown properties
            }
            
            UT_LOG_INFO("Testing property: %s (count: %d)", propertyName.c_str(), valueCount);
            UT_ASSERT_TRUE(valueCount > 0);
            
            // Loop through all supported values for this property
            for (int valueIdx = 0; valueIdx < valueCount; valueIdx++) {
                ::com::rdk::hal::PropertyValue propValue;
                propValue.value = ::com::rdk::hal::PropertyValue::Value();
                
                // Get value from profile based on property type
                switch (property) {
                    case ::com::rdk::hal::hdmioutput::Property::VIC: {
                        int32_t vic = getProfileSupportedVIC(i, valueIdx);
                        propValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(vic);
                        UT_LOG_INFO("  Testing %s value[%d]: %d", propertyName.c_str(), valueIdx, vic);
                        break;
                    }
                    case ::com::rdk::hal::hdmioutput::Property::CONTENT_TYPE: {
                        int32_t contentType = getProfileSupportedContentType(i, valueIdx);
                        propValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(contentType);
                        UT_LOG_INFO("  Testing %s value[%d]: %d", propertyName.c_str(), valueIdx, contentType);
                        break;
                    }
                    case ::com::rdk::hal::hdmioutput::Property::AFD: {
                        int32_t afd = getProfileSupportedAFD(i, valueIdx);
                        propValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(afd);
                        UT_LOG_INFO("  Testing %s value[%d]: %d", propertyName.c_str(), valueIdx, afd);
                        break;
                    }
                    case ::com::rdk::hal::hdmioutput::Property::HDR_OUTPUT_MODE: {
                        std::string hdrModeStr = getProfileSupportedHDRMode(i, valueIdx);
                        HDROutputMode hdrMode = convertStringToHDROutputMode(hdrModeStr);
                        propValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(static_cast<int32_t>(hdrMode));
                        UT_LOG_INFO("  Testing %s value[%d]: %s", propertyName.c_str(), valueIdx, hdrModeStr.c_str());
                        break;
                    }
                    case ::com::rdk::hal::hdmioutput::Property::SCAN_INFORMATION: {
                        int32_t scanInfo = getProfileSupportedScanInfo(i, valueIdx);
                        propValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(scanInfo);
                        UT_LOG_INFO("  Testing %s value[%d]: %d", propertyName.c_str(), valueIdx, scanInfo);
                        break;
                    }
                }
                
                // Set property
                bool setResult = false;
                HDMIOUTPUT_SET_PROPERTY(controller, property, propValue, setResult);
                UT_ASSERT_TRUE(setResult);
                
                // Get property and verify
                ::com::rdk::hal::PropertyValue retrievedValue;
                HDMIOUTPUT_GET_PROPERTY(hdmiOutput, property, retrievedValue);
                UT_ASSERT_TRUE(retrievedValue.value.has_value());
                
                int32_t setValue = propValue.value.value().get<::com::rdk::hal::PropertyValue::Value::Tag::intValue>();
                int32_t retrievedIntValue = retrievedValue.value.value().get<::com::rdk::hal::PropertyValue::Value::Tag::intValue>();
                UT_ASSERT_EQUAL(retrievedIntValue, setValue);
                
                UT_LOG_INFO("  %s value[%d] validated successfully", propertyName.c_str(), valueIdx);
            }
            
            UT_LOG_INFO("Completed testing property: %s", propertyName.c_str());
        }
        
        // Stop and close
        UT_LOG_INFO("Stopping and closing HDMI output resource %u", i);
        HDMIOUTPUT_STOP(controller);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        bool closeResult = false;
        HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
        UT_ASSERT_TRUE(closeResult);
        
        UT_LOG_INFO("Completed testing HDMI output resource %u", i);
    }
    
    UT_LOG_INFO("Test L2-03 Individual Property Validation: PASSED");
    return;
}

/**
 * @brief Test L2-04: Validate multi-property set/get operations
 *
 * This test validates setting multiple properties simultaneously using setPropertyMulti.
 *
 * **Test Group ID:** 02@n
 * **Test Case ID:** 004@n
 *
 * **Pre-conditions:** No external HDMI device required. Properties can be set/get without device connected.
 *
 * **Test Procedure:**
 * Refer to HDMI Output specification documentation [hdmioutput_High-Level_TestSpecification](../../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L2_HDMIOUTPUT, SetGetMultiProperties)
{
    UT_LOG_INFO("Starting Test: L2-04 Multi-Property Validation");
    
    // Get count of HDMI output resources
    uint32_t resourceCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources");
    UT_ASSERT_TRUE(resourceCount > 0);
    UT_LOG_INFO("Number of HDMI output resources: %u", resourceCount);
    
    // Loop through all HDMI output resources
    for (uint32_t i = 0; i < resourceCount; i++) {
        UT_LOG_INFO("Testing HDMI output resource %u", i);
        
        // Get HDMI output instance
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(i);
        UT_ASSERT_NOT_NULL(hdmiOutput.get());
        
        // Create controller listener
        android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
        
        // Open HDMI output
        UT_LOG_INFO("Test Step 1: Opening HDMI output");
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutputController> controller;
        HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
        UT_ASSERT_NOT_NULL(controller.get());
        
        // Start HDMI output
        UT_LOG_INFO("Test Step 2: Starting HDMI output");
        HDMIOUTPUT_START(controller);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Get all properties from Property enum
        constexpr auto allProperties = android::internal::enum_values<::com::rdk::hal::hdmioutput::Property>;
        
        // Build list of settable properties with their value counts
        struct PropertyInfo {
            ::com::rdk::hal::hdmioutput::Property property;
            int valueCount;
            std::string name;
        };
        std::vector<PropertyInfo> settableProperties;
        
        for (const auto& property : allProperties) {
            // Skip RESOURCE_ID and METRIC_xxx as they are not settable properties
            if (property == ::com::rdk::hal::hdmioutput::Property::RESOURCE_ID ||
                property == ::com::rdk::hal::hdmioutput::Property::METRIC_xxx) {
                continue;
            }
            
            int valueCount = 0;
            switch (property) {
                case ::com::rdk::hal::hdmioutput::Property::VIC:
                    valueCount = getProfileSupportedVICCount(i);
                    break;
                case ::com::rdk::hal::hdmioutput::Property::CONTENT_TYPE:
                    valueCount = getProfileSupportedContentTypeCount(i);
                    break;
                case ::com::rdk::hal::hdmioutput::Property::AFD:
                    valueCount = UT_KVP_PROFILE_GET_LIST_COUNT(("hdmioutput/resources/" + std::to_string(i) + "/supportedAFD").c_str());
                    break;
                case ::com::rdk::hal::hdmioutput::Property::HDR_OUTPUT_MODE:
                    valueCount = getProfileSupportedHDRModeCount(i);
                    break;
                case ::com::rdk::hal::hdmioutput::Property::SCAN_INFORMATION:
                    valueCount = UT_KVP_PROFILE_GET_LIST_COUNT(("hdmioutput/resources/" + std::to_string(i) + "/supportedScanInformation").c_str());
                    break;
                default:
                    continue;
            }
            
            if (valueCount > 0) {
                settableProperties.push_back({property, valueCount, ::com::rdk::hal::hdmioutput::toString(property)});
            }
        }
        
        UT_ASSERT_TRUE(settableProperties.size() > 0);
        UT_LOG_INFO("Found %zu settable properties", settableProperties.size());
        
        // Find minimum value count across all properties to determine test iterations
        int minValueCount = settableProperties[0].valueCount;
        for (const auto& propInfo : settableProperties) {
            if (propInfo.valueCount < minValueCount) {
                minValueCount = propInfo.valueCount;
            }
        }
        UT_LOG_INFO("Testing %d value combinations", minValueCount);
        
        // Test multiple value combinations
        for (int valueIdx = 0; valueIdx < minValueCount; valueIdx++) {
            UT_LOG_INFO("Test Step 3.%d: Setting multiple properties with value index %d", valueIdx, valueIdx);
            
            std::vector<::com::rdk::hal::hdmioutput::PropertyKVPair> propertyList;
            
            // Build property list with values at current index
            for (const auto& propInfo : settableProperties) {
                ::com::rdk::hal::hdmioutput::PropertyKVPair kvPair;
                kvPair.property = propInfo.property;
                kvPair.propertyValue.value = ::com::rdk::hal::PropertyValue::Value();
                
                // Get value from profile based on property type
                switch (propInfo.property) {
                    case ::com::rdk::hal::hdmioutput::Property::VIC: {
                        int32_t vic = getProfileSupportedVIC(i, valueIdx);
                        kvPair.propertyValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(vic);
                        UT_LOG_INFO("  %s value[%d]: %d", propInfo.name.c_str(), valueIdx, vic);
                        break;
                    }
                    case ::com::rdk::hal::hdmioutput::Property::CONTENT_TYPE: {
                        int32_t contentType = getProfileSupportedContentType(i, valueIdx);
                        kvPair.propertyValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(contentType);
                        UT_LOG_INFO("  %s value[%d]: %d", propInfo.name.c_str(), valueIdx, contentType);
                        break;
                    }
                    case ::com::rdk::hal::hdmioutput::Property::AFD: {
                        int32_t afd = getProfileSupportedAFD(i, valueIdx);
                        kvPair.propertyValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(afd);
                        UT_LOG_INFO("  %s value[%d]: %d", propInfo.name.c_str(), valueIdx, afd);
                        break;
                    }
                    case ::com::rdk::hal::hdmioutput::Property::HDR_OUTPUT_MODE: {
                        std::string hdrModeStr = getProfileSupportedHDRMode(i, valueIdx);
                        HDROutputMode hdrMode = convertStringToHDROutputMode(hdrModeStr);
                        kvPair.propertyValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(static_cast<int32_t>(hdrMode));
                        UT_LOG_INFO("  %s value[%d]: %s", propInfo.name.c_str(), valueIdx, hdrModeStr.c_str());
                        break;
                    }
                    case ::com::rdk::hal::hdmioutput::Property::SCAN_INFORMATION: {
                        int32_t scanInfo = getProfileSupportedScanInfo(i, valueIdx);
                        kvPair.propertyValue.value.value().set<::com::rdk::hal::PropertyValue::Value::Tag::intValue>(scanInfo);
                        UT_LOG_INFO("  %s value[%d]: %d", propInfo.name.c_str(), valueIdx, scanInfo);
                        break;
                    }
                }
                
                propertyList.push_back(kvPair);
            }
            
            // Set all properties at once
            bool setMultiResult = false;
            HDMIOUTPUT_SET_PROPERTYMULTI(controller, propertyList, setMultiResult);
            UT_ASSERT_TRUE(setMultiResult);
            UT_LOG_INFO("setPropertyMulti completed successfully for value index %d", valueIdx);
            
            // Verify each property
            UT_LOG_INFO("Test Step 4.%d: Verifying all properties", valueIdx);
            for (size_t propIdx = 0; propIdx < settableProperties.size(); propIdx++) {
                const auto& propInfo = settableProperties[propIdx];
                ::com::rdk::hal::PropertyValue retrievedValue;
                HDMIOUTPUT_GET_PROPERTY(hdmiOutput, propInfo.property, retrievedValue);
                UT_ASSERT_TRUE(retrievedValue.value.has_value());
                
                int32_t expectedValue = propertyList[propIdx].propertyValue.value.value().get<::com::rdk::hal::PropertyValue::Value::Tag::intValue>();
                int32_t actualValue = retrievedValue.value.value().get<::com::rdk::hal::PropertyValue::Value::Tag::intValue>();
                UT_ASSERT_EQUAL(actualValue, expectedValue);
                UT_LOG_INFO("  %s verified: value = %d", propInfo.name.c_str(), actualValue);
            }
            
            UT_LOG_INFO("All properties verified successfully for value index %d", valueIdx);
        }
        
        // Stop and close
        UT_LOG_INFO("Test Step 5: Stopping and closing HDMI output");
        HDMIOUTPUT_STOP(controller);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        bool closeResult = false;
        HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
        UT_ASSERT_TRUE(closeResult);
        
        UT_LOG_INFO("Completed testing HDMI output resource %u", i);
    }
    
    UT_LOG_INFO("Test L2-04 Multi-Property Validation: PASSED");
    return;
}

/**
 * @brief Test L2-05: Validate SPD InfoFrame set/get operations
 *
 * This test validates setting and getting Source Product Description InfoFrame.
 *
 * **Test Group ID:** 02@n
 * **Test Case ID:** 005@n
 *
 * **Pre-conditions:** No external HDMI device required. SPD InfoFrame can be set/get without device connected.
 *
 * **Test Procedure:**
 * Refer to HDMI Output specification documentation [hdmioutput_High-Level_TestSpecification](../../docs/pages/hdmioutput_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L2_HDMIOUTPUT, SPDInfoFrame)
{
    UT_LOG_INFO("Starting Test: L2-05 SPD InfoFrame Validation");
    
    // Get count of HDMI output resources
    uint32_t resourceCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources");
    UT_ASSERT_TRUE(resourceCount > 0);
    UT_LOG_INFO("Number of HDMI output resources: %u", resourceCount);
    
    // Loop through all HDMI output resources
    for (uint32_t i = 0; i < resourceCount; i++) {
        UT_LOG_INFO("Testing HDMI output resource %u", i);
        
        // Get HDMI output instance
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(i);
        UT_ASSERT_NOT_NULL(hdmiOutput.get());
        
        // Create controller listener
        android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
        
        // Open HDMI output
        UT_LOG_INFO("Test Step 1: Opening HDMI output");
        android::sp<com::rdk::hal::hdmioutput::IHDMIOutputController> controller;
        HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
        UT_ASSERT_NOT_NULL(controller.get());
        
        // Get count of supported SPD sources for this resource
        std::string spdSourceCountKey = "hdmioutput/resources/" + std::to_string(i) + "/supportedSPDSources";
        uint32_t spdSourceCount = UT_KVP_PROFILE_GET_LIST_COUNT(spdSourceCountKey.c_str());
        UT_ASSERT_TRUE(spdSourceCount > 0);
        UT_LOG_INFO("Number of supported SPD sources for resource %u: %u", i, spdSourceCount);
        
        // Loop through all supported SPD sources
        for (uint32_t spdIdx = 0; spdIdx < spdSourceCount; spdIdx++) {
            UT_LOG_INFO("Test Step 2.%u: Testing SPD source index %u", spdIdx, spdIdx);
            
            // Create SPD InfoFrame
            ::com::rdk::hal::hdmioutput::SPDInfoFrame spdInfoFrame;
            // vendorName is std::array<uint8_t, 8>
            std::string vendor = "RDK";
            for (size_t j = 0; j < vendor.length() && j < 8; j++) {
                spdInfoFrame.vendorName[j] = static_cast<uint8_t>(vendor[j]);
            }
            // productDescription is std::array<uint8_t, 16>
            std::string desc = "HDMI Test";
            for (size_t j = 0; j < desc.length() && j < 16; j++) {
                spdInfoFrame.productDescription[j] = static_cast<uint8_t>(desc[j]);
            }
            // Read SPD source from profile
            std::string spdSourceStr = getProfileSupportedSPDSource(i, spdIdx);
            SPDSource spdSource = convertStringToSPDSource(spdSourceStr);
            spdInfoFrame.spdSourceInformation = spdSource;
            UT_LOG_INFO("SPD InfoFrame created: vendor='%s', description='%s', source='%s'", 
                        vendor.c_str(), desc.c_str(), spdSourceStr.c_str());
            
            // Set SPD InfoFrame
            UT_LOG_INFO("Test Step 3.%u: Setting SPD InfoFrame", spdIdx);
            HDMIOUTPUT_SET_SPD_INFOFRAME(controller, spdInfoFrame);
            UT_LOG_INFO("setSPDInfoFrame completed successfully");
            
            // Get SPD InfoFrame
            UT_LOG_INFO("Test Step 4.%u: Getting SPD InfoFrame", spdIdx);
            ::com::rdk::hal::hdmioutput::SPDInfoFrame retrievedSPD;
            HDMIOUTPUT_GET_SPD_INFOFRAME(controller, retrievedSPD);
            std::string retrievedVendor(reinterpret_cast<const char*>(retrievedSPD.vendorName.data()));
            std::string retrievedDesc(reinterpret_cast<const char*>(retrievedSPD.productDescription.data()));
            UT_LOG_INFO("getSPDInfoFrame returned: vendor='%s', description='%s'",
                        retrievedVendor.c_str(), retrievedDesc.c_str());
            
            // Verify SPD InfoFrame data matches
            UT_ASSERT_TRUE(retrievedSPD.vendorName == spdInfoFrame.vendorName);
            UT_ASSERT_TRUE(retrievedSPD.productDescription == spdInfoFrame.productDescription);
            UT_ASSERT_EQUAL(retrievedSPD.spdSourceInformation, spdInfoFrame.spdSourceInformation);
            UT_LOG_INFO("SPD InfoFrame data verified successfully for source '%s'", spdSourceStr.c_str());
        }
        
        // Close HDMI output
        UT_LOG_INFO("Test Step 5: Closing HDMI output");
        bool closeResult = false;
        HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
        UT_ASSERT_TRUE(closeResult);
        
        UT_LOG_INFO("Completed testing HDMI output resource %u", i);
    }
    
    UT_LOG_INFO("Test L2-05 SPD InfoFrame Validation: PASSED");
    return;
}

/**
* @} // End of HDMIOUTPUT_HALTEST_L2
* @} // End of HDMIOUTPUT_HALTEST
* @} // End of HDMIOUTPUT
* @} // End of HPK
*/
/* End of vts_hdmioutput_L2_tests.cpp */
