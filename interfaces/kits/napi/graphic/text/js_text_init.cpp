/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "js_text_init.h"

#include "enum_napi/text_enum_napi.h"
#include "fontcollection_napi/js_fontcollection.h"
#include "paragraph_style_napi/js_paragraphstyle.h"
#include "paragraph_napi/js_paragraph.h"
#include "utils/log.h"

namespace OHOS::Rosen {
napi_value TextInit(napi_env env, napi_value exportObj)
{
    JsFontCollection::Init(env, exportObj);
    JsEnum::Init(env, exportObj);
    JsParagraph::Init(env, exportObj);
    return exportObj;
}
} // namespace OHOS::Rosen
