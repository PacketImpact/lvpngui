#ifndef CONFIG_H
#define CONFIG_H

#define _STR(x) #x
#define STR(x) _STR(x)


/*
 * PLEASE DO NOT MODIFY THIS FILE
 * if you only want to distribute LVPNGUI for your VPN provider.
 * You can change the displayed name, URL and version in provider.ini.
 */


// LVPNGUI version
#define VPNGUI_VERSION_MAJOR 1
#define VPNGUI_VERSION_MINOR 1
#define VPNGUI_VERSION_PATCH 0


// LVPNGUI copyright and default values
#define VPNGUI_ORGNAME "PacketImpact"
#define VPNGUI_ORGURL "https://packetimpact.net/"

#define VPNGUI_NAME "LVPNGUI"
#define VPNGUI_DISPLAY_NAME "LVPN GUI"
#define VPNGUI_URL "https://packetimpact.net/lvpngui"

#define VPNGUI_EXENAME "lvpngui.exe"


// OpenVPN version to run (must be present in the .qrc)
#define OPENVPN_VERSION "v2.4"


// Version formats
#define VPNGUI_VERSION_RC VPNGUI_VERSION_MAJOR,VPNGUI_VERSION_MINOR,VPNGUI_VERSION_PATCH,VERSION_PROVIDER
#define VPNGUI_VERSION STR(VPNGUI_VERSION_MAJOR) "." STR(VPNGUI_VERSION_MINOR) "." STR(VPNGUI_VERSION_PATCH) "-" STR(VERSION_PROVIDER)


#endif // CONFIG_H
