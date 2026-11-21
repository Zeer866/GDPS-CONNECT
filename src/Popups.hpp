#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "State.hpp"

class StatsPopup : public geode::Popup<GdpsServer const&> {
protected:
    GdpsServer m_server;
    cocos2d::CCLabelBMFont* m_info = nullptr;
    geode::EventListener<geode::utils::web::WebTask> m_pingListener;
    geode::EventListener<geode::utils::web::WebTask> m_topListener;
    std::string m_ping = "...";
    std::string m_total = "...";
    std::string m_status = "Comprobando...";
    std::string m_top = "...";

    bool setup(GdpsServer const& s) override;
    void render();
    void onDashboard(cocos2d::CCObject*);
public:
    static StatsPopup* create(GdpsServer const& s);
};

class AddPopup : public geode::Popup<std::function<void()>> {
protected:
    std::function<void()> m_done;
    geode::TextInput* m_name = nullptr;
    geode::TextInput* m_url = nullptr;
    geode::TextInput* m_dash = nullptr;
    CCMenuItemToggler* m_legacy = nullptr;

    bool setup(std::function<void()> done) override;
    void onSave(cocos2d::CCObject*);
public:
    static AddPopup* create(std::function<void()> done);
};

class ConnectPopup : public geode::Popup<> {
protected:
    cocos2d::CCLabelBMFont* m_status = nullptr;
    geode::ScrollLayer* m_scroll = nullptr;
    std::vector<GdpsServer> m_servers;
    geode::EventListener<geode::utils::web::WebTask> m_checkListener;

    bool setup() override;
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
