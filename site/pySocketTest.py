import socket
import re
import hashlib
import base64


def get_num(data):
    print("parsing frame")
    getWebSocket = re.compile("Sec-WebSocket-Key: (.*)")
    #getWebSocket = re.compile("^Get")

    key = getWebSocket.search(data)
    entireKey = key.group(1) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

    digest = hashlib.sha1(key.group(1).encode())

    base = base64.b64encode(bytes(str(digest), 'utf-8'))
    print(f"base str {base}")


def main():
    getFrame = re.compile("^GET")

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        s.bind(("127.0.0.1", 8080))
        s.listen()
        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")

            while True:
                data = conn.recv(1024)
                dataStr = data.decode('utf-8')
                if not data:
                    break

                print(dataStr)
                print("checkign frame")
                if(getFrame.search(dataStr) != None):
                    get_num(dataStr)



if __name__ == "__main__":
    main()
