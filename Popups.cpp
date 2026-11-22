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

    int gv = s.legacy19 ? 19 : 22;
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
                    if (kv[i] == "1") name = kv[i + 1];
                    if (kv[i] == "3") stars = kv[i + 1];
                }
                if (name.empty()) continue;
                out += std::to_string(rank++) + ". " + name + "  -  " + stars + " estrellas\n";
                if (rank > 5) break;
            }
            m_top = out.empty() ? "Sin datos" : out;
            render();
        });

    render();
    return true;
}

void StatsPopup::render() {
    std::string t;
    t += "Estado: " + m_status + "\n";
    t += "Ping: " + m_ping + "\n";
    t += "Niveles: " + m_total + "\n\n";
    t += "Top jugadores:\n" + m_top;
    m_info->setString(t.c_str());
}

void StatsPopup::onDashboard(CCObject*) {
    web::openLinkInBrowser(gc::dashboardUrl(m_server));
}

// ---------------------------------------------------------------- AddPopup

AddPopup* AddPopup::create(std::function<void()> done) {
    auto ret = new AddPopup();
    if (ret->init(done)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool AddPopup::init(std::function<void()> done) {
    if (!Popup::init(320.f, 260.f)) return false;
    m_done = done;
    setTitle("Anadir servidor");
    auto size = m_mainLayer->getContentSize();
    float cx = size.width / 2;

    m_name = TextInput::create(260.f, "Nombre (ej: ZeerGDPS)", "bigFont.fnt");
    m_name->setPosition({cx, size.height - 60.f});
    m_mainLayer->addChild(m_name);

    m_url = TextInput::create(260.f, "URL de la carpeta de los .php", "bigFont.fnt");
    m_url->setPosition({cx, size.height - 105.f});
    m_mainLayer->addChild(m_url);

    m_dash = TextInput::create(260.f, "Dashboard (opcional)", "bigFont.fnt");
    m_dash->setPosition({cx, size.height - 150.f});
    m_mainLayer->addChild(m_dash);

    auto menu = CCMenu::create();
    menu->setPosition({0.f, 0.f});
    m_mainLayer->addChild(menu);

    m_legacy = CCMenuItemToggler::createWithStandardSprites(this, nullptr, 0.6f);
    m_legacy->setPosition({40.f, 62.f});
    menu->addChild(m_legacy);
    auto lbl = CCLabelBMFont::create("Modo compatibilidad 1.9", "bigFont.fnt");
    lbl->setScale(0.4f);
    lbl->setAnchorPoint({0.f, 0.5f});
    lbl->setPosition({62.f, 62.f});
    m_mainLayer->addChild(lbl);

    auto spr = ButtonSprite::create("Guardar", 90, true, "bigFont.fnt", "GJ_button_01.png", 28.f, 0.7f);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AddPopup::onSave));
    btn->setPosition({cx, 26.f});
    menu->addChild(btn);
    return true;
}

void AddPopup::onSave(CCObject*) {
    GdpsServer s;
    s.name = m_name->getString();
    s.url = m_url->getString();
    s.dashboard = m_dash->getString();
    s.legacy19 = m_legacy->isToggled();
    if (s.name.empty() || s.url.empty()) {
        FLAlertLayer::create("GDPS Connect", "Pon al menos un nombre y la URL.", "OK")->show();
        return;
    }
    gc::addServer(s);
    if (m_done) m_done();
    this->onClose(nullptr);
}

// ---------------------------------------------------------------- ConnectPopup

