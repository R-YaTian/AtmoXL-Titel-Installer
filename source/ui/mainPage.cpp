#include <filesystem>
#include <switch.h>
#include "ui/MainApplication.hpp"
#include "ui/mainPage.hpp"
#include "util/util.hpp"
#include "util/config.hpp"
#include "util/lang.hpp"
#include "data/buffered_placeholder_writer.hpp"
#include "nx/usbhdd.h"

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
    extern MainApplication *mainApp;
    static s32 prev_touchcount = 0;
    bool appletFinished = false;

    void mainMenuThread() {
        bool menuLoaded = mainApp->IsShown();
        if (!appletFinished && appletGetAppletType() == AppletType_LibraryApplet) {
            tin::data::NUM_BUFFER_SEGMENTS = 2;
            if (menuLoaded) {
                inst::ui::appletFinished = true;
                mainApp->CreateShowDialog("main.applet.title"_lang, "main.applet.desc"_lang, {"common.ok"_lang}, true);
            } 
        } else if (!appletFinished) {
            inst::ui::appletFinished = true;
            tin::data::NUM_BUFFER_SEGMENTS = 128;
        }
    }

    MainPage::MainPage() : Layout::Layout() {
        this->SetBackgroundColor(COLOR("#670000FF"));
        if (std::filesystem::exists(inst::config::appDir + "/background.png")) this->SetBackgroundImage(inst::config::appDir + "/background.png");
        else this->SetBackgroundImage("romfs:/images/background.jpg");
        this->topRect = Rectangle::New(0, 0, 1280, 94, COLOR("#170909FF"));
        this->botRect = Rectangle::New(0, 659, 1280, 61, COLOR("#17090980"));
        this->titleImage = Image::New(0, 0, "romfs:/images/logo.png");
        this->appVersionText = TextBlock::New(490, 29, "v" + inst::config::appVersion, 42);
        this->appVersionText->SetColor(COLOR("#FFFFFFFF"));
        this->butText = TextBlock::New(10, 678, "main.buttons"_lang, 24);
        this->butText->SetColor(COLOR("#FFFFFFFF"));
        this->optionMenu = pu::ui::elm::Menu::New(0, 95, 1280, COLOR("#67000000"), 94, 6);
        this->optionMenu->SetOnFocusColor(COLOR("#00000033"));
        this->optionMenu->SetScrollbarColor(COLOR("#170909FF"));
        this->installMenuItem = pu::ui::elm::MenuItem::New("main.menu.sd"_lang);
        this->installMenuItem->SetColor(COLOR("#FFFFFFFF"));
        this->installMenuItem->SetIcon("romfs:/images/icons/micro-sd.png");
        this->netInstallMenuItem = pu::ui::elm::MenuItem::New("main.menu.net"_lang);
        this->netInstallMenuItem->SetColor(COLOR("#FFFFFFFF"));
        this->netInstallMenuItem->SetIcon("romfs:/images/icons/cloud-download.png");
        this->usbInstallMenuItem = pu::ui::elm::MenuItem::New("main.menu.usb"_lang);
        this->usbInstallMenuItem->SetColor(COLOR("#FFFFFFFF"));
        this->usbInstallMenuItem->SetIcon("romfs:/images/icons/usb-port.png");
        this->usbHDDInstallMenuItem = pu::ui::elm::MenuItem::New("main.menu.hdd"_lang);
        this->usbHDDInstallMenuItem->SetColor(COLOR("#FFFFFFFF"));
        this->usbHDDInstallMenuItem->SetIcon("romfs:/images/icons/usb-port.png");
        this->settingsMenuItem = pu::ui::elm::MenuItem::New("main.menu.set"_lang);
        this->settingsMenuItem->SetColor(COLOR("#FFFFFFFF"));
        this->settingsMenuItem->SetIcon("romfs:/images/icons/settings.png");
        this->exitMenuItem = pu::ui::elm::MenuItem::New("main.menu.exit"_lang);
        this->exitMenuItem->SetColor(COLOR("#FFFFFFFF"));
        this->exitMenuItem->SetIcon("romfs:/images/icons/exit-run.png");
        this->Add(this->topRect);
        this->Add(this->botRect);
        this->Add(this->titleImage);
        this->Add(this->appVersionText);
        this->Add(this->butText);
        this->optionMenu->AddItem(this->installMenuItem);
        this->optionMenu->AddItem(this->netInstallMenuItem);
        this->optionMenu->AddItem(this->usbInstallMenuItem);
        this->optionMenu->AddItem(this->usbHDDInstallMenuItem);
        this->optionMenu->AddItem(this->settingsMenuItem);
        this->optionMenu->AddItem(this->exitMenuItem);
        this->Add(this->optionMenu);
        this->AddThread(mainMenuThread);
    }

    void MainPage::installMenuItem_Click() {
        mainApp->sdinstPage->drawMenuItems(true, "sdmc:/");
        mainApp->sdinstPage->menu->SetSelectedIndex(0);
        mainApp->LoadLayout(mainApp->sdinstPage);
    }

    void MainPage::netInstallMenuItem_Click() {
        if (inst::util::getIPAddress() == "1.0.0.127") {
            inst::ui::mainApp->CreateShowDialog("main.net.title"_lang, "main.net.desc"_lang, {"common.ok"_lang}, true);
            return;
        }
        mainApp->netinstPage->startNetwork();
    }

    void MainPage::usbInstallMenuItem_Click() {
        if (!inst::config::usbAck) {
            if (mainApp->CreateShowDialog("main.usb.warn.title"_lang, "main.usb.warn.desc"_lang, {"common.ok"_lang, "main.usb.warn.opt1"_lang}, false) == 1) {
                inst::config::usbAck = true;
                inst::config::setConfig();
            }
        }
        if (inst::util::usbIsConnected()) mainApp->usbinstPage->startUsb();
        else mainApp->CreateShowDialog("main.usb.error.title"_lang, "main.usb.error.desc"_lang, {"common.ok"_lang}, false);
    }

    void MainPage::usbHDDInstallMenuItem_Click() {
		if(nx::hdd::count() && nx::hdd::rootPath()) {
			mainApp->usbhddinstPage->drawMenuItems(true, nx::hdd::rootPath());
			mainApp->usbhddinstPage->menu->SetSelectedIndex(0);
			mainApp->LoadLayout(mainApp->usbhddinstPage);
		} else {
			inst::ui::mainApp->CreateShowDialog("main.hdd.title"_lang, "main.hdd.notfound"_lang, {"common.ok"_lang}, true);
		}
    }

    void MainPage::exitMenuItem_Click() {
        mainApp->FadeOut();
        mainApp->Close();
    }

    void MainPage::settingsMenuItem_Click() {
        mainApp->LoadLayout(mainApp->optionspage);
    }

    void MainPage::onInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) {
        if ((Down & HidNpadButton_Plus) || (Down & HidNpadButton_Minus)) {
            mainApp->FadeOut();
            mainApp->Close();
        }
        if ((Down & HidNpadButton_A) || (pu::ui::Application::GetTouchState().count == 0 && prev_touchcount == 1)) {
            prev_touchcount = 0;
            switch (this->optionMenu->GetSelectedIndex()) {
                case 0:
                    this->installMenuItem_Click();
                    break;
                case 1:
                    this->netInstallMenuItem_Click();
                    break;
                case 2:
                    MainPage::usbInstallMenuItem_Click();
                    break;
                case 3:
                    MainPage::usbHDDInstallMenuItem_Click();
                    break;
                case 4:
                    MainPage::settingsMenuItem_Click();
                    break;
                case 5:
                    MainPage::exitMenuItem_Click();
                    break;
                default:
                    break;
            }
        }
        if (pu::ui::Application::GetTouchState().count == 1)
            prev_touchcount = 1;
    }
}
