# Simple Python test server
import socket

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('127.0.0.1', 8000))
server.listen()
print("Server listening on port 8000...")
conn, addr = server.accept()
print(f"Connected by {addr}")