# TCP echo server

import sys
import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('localhost', 12345)
sock.bind(server_address)
sock.listen(1)

while True:
    connection, client_address = sock.accept()
    try:
        print("connection from", client_address)
        while True:
            data = connection.recv(16)
            print("received ", data)
            if data:
                print("sending data back to the client")
                connection.sendall(data)
            else:
                print("no more data from", client_address)
                break

    finally:
        connection.close()
