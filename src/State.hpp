#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <string>
#include <vector>

struct GdpsServer {
    std::string name;
    std::string url;        // carpeta donde estan los .php, sin "/" final
    std::string dashboard;  // opcional
    bool legacy19 = false;  // modo compatibilidad 1.9
};

namespace gc {
    std::vector<GdpsServer> loadServers();
    void saveServers(std::vector<GdpsServer> const& list);
    void addServer(GdpsServer s);
    void removeServer(std::string const& name);

    std::string normalizeUrl(std::string url);
    std::string dashboardUrl(GdpsServer const& s);
    std::vector<std::string> split(std::string const& s, char sep);

    void restore();                       // al abrir el juego
    bool isActive();
    GdpsServer const& active();
    std::string activeName();
    void setActive(GdpsServer const& s);
    void disconnect();

    // reescribe URLs oficiales -> servidor activo
    bool rewriteUrl(std::string const& in, std::string& out);

    geode::utils::web::WebTask post(GdpsServer const& s, std::string const& endpoint, std::string const& body);
}
