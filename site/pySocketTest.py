#!/bin/python3
import asyncio
from websockets.server import serve

async def echo(websocket):
    async for message in websocket:
        await websocket.send(message)

async def main():
    async with serve(echo, "127.0.0.1", 8080):
        await asyncio.Future()

asyncio.run(main())
