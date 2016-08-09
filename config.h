#ifndef CONFIG_H
#define CONFIG_H

#define _STR(x) #x
#define STR(x) _STR(x)

#define VPNGUI_ORGNAME "PacketImpact"
#define VPNGUI_ORGURL "https://packetimpact.net/"
#define VPNGUI_EXENAME "lvpngui.exe"

#define VPNGUI_VERSION_MAJOR 1
#define VPNGUI_VERSION_MINOR 0
#define VPNGUI_VERSION_PATCH 0

// Default values. Replaced by branding.ini values
#define VPNGUI_NAME "LVPNGUI"
#define VPNGUI_DISPLAY_NAME "LVPN GUI"
#define VPNGUI_URL "https://github.com/PacketImpact/lvpngui/"

// Version formats
#define VPNGUI_VERSION_RC VPNGUI_VERSION_MAJOR,VPNGUI_VERSION_MINOR,VPNGUI_VERSION_PATCH,0
#define VPNGUI_VERSION STR(VPNGUI_VERSION_MAJOR) "." STR(VPNGUI_VERSION_MINOR) "." STR(VPNGUI_VERSION_PATCH)

#endif // CONFIG_H
