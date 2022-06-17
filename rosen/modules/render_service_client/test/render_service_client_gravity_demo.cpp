#include <iostream>

#include "wm/window.h"
#include "window_option.h"
#include "wm_common.h"
#include "transaction/rs_transaction.h"
#include "property/rs_properties_def.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Rosen;

std::shared_ptr<RSNode> rootNode;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    cout << "rs app demo Init Rosen Backend!" << endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorRED);

    rsUiDirector->SetRoot(rootNode->GetId());
}

int main()
{
    cout << "rs gravity demo start!" << endl;

    sptr<WindowOption> option = new WindowOption();
    option->SetDisplayId(0);
    option->SetWindowRect({0, 0, 600, 480});
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowName("gravity_demo");
    auto window = Window::Create(option->GetWindowName(), option);
    window->Show();

    const auto& rect = window->GetRect();
    cout << "rs app demo create window " << rect.width_ << " " << rect.height_ << endl;

    auto surfaceNode = window->GetSurfaceNode();
    auto surface = surfaceNode->GetSurface();
    surface->SetDefaultWidthAndHeight(480, 320);
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(1);

    surfaceNode->SetFrameGravity(Gravity::RIGHT);
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    surfaceNode->SetFrameGravity(Gravity::BOTTOM);
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    surfaceNode->SetFrameGravity(Gravity::LEFT);
    RSTransaction::FlushImplicitTransaction();

    cout << "rs gravity demo end!" << endl;
    return 0;
}
