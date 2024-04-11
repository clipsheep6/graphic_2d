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

#include <fstream>

#include "js_fontcollection.h"
#include "utils/log.h"
namespace OHOS::Rosen {
constexpr size_t FILE_HEAD_LENGTH = 7; // 7 is the size of "file://"
thread_local napi_ref JsFontCollection::constructor_ = nullptr;
const std::string CLASS_NAME = "FontCollection";
napi_value JsFontCollection::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        LOGE("JsFontCollection::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsFontCollection* jsFontCollection = new(std::nothrow) JsFontCollection();
    status = napi_wrap(env, jsThis, jsFontCollection,
        JsFontCollection::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsFontCollection;
        LOGE("JsFontCollection::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

napi_value JsFontCollection::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("disableFallback", JsFontCollection::DisableFallback),
        DECLARE_NAPI_FUNCTION("loadFont", JsFontCollection::LoadFont),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        LOGE("JsFontCollection::Init Failed to define FontCollection class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        LOGE("JsFontCollection::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        LOGE("JsFontCollection::Init Failed to set constructor");
        return nullptr;
    }
    return exportObj;
}

void JsFontCollection::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsFontCollection* napi = reinterpret_cast<JsFontCollection*>(nativeObject);
        delete napi;
    }
}

JsFontCollection::JsFontCollection()
{
    m_fontCollection = OHOS::Rosen::FontCollection::Create();
}

std::shared_ptr<FontCollection> JsFontCollection::GetFontCollection()
{
    return m_fontCollection;
}

napi_value JsFontCollection::DisableFallback(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnDisableFallback(env, info) : nullptr;
}

napi_value JsFontCollection::OnDisableFallback(napi_env env, napi_callback_info info)
{
    if (m_fontCollection == nullptr) {
        LOGE("JsFontCollection::OnClose path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    m_fontCollection->DisableFallback();
    return NapiGetUndefined(env);
}

napi_value JsFontCollection::LoadFont(napi_env env, napi_callback_info info)
{
    LOGE("LoadFontTrace | into JS LoadFont");
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnLoadFont(env, info) : nullptr;
}

bool JsFontCollection::SpiltAbsoluteFontPath(std::string& absolutePath)
{
    LOGE("LoadFontTrace | absolutePath FamilyStr=%s",absolutePath.c_str());
    auto iter = absolutePath.find_first_of(':');
    if (iter == std::string::npos) {
        LOGE("Path is not Absolute Path");
        return false;
    }
    std::string head = absolutePath.substr(0, iter);
    if ((head == "file" && absolutePath.size() > FILE_HEAD_LENGTH)) {
        absolutePath = absolutePath.substr(iter + 3);
        // the file uri format is like "file://system/fonts...",
        return true;
    }
    return false;
}

bool JsFontCollection::GetFontFileProperties(uint8_t* data, size_t& datalen, const std::string path)
{
    std::ifstream f(path.c_str());
    if (!f.good()) {
        LOGE("LoadFontTrace File NO exist");
        return false;
    }

    std::ifstream ifs(path, std::ios_base::in);
    if (!ifs.is_open()) {
        LOGE("LoadFontTrace | 1 Faild Get Font File and datalen=%zu",datalen);
        return false;
    }

    ifs.seekg(0, ifs.end);
    if (!ifs.good()) {
        ifs.close();
        LOGE("LoadFontTrace |  2Faild Get Font File and datalen=%zu",datalen);
        return false;
    }

    datalen = ifs.tellg();
    if (ifs.fail()) {
        ifs.close();
        LOGE("LoadFontTrace | 3 Faild Get Font File and datalen=%zu",datalen);
        return false;
    }

    ifs.seekg(ifs.beg);
    if (!ifs.good()) {
         LOGE("LoadFontTrace | 4 Faild Get Font File and datalen=%zu",datalen);
        ifs.close();
        return false;
    }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(datalen);
    ifs.read(buffer.get(), datalen);
    if (!ifs.good()) {
        ifs.close();
        LOGE("LoadFontTrace | 5 Faild Get Font File and datalen=%zu",datalen);
        return false;
    }
    ifs.close();

    data = reinterpret_cast<uint8_t*>(buffer.get());
    LOGE("LoadFontTrace | Success Get Font File and datalen=%zu",datalen);
    return true;
}

bool JsFontCollection::AddTypefaceInformation(Drawing::Typeface& typeface, const std::string familyName)
{
    LOGE("LoadFontTrace: into AddTypefaceInformation");
    std::shared_ptr<Drawing::Typeface> drawingTypeface(&typeface);
    std::string name = familyName;
    if (name.empty()) {
        name = drawingTypeface->GetFamilyName();
    }
    m_fontCollection->AddLoadedFamilyName(name);
    if (Drawing::Typeface::GetTypefaceRegisterCallBack() != nullptr) {
        bool ret = Drawing::Typeface::GetTypefaceRegisterCallBack()(drawingTypeface);
        if (!ret) {
            LOGE("LoadFontTrace: register typeface failed.");
            return false;
        }
    }
    LOGE("LoadFontTrace:  AddTypefaceInformation Successed");
    return true;
}

napi_value JsFontCollection::OnLoadFont(napi_env env, napi_callback_info info)
{
    LOGE("LoadFontTrace | Ready Get FontOptions Target");
    size_t argc = 2;
    napi_value argv[ARGC_TWO] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok ||
            argc < ARGC_TWO) {
            LOGE("LoadFontTrace Param unvaild");
            return nullptr;
    }
    //napi_value familyNameNApi = nullptr;
    //napi_value familySrcNApi = nullptr;
    std::string familyName;
    std::string familySrc;
    //napi_get_named_property(env, argv[0], "familyName", &familyNameNApi);
    ConvertFromJsValue(env, argv[0], familyName);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[1], &valueType);
    if (valueType != napi_object) {
        ConvertFromJsValue(env, argv[1], familySrc);
      //  napi_get_named_property(env, argv[1], "familySrc", &familySrcNApi);
    } else {
        return nullptr;
    }
    LOGE("LoadFontTrace |Ready into SpiltAbsoluteFontPath  and FamilyStr= %s  familyName=%s",familySrc.c_str(),
        familyName.c_str());
    if (!SpiltAbsoluteFontPath(familySrc)) {
        LOGE("LoadFontTrace UNVaild Font Path");
        return nullptr;
    }
    LOGE("LoadFontTrace | Split Over and FamilyStr= %s  familyName=%s",familySrc.c_str(),
    familyName.c_str());
    uint8_t* rawData = nullptr;
    size_t rawdatalen = 0;
    if (!GetFontFileProperties(rawData, rawdatalen, familySrc)) {
        LOGE("LoadFontTrace UNVaild Font File");
        return nullptr;
    }
    Drawing::Typeface* typeface = nullptr;
    typeface = m_fontCollection->LoadFont(familyName, rawData, rawdatalen);
    if (!AddTypefaceInformation(*typeface, familyName)) {
        LOGE("LoadFontTrace AddTypefaceInformation false");
        return nullptr;
    }
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
