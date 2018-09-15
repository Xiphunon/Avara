#include "CNetworkWindow.h"

#include "CAvaraApp.h"
#include "CNetManager.h"
#include "Preferences.h"
#include "CommandList.h"

CNetworkWindow::CNetworkWindow(CApplication *app) : CWindow(app, "Network") {
    setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Fill, 20, 10));

    addressBox = new nanogui::TextBox(this);
    addressBox->setValue("avara.jp");
    addressBox->setEditable(true);

    connectBtn = new nanogui::Button(this, "Connect");
    connectBtn->setCallback([this, app] {
        CAvaraApp *avara = (CAvaraApp *)app;
        if(avara->GetNet()->netStatus == kClientNet)
            avara->GetNet()->ChangeNet(kNullNet, "");
        else
            avara->GetNet()->ChangeNet(kClientNet, this->addressBox->value());
    });

    startBtn = new nanogui::Button(this, "Start Hosting");
    startBtn->setCallback([app] {
        CAvaraApp *avara = (CAvaraApp *)app;
        if(avara->GetNet()->netStatus == kServerNet)
            avara->GetNet()->ChangeNet(kNullNet, "");
        else
            avara->GetNet()->ChangeNet(kServerNet, "");
    });

    latencyBox = new nanogui::TextBox(this);
    latencyBox->setValue(std::to_string(app->Number(kLatencyToleranceTag)));
    latencyBox->setEditable(true);
    latencyBox->setCallback([app](std::string value) -> bool {
        app->Set(kLatencyToleranceTag, std::stoi(value));
        return true;
    });

    nanogui::CheckBox *autoLatencyBox = new nanogui::CheckBox(this, "Auto Latency", [app](bool checked) {
        long options = app->Number(kServerOptionsTag);
        if (checked)
            options |= 1 << kUseAutoLatencyBit;
        else
            options &= ~(long)(1 << kUseAutoLatencyBit);
        app->Set(kServerOptionsTag, options);
        ((CAvaraApp *)app)->GetNet()->ChangedServerOptions(options);
    });
    bool autoLatency = app->Number(kServerOptionsTag) & (1 << kUseAutoLatencyBit);
    autoLatencyBox->setChecked(autoLatency);
}

CNetworkWindow::~CNetworkWindow() {}

bool CNetworkWindow::DoCommand(int theCommand) {
    switch(theCommand) {
        case kNetChangedCmd:
            CAvaraApp *app = (CCAvaraApp *)gApplication;
            switch(app->GetNet()->netStatus) {
                case kNullNet:
                    addressBox->setEnabled(true);
                    connectBtn->setEnabled(true);
                    connectBtn->setEnabled("Connect");
                    startBtn->setEnabled(true);
                    startBtn->setCaption("Start Hosting");
                    break;
                case kClientNet:
                    addressBox->setEnabled(false);
                    connectBtn->setCaption("Disconnect");
                    startBtn->setEnabled(false);
                    break;
                case kServerNet:
                    addressBox->setEnabled(false);
                    connectBtn->setEnabled(false);
                    startBtn->setCaption("Stop Hosting");
                    break;
            }
            break;
    }
    return false;
}

void CNetworkWindow::PrefChanged(std::string name) {
    latencyBox->setValue(std::to_string(mApplication->Number(kLatencyToleranceTag)));
}
