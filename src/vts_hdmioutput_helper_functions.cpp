/**
 *  If not stated otherwise in this file or this component's LICENSE
 *  file the following copyright and licenses apply:
 *
 *  Copyright 2026 RDK Management
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
 * @defgroup HDMIOUTPUT_HALTEST_HELPER HDMIOUTPUT HAL Tests helper functions
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
 * @file vts_hdmioutput_helper_functions.cpp
 *
 */

#include "vts_hdmioutput_helper_functions.h"

const ut_control_keyStringMapping_t bool_mapTable_hdmioutput[] =
{
    {"false", (int32_t) false},
    {"true",  (int32_t) true},
    {NULL, -1}
};
/* Status::Exception */
const ut_control_keyStringMapping_t statusHdmiOutputException_mapTable[] =
{
    {"EX_NONE",                  (int32_t) Status::EX_NONE},
    {"EX_SECURITY",              (int32_t) Status::EX_SECURITY},
    {"EX_BAD_PARCELABLE",        (int32_t) Status::EX_BAD_PARCELABLE},
    {"EX_ILLEGAL_ARGUMENT",      (int32_t) Status::EX_ILLEGAL_ARGUMENT},
    {"EX_NULL_POINTER",          (int32_t) Status::EX_NULL_POINTER},
    {"EX_ILLEGAL_STATE",         (int32_t) Status::EX_ILLEGAL_STATE},
    {"EX_NETWORK_MAIN_THREAD",   (int32_t) Status::EX_NETWORK_MAIN_THREAD},
    {"EX_UNSUPPORTED_OPERATION", (int32_t) Status::EX_UNSUPPORTED_OPERATION},
    {"EX_SERVICE_SPECIFIC",      (int32_t) Status::EX_SERVICE_SPECIFIC},
    {"EX_PARCELABLE",            (int32_t) Status::EX_PARCELABLE},
    {"EX_HAS_REPLY_HEADER",      (int32_t) Status::EX_HAS_REPLY_HEADER},
    {"EX_TRANSACTION_FAILED",    (int32_t) Status::EX_TRANSACTION_FAILED},
    {NULL, -1}
};

/* HDCPProtocolVersion */
const ut_control_keyStringMapping_t hdcpProtocolVersion_mapTable[] =
{
    {"UNDEFINED",    (int32_t) HDCPProtocolVersion::UNDEFINED},
    {"VERSION_1_X",  (int32_t) HDCPProtocolVersion::VERSION_1_X},
    {"VERSION_2_X",  (int32_t) HDCPProtocolVersion::VERSION_2_X},
    {NULL, -1}
};

/* HDCPStatus */
const ut_control_keyStringMapping_t hdcpStatus_mapTable[] =
{
    {"UNKNOWN",                     (int32_t) hdmioutput::HDCPStatus::UNKNOWN},
    {"UNAUTHENTICATED",             (int32_t) hdmioutput::HDCPStatus::UNAUTHENTICATED},
    {"AUTHENTICATION_IN_PROGRESS",  (int32_t) hdmioutput::HDCPStatus::AUTHENTICATION_IN_PROGRESS},
    {"AUTHENTICATED",               (int32_t) hdmioutput::HDCPStatus::AUTHENTICATED},
    {NULL, -1}
};

std::string vecIntToString(std::vector<int32_t>& addrs)
{
    std::string s;
    for (size_t i = 0; i < addrs.size(); ++i)
    {
        s += std::to_string(addrs[i]);
        if (i != addrs.size() - 1) s += ", ";
    }
    return s;
}

std::string vecByteToHexString(const std::vector<uint8_t>& data)
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex;  // uppercase hex letters
    for (size_t i = 0; i < data.size(); ++i)
    {
        oss << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
        if (i + 1 < data.size())
            oss << ",";
    }
    return oss.str();
}

HDROutputMode convertStringToHDROutputMode(const std::string &val) {
  HDROutputMode mode;
  if (val == "AUTO") {
    mode = HDROutputMode::AUTO;
  } else if (val == "HLG") {
    mode = HDROutputMode::HLG;
  } else if (val == "HDR10") {
    mode = HDROutputMode::HDR10;
  } else if (val == "HDR10_PLUS") {
    mode = HDROutputMode::HDR10_PLUS;
  } else if (val == "DOLBY_VISION") {
    mode = HDROutputMode::DOLBY_VISION;
  }
  return mode;
}

