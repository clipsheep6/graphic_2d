#pragma once
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <SkBitmap.h>
#include <cstring>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <unordered_map>
// #include <securec.h>

#ifdef WIN_64
static std::string replace(std::string str, std::string substr1, std::string substr2)
{
    for (size_t i = str.find(substr1, 0); i != std::string::npos && substr1.length(); i = str.find(substr1, i + substr2.length()))
        str.replace(i, substr1.length(), substr2);
    return str;
}
template<typename ... Args>
static void logPrint(const char* head, std::string format, Args ..args)
{
    std::string target = "{public}";
    format = replace(format, target, std::string(""));
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'  zjz
    if (size_s <= 0) { return; }
    auto size = static_cast<size n't want the '\0' inside
    std::cout << head <<info.c_str(); 
}
template<typename ... Args>
static void RS_LOGE(std::string x, Args ...y) { logPrint("Error: ", x, y...); }
template<typename ... Args>
static void RS_LOGW(std::string x, Args ...y) { logPrint("Warning: ", x, y...); }
template<typename ... Args>
static void RS_LOGD(std::string x, Args ...y) { logPrint("Debug: ", x, y...); }
template<typename ... Args>
static void RS_LOGI(std::string x, Args ...y) { logPrint("Info: ", x, y...); }
#else
#include "securec.h"
#include "platform/common/rs_log.h"
#endif // WIN_64

