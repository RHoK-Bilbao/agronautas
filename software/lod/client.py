# -*- coding: utf-8 -*-
"""
Created on Thu Nov 22 10:33:44 2012

@author: Aitor
"""

import socket

insert = False
update = False
createDataset = True

HOST, PORT = "localhost", 9999

dataInsert = "INSERT@43.29564@-2.99729@supercocina@2012-11-30@4@arroz"
dataUpdate = "UPDATE@supercocina@134@23.0@70.3@2012-11-30"

if insert:
    # Create a socket (SOCK_STREAM means a TCP socket)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        # Connect to server and send data
        sock.connect((HOST, PORT))
        sock.sendall(dataInsert + "\n")
    
        # Receive data from the server and shut down
        received = sock.recv(2048)
    finally:
        sock.close()
elif update:
    try:
        # Connect to server and send data
        sock.connect((HOST, PORT))
        sock.sendall(dataUpdate + "\n")
    
        # Receive data from the server and shut down
        received = sock.recv(2048)
    finally:
        sock.close()
elif createDataset:
    data = "meh"
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Connect to server and send data
        sock.connect((HOST, PORT))
        sock.sendall("INSERT@43.29564@-2.99729@supercocina@2100000000@4@arroz\n")
    finally:
        sock.close()
    try:
        # Connect to server and send data
        sock.connect((HOST, PORT))
        sock.sendall("INSERT@43.29564@-2.99729@supercocina@3100000000@4@verduras\n")
    finally:
        sock.close()
        
        """
         
        sock.sendall("INSERT@43.29564@-2.99729@micrococina@5100000000@4@verduras\n")
        sock.sendall("UPDATE@supercocina@134@23.0@70.3@3100000000")
        sock.sendall("UPDATE@supercocina@14@13.0@72.3@2100000010")
        sock.sendall("UPDATE@micrococina@134@23.0@70.3@5100300000")
        sock.sendall("UPDATE@micrococina@114@24.0@60.3@5100500000")
    """

# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


print "Sent:     {}".format(data)
print "Received: {}".format(received)
