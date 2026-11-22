#include <Geode/Geode.hpp>
#include <Geode/modify/CCHttpClient.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "State.hpp"
#include "Popups.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    gc::restore();
}

// Redirige las peticiones oficiales (boomlings.com/database) al servidor activo.
class $modify(GCHttpClient, CCHttpClient) {
    void send(CCHttpRequest* req) {
        if (req && gc::isActive()) {
            std::string url = req->getUrl() ? req->getUrl() : "";
            std::string out;
            if (gc::rewriteUrl(url, out)) {
                req->setUrl(out.c_str());

                // Modo compatibilidad 1.9: anuncia gameVersion=19
                if (gc::active().legacy19) {
                    auto data = req->getRequestData();
                    auto size = req->getRequestDataSize();
                    if (data && size > 0) {
                        std::string body(data, size);
                        std::string from = "gameVersion=22";
                        auto pos = body.find(from);
                        if (pos != std::string::npos) {
                            body.replace(pos, from.size(), "gameVersion=19");
                            req->setRequestData(body.c_str(), body.size());
                        }
                    }
                }
            }
        }
        CCHttpClient::send(req);
    }
};

class $modify(GCMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool toastShown = false;
        if (!toastShown) {
            toastShown = true;
            if (gc::isActive()) {
                Notification::create("GDPS Connect: " + gc::activeName(), NotificationIcon::Success, 2.5f)->show();
            }
        }

        if (!Mod::get()->getSettingValue<bool>("show-button")) return true;

        // Boton con texto "GDPS" (no usa imagenes propias)
        auto icon = ButtonSprite::create("GDPS", 50, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);

        auto btn = CCMenuItemSpriteExtra::create(icon, this, menu_selector(GCMenu::onGC));
        btn->setID("gdps-connect-button"_spr);

        if (auto bottom = this->getChildByID("bottom-menu")) {
            bottom->addChild(btn);
            bottom->updateLayout();
        } else {
            auto fallback = CCMenu::create();
            fallback->setPosition({40.f, 40.f});
            fallback->addChild(btn);
            this->addChild(fallback);
        }
        return true;
    }

    void onGC(CCObject*) {
        ConnectPopup::create()->show();
    }
};
