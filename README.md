# lvpngui

lvpngui is a simple OpenVPN GUI looking very similar to the original OpenVPN GUI.
Its main difference is that it's focused on the ease of installation and use,
and being customized and distributed by the VPN service provider.

- self-contained binary that installs itself and maintain the local
  installation without bothering the user.
- contains all the required OpenVPN configuration
- gets the server list from a JSON API, to provide better load-balancing than
  the default OpenVPN behavior and show a clean list to the user.
- remembers username and passwords for the current user.
  (and tries not to store it in plaintext)

![Log](http://i.imgur.com/pgPz3s7.png)
![Settings](http://i.imgur.com/JwpuEtv.png)

## Building

(Please don't try.)

You need:
- Windows (+ mt.exe from the SDK)
- Qt 5 gui/core/network with OpenSSL
- Crypto++
- OpenVPN binaries and TAP-Windows installer

All statically linked for this to be really worth it,
so you will probably need to build them all too.

OpenVPN binaries can be extracted from the official OpenVPN installers.
(the ones included in openvpn-32/ and openvpn-64/ are.)
If you update them, don't forget to update run hash_files.ps1 to generate
the openvpn-*/index.txt files. They are used to keep track of updates without
hashing and extracting all the resources.

- Copy provider_default/ to provider/ and customize it as you want.
- Create a build/ directory and cd into it.
- Run the usual qmake/make

And you should have a nice large .exe to distribute.