namespace OHOS {
namespace Rosen {
namespace rs_rcd {
#define XML2_CAST(x) reinterpret_cast<char*>(x)
#ifdef WIN_64
const char PATH_CONFIG_FILE[] = "D:/DataSet/RoundCornerDisplay/mate40pro/config.xml";
const char PATH_CONFIG_DIR[] = "D:/DataSet/RoundCornerDisplay/mate40pro";
#else
const char PATH_CONFIG_FILE[] = "/data/display/RoundCornerDisplay/config.xml";
const char PATH_CONFIG_DIR[] = "/data/display/RoundCornerDisplay";
#endif // WIN_64

const char NODE_ROUNDCORNERDISPLAY[] = "RoundCornerDisplay";
const char NODE_LCDMODEL[] = "LCDModel";
const char NODE_SURFACECONFIG[] = "SurfaceConfig";
const char NODE_TOPSURFACE[] = "TopSurface";
const char NODE_BOTTOMSURFACE[] = "BottomSurface";
const char NODE_SIDEREGIONCONFIG[] = "SideRegionConfig";
const char NODE_SIDEREGION[] = "SideRegion";
const char NODE_HARDWARECOMPOSERCONFIG[] = "HardwareComposerConfig";
const char NODE_HARDWARECOMPOSER[] = "HardwareComposer";
const char NODE_ROG[] = "ROG";
const char NODE_PORTRAIT[] = "Portrait";
const char NODE_LANDSCAPE[] = "Landscape";
const char NODE_PORTRAIT_SIDEREGION[] = "Portrait_sideRegion";
const char NODE_LANDSCAPE_SIDEREGION[] = "Landscape_sideRegion";
const char NODE_LAYERUP[] = "LayerUp";
const char NODE_LAYERDOWN[] = "LayerDown";
const char NODE_LAYERHIDE[] = "LayerHide";
const char NODE_CAMERAAUO[] = "CameraAuo";

const char ATTR_SUPPORT[] = "Support";
const char ATTR_DISPLAYMODE[] = "DisplayMode";
const char ATTR_REGIONMODE[] = "RegionMode";
const char ATTR_NAME[] = "Name";
const char ATTR_WIDTH[] = "Width";
const char ATTR_HEIGHT[] = "Height";
const char ATTR_FILENAME[] = "Filename";
const char ATTR_BINFILENAME[] = "BinFilename";
const char ATTR_OFFSET_X[] = "OffsetX";
const char ATTR_OFFSET_Y[] = "OffsetY";
const char ATTR_BUFFERSIZE[] = "BufferSize";
const char ATTR_CLDWIDTH[] = "CldWidth";
const char ATTR_CLDHEIGHT[] = "CldHeight";
const char ATTR_DEFAULT[] = "default";

using XMLReader = struct XMLReader {
    XMLReader() {}
    virtual ~XMLReader() {
        if (pdoc != nullptr) {
            xmlFreeDoc(pdoc);
        }
        xmlCleanupParser();
        xmlMemoryDump();
    }
    static xmlNodePtr FindNode(const xmlNodePtr& src, const std::string& index);
    static char* ReadAttrStr(const xmlNodePtr& src, const char* attr);
    static int ReadAttrInt(const xmlNodePtr& src, const char* attr);
    static float ReadAttrFloat(const xmlNodePtr& src, const char* attr);
    static bool ReadAttrBool(const xmlNodePtr& src, const char* attr);
    bool Init(const char* file);
    xmlNodePtr ReadNode(std::vector<std::string> attribute);
    xmlChar* Read(std::vector<std::string> attribute);
private:
    xmlDocPtr pdoc = nullptr;
    xmlNodePtr proot = nullptr;
};

using SupportConfig = struct SupportConfig {
    bool support = false;
    int mode = 0;
    bool ReadXmlNode(const xmlNodePtr& ptr, const char* supportAttr, const char* modeAttr);
};

using RoundCornerLayer = struct RoundCornerLayer {
    std::string fileName;
    int offsetX = 0;
    int offsetY = 0;
    std::string binFileName;
    int bufferSize = 0;
    int cldWidth = 0;
    int cldHeight = 0;
    uint32_t layerWidth = 0;
    uint32_t layerHeight = 0;
    SkBitmap* curBitmap = nullptr;
    bool ReadXmlNode(const xmlNodePtr& ptr, std::vector<std::string> attrArray);
};

using RoundCornerHardware = struct RoundCornerHardware {
    bool supportHardware = false;
    RoundCornerLayer* topLayer = nullptr;
    RoundCornerLayer* bottomLayer = nullptr;
};

using RogPortrait = struct RogPortrait {
    RoundCornerLayer layerUp;
    RoundCornerLayer layerDown;
    RoundCornerLayer layerHide;
    bool ReadXmlNode(const xmlNodePtr& portraitNodePtr);
};

using RogLandscape = struct RogLandscape {
    RoundCornerLayer layerUp;
    bool ReadXmlNode(const xmlNodePtr& landNodePtr);
};

using ROGSetting = struct ROGSetting {
    int width = 0;
    int height = 0;
    std::unordered_map<std::string, RogPortrait> portraitMap;
    std::unordered_map<std::string, RogLandscape> landscapeMap;
    bool ReadXmlNode(const xmlNodePtr& rogNodePtr);
};

using SurfaceConfig = struct SurfaceConfig {
    SupportConfig topSurface;
    SupportConfig bottomSurface;
    bool ReadXmlNode(const xmlNodePtr& surfaceConfigNodePtr);
};

using SideRegionConfig = struct SideRegionConfig {
    SupportConfig sideRegion;
    bool ReadXmlNode(const xmlNodePtr& sideRegionNodePtr);
};

using HardwareComposer = struct HardwareComposer {
    bool support = false;
    bool ReadXmlNode(const xmlNodePtr& ptr, const char* supportAttr);
};

using HardwareComposerConfig = struct HardwareComposerConfig {
    HardwareComposer hardwareComposer;
    bool ReadXmlNode(const xmlNodePtr& hardwareComposerNodePtr);
};

using LCDModel = struct LCDModel {
    LCDModel() {}
    virtual ~LCDModel();
    std::string name;
    SurfaceConfig surfaceConfig;
    SideRegionConfig sideRegionConfig;
    HardwareComposerConfig hardwareConfig;
    std::vector<ROGSetting*> rogs;
    bool ReadXmlNode(const xmlNodePtr& lcdNodePtr);
    SurfaceConfig GetSurfaceConfig() const;
    SideRegionConfig GetSideRegionConfig() const;
    HardwareComposerConfig GetHardwareComposerConfig() const;
    ROGSetting* GetRog(const int w, const int h) const;
};

using RCDConfig = struct RCDConfig {
    RCDConfig() {}
    virtual ~RCDConfig();
    static void PrintLayer(const char* name, const rs_rcd::RoundCornerLayer& layer);
    static void PrintParseRog(rs_rcd::ROGSetting* rog);
    static void PrintParseLcdModel(rs_rcd::LCDModel* model);
    static void PrintLcdModel(rs_rcd::RCDConfig* rcdCfg);
    LCDModel* GetLcdModel(const char* name) const;
    bool Load(const char* configFile);
private:
    xmlDocPtr pDoc= nullptr;
    xmlNodePtr pRoot = nullptr;
    std::vector<LCDModel*> lcdModels;
};
} // namespace rs_rcd
} // namespace Rosen
} // namespace OHOS