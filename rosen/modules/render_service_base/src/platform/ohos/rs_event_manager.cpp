/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "platform/common/rs_event_manager.h"
#include "platform/common/rs_system_properties.h"
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
namespace OHOS {
namespace Rosen {

SysEventManager& SysEventManager::Instance()
{
	static SysEventManager eventManager;
	return eventManager;
}

SysEventManager::SysEventManager()
{
}

SysEventManager::~SysEventManager()
{
}

void SysEventManager::UpdateDetectorParam(std::shared_ptr<BaseEventDetector> detectorPtr)
{
	auto paramList = detectorPtr->GetParamList();
	for (auto& item : paramList) {
		std::string paraName = "rosen.RsDFXEvent." + detectorPtr->GetStringId() +
			"." + item.first;
        RS_LOGD("SysEventManager::UpdateDetectorParam paraName: %s", paraName.c_str());
		detectorPtr->SetParam(item.first, RSSystemProperties::GetRSEventProperty(paraName));
	}
}

void SysEventManager::UpdateEventIntervalMs(std::shared_ptr<BaseEventDetector> detectorPtr)
{
	if (eventStateList_.count(detectorPtr->GetStringId()) == 0) {
        RS_LOGD("SysEventManager::UpdateEventIntervalMs detector:%s is not in list",
        detectorPtr->GetStringId().c_str());
		return;
	}
	std::string paraName = "rosen.RsDFXEvent." + detectorPtr->GetStringId() +
		".eventIntervalMs";
    RS_LOGD("SysEventManager::UpdateEventIntervalMs paraName: %s", paraName.c_str());
	int valueInt = atoi(RSSystemProperties::GetRSEventProperty(paraName).c_str());
	if (valueInt <= 0 || valueInt > 1000000) {
        RS_LOGD("SysEventManager::UpdateEventIntervalMs detector:%s Invaild Value:%d",
        detectorPtr->GetStringId().c_str(), valueInt);
		return;
	}
	eventStateList_[detectorPtr->GetStringId()].eventIntervalMs = valueInt;
    RS_LOGD("SysEventManager::UpdateEventIntervalMs detector:%s eventIntervalMs:%d suceess",
    detectorPtr->GetStringId().c_str(), valueInt);
}

void SysEventManager::UpdateParam()
{
	updateCount_++;
	if (updateCount_ % 200 != 0) { 
		return;
	}
    updateCount_ = 0;
	std::unique_lock<std::mutex> listLock(listMutex_);
	for (auto& item : eventDetectorList_) {
		auto detectorPtr = item.second.lock();
		if (detectorPtr == nullptr) {
			return;
		}
		UpdateDetectorParam(detectorPtr);
		UpdateEventIntervalMs(detectorPtr);
	}
}

void SysEventManager::AddEvent(const std::shared_ptr<BaseEventDetector>& detectorPtr, int eventIntervalMs)
{
	std::weak_ptr<BaseEventDetector> detectorWeakPtr(detectorPtr);
	{
		std::unique_lock<std::mutex> listLock(listMutex_);
		if (eventDetectorList_.count(detectorPtr->GetStringId()) != 0) {
            RS_LOGD("SysEventManager::AddEvent %s failed ", detectorPtr->GetStringId().c_str());
			return;
		}
		eventDetectorList_[detectorPtr->GetStringId()] = detectorWeakPtr;
		RSEventState state = {
			eventIntervalMs,
			0
		};
		eventStateList_[detectorPtr->GetStringId()] = state;
        RS_LOGD("SysEventManager::AddEvent %s success ", detectorPtr->GetStringId().c_str());
	}
}

void SysEventManager::RemoveEvent(std::string stringId)
{
	std::unique_lock<std::mutex> listLock(listMutex_);
	if (eventDetectorList_.count(stringId) != 0) {
        RS_LOGD("SysEventManager::RemoveEvent %s failed ", stringId.c_str());
		return;
	}
	eventDetectorList_.erase(stringId);
	eventStateList_.erase(stringId);
    RS_LOGD("SysEventManager::RemoveEvent %s success ", stringId.c_str());
}

uint64_t SysEventManager::GetSysTimeMs()
{
	auto now = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

void SysEventManager::EventReport(const RSSysEventMsg& eventMsg)
{
	std::unique_lock<std::mutex> listLock(listMutex_);
	if (eventStateList_.count(eventMsg.stringId) == 0) {
        RS_LOGD("SysEventManager::EventReport %s failed ", eventMsg.stringId.c_str());
		return;
	}
	RSEventState& state = eventStateList_[eventMsg.stringId];
	uint64_t currentTimeMs = GetSysTimeMs();
	if (currentTimeMs > state.prevEventTimeStampMs&&
		currentTimeMs - state.prevEventTimeStampMs > state.eventIntervalMs) {
            std::string domain = "GRAPHIC";

    OHOS::HiviewDFX::HiSysEvent::Write(domain, eventMsg.stringId,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "MSG", eventMsg.msg);
		state.prevEventTimeStampMs = currentTimeMs;
	}
    RS_LOGD("SysEventManager::EventReport %s success ", eventMsg.stringId.c_str());
}


}
}