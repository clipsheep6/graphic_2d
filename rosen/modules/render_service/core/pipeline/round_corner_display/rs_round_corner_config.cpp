#include "rs_round_corner_config.h"

namespace OHOS {
namespace Rosen {
namespace rs_rcd {
xmlNodePtr XMLReader::FindNode(const xmlNodePtr& src, const std::string& index)
{
    xmlNodePtr startPtr = src->children;
    while (startPtr != nullptr) {
        auto name = startPtr->name;
        if (xmlStrEqual(name, BAD_CAST(index.c_str())) == 1){
            return startPtr;
        }
        startPtr = startPtr->next;
    }
    return nullptr;
}

char* XMLReader::ReadAttrStr(const xmlNodePtr& src, const char* attr)
{
    return XML2_CAST(xmlGetProp(src, BAD_CAST(attr)));
}

int XMLReader::ReadAttrInt(const xmlNodePtr& src, const char* attr)
{
    auto result = XML2_CAST(xmlGetProp(src, BAD_CAST(attr)));
    if (result == nullptr) {
        return 0;
    }
    return std::atoi(result);
}

float XMLReader::ReadAttrFloat(const xmlNodePtr& src, const char* attr)
{
    auto result = XML2_CAST(xmlGetProp(src, BAD_CAST(attr)));
    if (result == nullptr) {
        return 0.0f;
    }
    return std::atof(result);
}

bool XMLReader::ReadAttrBool(const xmlNodePtr& src, const char* attr)
{
    auto result = XML2_CAST(xmlGetProp(src, BAD_CAST(attr)));
    if (result == nullptr) {
        return false;
    }
    auto istrue = strcmp(result, "true") == 0 ? true : false;
    return istrue;
}

bool XMLReader::Init(const char* file)
{
    xmlKeepBlanksDefault(0);
    pdoc = xmlReadFile(file, "", XML_PARSE_RECOVER);
    if (pdoc == nullptr) {
        return false;
    }
    proot = xmlDocGetRootElement(pdoc);
    if (proot == nullptr) {
        return false;
    }
    return true;
}

xmlNodePtr XMLReader::ReadNode(std::vector<std::string> attribute)
{
    xmlNodePtr ptr = proot;
    auto size = static_cast<int>(attribute.size());
    for (int i = 0; i < size; i++) {
        ptr = FindNode(ptr, attribute[i]);
        if (i == static_cast<int>(attribute.size()) - 1) {
            return ptr;
        }
        if (ptr == nullptr) {
            return nullptr;
        }
        else {
            ptr = ptr->children;
        }
    }
    return ptr;
}

xmlChar* XMLReader::Read(std::vector<std::string> attribute)
{
    if (attribute.size() < 2) {
        return nullptr;
    }
    std::vector<std::string> nodeIndex(attribute.begin(), attribute.end() - 1);
    auto lastNode = ReadNode(nodeIndex);
    if (lastNode != nullptr) {
        return xmlGetProp(lastNode, BAD_CAST(attribute[attribute.size() - 1].c_str()));
    }
    return nullptr;
}

bool SupportConfig::ReadXmlNode(const xmlNodePtr& ptr, const char* supportAttr, const char* modeAttr)
{
    auto a1 = xmlGetProp(ptr, BAD_CAST(supportAttr));
    if (a1 == nullptr) {
        return false;
    }
    support = XMLReader::ReadAttrBool(ptr, supportAttr);
    mode = XMLReader::ReadAttrInt(ptr,modeAttr);
    return true;
}

bool RoundCornerLayer::ReadXmlNode(const xmlNodePtr& ptr, std::vector<std::string> attrArray)
{
    if (attrArray.size() < 7 || ptr == nullptr) {
        return false;
    }
    fileName = XMLReader::ReadAttrStr(ptr, attrArray[0].c_str());
    offsetX = XMLReader::ReadAttrInt(ptr, attrArray[1].c_str());
    offsetY = XMLReader::ReadAttrInt(ptr, attrArray[2].c_str());
    binFileName = XMLReader::ReadAttrStr(ptr, attrArray[3].c_str());
    bufferSize = XMLReader::ReadAttrInt(ptr, attrArray[4].c_str());
    cldWidth = XMLReader::ReadAttrInt(ptr, attrArray[5].c_str());
    cldHeight = XMLReader::ReadAttrInt(ptr, attrArray[6].c_str());
    return true;
}

bool RogPortrait::ReadXmlNode(const xmlNodePtr& portraitNodePtr)
{
    if (portraitNodePtr == nullptr) {
        return false;
    }
    std::vector<std::string> attrList = { ATTR_FILENAME, ATTR_OFFSET_X, ATTR_OFFSET_Y,
        ATTR_BINFILENAME, ATTR_BUFFERSIZE, ATTR_CLDWIDTH, ATTR_CLDHEIGHT };

    auto layerUpPtr = XMLReader::FindNode(portraitNodePtr, std::string(NODE_LAYERUP));
    if (layerUpPtr == nullptr) {
        return false;
    }
    layerUp.ReadXmlNode(layerUpPtr, attrList);

    auto layerDownPtr = XMLReader::FindNode(portraitNodePtr, std::string(NODE_LAYERDOWN));
    if (layerDownPtr == nullptr) {
        return false;
    }
    layerDown.ReadXmlNode(layerDownPtr, attrList);

    auto layerHidePtr = XMLReader::FindNode(portraitNodePtr, std::string(NODE_LAYERHIDE));
    if (layerHidePtr == nullptr) {
        return false;
    }
    layerHide.ReadXmlNode(layerHidePtr, attrList);
    return true;
}

bool RogLandscape::ReadXmlNode(const xmlNodePtr& landNodePtr)
{
    if (landNodePtr == nullptr) {
        return false;
    }
    std::vector<std::string> attrList = { ATTR_FILENAME, ATTR_OFFSET_X, ATTR_OFFSET_Y,
        ATTR_BINFILENAME, ATTR_BUFFERSIZE, ATTR_CLDWIDTH, ATTR_CLDHEIGHT };
    auto layerUpPtr = XMLReader::FindNode(landNodePtr, std::string(NODE_LAYERUP));
    if (layerUpPtr == nullptr) {
        return false;
    }
    layerUp.ReadXmlNode(layerUpPtr, attrList);
    return true;
}

bool ROGSetting::ReadXmlNode(const xmlNodePtr& rogNodePtr)
{
    if (rogNodePtr == nullptr) {
        return false;
    }
    width = XMLReader::ReadAttrInt(rogNodePtr, ATTR_WIDTH);
    height = XMLReader::ReadAttrInt(rogNodePtr, ATTR_HEIGHT);

    auto portPtr = XMLReader::FindNode(rogNodePtr, NODE_PORTRAIT);
    if (portPtr != nullptr) {
        RogPortrait p;
        p.ReadXmlNode(portPtr);
        portraitMap[NODE_PORTRAIT] = p;
    }

    auto portSidePtr = XMLReader::FindNode(rogNodePtr, NODE_PORTRAIT_SIDEREGION);
    if (portSidePtr != nullptr) {
        RogPortrait p;
        p.ReadXmlNode(portSidePtr);
        portraitMap[NODE_PORTRAIT_SIDEREGION] = p;
    }
    
    auto landPtr = XMLReader::FindNode(rogNodePtr, NODE_LANDSCAPE);
    if (landPtr != nullptr) {
        RogLandscape p;
        p.ReadXmlNode(landPtr);
        landscapeMap[NODE_LANDSCAPE] = p;
    }

    auto landSidePtr = XMLReader::FindNode(rogNodePtr, NODE_LANDSCAPE_SIDEREGION);
    if (landSidePtr != nullptr) {
        RogLandscape p;
        p.ReadXmlNode(landSidePtr);
        landscapeMap[NODE_LANDSCAPE_SIDEREGION] = p;
    }
    return true;
}

bool SurfaceConfig::ReadXmlNode(const xmlNodePtr& surfaceConfigNodePtr)
{
    if (surfaceConfigNodePtr == nullptr) {
        return false;
    }

    auto topSurfacePtr = XMLReader::FindNode(surfaceConfigNodePtr, std::string(NODE_TOPSURFACE));
    if (topSurfacePtr == nullptr) {
        return false;
    }
    topSurface.ReadXmlNode(topSurfacePtr, ATTR_SUPPORT, ATTR_DISPLAYMODE);
    
    auto bottomSurfacePtr = XMLReader::FindNode(surfaceConfigNodePtr, std::string(NODE_BOTTOMSURFACE));
    if (bottomSurfacePtr == nullptr) {
        return false;
    }
    bottomSurface.ReadXmlNode(bottomSurfacePtr, ATTR_SUPPORT, ATTR_DISPLAYMODE);
    return true;
}

bool SideRegionConfig::ReadXmlNode(const xmlNodePtr& sideRegionNodePtr)
{
    if (sideRegionNodePtr == nullptr) {
        return false;
    }
    auto sideRegionPtr = XMLReader::FindNode(sideRegionNodePtr, std::string(NODE_SIDEREGION));
    if (sideRegionPtr == nullptr) {
        return false;
    }
    sideRegion.ReadXmlNode(sideRegionPtr, ATTR_SUPPORT, ATTR_DISPLAYMODE);
    return true;
}

bool HardwareComposer::ReadXmlNode(const xmlNodePtr& ptr, const char* supportAttr)
{
    auto a1 = xmlGetProp(ptr, BAD_CAST(supportAttr));
    if (a1 == nullptr) {
        return false;
    }
    support = XMLReader::ReadAttrBool(ptr,supportAttr);
    return true;
}

bool HardwareComposerConfig::ReadXmlNode(const xmlNodePtr& hardwareComposerNodeptr)
{
    if (hardwareComposerNodeptr == nullptr) {
        return false;
    }
    auto hardwareComposerPtr = XMLReader::FindNode(hardwareComposerNodeptr, std::string(NODE_HARDWARECOMPOSER));
    if (hardwareComposerPtr == nullptr) {
        return false;
    }
    hardwareComposer.ReadXmlNode(hardwareComposerPtr, ATTR_SUPPORT);
    return true;
}

LCDModel::~LCDModel()
{
    auto size = static_cast<int>(rogs.size());
    for (int i = 0; i < size; i++) {
        if (rogs[i] != nullptr) {
            delete rogs[i];
            rogs[i] = nullptr;
        }
    }
}

bool LCDModel::ReadXmlNode(const xmlNodePtr& lcdNodePtr)
{
    if (lcdNodePtr == nullptr) {
        return false;
    }
    name = XMLReader::ReadAttrStr(lcdNodePtr, ATTR_NAME);
    auto surfaceConfigPtr = XMLReader::FindNode(lcdNodePtr, std::string(NODE_SURFACECONFIG));
    if (surfaceConfigPtr == nullptr) {
        return false;
    }
    surfaceConfig.ReadXmlNode(surfaceConfigPtr);

    auto sideRegionConfigPtr = XMLReader::FindNode(lcdNodePtr, std::string(NODE_SIDEREGIONCONFIG));
    if (sideRegionConfigPtr == nullptr) {
        RS_LOGW("no sideRegionConfig found \n");
    }
    sideRegionConfig.ReadXmlNode(sideRegionConfigPtr);

    auto hardwareComposerConfigPtr = XMLReader::FindNode(lcdNodePtr, std::string(NODE_HARDWARECOMPOSERCONFIG));
    if (hardwareComposerConfigPtr == nullptr) {
        RS_LOGW("no harewareComposer found \n");
    }
    hardwareConfig.ReadXmlNode(hardwareComposerConfigPtr);

    xmlNodePtr startPtr = lcdNodePtr->children;
    while (startPtr != nullptr) {
        auto name = startPtr->name;
        if (xmlStrEqual(name, BAD_CAST(NODE_ROG)) == 1) {
            ROGSetting *rog = new ROGSetting();
            if (rog->ReadXmlNode(startPtr)) {
                rogs.push_back(rog);
            }
            else {
                delete rog;
                rog = nullptr;
            }
        }
        startPtr = startPtr->next;
    }
    return true;
}

SurfaceConfig LCDModel::GetSurfaceConfig() const
{
    return surfaceConfig;
}

SideRegionConfig LCDModel::GetSideRegionConfig() const
{
    return sideRegionConfig;
}

HardwareComposerConfig LCDModel::GetHardwareComposerConfig() const
{
    return hardwareConfig;
}

ROGSetting* LCDModel::GetRog(const int w, const int h) const
{
    for (auto rog : rogs) {
        if (rog == nullptr) {
            continue;
        }
        if (rog->width == w && rog->height == h) {
            return rog;
        }
    }
    return nullptr;
}

void RCDConfig::PrintLayer(const char* name, const rs_rcd::RoundCornerLayer& layer)
{
    RS_LOGD("%{public}s->Filename: %{public}s, Offset: %{public}d, %{public}d \n",
        name, layer.fileName.c_str(), layer.offsetX, layer.offsetY);
    RS_LOGD("BinFilename: %{public}s, BufferSize: %{public}d, cldSize:%{public}d, %{public}d \n", 
        layer.binFileName.c_str(), layer.bufferSize, layer.cldWidth, layer.cldHeight);
}

void RCDConfig::PrintParseRog(rs_rcd::ROGSetting* rog)
{
    if (rog == nullptr) {
        RS_LOGE("no model input \n");
        return;
    }
    for (auto kv : rog->portraitMap) {
        auto port = kv.second;
        RS_LOGI("rog: %{public}d, %{public}d, %{public}s: \n", rog->width, rog->height, kv.first.c_str());
        PrintLayer("layerUp  ", port.layerUp);
        PrintLayer("layerDown", port.layerDown);
        PrintLayer("layerHide", port.layerHide);
    }
    for (auto kv : rog->landscapeMap) {
        auto port = kv.second;
        RS_LOGI("rog: %{public}d, %{public}d, %{public}s: \n", rog->width, rog->height, kv.first.c_str());
        PrintLayer("layerUp  ", port.layerUp);
    }
}

void RCDConfig::PrintParseLcdModel(rs_rcd::LCDModel* model)
{
    if (model == nullptr) {
        RS_LOGE("no model input \n");
        return;
    }
    RS_LOGI("lcdModel: %{public}s \n", model->name.c_str());
    auto surfaceConfig = model->GetSurfaceConfig();
    RS_LOGI("surfaceConfig: \n");
    RS_LOGI("topSurface: %{public}d, %{public}d \n", surfaceConfig.topSurface.support, surfaceConfig.topSurface.mode);
    RS_LOGI("bottomSurface: %{public}d, %{public}d \n", surfaceConfig.bottomSurface.support, surfaceConfig.bottomSurface.mode);

    auto sideRegionConfig = model->GetSideRegionConfig();
    RS_LOGI("sideRegionConfig: \n");
    RS_LOGI("sideRegion: %{public}d, %{public}d \n", sideRegionConfig.sideRegion.support,  sideRegionConfig.sideRegion.mode);

    auto hardwareConfig = model->GetHardwareComposerConfig();
    RS_LOGI("hardwareConfig: \n");
    RS_LOGI("hardwareComposer: %{public}d \n", hardwareConfig.hardwareComposer.support);

    struct Point2D {
        Point2D(int W, int H) : w(W), h(H) {}
        int w, h;
    };
    std::vector<Point2D> rogsRS = { Point2D(1344, 2772), Point2D(1152, 2376), Point2D(896, 1848),
        Point2D(1200, 2640), Point2D(800, 1760) };
    std::vector<rs_rcd::ROGSetting*> rogs;
    for (auto rogRS : rogsRS) {
        auto rog = model->GetRog(rogRS.w, rogRS.h);
        if (rog == nullptr) {
            RS_LOGE("get null rog via %{public}d, %{public}d \n", rogRS.w, rogRS.h);
        }
        else {
            RS_LOGI("hardwareComposer: %{public}d \n", hardwareConfig.hardwareComposer.support);
            rogs.push_back(rog);
        }
    }
    for (auto rog : rogs) {
        PrintParseRog(rog);
    }
}

void RCDConfig::PrintLcdModel(rs_rcd::RCDConfig* rcdCfg)
{
    if (rcdCfg == nullptr) {
        RS_LOGE("no rcdCfg input \n");
        return;
    }
    
}

bool RCDConfig::Load(const char* configFile)
{
    xmlKeepBlanksDefault(0);
    pDoc = xmlReadFile(configFile, "", XML_PARSE_RECOVER);
    if (pDoc == nullptr) {
        RS_LOGE("RoundCornerDisplay read xml failed \n");
        return false;
    }
    RS_LOGI("RoundCornerDisplay read xml ok \n");
    pRoot = xmlDocGetRootElement(pDoc);
    if (pRoot == nullptr) {
        RS_LOGE("RoundCornerDisplay get xml root failed \n");
        return false;
    }
    xmlNodePtr startPtr = pRoot->children;
    while (startPtr != nullptr) {
        auto name = startPtr->name;
        if (xmlStrEqual(name, BAD_CAST(NODE_LCDMODEL)) == 1) {
            LCDModel* lcdModel = new LCDModel();
            if (lcdModel->ReadXmlNode(startPtr)) {
                lcdModels.push_back(lcdModel);
            }
            else {
                delete lcdModel;
                lcdModel = nullptr;
            }
        }
        startPtr = startPtr->next;
    }
    return true;
}

LCDModel* RCDConfig::GetLcdModel(const char* name) const
{
    if (name == nullptr) {
        return nullptr;
    }
    for (auto model : lcdModels) {
        if (model == nullptr) {
            continue;
        }
        if (model->name.compare(name) == 0) {
            return model;
        }
    }
    return nullptr;
}

RCDConfig::~RCDConfig()
{
    if (pDoc != nullptr) {
        xmlFreeDoc(pDoc);
    }
    xmlCleanupParser();
    xmlMemoryDump();

    auto size = static_cast<int>(lcdModels.size());
    for (int i = 0; i < size; i++) {
        if (lcdModels[i] != nullptr) {
            delete lcdModels[i];
            lcdModels[i] = nullptr;
        }
    }
}
} // namespace rs_rcd
} // namespace Rosen
} // OHOS