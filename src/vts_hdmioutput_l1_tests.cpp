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
 * @defgroup HDMI_OUTPUT_HALTEST_L1 HDMI OUTPUT HAL Tests L1 File
 * @{
 * @parblock
 *
 * ### L1 Test Cases for HDMI OUTPUT HAL :
 *
 * This module includes Level 1 functional tests (success and failure scenarios).
 * This is to ensure that the API meets the operational requirements of the module across all vendors.
 *
 * **Pre-Conditions:** @n
 * **Dependencies:** None@n
 *
 * @endparblock
 *
 */

/**
 * @file vts_hdmioutput_l1_tests.cpp
 *
 */
#include "vts_hdmioutput.h"

/* Test class registration with UT_Core */
UT_ADD_TEST_TO_GROUP(VTS_L1_HDMIOUTPUT, UT_TESTS_L1)

class VTS_L1_HDMIOUTPUT : public UTCore
{
public:
    VTS_L1_HDMIOUTPUT() : UTCore() {}

    ~VTS_L1_HDMIOUTPUT() override = default;

    void SetUp() override
    {
        android::ProcessState::self()->startThreadPool();
    }

    void TearDown() override
    {
    }
};

/**
 * @brief Verify that getHDMIOutputIds returns valid non-empty HDMI output IDs.
 *
 * This test calls the getHDMIOutputIds API on the HDMIOutputManager service and verifies that the returned
 * list of HDMI output IDs is not empty and matches the expected identifiers retrieved from a predefined YAML profile.
 * The test iterates through the IDs and asserts that each ID is either one of the expected valid IDs or is marked as
 * UNDEFINED if extra entries exist.
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 001@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Call getHDMIOutputIds on the HDMIOutputManager service with no input parameters and capture the output vector and status. | input: none, output: hdmiOutputIds vector, status structure | API returns status OK and populates hdmiOutputIds vector | Should Pass |@n
 * | 02 | Verify that the returned status indicates success and that the hdmiOutputIds vector is not empty. | status.isOk() and hdmiOutputIds size | Status is OK; hdmiOutputIds vector is not empty | Should Pass |@n
 * | 03 | Iterate over each id in the hdmiOutputIds vector and assert that the id is either present in the expectedIds vector | Each id from hdmiOutputIds compared against expectedIds | Each id is validated successfully; assertions pass if id matches one of the expected values| Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getHDMIOutputIds_pos)
{
    std::vector<hdmioutput::IHDMIOutput::Id> hdmiOutputIds;

    /* Get the HDMI OUTPUT IDs */
    HDMIOUTPUT_GET_HDMIOUTPUT_IDS(gHDMIOUTPUTHandle, hdmiOutputIds);

    uint32_t expectedCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput.resources");
    UT_ASSERT_EQUAL(expectedCount, hdmiOutputIds.size());

    // Prepare expected IDs from profile
    std::vector<int32_t> expectedIds;
    for (uint32_t i = 0; i < expectedCount; ++i)
    {
        std::string key = "hdmioutput.resources." + std::to_string(i) + ".id";
        int32_t id = UT_KVP_PROFILE_GET_UINT32(key.c_str());
        expectedIds.push_back(id);
    }

    // Validate that each returned ID is in expectedIds
    for (const auto& id : hdmiOutputIds)
    {
        bool found = std::find(expectedIds.begin(), expectedIds.end(), id.value) != expectedIds.end();
        UT_ASSERT_TRUE(found);
    }
}

/**
 * @brief Validate the HDMI Output Manager's ability to fetch HDMI Output objects
 *
 * This test iterates over a list of HDMI output ports, retrieves their respective ID using helper methods,
 * and then calls the getHDMIOutput API to verify that a valid HDMI Output object is returned.
 * The test confirms that the API call returns a successful status and a non-null object for each port.
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 002@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Retrieve the count of HDMI output ports using the helper function UT_KVP_PROFILE_GET_LIST_COUNT. | hdmioutput.resources list | Returns the total number of HDMI output ports available. | Should be successful |
 * | 02 | Start a loop to iterate over each HDMI output port. | For each iteration: index = 0, 1, ... , numPorts-1 | Iteration proceeds for each available HDMI output port. | Should be successful |
 * | 03 | For the current iteration, extract the HDMI output's ID using UT_KVP_PROFILE_GET_UINT32 from hdmioutput.resources at the current index. | index = current loop index, hdmioutput.resources.<index>.id | Extracts a valid uint32 value representing the HDMI output ID. | Should be successful |
 * | 04 | Log the action of calling getHDMIOutput with the retrieved HDMI output ID and then invoke gHDMIOUTPUTHandle->getHDMIOutput. | Input: hdmiOutputId = current port ID | API returns a binder Status with isOk true and a non-null HDMI Output object. | Should Pass |
 * | 05 | Assert that the status returned from getHDMIOutput is ok and that the HDMI Output object is not null. | Assert status.isOk and hdmiOutput != nullptr | Both assertions pass confirming successful API execution. | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getHDMIOutput_pos)
{
    std::vector<hdmioutput::IHDMIOutput::Id> hdmiOutputIds;

    /* Get the HDMI OUTPUT IDs */
    HDMIOUTPUT_GET_HDMIOUTPUT_IDS(gHDMIOUTPUTHandle, hdmiOutputIds);

    for (const auto& id : hdmiOutputIds)
    {
        android::sp<hdmioutput::IHDMIOutput> hdmiOutput;

        HDMIOUTPUT_GET_HDMIOUTPUT(gHDMIOUTPUTHandle, id, hdmiOutput);
        UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);
    }
}

/**
 * @brief Negative test for getHDMIOutput with invalid HDMI output IDs
 *
 * This test verifies that the HDMI Output Manager's getHDMIOutput API correctly handles invalid input IDs.
 * It attempts to fetch HDMI output objects using IDs that are outside the valid range (one less than the minimum,
 * one more than the maximum, and the UNDEFINED value). The test expects the API to fail gracefully and not return
 * a valid HDMI output object for these invalid IDs.
 *
 * **Test Group ID:** Negative: 01@n
 * **Test Case ID:** 003@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Retrieve the list of valid HDMI output IDs. | hdmiOutputIds vector | List of valid HDMI output IDs is obtained. | Should be successful |
 * | 02 | Attempt to fetch HDMI output with an ID less than the minimum valid ID. | invalidId = min(validIds) - 1 | API returns failure status and null object | Should Pass |
 * | 03 | Attempt to fetch HDMI output with an ID greater than the maximum valid ID. | invalidId = max(validIds) + 1 | API returns failure status and null object | Should Pass |
 * | 04 | Attempt to fetch HDMI output with the UNDEFINED ID value. | invalidId = UNDEFINED | API returns failure status and null object | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getHDMIOutput_neg)
{
    std::vector<hdmioutput::IHDMIOutput::Id> hdmiOutputIds;
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput;
    hdmioutput::IHDMIOutput::Id invalidId;

    /* Get the HDMI OUTPUT IDs */
    HDMIOUTPUT_GET_HDMIOUTPUT_IDS(gHDMIOUTPUTHandle, hdmiOutputIds);

    if(hdmiOutputIds.size() > 1)
        std::sort(hdmiOutputIds.begin(), hdmiOutputIds.end());

    // Test with ID less than minimum
    invalidId = hdmiOutputIds.front();
    invalidId.value -= 1;
    
    android::binder::Status status;
    HDMIOUTPUT_GET_HDMIOUTPUT_NEG(gHDMIOUTPUTHandle, invalidId, hdmiOutput, status);
    UT_ASSERT_TRUE(!status.isOk() && hdmiOutput == nullptr);

    // Test with ID greater than maximum
    invalidId = hdmiOutputIds.back();
    invalidId.value += 1;
    
    android::binder::Status status1;
    HDMIOUTPUT_GET_HDMIOUTPUT_NEG(gHDMIOUTPUTHandle, invalidId, hdmiOutput, status1);
    UT_ASSERT_TRUE(!status1.isOk() && hdmiOutput == nullptr);

    // Test with UNDEFINED
    invalidId.value = IHDMIOutput::Id::UNDEFINED;
    
    android::binder::Status status2;
    HDMIOUTPUT_GET_HDMIOUTPUT_NEG(gHDMIOUTPUTHandle, invalidId, hdmiOutput, status2);
    UT_ASSERT_TRUE(!status2.isOk() && hdmiOutput == nullptr);
}

