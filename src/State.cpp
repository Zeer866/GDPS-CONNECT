#include "State.hpp"
#include "Defaults.hpp"
#include <optional>
#include <algorithm>

using namespace geode::prelude;

static std::optional<GdpsServer> s_active;

static std::string trim(std::string s) {
    auto notSpace = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

static std::string clean(std::string s) {
    for (auto& c : s) if (c == '\t' || c == '\n' || c == '\r') c = ' ';
    return trim(s);
}

std::vector<std::string> gc::split(std::string const& s, char sep) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == sep) { out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}

std::string gc::normalizeUrl(std::string url) {
    url = clean(url);
    if (url.empty()) return url;
    if (url.find("://") == std::string::npos) url = "https://" + url;
    while (!url.empty() && url.back() == '/') url.pop_back();
    return url;
}

std::string gc::dashboardUrl(GdpsServer const& s) {
    if (!s.dashboard.empty()) return s.dashboard;
    auto p = s.url.find("://");
    if (p == std::string::npos) return s.url;
    auto e = s.url.find('/', p + 3);
    auto host = e == std::string::npos ? s.url : s.url.substr(0, e);
    return host + "/dashboard";
}

std::vector<GdpsServer> gc::loadServers() {
    auto mod = Mod::get();
    if (!mod->hasSavedValue("servers")) {
        auto d = gcDefaultServers();
        saveServers(d);
        return d;
    }
    std::vector<GdpsServer> out;
    auto raw = mod->getSavedValue<std::string>("servers", "");
    for (auto& line : split(raw, '\n')) {
        if (line.empty()) continue;
        auto f = split(line, '\t');
        if (f.size() < 2) continue;
        GdpsServer s;
        s.name = f[0];
        s.url = f[1];
        if (f.size() > 2) s.dashboard = f[2];
        if (f.size() > 3) s.legacy19 = f[3] == "1";
        out.push_back(s);
    }
    return out;
}

void gc::saveServers(std::vector<GdpsServer> const& list) {
    std::string raw;
    for (auto& s : list) {
        raw += clean(s.name) + "\t" + clean(s.url) + "\t" + clean(s.dashboard) + "\t" + (s.legacy19 ? "1" : "0") + "\n";
    }
    Mod::get()->setSavedValue<std::string>("servers", raw);
}

void gc::addServer(GdpsServer s) {
    s.name = clean(s.name);
    s.url = normalizeUrl(s.url);
    s.dashboard = clean(s.dashboard);
    auto list = loadServers();
    bool replaced = false;
    for (auto& x : list) {
        if (x.name == s.name) { x = s; replaced = true; }
    }
    if (!replaced) list.push_back(s);
    saveServers(list);
    if (s_active && s_active->name == s.name) s_active = s;
}

void gc::removeServer(std::string const& name) {
    auto list = loadServers();
    list.erase(std::remove_if(list.begin(), list.end(), [&](GdpsServer const& s) { return s.name == name; }), list.end());
    saveServers(list);
    if (s_active && s_active->name == name) disconnect();
}

void gc::restore() {
    auto mod = Mod::get();
    bool firstRun = !mod->hasSavedValue("servers");
    auto list = loadServers(); // siembra los defaults si es la primera vez

    if (firstRun && !list.empty()) {
        setActive(list.front());
        return;
    }
    if (!mod->getSettingValue<bool>("auto-connect")) {
        disconnect();
        return;
    }
    auto name = mod->getSavedValue<std::string>("active-name", "");
    if (name.empty()) return;
    for (auto& s : list) {
        if (s.name == name) { s_active = s; return; }
    }
}

bool gc::isActive() { return s_active.has_value(); }

GdpsServer const& gc::active() {
    static GdpsServer empty;
    return s_active ? *s_active : empty;
}

std::string gc::activeName() { return s_active ? s_active->name : ""; }

void gc::setActive(GdpsServer const& s) {
    s_active = s;
    Mod::get()->setSavedValue<std::string>("active-name", s.name);
}

void gc::disconnect() {
    s_active.reset();
    Mod::get()->setSavedValue<std::string>("active-name", "");
}

bool gc::rewriteUrl(std::string const& in, std::string& out) {
    if (!s_active) return false;
    static const char* prefixes[] = {
        "https://www.boomlings.com/database",
        "http://www.boomlings.com/database",
        "https://boomlings.com/database",
        "http://boomlings.com/database",
    };
    for (auto p : prefixes) {
        std::string pre = p;
        if (in.rfind(pre, 0) == 0) {
            out = s_active->url + in.substr(pre.size());
            return true;
        }
    }
    return false;
}

web::WebTask gc::post(GdpsServer const& s, std::string const& endpoint, std::string const& body) {
    web::WebRequest req;
    req.userAgent("");
    req.header("Content-Type", "application/x-www-form-urlencoded");
    req.bodyString(body);
    req.timeout(std::chrono::seconds(10));
    return req.post(s.url + "/" + endpoint);
}
