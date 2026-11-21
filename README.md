# GDPS Connect

Mod de Geode (Windows + Android) para conectarte a un GDPS sin reinstalar el juego.

## Funciones
- Botón con el logo en el menú principal que abre la lista de servidores
- **Ir** (conectar), **Stats**, **Web** (dashboard) y **X** (borrar) en cada servidor
- Comprueba que el servidor responda antes de cambiar
- Estadísticas: estado, ping, total de niveles y top 5 de jugadores
- Reconexión automática al último servidor (se puede desactivar en los ajustes)
- Importar servidores desde el portapapeles con el formato `nombre|url|dashboard`
- Modo compatibilidad 1.9 por servidor
- Botón **Oficial** para volver a los servidores de RobTop

## Configurar tu GDPS
Edita `src/Defaults.hpp` y pon la URL de la carpeta donde están los `.php`
(donde está `getGJLevels21.php`), sin `/` al final.

## Compilar con GitHub Actions
1. Sube todos los archivos a un repositorio de GitHub.
2. Ve a **Actions → Build Mod → Run workflow**.
3. Cuando termine, descarga el `.geode` desde **Artifacts**.
4. Copia el `.geode` a la carpeta `geode/mods` del juego.

## Estructura
- `mod.json`: datos y ajustes del mod
- `CMakeLists.txt`: configuración de compilación
- `src/`: código del mod
- `resources/gc-logo.png`: icono del botón del menú
- `logo.png`: logo del mod (336x336)
- `.github/workflows/build.yml`: compilación automática

## Créditos
Hecho por Zeer866.
