#!/bin/python3
#import hashlib, base64
import asyncio
from websockets.server import serve

async def echo(websocket):
    async for message in websocket:
        websocket.send("test")
        await websocket.send(message)
        websocket.send("test")

async def main():
    async with serve(echo, "127.0.0.1", 8080):
        await asyncio.Future()

asyncio.run(main())
#h = hashlib.sha1(b"Xz5J2TjH9ilnOEg2nKDW4g==258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
#h = hashlib.sha1(b"mKPB16b7FjtoEz/tVSGuUQ==")

#print("digest:", h.digest())
#print()
#print("right result:", base64.b64encode(h.digest()).decode())
