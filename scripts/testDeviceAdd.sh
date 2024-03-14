#!/bin/bash

dbus-send --system --type=signal / org.freedesktop.DBus.ObjectManager.InterfacesRemoved objpath:/org/bluez dict:string:dict:string:variant:"test",dict:string:variant:string:"test","about"
