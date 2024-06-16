#!/bin/python3
#import hashlib, base64
import asyncio
from websockets.server import serve

async def echo(websocket):
    async for message in websocket:
        #websocket.send("test")
        await websocket.send("hello")
        #await websocket.send("{Address:18:26:54:55:80:09,Name:Galaxy Buds2 Pro,Path:/org/bluez/hci0/dev_18_26_54_55_80_09,UUID:[00001101-0000-1000-8000-00805f9b34fb,0000110b-0000-1000-8000-00805f9b34fb,0000110c-0000-1000-8000-00805f9b34fb,0000110e-0000-1000-8000-00805f9b34fb,0000111e-0000-1000-8000-00805f9b34fb,00001200-0000-1000-8000-00805f9b34fb,2e73a4ad-332d-41fc-90e2-16bef06523f2,a23d00bc-217c-123b-9c00-fc44577136ee,b4a9d6a0-b2e3-4e40-976d-a69f167ea895,d908aab5-7a90-4cbe-8641-86a553db0146,e7ab2241-ca64-4a69-ac02-05f5c6fe2d62,f8620674-a1ed-41ab-a8b9-de9ad655729d,],}")
        await websocket.send(r'{"Address":"18:26:54:55:80:09","Name":"Galaxy Buds2 Pro","Path":"/org/bluez/hci0/dev_18_26_54_55_80_09","UUID":["00001101-0000-1000-8000-00805f9b34fb","0000110b-0000-1000-8000-00805f9b34fb","0000110c-0000-1000-8000-00805f9b34fb","0000110e-0000-1000-8000-00805f9b34fb","0000111e-0000-1000-8000-00805f9b34fb","00001200-0000-1000-8000-00805f9b34fb","2e73a4ad-332d-41fc-90e2-16bef06523f2","a23d00bc-217c-123b-9c00-fc44577136ee","b4a9d6a0-b2e3-4e40-976d-a69f167ea895","d908aab5-7a90-4cbe-8641-86a553db0146","e7ab2241-ca64-4a69-ac02-05f5c6fe2d62","f8620674-a1ed-41ab-a8b9-de9ad655729d",],}')

async def main():
    async with serve(echo, "127.0.0.1", 8080, compression=None):
        await asyncio.Future()

asyncio.run(main())
#h = hashlib.sha1(b"Xz5J2TjH9ilnOEg2nKDW4g==258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
#h = hashlib.sha1(b"mKPB16b7FjtoEz/tVSGuUQ==")

#print("digest:", h.digest())
#print()
#print("right result:", base64.b64encode(h.digest()).decode())
