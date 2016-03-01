#!/usr/bin/env python
import struct
from socket import socket, SOCK_DGRAM, AF_UNIX
from threading import *
from contextlib import closing
import time

def network():
    _SOCK_ADDR = '/tmp/empr_ipc_socket_network'
    # MEASURE 

    MODE = lambda x, y: struct.pack('B', x) + struct.pack('I', y)
    MEASURE = 2
    MEASURE_DO = 13
    _OUT = struct.pack('B', MEASURE)

    for i in range(25):
        _OUT += MODE(MEASURE_DO, i*int(5e4))

    with closing(socket(AF_UNIX, SOCK_DGRAM)) as sock:
        try:
            sock.connect(_SOCK_ADDR)
            while True:
                for x in _OUT:
                    sock.send(x)
                    time.sleep(0.1)
        except:
            pass

def lcd():
    _SOCK_ADDR = '/tmp/empr_ipc_socket_lcd'
    # MEASURE 

    MODE = lambda x, y: struct.pack('B', x) + struct.pack(str(len(y)) + 's', y)

    _OUT = b''
    for i in range(25):
        _OUT += MODE(1, "Line 1\0") + MODE(2, "Line 2\0")

    with closing(socket(AF_UNIX, SOCK_DGRAM)) as sock:
        sock.connect(_SOCK_ADDR)
        while True:
            for x in _OUT:
                sock.send(x)
                time.sleep(0.1)

if __name__ == '__main__':
    t1 = Thread(target=network)
    t1.daemon = True
    t1.start()

    lcd()
