/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "xml_parser.h"

namespace OHOS::Rosen {
int32_t XMLParser::LoadConfiguration()
{
    xmlDocument_ = xmlReadFile(CONFIG_FILE, nullptr, 0);
    if (xmlDocument_ == nullptr) {
        HGM_LOGE("XMLParser xmlReadFile failed");
        return XML_FILE_LOAD_FAIL;
    }

    if (!mParsedData_) {
        HGM_LOGI("XMLParserr initializing parsed data");
        mParsedData_ = std::make_unique<ParsedConfigData>();
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::Parse()
{
    HGM_LOGD("XMLParser Parse");
    xmlNode *root = xmlDocGetRootElement(xmlDocument_);
    if (root == nullptr) {
        HGM_LOGE("XMLParser xmlDocGetRootElement failed");
        return XML_GET_ROOT_FAIL;
    }

    if (!ParseInternal(*root)) {
        return XML_PARSE_INTERNAL_FAIL;
    }

    return EXEC_SUCCESS;
}

void XMLParser::Destroy()
{
    HGM_LOGD("XMLParser Destroying the parser");
    if (xmlDocument_ != nullptr) {
        xmlFreeDoc(xmlDocument_);
    }
}

bool XMLParser::ParseInternal(xmlNode &node)
{
    HGM_LOGD("XMLParser parsing an internal node");
    xmlNode *currNode = &node;
    currNode = currNode->xmlChildrenNode;
    int32_t parseSuccess = EXEC_SUCCESS;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (parseSuccess != EXEC_SUCCESS) {
            return false;
        }

        if (GetHgmXmlNodeAsInt(*currNode) == HGM_XML_PARAM) {
            parseSuccess = ParseParam(*currNode);
        } else if (GetHgmXmlNodeAsInt(*currNode) == HGM_XML_PARAMS) {
            parseSuccess = ParseParams(*currNode);
        }
    }
    return true;
}

int32_t XMLParser::ParseParam(xmlNode &node)
{
    HGM_LOGI("XMLParser parsing a parameter");
    std::string paraName = ExtractPropertyValue("name", node);
    if (paraName == "general_config_enable") {
        HGM_LOGD("XMLParser parsing general_config_enable");
        std::string parsed = ExtractPropertyValue("value", node);
        mParsedData_->isDynamicFrameRateEnable_ = parsed;
    }

    HGM_LOGD("XMLParser parsing general_config_enable finish: %{public}s",
        mParsedData_->isDynamicFrameRateEnable_.c_str());
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseParams(xmlNode &node)
{
    std::string paraName = ExtractPropertyValue("name", node);
    if (paraName.empty()) {
        HGM_LOGD("XMLParser No name provided for %{public}s", node.name);
        return XML_PARSE_INTERNAL_FAIL;
    }

    if (paraName == "detailed_strats") {
        if (ParseStrat(node)) {
            HGM_LOGD("XMLParser failed to parse detailed_strats");
        }
        return EXEC_SUCCESS;
    }

    if (paraName == "customer_setting_config") {
        if (ParseSetting(node, mParsedData_->customerSettingConfig_)) {
            HGM_LOGD("XMLParser failed to parse customer_setting_config");
        }
        return EXEC_SUCCESS;
    }

    if (paraName == "animation_dynamic_settings") {
        if (ParseSetting(node, mParsedData_->animationDynamicStrats_)) {
            HGM_LOGD("XMLParser failed to parse animation_dynamic_settings");
        }
        return EXEC_SUCCESS;
    }
    HGM_LOGD("XMLParser parsing params finish");
    return XML_PARSE_INTERNAL_FAIL;
}

int32_t XMLParser::ParseStrat(xmlNode &node)
{
    xmlNode *currNode = &node;
    currNode = currNode->xmlChildrenNode;

    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        ParsedConfigData::detailedStrat strat;
        strat.name = ExtractPropertyValue("name", *currNode);
        strat.isDynamic = ExtractPropertyValue("isDynamic", *currNode);
        strat.min = ExtractPropertyValue("min", *currNode);
        strat.max = ExtractPropertyValue("max", *currNode);
        mParsedData_->detailedStrats_[strat.name] = strat;
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSetting(xmlNode &node, std::unordered_map<std::string, std::string> &config)
{
    HGM_LOGD("XMLParser parsing strats");
    xmlNode *currNode = &node;
    currNode = currNode->xmlChildrenNode;

    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        std::string name = "";
        std::string value = "";
        name = ExtractPropertyValue("name", *currNode);
        value = ExtractPropertyValue("value", *currNode);
        if (name.empty() || value.empty()) {
            return XML_PARSE_INTERNAL_FAIL;
        }
        config[name] = value;
    }

    return EXEC_SUCCESS;
}

std::string XMLParser::ExtractPropertyValue(const std::string &propName, xmlNode &node)
{
    HGM_LOGD("XMLParser extracting value : %{public}s", propName.c_str());
    std::string propValue = "";
    xmlChar *tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        HGM_LOGD("XMLParser not aempty tempValue");
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
    }

    return propValue;
}

int32_t XMLParser::GetHgmXmlNodeAsInt(xmlNode &node)
{
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("Param"))) {
        return HGM_XML_PARAM;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("Params"))) {
        return HGM_XML_PARAMS;
    }
    HGM_LOGD("XMLParser failed to identify a xml node : %{public}s", node.name);
    return HGM_XML_UNDEFINED;
}
} // namespace OHOS::Rosen