static bool endsWith(std::string const& s, std::string const& suf) {
    return s.size() >= suf.size() && s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

ConnectPopup* ConnectPopup::create() {
    auto ret = new ConnectPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ConnectPopup::init() {
    if (!Popup::init(380.f, 270.f)) return false;
    setTitle("GDPS Connect");
    auto size = m_mainLayer->getContentSize();

    m_status = CCLabelBMFont::create("", "goldFont.fnt");
    m_status->setScale(0.5f);
    m_status->setPosition({size.width / 2, size.height - 42.f});
    m_mainLayer->addChild(m_status);

    auto bg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    bg->setColor({0, 0, 0});
    bg->setOpacity(90);
    bg->setContentSize({340.f, 150.f});
    bg->setPosition({size.width / 2, 140.f});
    m_mainLayer->addChild(bg);

    m_scroll = ScrollLayer::create({340.f, 150.f});
    m_scroll->setPosition({20.f, 65.f});
    m_mainLayer->addChild(m_scroll);

    // linea de diagnostico: URL activa y peticiones redirigidas
    m_info = CCLabelBMFont::create("", "chatFont.fnt");
    m_info->setScale(0.5f);
    m_info->setPosition({size.width / 2, 56.f});
    m_mainLayer->addChild(m_info);

    auto menu = CCMenu::create();
    menu->setPosition({0.f, 0.f});
    m_mainLayer->addChild(menu);

    auto mk = [&](char const* text, int w, SEL_MenuHandler h, float x) {
        auto spr = ButtonSprite::create(text, w, true, "bigFont.fnt", "GJ_button_01.png", 28.f, 0.6f);
        auto b = CCMenuItemSpriteExtra::create(spr, this, h);
        b->setPosition({x, 30.f});
        menu->addChild(b);
    };
    mk("Anadir", 70, menu_selector(ConnectPopup::onAdd), 75.f);
    mk("Importar", 80, menu_selector(ConnectPopup::onImport), 190.f);
    mk("Oficial", 70, menu_selector(ConnectPopup::onOfficial), 305.f);

    this->schedule(schedule_selector(ConnectPopup::updateStatus), 1.f);
    refresh();
    return true;
}

void ConnectPopup::updateStatus(float) {
    if (!m_info) return;
    if (gc::isActive()) {
        std::string t = gc::active().url + "  |  redirigidas: " + std::to_string(gc::redirectCount());
        m_info->setString(t.c_str());
    } else {
        m_info->setString("Sin redireccion (servidores de RobTop)");
    }
}

void ConnectPopup::refresh() {
    m_servers = gc::loadServers();
    if (gc::isActive()) m_status->setString(("Conectado: " + gc::activeName()).c_str());
    else m_status->setString("Servidores oficiales (RobTop)");

    auto content = m_scroll->m_contentLayer;
    content->removeAllChildren();

    float rowH = 36.f;
    float h = std::max(150.f, rowH * m_servers.size());
    content->setContentSize({340.f, h});

    for (size_t i = 0; i < m_servers.size(); i++) {
        float y = h - rowH / 2 - i * rowH;
        auto& s = m_servers[i];

        auto lbl = CCLabelBMFont::create(s.name.c_str(), "bigFont.fnt");
        lbl->setAnchorPoint({0.f, 0.5f});
        lbl->limitLabelWidth(110.f, 0.45f, 0.15f);
        lbl->setPosition({8.f, y});
        if (gc::isActive() && gc::activeName() == s.name) lbl->setColor({100, 255, 100});
        content->addChild(lbl);

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});
        content->addChild(menu);

        auto add = [&](char const* text, int w, SEL_MenuHandler h, float x, char const* tex) {
            auto spr = ButtonSprite::create(text, w, true, "bigFont.fnt", tex, 24.f, 0.5f);
            auto b = CCMenuItemSpriteExtra::create(spr, this, h);
            b->setTag(static_cast<int>(i));
            b->setPosition({x, y});
            menu->addChild(b);
        };
        add("Ir", 30, menu_selector(ConnectPopup::onConnect), 160.f, "GJ_button_01.png");
        add("Stats", 44, menu_selector(ConnectPopup::onStats), 208.f, "GJ_button_02.png");
        add("Web", 36, menu_selector(ConnectPopup::onWeb), 262.f, "GJ_button_02.png");
        add("X", 24, menu_selector(ConnectPopup::onDelete), 308.f, "GJ_button_06.png");
    }
    m_scroll->moveToTop();
    updateStatus(0.f);
}

