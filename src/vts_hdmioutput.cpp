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
 * @file vts_hdmioutput.cpp
 *
 */


#include "vts_hdmioutput.h"
#include "vts_hdmioutput_init.h"
#include <thread>

// HDMIOutputEventListener implementations
::android::binder::Status HDMIOutputEventListener::onStateChanged(::com::rdk::hal::hdmioutput::State oldState,
                                         ::com::rdk::hal::hdmioutput::State newState) {
    {
        std::lock_guard<std::mutex> lock(mStateChangeMutex);
        mStateChangeQueue.emplace(oldState, newState);
    }
    UT_LOG_INFO("HDMIOutputEventListener::onStateChanged called with oldState[%d], newState[%d]",
                static_cast<int>(oldState), static_cast<int>(newState));
    return ::android::binder::Status::ok();
}

bool HDMIOutputEventListener::getNextStateChanges(std::pair<::com::rdk::hal::hdmioutput::State, ::com::rdk::hal::hdmioutput::State>& stateChanges) {
    std::lock_guard<std::mutex> lock(mStateChangeMutex);
    if (mStateChangeQueue.empty()) {
        return false;
    }
    stateChanges = mStateChangeQueue.front();
    mStateChangeQueue.pop();
    return true;
}

void HDMIOutputEventListener::reset() {
    std::lock_guard<std::mutex> lock(mStateChangeMutex);
    // Clear the queue
    while (!mStateChangeQueue.empty()) {
        mStateChangeQueue.pop();
    }
}

bool HDMIOutputEventListener::hasStateChanged() const {
    std::lock_guard<std::mutex> lock(mStateChangeMutex);
    return !mStateChangeQueue.empty();
}

// HDMIOutputControllerListener implementations
::android::binder::Status HDMIOutputControllerListener::onHotPlugDetectStateChanged(bool state) {
    {
        std::lock_guard<std::mutex> lock(mHPDStateMutex);
        mHPDStateQueue.push(state);
        // Update legacy single-value storage
        m_hpdState = state;
        m_hpdStateChanged = true;
    }
    UT_LOG_INFO("HDMIOutputControllerListener::onHotPlugDetectStateChanged called with state[%s]",
                state ? "true" : "false");
    return ::android::binder::Status::ok();
}

bool HDMIOutputControllerListener::getNextHPDStateChange(bool& state) {
    std::lock_guard<std::mutex> lock(mHPDStateMutex);
    if (mHPDStateQueue.empty()) {
        return false;
    }
    state = mHPDStateQueue.front();
    mHPDStateQueue.pop();
    return true;
}

::android::binder::Status HDMIOutputControllerListener::onFrameRateChanged() {
    {
        std::lock_guard<std::mutex> lock(mFrameRateMutex);
        mFrameRateQueue.push(true);
        // Update legacy single-value storage
        m_frameRateChanged = true;
    }
    UT_LOG_INFO("HDMIOutputControllerListener::onFrameRateChanged called");
    return ::android::binder::Status::ok();
}

bool HDMIOutputControllerListener::getNextFrameRateChange() {
    std::lock_guard<std::mutex> lock(mFrameRateMutex);
    if (mFrameRateQueue.empty()) {
        return false;
    }
    mFrameRateQueue.pop();
    return true;
}

::android::binder::Status HDMIOutputControllerListener::onHDCPStatusChanged(
        ::com::rdk::hal::hdmioutput::HDCPStatus hdcpStatus,
        ::com::rdk::hal::hdmioutput::HDCPProtocolVersion hdcpProtocolVersion) {
    {
        std::lock_guard<std::mutex> lock(mHDCPStatusMutex);
        mHDCPStatusQueue.emplace(hdcpStatus, hdcpProtocolVersion);
        // Update legacy single-value storage
        m_hdcpStatus = hdcpStatus;
        m_hdcpProtocolVersion = hdcpProtocolVersion;
        m_hdcpStatusChanged = true;
    }
    UT_LOG_INFO("HDMIOutputControllerListener::onHDCPStatusChanged called with status[%d], version[%d]",
                static_cast<int>(hdcpStatus), static_cast<int>(hdcpProtocolVersion));
    return ::android::binder::Status::ok();
}