HDCPProtocolVersion convertStringToHDCPProtocolVersions(const std::string &val) {
  HDCPProtocolVersion mode;
  if (val == "VERSION_1_X") {
    mode = HDCPProtocolVersion::VERSION_1_X;
  } else if (val == "VERSION_2_X") {
    mode = HDCPProtocolVersion::VERSION_2_X;
  } else {
    mode = HDCPProtocolVersion::UNDEFINED;
  }
  return mode;
}

Colorimetry convertStringToColorimetry(const std::string &val) {
  Colorimetry mode;
  if (val == "NO_DATA") {
    mode = Colorimetry::NO_DATA;
  } else if (val == "SMPTE_170M") {
    mode = Colorimetry::SMPTE_170M;
  } else if (val == "ITU_R_BT709") {
    mode = Colorimetry::ITU_R_BT709;
  } else if (val == "EXTENDED_COLORIMETRY") {
    mode = Colorimetry::EXTENDED_COLORIMETRY;
  }
  return mode;
}

PixelFormat convertStringToPixelFormat(const std::string &val) {
  PixelFormat mode;
  if (val == "RGB_444") {
    mode = PixelFormat::RGB_444;
  } else if (val == "YCBCR_422") {
    mode = PixelFormat::YCBCR_422;
  } else if (val == "YCBCR_444") {
    mode = PixelFormat::YCBCR_444;
  } else if (val == "YCBCR_420") {
    mode = PixelFormat::YCBCR_420;
  }
  return mode;
}

SPDSource convertStringToSPDSource(const std::string &val) {
  SPDSource source;
  if (val == "UNKNOWN") {
    source = SPDSource::UNKNOWN;
  } else if (val == "DIGITAL_STB") {
    source = SPDSource::DIGITAL_STB;
  } else if (val == "DVD_PLAYER") {
    source = SPDSource::DVD_PLAYER;
  } else if (val == "D_VHS") {
    source = SPDSource::D_VHS;
  } else if (val == "HDD_VIDEORECORDER") {
    source = SPDSource::HDD_VIDEORECORDER;
  } else if (val == "DVC") {
    source = SPDSource::DVC;
  } else if (val == "DSC") {
    source = SPDSource::DSC;
  } else if (val == "VIDEO_CD") {
    source = SPDSource::VIDEO_CD;
  } else if (val == "GAME") {
    source = SPDSource::GAME;
  } else if (val == "PC_GENERAL") {
    source = SPDSource::PC_GENERAL;
  } else if (val == "BLU_RAY_DISC") {
    source = SPDSource::BLU_RAY_DISC;
  } else if (val == "SUPER_AUDIO_CD") {
    source = SPDSource::SUPER_AUDIO_CD;
  } else if (val == "HD_DVD") {
    source = SPDSource::HD_DVD;
  } else {
    source = SPDSource::UNKNOWN;
  }
  return source;
}

FreeSync convertStringToFreeSync(const std::string &val) {
  FreeSync freeSync;
  if (val == "UNSUPPORTED") {
    freeSync = FreeSync::UNSUPPORTED;
  } else if (val == "FREESYNC") {
    freeSync = FreeSync::FREESYNC;
  } else if (val == "FREESYNC_PREMIUM") {
    freeSync = FreeSync::FREESYNC_PREMIUM;
  } else if (val == "FREESYNC_PREMIUM_PRO") {
    freeSync = FreeSync::FREESYNC_PREMIUM_PRO;
  } else {
    freeSync = FreeSync::UNSUPPORTED;
  }
  return freeSync;
}

HDMIVersion convertStringToHDMIVersion(const std::string &val) {
  HDMIVersion version;
  if (val == "HDMI_1_3") {
    version = HDMIVersion::HDMI_1_3;
  } else if (val == "HDMI_1_4") {
    version = HDMIVersion::HDMI_1_4;
  } else if (val == "HDMI_2_0") {
    version = HDMIVersion::HDMI_2_0;
  } else if (val == "HDMI_2_1") {
    version = HDMIVersion::HDMI_2_1;
  } else {
    version = HDMIVersion::HDMI_1_3;
  }
  return version;
}

VIC convertStringToVIC(const std::string &val) {
  // Extract numeric value from string like "VIC1_640_480_P_60_4_3"
  if (val.find("VIC") == 0) {
    size_t pos = 3; // After "VIC"
    size_t end = val.find('_', pos);
    if (end != std::string::npos) {
      int vicNum = std::stoi(val.substr(pos, end - pos));
      return static_cast<VIC>(vicNum);
    }
  }
  return VIC::VIC0_UNAVAILABLE;
}

