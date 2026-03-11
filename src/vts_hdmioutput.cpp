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
    UT_LOG_INFO("HDMIOutputEventListener::onStateChanged called with oldState[%d], newState[%d]\n",
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
    std::queue<std::pair<::com::rdk::hal::hdmioutput::State, ::com::rdk::hal::hdmioutput::State>> emptyQueue;
    std::swap(mStateChangeQueue, emptyQueue);
}

bool HDMIOutputEventListener::hasStateChanged() const {
    std::lock_guard<std::mutex> lock(mStateChangeMutex);
    return !mStateChangeQueue.empty();
}

// HDMIOutputControllerListener implementations
::android::binder::Status HDMIOutputControllerListener::onHotPlugDetectStateChanged(bool state) {
    // Check if we should update the queue based on current state
    bool shouldUpdateQueue = true;
    if (mHDMIOutput != nullptr) {
        hdmioutput::State currentState;
        ::android::binder::Status status = mHDMIOutput->getState(&currentState);
        if (status.isOk()) {
            // Only update queue if in OPENING or STARTED states
            shouldUpdateQueue = (currentState == hdmioutput::State::OPENING ||
                                currentState == hdmioutput::State::STARTED);
            
            if (!shouldUpdateQueue) {
                UT_LOG_INFO("HDMIOutputControllerListener::onHotPlugDetectStateChanged called with state[%s] but current device state is [%d] - not updating queue\n",
                           state ? "true" : "false", static_cast<int>(currentState));
            }
        }
    }
    
    {
        std::lock_guard<std::mutex> lock(mHPDStateMutex);
        if (shouldUpdateQueue) {
            mHPDStateQueue.push(state);
            mHPDStateCV.notify_all();
        }
        // Always update legacy single-value storage
        m_hpdState = state;
        m_hpdStateChanged = true;
    }
    
    if (shouldUpdateQueue) {
        UT_LOG_INFO("HDMIOutputControllerListener::onHotPlugDetectStateChanged called with state[%s] - queue updated\n",
                    state ? "true" : "false");
    }
    return ::android::binder::Status::ok();
}

bool HDMIOutputControllerListener::getNextHPDStateChange(bool& state, chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mHPDStateMutex);
    if (timeout.count() > 0) {
        if (!mHPDStateCV.wait_for(lock, timeout, [this]{ return !mHPDStateQueue.empty(); })) {
            return false; // Timed out
        }
    } else if (mHPDStateQueue.empty()) {
        return false;
    }
    state = mHPDStateQueue.front();
    mHPDStateQueue.pop();
    return true;
}

bool HDMIOutputControllerListener::waitForHPDCount(int32_t count, chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mHPDStateMutex);
    if (!mHPDStateCV.wait_for(lock, timeout, [this, count]{ return static_cast<int32_t>(mHPDStateQueue.size()) >= count; })) {
        return false; // Timed out
    }
    return true;
}

void HDMIOutputControllerListener::clearHPDQueue() {
    std::lock_guard<std::mutex> lock(mHPDStateMutex);
    std::queue<bool> emptyQueue;
    std::swap(mHPDStateQueue, emptyQueue);
}

::android::binder::Status HDMIOutputControllerListener::onFrameRateChanged() {
    {
        std::lock_guard<std::mutex> lock(mFrameRateMutex);
        mFrameRateQueue.push(true);
        // Update legacy single-value storage
        m_frameRateChanged = true;
    }
    mFrameRateCV.notify_all();
    UT_LOG_INFO("HDMIOutputControllerListener::onFrameRateChanged called\n");
    return ::android::binder::Status::ok();
}

bool HDMIOutputControllerListener::getNextFrameRateChange(chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mFrameRateMutex);
    if (timeout.count() > 0) {
        if (!mFrameRateCV.wait_for(lock, timeout, [this]{ return !mFrameRateQueue.empty(); })) {
            return false; // Timed out
        }
    } else if (mFrameRateQueue.empty()) {
        return false;
    }
    mFrameRateQueue.pop();
    return true;
}

bool HDMIOutputControllerListener::waitForFrameRateCount(int32_t count, chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mFrameRateMutex);
    if (!mFrameRateCV.wait_for(lock, timeout, [this, count]{ return static_cast<int32_t>(mFrameRateQueue.size()) >= count; })) {
        return false; // Timed out
    }
    return true;
}

