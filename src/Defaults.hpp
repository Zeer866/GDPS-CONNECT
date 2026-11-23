#pragma once
#include "State.hpp"

// Servidores que vienen precargados la primera vez que se abre el mod.
// EDITA la URL: debe ser la carpeta donde estan tus .php (getGJLevels21.php, etc.), sin "/" al final.
inline std::vector<GdpsServer> gcDefaultServers() {
    return {
        {"ZeerGDPS", "https://zeergdps.ps.fhgdps.com/", "https://zeergdps.ps.fhgdps.com/dashboard/", 22},
    };
}