/**
 * @brief Validate correct behavior of platform getCapabilities API in positive scenario
 *
 * This test case verifies that the getCapabilities API correctly returns HDMI output platform capabilities,
 * ensuring that the returned data matches the expected values from the YAML configuration.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 004
 * **Priority:** High
 *
 * **Pre-Conditions:** None
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke getCapabilities API and capture the output. | IN: none, OUT: platform capabilities structure (caps) | API returns valid status with capabilities populated | Should Pass |
 * | 02 | Verify that capabilities match expected values from YAML profile. | caps fields compared with YAML configuration | Values match YAML configuration | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getPlatformCapabilities_pos)
{
    hdmioutput::PlatformCapabilities caps;

    HDMIOUTPUT_GET_PLATCAPABILITIES(gHDMIOUTPUTHandle, caps);

    // Verify platform capabilities match profile
    double expectedNativeFrameRate = ut_kvp_getDoubleField(ut_kvp_profile_getInstance(), "hdmioutput/platformCapabilities/nativeFrameRate");
    UT_ASSERT_EQUAL(caps.nativeFrameRate, expectedNativeFrameRate);
    UT_LOG_INFO("Native frame rate validated: %.2f", caps.nativeFrameRate);

    char freeSyncStr[UT_KVP_MAX_ELEMENT_SIZE];
    UT_KVP_PROFILE_GET_STRING("hdmioutput/platformCapabilities/freeSync", freeSyncStr);
    hdmioutput::FreeSync expectedFreeSync = convertStringToFreeSync(std::string(freeSyncStr));
    UT_ASSERT_EQUAL(static_cast<int32_t>(caps.freeSync), static_cast<int32_t>(expectedFreeSync));
    UT_LOG_INFO("FreeSync level validated: %s", freeSyncStr);
}

/**
 * @brief Validate HDMI output port capabilities
 *
 * This test case verifies that the getCapabilities API returns correct HDMI output capabilities for each port.
 * For every port defined in the profile,the test fetches the HDMI output object and its capabilities,
 * then validates all capability fields (booleans and supported lists) against the expected values from the profile.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 005
 * **Priority:** High
 *
 * **Pre-Conditions:** None
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | For each port in profile, fetch HDMI output object and its capabilities. | port.id, expected capabilities | API returns valid status and capabilities for each port | Should Pass |
 * | 02 | Validate all capability fields (booleans and supported lists) against profile values. | supports3D, supportedVersions, etc. | All fields match expected values | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getCapabilities_pos)
{
    uint32_t resourceCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput.resources");

    for (uint32_t i = 0; i < resourceCount; ++i)
    {
        hdmioutput::Capabilities capabilities;
        android::sp<hdmioutput::IHDMIOutput> hdmiOutput;
        
        std::string idKey = "hdmioutput.resources." + std::to_string(i) + ".id";
        hdmioutput::IHDMIOutput::Id hdmiOutputId;
        hdmiOutputId.value = UT_KVP_PROFILE_GET_UINT32(idKey.c_str());

        HDMIOUTPUT_GET_HDMIOUTPUT(gHDMIOUTPUTHandle, hdmiOutputId, hdmiOutput);
        HDMIOUTPUT_GET_CAPABILITIES(hdmiOutput, capabilities);

        // Verify boolean capabilities
        std::string key = "hdmioutput.resources." + std::to_string(i) + ".supports3D";
        bool expectedSupports3D = UT_KVP_PROFILE_GET_BOOL(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supports3D, expectedSupports3D);

        key = "hdmioutput.resources." + std::to_string(i) + ".supportsFRL";
        bool expectedSupportsFRL = UT_KVP_PROFILE_GET_BOOL(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportsFRL, expectedSupportsFRL);

        key = "hdmioutput.resources." + std::to_string(i) + ".supportsVRR";
        bool expectedSupportsVRR = UT_KVP_PROFILE_GET_BOOL(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportsVRR, expectedSupportsVRR);

        key = "hdmioutput.resources." + std::to_string(i) + ".supportsFreeSync";
        bool expectedSupportsFreeSync = UT_KVP_PROFILE_GET_BOOL(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportsFreeSync, expectedSupportsFreeSync);

        key = "hdmioutput.resources." + std::to_string(i) + ".supportsQMS";
        bool expectedSupportsQMS = UT_KVP_PROFILE_GET_BOOL(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportsQMS, expectedSupportsQMS);

        key = "hdmioutput.resources." + std::to_string(i) + ".supportsALLM";
        bool expectedSupportsALLM = UT_KVP_PROFILE_GET_BOOL(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportsALLM, expectedSupportsALLM);

        key = "hdmioutput.resources." + std::to_string(i) + ".supportsQFT";
        bool expectedSupportsQFT = UT_KVP_PROFILE_GET_BOOL(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportsQFT, expectedSupportsQFT);

        //key = "hdmioutput.resources." + std::to_string(i) + ".supportsForcedHDROutputModes";
        //bool expectedSupportsForcedHDROutputModes = UT_KVP_PROFILE_GET_BOOL(key.c_str());
        //UT_ASSERT_EQUAL(capabilities.supportsForcedHDROutputModes, expectedSupportsForcedHDROutputModes);

        // Verify supported versions - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedVersions";
        uint32_t versionCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedVersions.size(), versionCount);
        
        // Load all expected versions from HFP
        std::vector<hdmioutput::HDMIVersion> expectedVersions;
        for (uint32_t j = 0; j < versionCount; ++j) {
            char versionStr[UT_KVP_MAX_ELEMENT_SIZE];
            std::string versionKey = key + "." + std::to_string(j);
            UT_KVP_PROFILE_GET_STRING(versionKey.c_str(), versionStr);
            expectedVersions.push_back(convertStringToHDMIVersion(std::string(versionStr)));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& version : capabilities.supportedVersions) {
            bool found = std::find(expectedVersions.begin(), expectedVersions.end(), version) != expectedVersions.end();
            UT_ASSERT_TRUE(found);
        }

        // Verify supported VICs - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedVICs";
        uint32_t vicCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedVICs.size(), vicCount);
        
        // Load all expected VICs from HFP
        std::vector<hdmioutput::VIC> expectedVICs;
        for (uint32_t j = 0; j < vicCount; ++j) {
            char vicStr[UT_KVP_MAX_ELEMENT_SIZE];
            std::string vicKey = key + "." + std::to_string(j);
            UT_KVP_PROFILE_GET_STRING(vicKey.c_str(), vicStr);
            expectedVICs.push_back(convertStringToVIC(std::string(vicStr)));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& vic : capabilities.supportedVICs) {
            bool found = std::find(expectedVICs.begin(), expectedVICs.end(), vic) != expectedVICs.end();
            UT_ASSERT_TRUE(found);
        }

        // Verify supported HDR output modes - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedHDROutputModes";
        uint32_t hdrModeCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedHDROutputModes.size(), hdrModeCount);
        
        // Load all expected HDR modes from HFP
        std::vector<hdmioutput::HDROutputMode> expectedHDRModes;
        for (uint32_t j = 0; j < hdrModeCount; ++j) {
            char hdrModeStr[UT_KVP_MAX_ELEMENT_SIZE];
            std::string hdrModeKey = key + "." + std::to_string(j);
            UT_KVP_PROFILE_GET_STRING(hdrModeKey.c_str(), hdrModeStr);
            expectedHDRModes.push_back(convertStringToHDROutputMode(std::string(hdrModeStr)));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& mode : capabilities.supportedHDROutputModes) {
            bool found = std::find(expectedHDRModes.begin(), expectedHDRModes.end(), mode) != expectedHDRModes.end();
            UT_ASSERT_TRUE(found);
        }

        // Verify supported HDCP versions - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedHDCPVersions";
        uint32_t hdcpCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedHDCPProtocolVersions.size(), hdcpCount);
        
        // Load all expected HDCP versions from HFP
        std::vector<hdmioutput::HDCPProtocolVersion> expectedHDCPVersions;
        for (uint32_t j = 0; j < hdcpCount; ++j) {
            char hdcpStr[UT_KVP_MAX_ELEMENT_SIZE];
            std::string hdcpKey = key + "." + std::to_string(j);
            UT_KVP_PROFILE_GET_STRING(hdcpKey.c_str(), hdcpStr);
            expectedHDCPVersions.push_back(convertStringToHDCPProtocolVersions(std::string(hdcpStr)));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& version : capabilities.supportedHDCPProtocolVersions) {
            bool found = std::find(expectedHDCPVersions.begin(), expectedHDCPVersions.end(), version) != expectedHDCPVersions.end();
            UT_ASSERT_TRUE(found);
        }

        // Verify supported colorimetries - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedColorimetries";
        uint32_t colorimetryCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedColorimetries.size(), colorimetryCount);
        
        // Load all expected colorimetries from HFP
        std::vector<hdmioutput::Colorimetry> expectedColorimetries;
        for (uint32_t j = 0; j < colorimetryCount; ++j) {
            char colorimetryStr[UT_KVP_MAX_ELEMENT_SIZE];
            std::string colorimetryKey = key + "." + std::to_string(j);
            UT_KVP_PROFILE_GET_STRING(colorimetryKey.c_str(), colorimetryStr);
            expectedColorimetries.push_back(convertStringToColorimetry(std::string(colorimetryStr)));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& colorimetry : capabilities.supportedColorimetries) {
            bool found = std::find(expectedColorimetries.begin(), expectedColorimetries.end(), colorimetry) != expectedColorimetries.end();
            UT_ASSERT_TRUE(found);
        }

        // Verify supported extended colorimetries - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedExtendedColorimetries";
        uint32_t extColorimetryCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedExtendedColorimetries.size(), extColorimetryCount);
        
        // Load all expected extended colorimetries from HFP
        std::vector<hdmioutput::ExtendedColorimetry> expectedExtColorimetries;
        for (uint32_t j = 0; j < extColorimetryCount; ++j) {
            char extColorStr[UT_KVP_MAX_ELEMENT_SIZE];
            std::string extColorKey = key + "." + std::to_string(j);
            UT_KVP_PROFILE_GET_STRING(extColorKey.c_str(), extColorStr);
            expectedExtColorimetries.push_back(convertStringToExtendedColorimetry(std::string(extColorStr)));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& extColor : capabilities.supportedExtendedColorimetries) {
            bool found = std::find(expectedExtColorimetries.begin(), expectedExtColorimetries.end(), extColor) != expectedExtColorimetries.end();
            UT_ASSERT_TRUE(found);
        }

        // Verify supported additional colorimetry extensions - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedAdditionalColorimetryExtensions";
        uint32_t additionalColorimetryCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedAdditionalColorimetryExtensions.size(), additionalColorimetryCount);
        
        // Load all expected additional colorimetry extensions from HFP
        std::vector<hdmioutput::AdditionalColorimetryExtension> expectedAddColorimetries;
        for (uint32_t j = 0; j < additionalColorimetryCount; ++j) {
            char addColorStr[UT_KVP_MAX_ELEMENT_SIZE];
            std::string addColorKey = key + "." + std::to_string(j);
            UT_KVP_PROFILE_GET_STRING(addColorKey.c_str(), addColorStr);
            expectedAddColorimetries.push_back(convertStringToAdditionalColorimetryExtension(std::string(addColorStr)));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& addColor : capabilities.supportedAdditionalColorimetryExtensions) {
            bool found = std::find(expectedAddColorimetries.begin(), expectedAddColorimetries.end(), addColor) != expectedAddColorimetries.end();
            UT_ASSERT_TRUE(found);
        }

        // Verify supported color depths - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedColorDepths";
        uint32_t colorDepthCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedColorDepths.size(), colorDepthCount);
        
        // Load all expected color depths from HFP
        std::vector<int32_t> expectedColorDepths;
        for (uint32_t j = 0; j < colorDepthCount; ++j) {
            std::string depthKey = key + "." + std::to_string(j);
            expectedColorDepths.push_back(UT_KVP_PROFILE_GET_UINT32(depthKey.c_str()));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& depth : capabilities.supportedColorDepths) {
            bool found = std::find(expectedColorDepths.begin(), expectedColorDepths.end(), depth) != expectedColorDepths.end();
            UT_ASSERT_TRUE(found);
        }

        // Verify supported pixel formats - count and values
        key = "hdmioutput.resources." + std::to_string(i) + ".supportedPixelFormats";
        uint32_t pixelFormatCount = UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
        UT_ASSERT_EQUAL(capabilities.supportedPixelFormats.size(), pixelFormatCount);
        
        // Load all expected pixel formats from HFP
        std::vector<hdmioutput::PixelFormat> expectedPixelFormats;
        for (uint32_t j = 0; j < pixelFormatCount; ++j) {
            char pixelFormatStr[UT_KVP_MAX_ELEMENT_SIZE];
            std::string pixelFormatKey = key + "." + std::to_string(j);
            UT_KVP_PROFILE_GET_STRING(pixelFormatKey.c_str(), pixelFormatStr);
            expectedPixelFormats.push_back(convertStringToPixelFormat(std::string(pixelFormatStr)));
        }
        
        // Verify each capability value exists in expected values
        for (const auto& format : capabilities.supportedPixelFormats) {
            bool found = std::find(expectedPixelFormats.begin(), expectedPixelFormats.end(), format) != expectedPixelFormats.end();
            UT_ASSERT_TRUE(found);
        }
    }
}

/**
 * @brief Validate that getState returns correct initial state
 *
 * This test verifies that a newly accessed HDMI output object returns CLOSED state
 * before any open operation is performed.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 006
 * **Priority:** High
 *
 * **Pre-Conditions:** None
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Get first HDMI output object | port ID | Valid object returned | Should Pass |
 * | 02 | Call getState on the object | none | State is CLOSED | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getState_initial)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    hdmioutput::State state;
    HDMIOUTPUT_GET_STATE(hdmiOutput, state);
    UT_ASSERT_EQUAL(state, hdmioutput::State::CLOSED);
}

/**
 * @brief Validate open API positive scenario
 *
 * This test verifies that the open API successfully transitions the HDMI output from CLOSED to READY state
 * and returns a valid controller object.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 008
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in CLOSED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Get HDMI output object | port ID | Valid object returned | Should Pass |
 * | 02 | Create controller listener | listener object | Valid listener created | Should Pass |
 * | 03 | Call open with listener | listener | Controller returned, state is READY | Should Pass |
 * | 04 | Close the output | controller | Closed successfully | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, open_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    UT_ASSERT_NOT_EQUAL(controller, nullptr);

    hdmioutput::State state;
    HDMIOUTPUT_GET_STATE(hdmiOutput, state);
    UT_ASSERT_EQUAL(state, hdmioutput::State::READY);

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
    UT_ASSERT_TRUE(closeResult);
}

/**
 * @brief Validate open API negative scenario with null listener
 *
 * This test verifies that the open API handles null listener gracefully.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 009
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in CLOSED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Get HDMI output object | port ID | Valid object returned | Should Pass |
 * | 02 | Call open with null listener | null | API returns OK but controller is null | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, open_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<hdmioutput::IHDMIOutputController> controller;

    android::binder::Status status;
    HDMIOUTPUT_OPEN_NEG(hdmiOutput, nullptr, controller, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_EQUAL(controller, nullptr);
    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    UT_ASSERT_NOT_EQUAL(controller, nullptr);

    hdmioutput::State state;
    HDMIOUTPUT_GET_STATE(hdmiOutput, state);
    UT_ASSERT_EQUAL(state, hdmioutput::State::READY);
    
    HDMIOUTPUT_OPEN_NEG(hdmiOutput, controllerListener, controller, status);
    UT_ASSERT_EQUAL(status.exceptionCode(), android::binder::Status::EX_ILLEGAL_STATE);
    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
    UT_ASSERT_TRUE(closeResult);
}

/**
 * @brief Validate close API positive scenario
 *
 * This test verifies that the close API successfully transitions the HDMI output from READY to CLOSED state.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 010
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | valid listener | Controller returned, READY state | Should Pass |
 * | 02 | Call close with controller | controller | Returns true, state is CLOSED | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, close_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    UT_ASSERT_NOT_EQUAL(controller, nullptr);

    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
    UT_ASSERT_TRUE(closeResult);

    hdmioutput::State state;
    HDMIOUTPUT_GET_STATE(hdmiOutput, state);
    UT_ASSERT_EQUAL(state, hdmioutput::State::CLOSED);
}

/**
 * @brief Validate close API negative scenario
 *
 * This test verifies that the close API fails when called with null controller or in wrong state.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 011
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in CLOSED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Try to close before opening | null controller | Returns false | Should Pass |
 * | 02 | Open HDMI output | valid controller | READY state | Setup |
 * | 03 | Try to close with null controller | null controller | Returns false | Should Pass |
 * | 04 | Close with valid controller | valid controller | Returns true | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, close_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    // Now open and get valid controller
    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;
    // Verify initial state is CLOSED
    hdmioutput::State state;
    HDMIOUTPUT_GET_STATE(hdmiOutput, state);
    UT_ASSERT_EQUAL(state, hdmioutput::State::CLOSED);

    // Try to close before opening - should fail
    bool closeResult = false;
    android::binder::Status status1;
    HDMIOUTPUT_CLOSE_NEG(hdmiOutput, controller, closeResult, status1);
    UT_ASSERT_FALSE(status1.isOk());
    UT_ASSERT_FALSE(closeResult);

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    UT_ASSERT_NOT_EQUAL(controller, nullptr);

    // Try to close with null controller - should fail
    closeResult = false;
    android::binder::Status status2;
    HDMIOUTPUT_CLOSE_NEG(hdmiOutput, nullptr, closeResult, status2);
    UT_ASSERT_FALSE(status2.isOk());
    UT_ASSERT_FALSE(closeResult);

    // Proper cleanup with valid controller
    closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
    UT_ASSERT_TRUE(closeResult);
}

/**
 * @brief Validate registerEventListener positive scenario
 *
 * This test verifies that event listeners can be successfully registered.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 012
 * **Priority:** High
 *
 * **Pre-Conditions:** None
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Get HDMI output object | port ID | Valid object returned | Should Pass |
 * | 02 | Create event listener | listener object | Valid listener created | Should Pass |
 * | 03 | Register event listener | listener | Returns true | Should Pass |
 * | 04 | Unregister listener | listener | Returns true | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, registerEventListener_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputEventListener> eventListener = new HDMIOutputEventListener();
    bool registerResult = false;

    HDMIOUTPUT_REGISTER_EVENT_LISTENER(hdmiOutput, eventListener, registerResult);
    UT_ASSERT_TRUE(registerResult);

    // Cleanup
    bool unregisterResult = false;
    HDMIOUTPUT_UNREGISTER_EVENT_LISTENER(hdmiOutput, eventListener, unregisterResult);
    UT_ASSERT_TRUE(unregisterResult);
}

/**
 * @brief Validate unregisterEventListener positive scenario
 *
 * This test verifies that a previously registered event listener can be successfully unregistered.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 013
 * **Priority:** High
 *
 * **Pre-Conditions:** Event listener registered
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Register event listener | listener | Returns true | Setup |
 * | 02 | Unregister event listener | listener | Returns true | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, unregisterEventListener_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputEventListener> eventListener = new HDMIOutputEventListener();
    bool registerResult = false;

    HDMIOUTPUT_REGISTER_EVENT_LISTENER(hdmiOutput, eventListener, registerResult);
    UT_ASSERT_TRUE(registerResult);

    bool unregisterResult = false;
    HDMIOUTPUT_UNREGISTER_EVENT_LISTENER(hdmiOutput, eventListener, unregisterResult);
    UT_ASSERT_TRUE(unregisterResult);
}

/**
 * @brief Validate registerEventListener negative scenario
 *
 * This test verifies that attempting to register the same listener twice fails.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 014
 * **Priority:** High
 *
 * **Pre-Conditions:** None
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Register event listener | listener | Returns true | Setup |
 * | 02 | Register same listener again | same listener | Returns false | Should Pass |
 * | 03 | Unregister listener | listener | Returns true | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, registerEventListener_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputEventListener> eventListener = new HDMIOutputEventListener();
    bool registerResult = false;

    // First registration should succeed
    HDMIOUTPUT_REGISTER_EVENT_LISTENER(hdmiOutput, eventListener, registerResult);
    UT_ASSERT_TRUE(registerResult);

    // Second registration should fail
    android::binder::Status status;
    bool registerResult2 = false;  // Use fresh variable for negative test
    HDMIOUTPUT_REGISTER_EVENT_LISTENER_NEG(hdmiOutput, eventListener, registerResult2, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_EQUAL(status.exceptionCode(), android::binder::Status::EX_UNSUPPORTED_OPERATION);

    // Cleanup
    bool unregisterResult = false;
    HDMIOUTPUT_UNREGISTER_EVENT_LISTENER(hdmiOutput, eventListener, unregisterResult);
    UT_ASSERT_TRUE(unregisterResult);
}

/**
 * @brief Validate unregisterEventListener negative scenario
 *
 * This test verifies that attempting to unregister a listener that was never registered fails.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 015
 * **Priority:** High
 *
 * **Pre-Conditions:** None
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Try to unregister never-registered listener | listener | Returns false | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, unregisterEventListener_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputEventListener> eventListener = new HDMIOutputEventListener();
    bool unregisterResult = false;

    android::binder::Status status;
    HDMIOUTPUT_UNREGISTER_EVENT_LISTENER_NEG(hdmiOutput, eventListener, unregisterResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(unregisterResult);
}

/**
 * @brief Validate getProperty API positive scenario for all properties
 *
 * This test verifies that the getProperty API correctly retrieves all property values from HDMI output objects
 * and validates their default values.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 016
 * **Priority:** High
 *
 * **Pre-Conditions:** None
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Get HDMI output object | port ID | Valid object returned | Should Pass |
 * | 02 | Query RESOURCE_ID property | Property::RESOURCE_ID | Returns port ID value | Should Pass |
 * | 03 | Query VIC property | Property::VIC | Returns VIC0_UNAVAILABLE | Should Pass |
 * | 04 | Query CONTENT_TYPE property | Property::CONTENT_TYPE | Returns UNSPECIFIED | Should Pass |
 * | 05 | Query AFD property | Property::AFD | Returns UNSPECIFIED | Should Pass |
 * | 06 | Query HDR_OUTPUT_MODE property | Property::HDR_OUTPUT_MODE | Returns AUTO | Should Pass |
 * | 07 | Query SCAN_INFORMATION property | Property::SCAN_INFORMATION | Returns NO_DATA | Should Pass |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getProperty_pos)
{
    std::vector<hdmioutput::IHDMIOutput::Id> hdmiOutputIds;

    HDMIOUTPUT_GET_HDMIOUTPUT_IDS(gHDMIOUTPUTHandle, hdmiOutputIds);

    for (const auto& id : hdmiOutputIds)
    {
        android::sp<hdmioutput::IHDMIOutput> hdmiOutput;
        std::optional<PropertyValue> propertyValue;

        HDMIOUTPUT_GET_HDMIOUTPUT(gHDMIOUTPUTHandle, id, hdmiOutput);
        UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

        // Test RESOURCE_ID property
        HDMIOUTPUT_GET_PROPERTY(hdmiOutput, hdmioutput::Property::RESOURCE_ID, propertyValue);
        UT_ASSERT_TRUE(propertyValue.has_value());
        UT_ASSERT_EQUAL(propertyValue->value->get<PropertyValue::Value::Tag::intValue>(), id.value);
        UT_LOG_INFO("RESOURCE_ID validated: %d", id.value);

        // Test VIC property - default should be VIC0_UNAVAILABLE (0)
        propertyValue.reset();
        HDMIOUTPUT_GET_PROPERTY(hdmiOutput, hdmioutput::Property::VIC, propertyValue);
        UT_ASSERT_TRUE(propertyValue.has_value());
        UT_ASSERT_EQUAL(propertyValue->value->get<PropertyValue::Value::Tag::intValue>(), 
                        static_cast<int32_t>(hdmioutput::VIC::VIC0_UNAVAILABLE));
        UT_LOG_INFO("VIC validated: VIC0_UNAVAILABLE");

        // Test CONTENT_TYPE property - default should be UNSPECIFIED (-1)
        propertyValue.reset();
        HDMIOUTPUT_GET_PROPERTY(hdmiOutput, hdmioutput::Property::CONTENT_TYPE, propertyValue);
        UT_ASSERT_TRUE(propertyValue.has_value());
        UT_ASSERT_EQUAL(propertyValue->value->get<PropertyValue::Value::Tag::intValue>(), 
                        static_cast<int32_t>(hdmioutput::ContentType::UNSPECIFIED));
        UT_LOG_INFO("CONTENT_TYPE validated: UNSPECIFIED");

        // Test AFD property - default should be UNSPECIFIED (-1)
        propertyValue.reset();
        HDMIOUTPUT_GET_PROPERTY(hdmiOutput, hdmioutput::Property::AFD, propertyValue);
        UT_ASSERT_TRUE(propertyValue.has_value());
        UT_ASSERT_EQUAL(propertyValue->value->get<PropertyValue::Value::Tag::intValue>(), 
                        static_cast<int32_t>(hdmioutput::AFD::UNSPECIFIED));
        UT_LOG_INFO("AFD validated: UNSPECIFIED");

        // Test HDR_OUTPUT_MODE property - default should be AUTO (0)
        propertyValue.reset();
        HDMIOUTPUT_GET_PROPERTY(hdmiOutput, hdmioutput::Property::HDR_OUTPUT_MODE, propertyValue);
        UT_ASSERT_TRUE(propertyValue.has_value());
        UT_ASSERT_EQUAL(propertyValue->value->get<PropertyValue::Value::Tag::intValue>(), 
                        static_cast<int32_t>(hdmioutput::HDROutputMode::AUTO));
        UT_LOG_INFO("HDR_OUTPUT_MODE validated: AUTO");

        // Test SCAN_INFORMATION property - default should be NO_DATA (0)
        propertyValue.reset();
        HDMIOUTPUT_GET_PROPERTY(hdmiOutput, hdmioutput::Property::SCAN_INFORMATION, propertyValue);
        UT_ASSERT_TRUE(propertyValue.has_value());
        UT_ASSERT_EQUAL(propertyValue->value->get<PropertyValue::Value::Tag::intValue>(), 
                        static_cast<int32_t>(hdmioutput::ScanInformation::NO_DATA));
        UT_LOG_INFO("SCAN_INFORMATION validated: NO_DATA");
    }
}

/**
 * @brief Validate getProperty API negative scenario
 *
 * This test verifies that the getProperty API handles invalid property keys gracefully.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 017
 * **Priority:** High
 *
 * **Pre-Conditions:** None
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Get HDMI output object | port ID | Valid object returned | Should Pass |
 * | 02 | Query invalid property (below range) | invalid enum value | Returns no value or error | Should Pass |
 * | 03 | Query invalid property (out of bounds) | out-of-bound enum value | Returns no value or error | Should Pass |
 */ 
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getProperty_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    std::optional<PropertyValue> propertyValue;
    
    // Test with property value below valid range
    int32_t firstValue = static_cast<int32_t>(hdmioutput::Property::RESOURCE_ID);
    hdmioutput::Property invalidProperty = static_cast<hdmioutput::Property>(firstValue - 1);

    android::binder::Status status;
    HDMIOUTPUT_GET_PROPERTY_NEG(hdmiOutput, invalidProperty, propertyValue, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(propertyValue.has_value());

    // Test with property value out of bounds (above valid range)
    propertyValue.reset();
    hdmioutput::Property outOfBoundProperty = static_cast<hdmioutput::Property>(999);

    android::binder::Status status2;
    HDMIOUTPUT_GET_PROPERTY_NEG(hdmiOutput, outOfBoundProperty, propertyValue, status2);
    UT_ASSERT_FALSE(status2.isOk());
    UT_ASSERT_FALSE(propertyValue.has_value());
}

