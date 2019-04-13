#define PROVIDER_VERSION 1


namespace VpnFeatures {
    const bool ipv6 = false;
    const bool compression = true;
    const bool default_gw = true;

    const char * const protocols[] = {"udp", "tcp"};
    const char * const default_protocol = "udp";

    const char * const name = "VPN GUI";
    const char * const display_name = "A lambda VPN";
    const char * const url = "";
    const char * const nameserver = "";
    const char * const locations_url = "";
    const char * const releases_url = "";

    const char * const openvpn_ca = "-----BEGIN CERTIFICATE-----\n"
"...\n"
"-----END CERTIFICATE-----\n";
};
