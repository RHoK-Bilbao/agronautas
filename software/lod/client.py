# -*- coding: utf-8 -*-
"""
Created on Thu Nov 22 10:33:44 2012

@author: Aitor
"""

import socket

insert = True

HOST, PORT = "localhost", 9999
dataInsert = "INSERT@43.29564@-2.99729@supercocina@2012-11-30"
dataUpdate = "UPDATE@supercocina@134@23.0@70.3@2012-11-30"
# data = "INSERT@43.29564@-2.99729@nameBlah@lumiBlah@exterBalh@interblah"

if insert:
    data = dataInsert
else:
    data = dataUpdate        

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