void ConnectPopup::doConnect(GdpsServer const& s) {
    gc::addServer(s);   // guarda la URL que si funciono
    gc::setActive(s);
    refresh();
    createQuickPopup("GDPS Connect",
        "Conectado a <cy>" + s.name + "</c>.\nReinicia el juego para que todo cargue bien.",
        "Mas tarde", "Reiniciar",
        [](FLAlertLayer*, bool restart) { if (restart) game::restart(); });
}

// Prueba la URL tal cual y, si falla, con "/database" al final.
void ConnectPopup::onConnect(CCObject* sender) {
    size_t i = sender->getTag();
    if (i >= m_servers.size()) return;
    GdpsServer s = m_servers[i];

    std::vector<std::string> candidates = {s.url};
    if (!endsWith(s.url, "/database")) candidates.push_back(s.url + "/database");

    Notification::create("Comprobando " + s.name + "...", NotificationIcon::Loading, 1.5f)->show();
    probe(s, candidates, 0);
}

void ConnectPopup::probe(GdpsServer s, std::vector<std::string> candidates, size_t idx) {
    GdpsServer test = s;
    test.url = candidates[idx];
    std::string gvs = s.legacy19 ? "19" : "22";

    async::spawn(
        gc::post(test, "getGJLevels21.php",
            "gameVersion=" + gvs + "&binaryVersion=45&type=4&str=&diff=-&len=-&page=0&total=0&secret=Wmfd2893gb7"),
        [self = Ref<ConnectPopup>(this), s, candidates, idx](web::WebResponse res) {
            std::string body = res.string().unwrapOr("");
            // respuesta valida de GDPS: HTTP 200 y que no sea una pagina HTML
            bool good = res.ok() && !body.empty() && body[0] != '<';
            if (good) {
                GdpsServer ok = s;
                ok.url = candidates[idx];
                self->doConnect(ok);
                return;
            }
            if (idx + 1 < candidates.size()) {
                self->probe(s, candidates, idx + 1);
                return;
            }
            createQuickPopup("Sin respuesta",
                "No pude hablar con <cy>" + candidates[0] + "</c> (HTTP " + std::to_string(res.code()) +
                ").\nRevisa que la URL sea la carpeta donde esta <cy>getGJLevels21.php</c>.\n<cy>Conectar igualmente?</c>",
                "Cancelar", "Conectar",
                [self, s](FLAlertLayer*, bool yes) { if (yes) self->doConnect(s); });
        });
}

void ConnectPopup::onStats(CCObject* sender) {
    size_t i = sender->getTag();
    if (i < m_servers.size()) StatsPopup::create(m_servers[i])->show();
}

void ConnectPopup::onWeb(CCObject* sender) {
    size_t i = sender->getTag();
    if (i < m_servers.size()) web::openLinkInBrowser(gc::dashboardUrl(m_servers[i]));
}

void ConnectPopup::onDelete(CCObject* sender) {
    size_t i = sender->getTag();
    if (i >= m_servers.size()) return;
    std::string name = m_servers[i].name;
    createQuickPopup("Borrar", "Borrar <cy>" + name + "</c> de la lista?", "No", "Si",
        [this, name](FLAlertLayer*, bool yes) {
            if (!yes) return;
            gc::removeServer(name);
            refresh();
        });
}

void ConnectPopup::onAdd(CCObject*) {
    AddPopup::create([this]() { refresh(); })->show();
}

// Formato del portapapeles: nombre|url|dashboard(opcional)
void ConnectPopup::onImport(CCObject*) {
    auto text = clipboard::read();
    auto parts = gc::split(text, '|');
    if (parts.size() < 2 || parts[0].empty() || parts[1].empty()) {
        FLAlertLayer::create("Importar",
            "Copia un texto con este formato:\n<cy>nombre|url|dashboard</c>\n(el dashboard es opcional)", "OK")->show();
        return;
    }
    GdpsServer s;
    s.name = parts[0];
    s.url = parts[1];
    if (parts.size() > 2) s.dashboard = parts[2];
    gc::addServer(s);
    Notification::create("Importado: " + s.name, NotificationIcon::Success)->show();
    refresh();
}

void ConnectPopup::onOfficial(CCObject*) {
    gc::disconnect();
    Notification::create("Servidores oficiales", NotificationIcon::Success)->show();
    refresh();
}
