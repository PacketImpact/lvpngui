#ifndef CONFIG_H
#define CONFIG_H

#define xX_STR_Xx(x) #x
#define STR(x) xX_STR_Xx(x)


/*
 * PLEASE DO NOT MODIFY THIS FILE
 * Features and branding can be changed in provider/provider.h
 */


// LVPNGUI version
#define VPNGUI_VERSION_MAJOR 1
#define VPNGUI_VERSION_MINOR 1
#define VPNGUI_VERSION_PATCH 0
#define VPNGUI_VERSION_BUILD "-rc1"


// LVPNGUI copyright and default values
#define VPNGUI_ORGNAME "PacketImpact"
#define VPNGUI_ORGURL "https://packetimpact.net/"

#define VPNGUI_NAME "LVPNGUI"
#define VPNGUI_DISPLAY_NAME "LVPN GUI"
#define VPNGUI_URL "https://packetimpact.net/lvpngui"

#define VPNGUI_EXENAME "lvpngui.exe"


// OpenVPN version to run (must be present in the .qrc)
#define OPENVPN_VERSION "v2.4"


#include "provider/provider.h"


// Version formats
#define VPNGUI_VERSION_RC VPNGUI_VERSION_MAJOR,VPNGUI_VERSION_MINOR,VPNGUI_VERSION_PATCH,PROVIDER_VERSION
#define VPNGUI_VERSION STR(VPNGUI_VERSION_MAJOR) "." STR(VPNGUI_VERSION_MINOR) "." STR(VPNGUI_VERSION_PATCH) VPNGUI_VERSION_BUILD "-" STR(PROVIDER_VERSION)


#endif // CONFIG_H
