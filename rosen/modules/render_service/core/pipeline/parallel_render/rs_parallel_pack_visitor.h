#ifndef RS_PARALLEL_PACK_VISITOR_H
#define RS_PARALLEL_PACK_VISITOR_H

#include <vector>
#include <string>
#include "platform/common/rs_system_properties.h"
#include "visitor/rs_node_visitor.h" 
namespace OHOS {
namespace Rosen {
class RSUniRenderVisitor;
class RSParallelPackVisitor : public RSNodeVisitor {
public:
    RSParallelPackVisitor(RSUniRenderVisitor &visitor);
    ~RSParallelPackVisitor() = default;

    void PrepareBaseRenderNode(RSBaseRenderNode& node) override {}
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void PrepareDisplayRenderNode(RSDisplayNode& node) override {}
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
    void PrepareRootRenderNode(RSRootRenderNode& node) override {}
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}

    void ProcessBaseRenderNode(RSBaseRenderNode& node) override {}
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void ProcessDisplayRenderNode(RSDisplayNode& node) override {}
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
    void ProcessRootRenderNode(RSRootRenderNode& node) override {}
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
private:
    bool isOpDropped_ = false;
    bool isPartialRenderEnable_ = false;
    bool isDirtyRegionDfxEnable_ = false;
    bool isTargetDirtyRegionDfxEnable_ = false;
    std::vector<std::string> dfxTargetSurfaceNames_;
    bool isSecurityDisplay_ = false;
    bool doAnimate_ = false;
    PartialRenerType partialRenderType_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_PARALLEL_PACK_VISITOR_H