ExtendedColorimetry convertStringToExtendedColorimetry(const std::string &val) {
  ExtendedColorimetry extColor;
  if (val == "XV_YCC_601") {
    extColor = ExtendedColorimetry::XV_YCC_601;
  } else if (val == "XV_YCC_709") {
    extColor = ExtendedColorimetry::XV_YCC_709;
  } else if (val == "S_YCC_601") {
    extColor = ExtendedColorimetry::S_YCC_601;
  } else if (val == "OP_YCC_601") {
    extColor = ExtendedColorimetry::OP_YCC_601;
  } else if (val == "OP_RGB") {
    extColor = ExtendedColorimetry::OP_RGB;
  } else if (val == "BT2020_C_YCC") {
    extColor = ExtendedColorimetry::BT2020_C_YCC;
  } else if (val == "BT2020_RGB_YCC") {
    extColor = ExtendedColorimetry::BT2020_RGB_YCC;
  } else if (val == "ADDITIONAL_COLORIMETRY_EXTENSION") {
    extColor = ExtendedColorimetry::ADDITIONAL_COLORIMETRY_EXTENSION;
  } else {
    extColor = ExtendedColorimetry::XV_YCC_601;
  }
  return extColor;
}

AdditionalColorimetryExtension convertStringToAdditionalColorimetryExtension(const std::string &val) {
  AdditionalColorimetryExtension addColor;
  if (val == "DCI_P3_RGB_D65") {
    addColor = AdditionalColorimetryExtension::DCI_P3_RGB_D65;
  } else if (val == "DCI_P3_RGB_THEATER") {
    addColor = AdditionalColorimetryExtension::DCI_P3_RGB_THEATER;
  } else {
    addColor = AdditionalColorimetryExtension::DCI_P3_RGB_D65;
  }
  return addColor;
}

// Profile reading helper functions
int getProfileSupportedVICCount(int resourceIndex) {
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedVICs";
    return UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
}

int getProfileSupportedContentTypeCount(int resourceIndex) {
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedContentTypes";
    return UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
}

int getProfileSupportedHDRModeCount(int resourceIndex) {
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedHDROutputModes";
    return UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
}

int getProfileSupportedHDCPProtocolVersionCount(int resourceIndex) {
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedHDCPVersions";
    return UT_KVP_PROFILE_GET_LIST_COUNT(key.c_str());
}

int32_t getProfileSupportedVIC(int resourceIndex, int vicIndex) {
    char vicString[UT_KVP_MAX_ELEMENT_SIZE];
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedVICs." + std::to_string(vicIndex);
    UT_KVP_PROFILE_GET_STRING(key.c_str(), vicString);
    
    // Parse VIC enum string like "VIC16_1920_1080_P_60_16_9" to extract number
    std::string vicStr(vicString);
    if (vicStr.find("VIC") == 0) {
        size_t underscorePos = vicStr.find('_');
        if (underscorePos != std::string::npos) {
            return std::stoi(vicStr.substr(3, underscorePos - 3));
        }
    }
    return 0;
}

int32_t getProfileSupportedContentType(int resourceIndex, int typeIndex) {
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedContentTypes." + std::to_string(typeIndex);
    UT_KVP_PROFILE_GET_STRING(key.c_str(), typeString);
    
    // Map ContentType enum strings to numeric values
    std::string typeStr(typeString);
    if (typeStr == "UNSPECIFIED") return static_cast<int32_t>(ContentType::UNSPECIFIED);
    if (typeStr == "GRAPHICS") return static_cast<int32_t>(ContentType::GRAPHICS);
    if (typeStr == "PHOTO") return static_cast<int32_t>(ContentType::PHOTO);
    if (typeStr == "CINEMA") return static_cast<int32_t>(ContentType::CINEMA);
    if (typeStr == "GAME") return static_cast<int32_t>(ContentType::GAME);
    return static_cast<int32_t>(ContentType::UNSPECIFIED);
}

std::string getProfileSupportedHDRMode(int resourceIndex, int modeIndex) {
    char modeString[UT_KVP_MAX_ELEMENT_SIZE];
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedHDROutputModes." + std::to_string(modeIndex);
    UT_KVP_PROFILE_GET_STRING(key.c_str(), modeString);
    return std::string(modeString);
}

