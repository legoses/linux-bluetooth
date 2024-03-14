#!/bin/bash
dbus-send --system --type=signal / org.freedesktop.DBus.ObjectManager.InterfacesRemoved objpath:/org/bluez array:string:"test","test","test"
