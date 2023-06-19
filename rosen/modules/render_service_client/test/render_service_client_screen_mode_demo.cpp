/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <iostream>
#include <string>
#include "transaction/rs_interfaces.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

class RSScreenModeTestCase {
public:
    inline static RSScreenModeTestCase& GetInstance()
    {
        static RSScreenModeTestCase instance;
        return instance;
    }

    void TestRun()
    {
        std::cout << "Render service Client screen mode Demo.cpp testRun Start\n";
        decltype(supportedModes_.size()) size = supportedModes_.size();
        if (size == 0) {
            std::cout << "The default screen id is " << defaultDisplayId_ <<
                ", and it dosen't spupport any screen mode." << std::endl;
            return;
        }
        std::cout << "The default screen id is " << defaultDisplayId_ <<
            ", and it spupports these screen modes:." << std::endl;
        for (decltype(supportedModes_.size()) i = 0; i < size; i++) {
            std::cout << "supporedMode[" << i << "]: modeId=" << supportedModes_[i].GetScreenModeId() <<
                ", phyWidth*phyHeight=" << supportedModes_[i].GetScreenWidth() << "*" <<
                supportedModes_[i].GetScreenHeight() <<
                ", refreshrate=" << supportedModes_[i].GetScreenRefreshRate() << std::endl;
        }
        RSScreenModeInfo currScreenMode = rsInterface_.GetScreenActiveMode(defaultDisplayId_);
        std::cout << "The current active screen mode :\n modeIndex: " << currScreenMode.GetScreenModeId() <<
            ", phyWidth*phyHeight=" << currScreenMode.GetScreenWidth() << "*" << currScreenMode.GetScreenHeight() <<
            ", refreshrate=" << currScreenMode.GetScreenRefreshRate() << std::endl;

        ChangeScreenActiveMode();
        currScreenMode = rsInterface_.GetScreenActiveMode(defaultDisplayId_);
        std::cout << "The current active screen mode :\n modeIndex: " << currScreenMode.GetScreenModeId() <<
            ", phyWidth*phyHeight=" << currScreenMode.GetScreenWidth() << "*" << currScreenMode.GetScreenHeight() <<
            ", refreshrate=" << currScreenMode.GetScreenRefreshRate() << std::endl;

        std::cout << "Render service Client screen mode Demo.cpp testRun end\n";
    }

    ~RSScreenModeTestCase() {};
private:
    RSScreenModeTestCase() : rsInterface_(RSInterfaces::GetInstance())
    {
        defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
        supportedModes_ = rsInterface_.GetScreenSupportedModes(defaultDisplayId_);
    };

    void ChangeScreenActiveMode()
    {
        std::string inputStr;
        std::cout << "\n\nPlase input screen mode index:\n";
        std::cin >> inputStr;
        uint32_t newScreenModeId = 0;
        for (auto iter = inputStr.begin(); iter != inputStr.end(); iter++) {
            newScreenModeId *= 10; // 10 : decimal
            newScreenModeId += (*iter - '0');
        }
        std::cout << "The new screen mode index is " << newScreenModeId << "\n\n";
        rsInterface_.SetScreenActiveMode(defaultDisplayId_, newScreenModeId);
    }

    RSInterfaces& rsInterface_;
    ScreenId defaultDisplayId_ = 0;
    std::vector<RSScreenModeInfo> supportedModes_;
};

int main()
{
    RSScreenModeTestCase::GetInstance().TestRun();
    return 0;
}