/**
 * @brief Validate controller start API positive scenario
 *
 * This test verifies that the start API successfully transitions from READY to STARTED state.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 018
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | valid listener | Controller returned, READY state | Should Pass |
 * | 02 | Call start on controller | none | State transitions to STARTED | Should Pass |
 * | 03 | Stop and close | controller | Cleanup successful | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, controller_start_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    UT_ASSERT_NOT_EQUAL(controller, nullptr);

    HDMIOUTPUT_START(controller);

    hdmioutput::State state;
    HDMIOUTPUT_GET_STATE(hdmiOutput, state);
    UT_ASSERT_EQUAL(state, hdmioutput::State::STARTED);

    // Cleanup
    HDMIOUTPUT_STOP(controller);
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate controller stop API positive scenario
 *
 * This test verifies that the stop API successfully transitions from STARTED to READY state.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 019
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in STARTED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Start HDMI output | controller | State is STARTED | Setup |
 * | 02 | Call stop on controller | none | State transitions to READY | Should Pass |
 * | 03 | Close output | controller | Cleanup successful | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, controller_stop_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    HDMIOUTPUT_START(controller);

    hdmioutput::State state;
    HDMIOUTPUT_GET_STATE(hdmiOutput, state);
    UT_ASSERT_EQUAL(state, hdmioutput::State::STARTED);

    HDMIOUTPUT_STOP(controller);

    HDMIOUTPUT_GET_STATE(hdmiOutput, state);
    UT_ASSERT_EQUAL(state, hdmioutput::State::READY);

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate controller start API negative scenario
 *
 * This test verifies that calling start in wrong state throws an exception.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 020
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in CLOSED or STARTED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Start HDMI output | controller | STARTED state | Setup |
 * | 02 | Call start again | none | Exception thrown | Should Pass |
 * | 03 | Stop and close | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, controller_start_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    HDMIOUTPUT_START(controller);

    // Try starting again - should fail with exception
    android::binder::Status status;
    HDMIOUTPUT_START_NEG(controller, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_EQUAL(status.exceptionCode(), android::binder::Status::EX_ILLEGAL_STATE);

    // Cleanup
    HDMIOUTPUT_STOP(controller);
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate controller stop API negative scenario
 *
 * This test verifies that calling stop in wrong state throws an exception.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 021
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Call stop without start | none | Exception thrown | Should Pass |
 * | 03 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, controller_stop_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    // Try stopping without starting - should fail
    android::binder::Status status;
    HDMIOUTPUT_STOP_NEG(controller, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_EQUAL(status.exceptionCode(), android::binder::Status::EX_ILLEGAL_STATE);

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}


/**
 * @brief Validate getHotPlugDetectState API
 *
 * This test verifies that the getHotPlugDetectState API returns a valid boolean value.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 022
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY or STARTED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Call getHotPlugDetectState | none | Returns boolean value | Should Pass |
 * | 03 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getHotPlugDetectState_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    UT_ASSERT_NOT_EQUAL(controller, nullptr);

    bool hpdState = false;
    HDMIOUTPUT_GET_HPD_STATE(controller, hpdState);

    UT_LOG_INFO("HPD State: %s", hpdState ? "Connected" : "Disconnected");

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate setProperty API positive scenario for all property types
 *
 * This test verifies that all properties can be set successfully on the controller using all supported values from HFP.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 024
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Set VIC property for all supported values | All VIC values from HFP | Returns true for each | Should Pass |
 * | 03 | Set CONTENT_TYPE property for all supported values | All ContentType values from HFP | Returns true for each | Should Pass |
 * | 04 | Set AFD property for all supported values | All AFD values from HFP | Returns true for each | Should Pass |
 * | 05 | Set HDR_OUTPUT_MODE property for all supported values | All HDROutputMode values from HFP | Returns true for each | Should Pass |
 * | 06 | Set SCAN_INFORMATION property for all supported values | All ScanInformation values from HFP | Returns true for each | Should Pass |
 * | 07 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, setProperty_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);
    UT_ASSERT_NOT_EQUAL(controller, nullptr);

    PropertyValue propValue;
    bool setResult = false;

    // Test VIC property - loop through all supported VIC values from profile
    int vicCount = getProfileSupportedVICCount(0);
    for (int i = 0; i < vicCount; i++)
    {
        int32_t vicValue = getProfileSupportedVIC(0, i);
        propValue.value = PropertyValue::Value();
        propValue.value.value().set<PropertyValue::Value::Tag::intValue>(vicValue);
        setResult = false;
        HDMIOUTPUT_SET_PROPERTY(controller, hdmioutput::Property::VIC, propValue, setResult);
        UT_ASSERT_TRUE(setResult);
        UT_LOG_INFO("VIC property set successfully: %d", vicValue);
    }

    // Test CONTENT_TYPE property - loop through all supported content type values from profile
    int contentTypeCount = getProfileSupportedContentTypeCount(0);
    for (int i = 0; i < contentTypeCount; i++)
    {
        int32_t contentTypeValue = getProfileSupportedContentType(0, i);
        propValue.value = PropertyValue::Value();
        propValue.value.value().set<PropertyValue::Value::Tag::intValue>(contentTypeValue);
        setResult = false;
        HDMIOUTPUT_SET_PROPERTY(controller, hdmioutput::Property::CONTENT_TYPE, propValue, setResult);
        UT_ASSERT_TRUE(setResult);
        UT_LOG_INFO("CONTENT_TYPE property set successfully: %d", contentTypeValue);
    }

    // Test AFD property - loop through all supported AFD values from profile
    int afdCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources/0/supportedAFD");
    for (int i = 0; i < afdCount; i++)
    {
        int32_t afdValue = getProfileSupportedAFD(0, i);
        propValue.value = PropertyValue::Value();
        propValue.value.value().set<PropertyValue::Value::Tag::intValue>(afdValue);
        setResult = false;
        HDMIOUTPUT_SET_PROPERTY(controller, hdmioutput::Property::AFD, propValue, setResult);
        UT_ASSERT_TRUE(setResult);
        UT_LOG_INFO("AFD property set successfully: %d", afdValue);
    }

    // Test HDR_OUTPUT_MODE property - loop through all supported HDR mode values from profile
    int hdrModeCount = getProfileSupportedHDRModeCount(0);
    for (int i = 0; i < hdrModeCount; i++)
    {
        std::string hdrModeStr = getProfileSupportedHDRMode(0, i);
        hdmioutput::HDROutputMode hdrMode = convertStringToHDROutputMode(hdrModeStr);
        propValue.value = PropertyValue::Value();
        propValue.value.value().set<PropertyValue::Value::Tag::intValue>(static_cast<int32_t>(hdrMode));
        setResult = false;
        HDMIOUTPUT_SET_PROPERTY(controller, hdmioutput::Property::HDR_OUTPUT_MODE, propValue, setResult);
        UT_ASSERT_TRUE(setResult);
        UT_LOG_INFO("HDR_OUTPUT_MODE property set successfully: %s", hdrModeStr.c_str());
    }

    // Test SCAN_INFORMATION property - loop through all supported scan info values from profile
    int scanInfoCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources/0/supportedScanInformation");
    for (int i = 0; i < scanInfoCount; i++)
    {
        int32_t scanInfoValue = getProfileSupportedScanInfo(0, i);
        propValue.value = PropertyValue::Value();
        propValue.value.value().set<PropertyValue::Value::Tag::intValue>(scanInfoValue);
        setResult = false;
        HDMIOUTPUT_SET_PROPERTY(controller, hdmioutput::Property::SCAN_INFORMATION, propValue, setResult);
        UT_ASSERT_TRUE(setResult);
        UT_LOG_INFO("SCAN_INFORMATION property set successfully: %d", scanInfoValue);
    }

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate setProperty API negative scenario
 *
 * This test verifies that setting invalid properties and property values fails gracefully.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 025
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Set invalid property (below range) | invalid enum value | Returns false | Should Pass |
 * | 03 | Set invalid property (out of bounds) | out-of-bound enum value | Returns false | Should Pass |
 * | 04 | Set VIC with out of bound value (high) | VIC property, value=9999 | Returns false | Should Pass |
 * | 05 | Set CONTENT_TYPE with out of bound value (high) | CONTENT_TYPE property, value=100 | Returns false | Should Pass |
 * | 06 | Set AFD with out of bound value (high) | AFD property, value=100 | Returns false | Should Pass |
 * | 07 | Set HDR_OUTPUT_MODE with out of bound value (high) | HDR_OUTPUT_MODE property, value=100 | Returns false | Should Pass |
 * | 08 | Set SCAN_INFORMATION with out of bound value (high) | SCAN_INFORMATION property, value=100 | Returns false | Should Pass |
 * | 09 | Set VIC with value below first enum | VIC property, value=-1 | Returns false | Should Pass |
 * | 10 | Set CONTENT_TYPE with value below first enum | CONTENT_TYPE property, value=-2 | Returns false | Should Pass |
 * | 11 | Set AFD with value below first enum | AFD property, value=-2 | Returns false | Should Pass |
 * | 12 | Set HDR_OUTPUT_MODE with value below first enum | HDR_OUTPUT_MODE property, value=-1 | Returns false | Should Pass |
 * | 13 | Set SCAN_INFORMATION with value below first enum | SCAN_INFORMATION property, value=-1 | Returns false | Should Pass |
 * | 14 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, setProperty_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    PropertyValue propValue;
    bool setResult = false;
    android::binder::Status status;

    // Test 1: Try setting invalid property (below range)
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(0);

    int32_t firstValue = static_cast<int32_t>(hdmioutput::Property::RESOURCE_ID);
    hdmioutput::Property invalidProperty = static_cast<hdmioutput::Property>(firstValue - 1);

    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, invalidProperty, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("Invalid property (below range) rejected successfully");

    // Test 2: Try setting out of bound property (above range)
    hdmioutput::Property outOfBoundProperty = static_cast<hdmioutput::Property>(999);
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, outOfBoundProperty, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("Out of bound property rejected successfully");

    // Test 3: Try setting VIC with out of bound value
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(9999); // Invalid VIC value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::VIC, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("VIC with out of bound value rejected successfully");

    // Test 4: Try setting CONTENT_TYPE with out of bound value
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(100); // Invalid ContentType value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::CONTENT_TYPE, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("CONTENT_TYPE with out of bound value rejected successfully");

    // Test 5: Try setting AFD with out of bound value
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(100); // Invalid AFD value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::AFD, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("AFD with out of bound value rejected successfully");

    // Test 6: Try setting HDR_OUTPUT_MODE with out of bound value
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(100); // Invalid HDROutputMode value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::HDR_OUTPUT_MODE, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("HDR_OUTPUT_MODE with out of bound value rejected successfully");

    // Test 7: Try setting SCAN_INFORMATION with out of bound value
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(100); // Invalid ScanInformation value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::SCAN_INFORMATION, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("SCAN_INFORMATION with out of bound value rejected successfully");

    // Test 8: Try setting VIC with value below first enum (VIC0_UNAVAILABLE = 0, test with -1)
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(-1); // Below minimum VIC value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::VIC, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("VIC with value below first enum rejected successfully");

    // Test 9: Try setting CONTENT_TYPE with value below first enum (UNSPECIFIED = -1, test with -2)
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(-2); // Below minimum ContentType value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::CONTENT_TYPE, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("CONTENT_TYPE with value below first enum rejected successfully");

    // Test 10: Try setting AFD with value below first enum (UNSPECIFIED = -1, test with -2)
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(-2); // Below minimum AFD value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::AFD, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("AFD with value below first enum rejected successfully");

    // Test 11: Try setting HDR_OUTPUT_MODE with value below first enum (AUTO = 0, test with -1)
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(-1); // Below minimum HDROutputMode value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::HDR_OUTPUT_MODE, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("HDR_OUTPUT_MODE with value below first enum rejected successfully");

    // Test 12: Try setting SCAN_INFORMATION with value below first enum (NO_DATA = 0, test with -1)
    propValue.value = PropertyValue::Value();
    propValue.value.value().set<PropertyValue::Value::Tag::intValue>(-1); // Below minimum ScanInformation value
    setResult = false;
    HDMIOUTPUT_SET_PROPERTY_NEG(controller, hdmioutput::Property::SCAN_INFORMATION, propValue, setResult, status);
    UT_ASSERT_FALSE(status.isOk());
    UT_ASSERT_FALSE(setResult);
    UT_LOG_INFO("SCAN_INFORMATION with value below first enum rejected successfully");

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate setPropertyMulti API
 *
 * This test verifies that multiple properties can be set simultaneously in various combinations.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 026
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Set all properties together with different value combinations | All property values from HFP | Returns true for each combination | Should Pass |
 * | 03 | Verify all properties after each set operation | All properties | Values match what was set | Should Pass |
 * | 04 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, setPropertyMulti_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    // Build list of settable properties with their value counts
    struct PropertyInfo {
        hdmioutput::Property property;
        int valueCount;
        std::string name;
    };
    std::vector<PropertyInfo> settableProperties;

    // VIC property
    int vicCount = getProfileSupportedVICCount(0);
    if (vicCount > 0) {
        settableProperties.push_back({hdmioutput::Property::VIC, vicCount, "VIC"});
    }

    // CONTENT_TYPE property
    int contentTypeCount = getProfileSupportedContentTypeCount(0);
    if (contentTypeCount > 0) {
        settableProperties.push_back({hdmioutput::Property::CONTENT_TYPE, contentTypeCount, "CONTENT_TYPE"});
    }

    // AFD property
    int afdCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources/0/supportedAFD");
    if (afdCount > 0) {
        settableProperties.push_back({hdmioutput::Property::AFD, afdCount, "AFD"});
    }

    // HDR_OUTPUT_MODE property
    int hdrModeCount = getProfileSupportedHDRModeCount(0);
    if (hdrModeCount > 0) {
        settableProperties.push_back({hdmioutput::Property::HDR_OUTPUT_MODE, hdrModeCount, "HDR_OUTPUT_MODE"});
    }

    // SCAN_INFORMATION property
    int scanInfoCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources/0/supportedScanInformation");
    if (scanInfoCount > 0) {
        settableProperties.push_back({hdmioutput::Property::SCAN_INFORMATION, scanInfoCount, "SCAN_INFORMATION"});
    }

    UT_ASSERT_TRUE(settableProperties.size() > 0);
    UT_LOG_INFO("Found %zu settable properties", settableProperties.size());

    // Find minimum value count to determine test iterations
    int minValueCount = settableProperties[0].valueCount;
    for (const auto& propInfo : settableProperties) {
        if (propInfo.valueCount < minValueCount) {
            minValueCount = propInfo.valueCount;
        }
    }
    UT_LOG_INFO("Testing %d value combinations", minValueCount);

    // Test multiple value combinations
    for (int valueIdx = 0; valueIdx < minValueCount; valueIdx++) {
        UT_LOG_INFO("Setting all properties with value index %d", valueIdx);
        
        std::vector<hdmioutput::PropertyKVPair> propertyList;
        
        // Build property list with values at current index
        for (const auto& propInfo : settableProperties) {
            hdmioutput::PropertyKVPair kvPair;
            kvPair.property = propInfo.property;
            kvPair.propertyValue.value = PropertyValue::Value();
            
            // Get value from profile based on property type
            switch (propInfo.property) {
                case hdmioutput::Property::VIC: {
                    int32_t vic = getProfileSupportedVIC(0, valueIdx);
                    kvPair.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(vic);
                    UT_LOG_INFO("  %s value[%d]: %d", propInfo.name.c_str(), valueIdx, vic);
                    break;
                }
                case hdmioutput::Property::CONTENT_TYPE: {
                    int32_t contentType = getProfileSupportedContentType(0, valueIdx);
                    kvPair.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(contentType);
                    UT_LOG_INFO("  %s value[%d]: %d", propInfo.name.c_str(), valueIdx, contentType);
                    break;
                }
                case hdmioutput::Property::AFD: {
                    int32_t afd = getProfileSupportedAFD(0, valueIdx);
                    kvPair.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(afd);
                    UT_LOG_INFO("  %s value[%d]: %d", propInfo.name.c_str(), valueIdx, afd);
                    break;
                }
                case hdmioutput::Property::HDR_OUTPUT_MODE: {
                    std::string hdrModeStr = getProfileSupportedHDRMode(0, valueIdx);
                    hdmioutput::HDROutputMode hdrMode = convertStringToHDROutputMode(hdrModeStr);
                    kvPair.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(static_cast<int32_t>(hdrMode));
                    UT_LOG_INFO("  %s value[%d]: %s", propInfo.name.c_str(), valueIdx, hdrModeStr.c_str());
                    break;
                }
                case hdmioutput::Property::SCAN_INFORMATION: {
                    int32_t scanInfo = getProfileSupportedScanInfo(0, valueIdx);
                    kvPair.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(scanInfo);
                    UT_LOG_INFO("  %s value[%d]: %d", propInfo.name.c_str(), valueIdx, scanInfo);
                    break;
                }
                default:
                    break;
            }
            
            propertyList.push_back(kvPair);
        }
        
        // Set all properties at once
        bool setResult = false;
        HDMIOUTPUT_SET_PROPERTYMULTI(controller, propertyList, setResult);
        UT_ASSERT_TRUE(setResult);
        UT_LOG_INFO("setPropertyMulti completed successfully for value index %d", valueIdx);
        
    }

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate setPropertyMulti API negative scenarios
 *
 * This test verifies that setPropertyMulti API handles invalid properties and values gracefully.
 *
 * **Test Group ID:** Negative: 01
 * **Test Case ID:** 027
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Set multiple properties with one invalid property type (below range) | Invalid property enum | Returns false | Should Pass |
 * | 03 | Set multiple properties with one out-of-bound property (above range) | out-of-bound enum value | Returns false | Should Pass |
 * | 04 | Set multiple properties with VIC out-of-bound value (high) | VIC with value 9999 | Returns false | Should Pass |
 * | 05 | Set multiple properties with CONTENT_TYPE out-of-bound value (low) | CONTENT_TYPE with value -2 | Returns false | Should Pass |
 * | 06 | Set multiple properties with VIC out-of-bound value (low) | VIC with value -1 | Returns false | Should Pass |
 * | 07 | Set multiple properties with CONTENT_TYPE out-of-bound value (high) | CONTENT_TYPE with value 100 | Returns false | Should Pass |
 * | 08 | Set multiple properties with AFD out-of-bound value (high) | AFD with value 100 | Returns false | Should Pass |
 * | 09 | Set multiple properties with AFD out-of-bound value (low) | AFD with value -2 | Returns false | Should Pass |
 * | 10 | Set multiple properties with HDR_OUTPUT_MODE out-of-bound value (high) | HDR_OUTPUT_MODE with value 100 | Returns false | Should Pass |
 * | 11 | Set multiple properties with HDR_OUTPUT_MODE out-of-bound value (low) | HDR_OUTPUT_MODE with value -1 | Returns false | Should Pass |
 * | 12 | Set multiple properties with SCAN_INFORMATION out-of-bound value (high) | SCAN_INFORMATION with value 100 | Returns false | Should Pass |
 * | 13 | Set multiple properties with SCAN_INFORMATION out-of-bound value (low) | SCAN_INFORMATION with value -1 | Returns false | Should Pass |
 * | 14 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, setPropertyMulti_neg)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    // Test 1: Invalid property type (below range)
    {
        std::vector<hdmioutput::PropertyKVPair> properties;
        
        // Add valid property
        hdmioutput::PropertyKVPair validProp;
        validProp.property = hdmioutput::Property::VIC;
        validProp.propertyValue.value = PropertyValue::Value();
        validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedVIC(0, 0));
        properties.push_back(std::move(validProp));

        // Add invalid property type (below range)
        hdmioutput::PropertyKVPair invalidProp;
        int32_t firstValue = static_cast<int32_t>(hdmioutput::Property::RESOURCE_ID);
        invalidProp.property = static_cast<hdmioutput::Property>(firstValue - 1);
        invalidProp.propertyValue.value = PropertyValue::Value();
        invalidProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(0);
        properties.push_back(std::move(invalidProp));

        bool setResult = false;
        android::binder::Status status;
        HDMIOUTPUT_SET_PROPERTYMULTI_NEG(controller, properties, setResult, status);
        UT_ASSERT_FALSE(status.isOk());
        UT_ASSERT_FALSE(setResult);
        UT_LOG_INFO("Invalid property type (below range) rejected successfully");
    }

    // Test 2: Invalid property type (above range)
    {
        std::vector<hdmioutput::PropertyKVPair> properties;
        
        // Add valid property
        hdmioutput::PropertyKVPair validProp;
        validProp.property = hdmioutput::Property::CONTENT_TYPE;
        validProp.propertyValue.value = PropertyValue::Value();
        validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedContentType(0, 0));
        properties.push_back(std::move(validProp));

        // Add invalid property type (above range)
        hdmioutput::PropertyKVPair outOfBoundProp;
        outOfBoundProp.property = static_cast<hdmioutput::Property>(999);
        outOfBoundProp.propertyValue.value = PropertyValue::Value();
        outOfBoundProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(0);
        properties.push_back(std::move(outOfBoundProp));

        bool setResult = false;
        android::binder::Status status;
        HDMIOUTPUT_SET_PROPERTYMULTI_NEG(controller, properties, setResult, status);
        UT_ASSERT_FALSE(status.isOk());
        UT_ASSERT_FALSE(setResult);
        UT_LOG_INFO("Invalid property type (above range) rejected successfully");
    }

    // Build list of settable properties with boundary information
    struct PropertyBoundary {
        hdmioutput::Property property;
        std::string name;
        int32_t lowInvalid;   // Invalid low boundary value
        int32_t highInvalid;  // Invalid high boundary value
        
        // Helper to get a valid property for combining with invalid one
        hdmioutput::Property getValidCompanionProperty(size_t index) const {
            const hdmioutput::Property companions[] = {
                hdmioutput::Property::VIC,
                hdmioutput::Property::CONTENT_TYPE,
                hdmioutput::Property::AFD,
                hdmioutput::Property::HDR_OUTPUT_MODE,
                hdmioutput::Property::SCAN_INFORMATION
            };
            for (size_t i = 0; i < 5; i++) {
                if (companions[(index + i) % 5] != property) {
                    return companions[(index + i) % 5];
                }
            }
            return hdmioutput::Property::VIC;
        }
    };
    
    std::vector<PropertyBoundary> propertyBoundaries;

    // VIC: valid range starts at 0 (VIC0_UNAVAILABLE)
    int vicCount = getProfileSupportedVICCount(0);
    if (vicCount > 0) {
        propertyBoundaries.push_back({hdmioutput::Property::VIC, "VIC", -1, 9999});
    }

    // CONTENT_TYPE: valid range starts at -1 (UNSPECIFIED)
    int contentTypeCount = getProfileSupportedContentTypeCount(0);
    if (contentTypeCount > 0) {
        propertyBoundaries.push_back({hdmioutput::Property::CONTENT_TYPE, "CONTENT_TYPE", -2, 100});
    }

    // AFD: valid range starts at -1 (UNSPECIFIED)
    int afdCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources/0/supportedAFD");
    if (afdCount > 0) {
        propertyBoundaries.push_back({hdmioutput::Property::AFD, "AFD", -2, 100});
    }

    // HDR_OUTPUT_MODE: valid range starts at 0 (AUTO)
    int hdrModeCount = getProfileSupportedHDRModeCount(0);
    if (hdrModeCount > 0) {
        propertyBoundaries.push_back({hdmioutput::Property::HDR_OUTPUT_MODE, "HDR_OUTPUT_MODE", -1, 100});
    }

    // SCAN_INFORMATION: valid range starts at 0 (NO_DATA)
    int scanInfoCount = UT_KVP_PROFILE_GET_LIST_COUNT("hdmioutput/resources/0/supportedScanInformation");
    if (scanInfoCount > 0) {
        propertyBoundaries.push_back({hdmioutput::Property::SCAN_INFORMATION, "SCAN_INFORMATION", -1, 100});
    }

    UT_ASSERT_TRUE(propertyBoundaries.size() > 0);
    UT_LOG_INFO("Testing boundary violations for %zu properties", propertyBoundaries.size());

    // Test high out-of-bound values for each property
    for (size_t i = 0; i < propertyBoundaries.size(); i++) {
        const auto& pb = propertyBoundaries[i];
        std::vector<hdmioutput::PropertyKVPair> properties;
        
        // Add a valid companion property
        hdmioutput::PropertyKVPair validProp;
        hdmioutput::Property companionProp = pb.getValidCompanionProperty(i);
        validProp.property = companionProp;
        validProp.propertyValue.value = PropertyValue::Value();
        
        // Get valid value for companion property
        switch (companionProp) {
            case hdmioutput::Property::VIC:
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedVIC(0, 0));
                break;
            case hdmioutput::Property::CONTENT_TYPE:
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedContentType(0, 0));
                break;
            case hdmioutput::Property::AFD:
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedAFD(0, 0));
                break;
            case hdmioutput::Property::HDR_OUTPUT_MODE: {
                std::string hdrModeStr = getProfileSupportedHDRMode(0, 0);
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(static_cast<int32_t>(convertStringToHDROutputMode(hdrModeStr)));
                break;
            }
            case hdmioutput::Property::SCAN_INFORMATION:
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedScanInfo(0, 0));
                break;
            default:
                break;
        }
        properties.push_back(std::move(validProp));

        // Add property with high out-of-bound value
        hdmioutput::PropertyKVPair highValueProp;
        highValueProp.property = pb.property;
        highValueProp.propertyValue.value = PropertyValue::Value();
        highValueProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(pb.highInvalid);
        properties.push_back(std::move(highValueProp));

        bool setResult = false;
        android::binder::Status status;
        HDMIOUTPUT_SET_PROPERTYMULTI_NEG(controller, properties, setResult, status);
        UT_ASSERT_FALSE(status.isOk());
        UT_ASSERT_FALSE(setResult);
        UT_LOG_INFO("%s with high out-of-bound value (%d) rejected successfully", pb.name.c_str(), pb.highInvalid);
    }

    // Test low out-of-bound values for each property
    for (size_t i = 0; i < propertyBoundaries.size(); i++) {
        const auto& pb = propertyBoundaries[i];
        std::vector<hdmioutput::PropertyKVPair> properties;
        
        // Add a valid companion property
        hdmioutput::PropertyKVPair validProp;
        hdmioutput::Property companionProp = pb.getValidCompanionProperty(i + 1);
        validProp.property = companionProp;
        validProp.propertyValue.value = PropertyValue::Value();
        
        // Get valid value for companion property
        switch (companionProp) {
            case hdmioutput::Property::VIC:
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedVIC(0, 0));
                break;
            case hdmioutput::Property::CONTENT_TYPE:
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedContentType(0, 0));
                break;
            case hdmioutput::Property::AFD:
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedAFD(0, 0));
                break;
            case hdmioutput::Property::HDR_OUTPUT_MODE: {
                std::string hdrModeStr = getProfileSupportedHDRMode(0, 0);
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(static_cast<int32_t>(convertStringToHDROutputMode(hdrModeStr)));
                break;
            }
            case hdmioutput::Property::SCAN_INFORMATION:
                validProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(getProfileSupportedScanInfo(0, 0));
                break;
            default:
                break;
        }
        properties.push_back(std::move(validProp));

        // Add property with low out-of-bound value
        hdmioutput::PropertyKVPair lowValueProp;
        lowValueProp.property = pb.property;
        lowValueProp.propertyValue.value = PropertyValue::Value();
        lowValueProp.propertyValue.value.value().set<PropertyValue::Value::Tag::intValue>(pb.lowInvalid);
        properties.push_back(std::move(lowValueProp));

        bool setResult = false;
        android::binder::Status status;
        HDMIOUTPUT_SET_PROPERTYMULTI_NEG(controller, properties, setResult, status);
        UT_ASSERT_FALSE(status.isOk());
        UT_ASSERT_FALSE(setResult);
        UT_LOG_INFO("%s with low out-of-bound value (%d) rejected successfully", pb.name.c_str(), pb.lowInvalid);
    }

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate getHDCPCurrentVersion API
 *
 * This test verifies that the current HDCP version can be queried.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 028
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY or STARTED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Get HDCP current version | none | Returns valid version enum | Should Pass |
 * | 03 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getHDCPCurrentVersion_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    hdmioutput::HDCPProtocolVersion version;
    HDMIOUTPUT_GET_HDCP_CURRENT_VERSION(controller, version);

    // Version should be UNDEFINED initially (no active HDCP session)
    UT_ASSERT_EQUAL(version, hdmioutput::HDCPProtocolVersion::UNDEFINED);

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate getHDCPReceiverVersion API
 *
 * This test verifies that the sink's HDCP capabilities can be queried.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 029
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY or STARTED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Get HDCP receiver version | none | Returns valid version enum | Should Pass |
 * | 03 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getHDCPReceiverVersion_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    hdmioutput::HDCPProtocolVersion version;
    HDMIOUTPUT_GET_HDCP_RECEIVER_VERSION(controller, version);
    // Version should be UNDEFINED initially (no active HDCP session)
    UT_ASSERT_EQUAL(version, hdmioutput::HDCPProtocolVersion::UNDEFINED);

    UT_LOG_INFO("HDCP Receiver Version: %d", (int)version);

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate getHDCPStatus API
 *
 * This test verifies that HDCP authentication status can be queried.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 030
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY or STARTED state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Get HDCP status | none | Returns valid status enum | Should Pass |
 * | 03 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getHDCPStatus_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    hdmioutput::HDCPStatus hdcpStatus;
    HDMIOUTPUT_GET_HDCP_STATUS(controller, hdcpStatus);

    // Status should be UNKNOWN initially (no active HDCP session)
    UT_ASSERT_EQUAL(hdcpStatus, hdmioutput::HDCPStatus::UNKNOWN);

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate setSPDInfoFrame and getSPDInfoFrame APIs
 *
 * This test verifies that SPD InfoFrame can be set and retrieved.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 031
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Set SPD InfoFrame | SPD data | Success | Should Pass |
 * | 03 | Get SPD InfoFrame | none | Returns same SPD data | Should Pass |
 * | 04 | Set null SPD | null | Clears SPD | Should Pass |
 * | 05 | Get SPD after clear | none | Returns null | Should Pass |
 * | 06 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, setSPDInfoFrame_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    // Create SPD InfoFrame
    hdmioutput::SPDInfoFrame spd;
    std::string vendorName = "RDK";
    std::string productDesc = "TestBox";
    std::copy_n(vendorName.begin(), std::min(vendorName.size(), spd.vendorName.size()), spd.vendorName.begin());
    std::copy_n(productDesc.begin(), std::min(productDesc.size(), spd.productDescription.size()), spd.productDescription.begin());
    spd.spdSourceInformation = hdmioutput::SPDSource::DIGITAL_STB;

    // Set SPD
    HDMIOUTPUT_SET_SPD_INFOFRAME(controller, std::optional<hdmioutput::SPDInfoFrame>(spd));

    // Get SPD and verify
    std::optional<hdmioutput::SPDInfoFrame> retrievedSpd;
    HDMIOUTPUT_GET_SPD_INFOFRAME(controller, retrievedSpd);
    UT_ASSERT_TRUE(retrievedSpd.has_value());

    // Clear SPD
    HDMIOUTPUT_SET_SPD_INFOFRAME(controller, std::nullopt);

    // Verify cleared
    retrievedSpd.reset();
    HDMIOUTPUT_GET_SPD_INFOFRAME(controller, retrievedSpd);
    UT_ASSERT_FALSE(retrievedSpd.has_value());

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}

/**
 * @brief Validate getSPDInfoFrame returns null when not set
 *
 * This test verifies that getSPDInfoFrame returns null when no SPD has been configured.
 *
 * **Test Group ID:** Basic: 01
 * **Test Case ID:** 032
 * **Priority:** High
 *
 * **Pre-Conditions:** HDMI output in READY state
 * **Dependencies:** None
 * **User Interaction:** None
 *
 * **Test Procedure:**
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Open HDMI output | controller | READY state | Setup |
 * | 02 | Get SPD without setting | none | Returns null | Should Pass |
 * | 03 | Close output | controller | Cleanup | Cleanup |
 */
UT_ADD_TEST(VTS_L1_HDMIOUTPUT, getSPDInfoFrame_pos)
{
    android::sp<hdmioutput::IHDMIOutput> hdmiOutput = getHDMIOutput(0);
    UT_ASSERT_NOT_EQUAL(hdmiOutput, nullptr);

    android::sp<HDMIOutputControllerListener> controllerListener = new HDMIOutputControllerListener();
    android::sp<hdmioutput::IHDMIOutputController> controller;

    HDMIOUTPUT_OPEN(hdmiOutput, controllerListener, controller);

    // Get SPD before setting - should return null
    std::optional<hdmioutput::SPDInfoFrame> spd;
    HDMIOUTPUT_GET_SPD_INFOFRAME(controller, spd);
    UT_ASSERT_FALSE(spd.has_value());

    // Cleanup
    bool closeResult = false;
    HDMIOUTPUT_CLOSE(hdmiOutput, controller, closeResult);
}


/** @} */ // End of HDMI_OUTPUT_HALTEST_L1
/** @} */ // End of HDMI_OUTPUT_HALTEST
/** @} */ // End of HDMI_OUTPUT
/** @} */ // End of HPK
