#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp>
#include <functional>
#include "State.hpp"

class StatsPopup : public geode::Popup {
protected:
    GdpsServer m_server;
    cocos2d::CCLabelBMFont* m_info = nullptr;
    geode::async::TaskHolder<geode::utils::web::WebResponse> m_pingListener;
    geode::async::TaskHolder<geode::utils::web::WebResponse> m_topListener;
    std::string m_ping = "...";
    std::string m_total = "...";
    std::string m_status = "Comprobando...";
    std::string m_top = "...";

    bool init(GdpsServer const& s);
    void render();
    void onDashboard(cocos2d::CCObject*);
public:
    static StatsPopup* create(GdpsServer const& s);
};

class AddPopup : public geode::Popup {
protected:
    std::function<void()> m_done;
    geode::TextInput* m_name = nullptr;
    geode::TextInput* m_url = nullptr;
    geode::TextInput* m_dash = nullptr;
    CCMenuItemToggler* m_legacy = nullptr;

    bool init(std::function<void()> done);
    void onSave(cocos2d::CCObject*);
public:
    static AddPopup* create(std::function<void()> done);
};

class ConnectPopup : public geode::Popup {
protected:
    cocos2d::CCLabelBMFont* m_status = nullptr;
    geode::ScrollLayer* m_scroll = nullptr;
    std::vector<GdpsServer> m_servers;
    geode::async::TaskHolder<geode::utils::web::WebResponse> m_checkListener;

    bool init();
    void refresh();
    void doConnect(GdpsServer const& s);
    void onConnect(cocos2d::CCObject*);
    void onStats(cocos2d::CCObject*);
    void onWeb(cocos2d::CCObject*);
    void onDelete(cocos2d::CCObject*);
    void onAdd(cocos2d::CCObject*);
    void onImport(cocos2d::CCObject*);
    void onOfficial(cocos2d::CCObject*);
public:
    static ConnectPopup* create();
};
