#include "Popups.hpp"
#include <chrono>

using namespace geode::prelude;

// ---------------------------------------------------------------- StatsPopup

StatsPopup* StatsPopup::create(GdpsServer const& s) {
    auto ret = new StatsPopup();
    if (ret->init(s)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool StatsPopup::init(GdpsServer const& s) {
    if (!Popup::init(320.f, 250.f)) return false;
    m_server = s;
    setTitle(("Stats: " + s.name).c_str());
    auto size = m_mainLayer->getContentSize();

    m_info = CCLabelBMFont::create("", "chatFont.fnt");
    m_info->setAnchorPoint({0.f, 1.f});
    m_info->setAlignment(kCCTextAlignmentLeft);
    m_info->setScale(0.8f);
    m_info->setPosition({24.f, size.height - 45.f});
    m_mainLayer->addChild(m_info);

    auto menu = CCMenu::create();
    menu->setPosition({0.f, 0.f});
    m_mainLayer->addChild(menu);
    auto spr = ButtonSprite::create("Abrir dashboard", 120, true, "bigFont.fnt", "GJ_button_01.png", 28.f, 0.6f);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(StatsPopup::onDashboard));
    btn->setPosition({size.width / 2, 28.f});
    menu->addChild(btn);

    int gv = s.gameVersion;
    std::string gvs = std::to_string(gv);
    auto start = std::chrono::steady_clock::now();

    // Ping + total de niveles
    m_pingListener.spawn(
        gc::post(s, "getGJLevels21.php",
            "gameVersion=" + gvs + "&binaryVersion=45&type=4&str=&diff=-&len=-&page=0&total=0&secret=Wmfd2893gb7"),
        [this, start](web::WebResponse res) {
            if (res.ok()) {
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
                m_ping = std::to_string(ms) + " ms";
                m_status = "En linea";
                std::string str = res.string().unwrapOr("");
                auto parts = gc::split(str, '#');
                m_total = "?";
                if (parts.size() > 3) {
                    auto pg = gc::split(parts[3], ':');
                    if (!pg.empty() && !pg[0].empty()) m_total = pg[0];
                }
            } else {
                m_status = "Error HTTP " + std::to_string(res.code());
                m_ping = "-";
                m_total = "-";
            }
            render();
        });

    // Top de jugadores
    m_topListener.spawn(
        gc::post(s, "getGJScores20.php",
            "gameVersion=" + gvs + "&binaryVersion=45&type=top&count=5&secret=Wmfd2893gb7"),
        [this](web::WebResponse res) {
            if (!res.ok()) { m_top = "No disponible"; render(); return; }
            std::string str = res.string().unwrapOr("");
            std::string out;
            int rank = 1;
            for (auto& entry : gc::split(str, '|')) {
                if (entry.empty()) continue;
                auto kv = gc::split(entry, ':');
                std::string name, stars = "0";
                for (size_t i = 0; i + 1 < kv.size(); i += 2) {