void HDMIOutputControllerListener::clearFrameRateQueue() {
    std::lock_guard<std::mutex> lock(mFrameRateMutex);
    std::queue<bool> emptyQueue;
    std::swap(mFrameRateQueue, emptyQueue);
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
    mHDCPStatusCV.notify_all();
    UT_LOG_INFO("HDMIOutputControllerListener::onHDCPStatusChanged called with status[%d], version[%d]\n",
                static_cast<int>(hdcpStatus), static_cast<int>(hdcpProtocolVersion));
    return ::android::binder::Status::ok();
}

bool HDMIOutputControllerListener::getNextHDCPStatusChange(std::pair<::com::rdk::hal::hdmioutput::HDCPStatus, ::com::rdk::hal::hdmioutput::HDCPProtocolVersion>& hdcpInfo, chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mHDCPStatusMutex);
    if (timeout.count() > 0) {
        if (!mHDCPStatusCV.wait_for(lock, timeout, [this]{ return !mHDCPStatusQueue.empty(); })) {
            return false; // Timed out
        }
    } else if (mHDCPStatusQueue.empty()) {
        return false;
    }
    hdcpInfo = mHDCPStatusQueue.front();
    mHDCPStatusQueue.pop();
    return true;
}

bool HDMIOutputControllerListener::waitForHDCPStatusCount(int32_t count, chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mHDCPStatusMutex);
    if (!mHDCPStatusCV.wait_for(lock, timeout, [this, count]{ return static_cast<int32_t>(mHDCPStatusQueue.size()) >= count; })) {
        return false; // Timed out
    }
    return true;
}

void HDMIOutputControllerListener::clearHDCPStatusQueue() {
    std::lock_guard<std::mutex> lock(mHDCPStatusMutex);
    std::queue<std::pair<::com::rdk::hal::hdmioutput::HDCPStatus, ::com::rdk::hal::hdmioutput::HDCPProtocolVersion>> emptyQueue;
    std::swap(mHDCPStatusQueue, emptyQueue);
}

::android::binder::Status HDMIOutputControllerListener::onEDID(const ::std::vector<uint8_t>& edid) {
    {
        std::lock_guard<std::mutex> lock(mEDIDMutex);
        mEDIDQueue.push(edid);
        // Update legacy single-value storage
        m_edid = edid;
        m_edidReceived = true;
    }
    mEDIDCV.notify_all();
    UT_LOG_INFO("HDMIOutputControllerListener::onEDID called, received %zu bytes\n", edid.size());
    return ::android::binder::Status::ok();
}

bool HDMIOutputControllerListener::getNextEDID(::std::vector<uint8_t>& edid, chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mEDIDMutex);
    if (timeout.count() > 0) {
        if (!mEDIDCV.wait_for(lock, timeout, [this]{ return !mEDIDQueue.empty(); })) {
            return false; // Timed out
        }
    } else if (mEDIDQueue.empty()) {
        return false;
    }
    edid = mEDIDQueue.front();
    mEDIDQueue.pop();
    return true;
}

bool HDMIOutputControllerListener::waitForEDIDCount(int32_t count, chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mEDIDMutex);
    if (!mEDIDCV.wait_for(lock, timeout, [this, count]{ return static_cast<int32_t>(mEDIDQueue.size()) >= count; })) {
        return false; // Timed out
    }
    return true;
}

void HDMIOutputControllerListener::clearEDIDQueue() {
    std::lock_guard<std::mutex> lock(mEDIDMutex);
    std::queue<::std::vector<uint8_t>> emptyQueue;
    std::swap(mEDIDQueue, emptyQueue);
}

bool HDMIOutputControllerListener::getNextVICChange(int32_t& vic, chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mVICChangeMutex);
    if (timeout.count() > 0) {
        if (!mVICChangeCV.wait_for(lock, timeout, [this]{ return !mVICChangeQueue.empty(); })) {
            return false; // Timed out
        }
    } else if (mVICChangeQueue.empty()) {
        return false;
    }
    vic = mVICChangeQueue.front();
    mVICChangeQueue.pop();
    return true;
}

void HDMIOutputControllerListener::reset() {
    // Clear all queues
    clearHPDQueue();
    clearFrameRateQueue();
    clearHDCPStatusQueue();
    clearEDIDQueue();
    
    // Reset legacy single-value storage
    {
        std::lock_guard<std::mutex> lock(mHPDStateMutex);
        m_hpdStateChanged = false;
        m_hpdState = false;
    }
    {
        std::lock_guard<std::mutex> lock(mFrameRateMutex);
        m_frameRateChanged = false;
    }
    {
        std::lock_guard<std::mutex> lock(mHDCPStatusMutex);
        m_hdcpStatusChanged = false;
    }
    {
        std::lock_guard<std::mutex> lock(mEDIDMutex);
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
