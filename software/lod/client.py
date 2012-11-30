# -*- coding: utf-8 -*-
"""
Created on Thu Nov 22 10:33:44 2012

@author: Aitor
"""

import socket

HOST, PORT = "localhost", 9999
data = "INSERT@43.29564@-2.99729@nameBlah"
# data = "INSERT@43.29564@-2.99729@nameBlah@lumiBlah@exterBalh@interblah"

# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall(data + "\n")

    # Receive data from the server and shut down
    received = sock.recv(2048)
finally:
    sock.close()

print "Sent:     {}".format(data)
print "Received: {}".format(received)