bool HDMIOutputControllerListener::getNextHDCPStatusChange(std::pair<::com::rdk::hal::hdmioutput::HDCPStatus, ::com::rdk::hal::hdmioutput::HDCPProtocolVersion>& hdcpInfo) {
    std::lock_guard<std::mutex> lock(mHDCPStatusMutex);
    if (mHDCPStatusQueue.empty()) {
        return false;
    }
    hdcpInfo = mHDCPStatusQueue.front();
    mHDCPStatusQueue.pop();
    return true;
}

::android::binder::Status HDMIOutputControllerListener::onEDID(const ::std::vector<uint8_t>& edid) {
    {
        std::lock_guard<std::mutex> lock(mEDIDMutex);
        mEDIDQueue.push(edid);
        // Update legacy single-value storage
        m_edid = edid;
        m_edidReceived = true;
    }
    UT_LOG_INFO("HDMIOutputControllerListener::onEDID called, received %zu bytes", edid.size());
    return ::android::binder::Status::ok();
}

bool HDMIOutputControllerListener::getNextEDID(::std::vector<uint8_t>& edid) {
    std::lock_guard<std::mutex> lock(mEDIDMutex);
    if (mEDIDQueue.empty()) {
        return false;
    }
    edid = mEDIDQueue.front();
    mEDIDQueue.pop();
    return true;
}

void HDMIOutputControllerListener::reset() {
    {
        std::lock_guard<std::mutex> lock(mHPDStateMutex);
        while (!mHPDStateQueue.empty()) {
            mHPDStateQueue.pop();
        }
        m_hpdStateChanged = false;
        m_hpdState = false;
    }
    {
        std::lock_guard<std::mutex> lock(mFrameRateMutex);
        while (!mFrameRateQueue.empty()) {
            mFrameRateQueue.pop();
        }
        m_frameRateChanged = false;
    }
    {
        std::lock_guard<std::mutex> lock(mHDCPStatusMutex);
        while (!mHDCPStatusQueue.empty()) {
            mHDCPStatusQueue.pop();
        }
        m_hdcpStatusChanged = false;
    }
    {
        std::lock_guard<std::mutex> lock(mEDIDMutex);
        while (!mEDIDQueue.empty()) {
            mEDIDQueue.pop();
        }
        m_edidReceived = false;
        m_edid.clear();
    }
}

bool HDMIOutputControllerListener::hasHPDStateChanged() const {
    std::lock_guard<std::mutex> lock(mHPDStateMutex);
    return m_hpdStateChanged;
}

bool HDMIOutputControllerListener::hasFrameRateChanged() const {
    std::lock_guard<std::mutex> lock(mFrameRateMutex);
    return m_frameRateChanged;
}

bool HDMIOutputControllerListener::hasHDCPStatusChanged() const {
    std::lock_guard<std::mutex> lock(mHDCPStatusMutex);
    return m_hdcpStatusChanged;
}

bool HDMIOutputControllerListener::hasEDIDReceived() const {
    std::lock_guard<std::mutex> lock(mEDIDMutex);
    return m_edidReceived;
}

android::sp<com::rdk::hal::hdmioutput::IHDMIOutput> getHDMIOutput(int32_t i) {
    com::rdk::hal::hdmioutput::IHDMIOutput::Id hdmiOutputId;
    android::sp<com::rdk::hal::hdmioutput::IHDMIOutput> hdmiOutput;
    std::string key_string = "hdmioutput.resources." + std::to_string(i) + ".id";
    hdmiOutputId.value = UT_KVP_PROFILE_GET_UINT32(key_string.c_str());
    HDMIOUTPUT_GET_HDMIOUTPUT(gHDMIOUTPUTHandle, hdmiOutputId, hdmiOutput);
    return hdmiOutput;
}

/**
* @} // End of HDMIOUTPUT_HALTEST_CLASS
* @} // End of HDMIOUTPUT_HALTEST
* @} // End of HDMIOUTPUT
* @} // End of HPK
*/
/* End of vts_hdmioutput.cpp */
