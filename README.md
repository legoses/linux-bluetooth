# Linux Bluetooth
## Builing dbux-cxx
### Arch
- By default, libraries are installed to /usr/local/lib. Move these to /usr/lib
- Move dbus-cxx-2.0.pc from /usr/local/lib/pkgconfig to /usr/lib/pkgconfig
- Open dbus-cxx-2.0.pc and edit the prefix to point to the dbus-cxx/ directory if it does not already
