/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <string>
#include <iostream>
#include "service_dumper.h"
#include "system_ability_definition.h"

using namespace OHOS;

int PrintHelpInfo(int argc, char *argv[])
{
    int retCode = -1;
    if (argc <= 1) {
        return retCode;
    }

    std::string param1(argv[1]);
    if (param1 == "--help") {
        std::cout << "help info:" << std::endl;
        std::cout << "display:           Show the screens info." << std::endl;
        std::cout << "surface:           Show the foreground surfaces info." << std::endl;
        std::cout << "fps:               Show the fps info." << std::endl;
        std::cout << "nodeNotOnTree:     Show the surfaces info which are not on the tree." << std::endl;
        std::cout << "NULL:              Show all of the information above." << std::endl;
        retCode = 1;
    }

    return retCode;
}

// dump
int main(int argc, char *argv[])
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> renderService = samgr->GetSystemAbility(RENDER_SERVICE);
    if (renderService == nullptr) {
        fprintf(stderr, "Can't find render service!");
        return -1;
    }

    int retCode = PrintHelpInfo(argc, argv);
    if (retCode == 1) {
        return retCode;
    }

    Detail::ServiceDumper dumper(renderService, "Render_Service", 10000); // timeout: 10000 ms.
    return dumper.Run(argc, argv);
}