int32_t getProfileSupportedAFD(int resourceIndex, int afdIndex) {
    char afdString[UT_KVP_MAX_ELEMENT_SIZE];
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedAFD." + std::to_string(afdIndex);
    UT_KVP_PROFILE_GET_STRING(key.c_str(), afdString);
    
    // Map AFD enum strings to numeric values
    std::string afdStr(afdString);
    if (afdStr == "UNSPECIFIED") return static_cast<int32_t>(AFD::UNSPECIFIED);
    if (afdStr == "SAME_AS_PICTURE") return static_cast<int32_t>(AFD::SAME_AS_PICTURE);
    if (afdStr == "CENTER_4_3") return static_cast<int32_t>(AFD::CENTER_4_3);
    if (afdStr == "CENTER_16_9") return static_cast<int32_t>(AFD::CENTER_16_9);
    if (afdStr == "CENTER_14_9") return static_cast<int32_t>(AFD::CENTER_14_9);
    return static_cast<int32_t>(AFD::SAME_AS_PICTURE); // Default to SAME_AS_PICTURE
}

int32_t getProfileSupportedScanInfo(int resourceIndex, int scanIndex) {
    char scanString[UT_KVP_MAX_ELEMENT_SIZE];
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedScanInformation." + std::to_string(scanIndex);
    UT_KVP_PROFILE_GET_STRING(key.c_str(), scanString);
    
    // Map ScanInformation enum strings to numeric values
    std::string scanStr(scanString);
    if (scanStr == "NO_DATA") return static_cast<int32_t>(ScanInformation::NO_DATA);
    if (scanStr == "OVERSCAN") return static_cast<int32_t>(ScanInformation::OVERSCAN);
    if (scanStr == "UNDERSCAN") return static_cast<int32_t>(ScanInformation::UNDERSCAN);
    if (scanStr == "RESERVED") return static_cast<int32_t>(ScanInformation::RESERVED);
    return static_cast<int32_t>(ScanInformation::NO_DATA);
}

std::string getProfileSupportedSPDSource(int resourceIndex, int sourceIndex) {
    char sourceString[UT_KVP_MAX_ELEMENT_SIZE];
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedSPDSources." + std::to_string(sourceIndex);
    UT_KVP_PROFILE_GET_STRING(key.c_str(), sourceString);
    return std::string(sourceString);
}

std::string getProfileSupportedHDCPProtocolVersion(int resourceIndex, int versionIndex) {
    char versionString[UT_KVP_MAX_ELEMENT_SIZE];
    std::string key = "hdmioutput.resources." + std::to_string(resourceIndex) + ".supportedHDCPVersions." + std::to_string(versionIndex);
    UT_KVP_PROFILE_GET_STRING(key.c_str(), versionString);
    return std::string(versionString);
}

HDCPProtocolVersion getProfileMaxSupportedHDCPProtocolVersion(int resourceIndex) {
    int versionCount = getProfileSupportedHDCPProtocolVersionCount(resourceIndex);
    HDCPProtocolVersion maxVersion = HDCPProtocolVersion::UNDEFINED;
    
    for (int i = 0; i < versionCount; i++) {
        std::string versionStr = getProfileSupportedHDCPProtocolVersion(resourceIndex, i);
        HDCPProtocolVersion currentVersion = convertStringToHDCPProtocolVersions(versionStr);
        
        // VERSION_2_X (2) > VERSION_1_X (1) > UNDEFINED (0)
        if (static_cast<int>(currentVersion) > static_cast<int>(maxVersion)) {
            maxVersion = currentVersion;
        }
    }
    
    return maxVersion;
}

hdmioutput::Property convertStringToProperty(const std::string& propertyStr) {
    if (propertyStr == "VIC")
        return hdmioutput::Property::VIC;
    else if (propertyStr == "CONTENT_TYPE")
        return hdmioutput::Property::CONTENT_TYPE;
    else if (propertyStr == "AFD")
        return hdmioutput::Property::AFD;
    else if (propertyStr == "HDR_OUTPUT_MODE")
        return hdmioutput::Property::HDR_OUTPUT_MODE;
    else if (propertyStr == "SCAN_INFORMATION")
        return hdmioutput::Property::SCAN_INFORMATION;
    else if (propertyStr == "RESOURCE_ID")
        return hdmioutput::Property::RESOURCE_ID;
    else
        return hdmioutput::Property::RESOURCE_ID; // Default fallback
}

/** @} */ // End of HDMIOUTPUT_HALTEST_HELPER
/** @} */ // End of HDMIOUTPUT_HALTEST
/** @} */ // End of HDMIOUTPUT
/** @} */ // End of HPK