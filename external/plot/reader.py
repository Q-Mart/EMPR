import struct
import os
import collections
import time

from socket import AF_UNIX, SOCK_DGRAM, socket
from contextlib import closing

SOCK_ADDR = '/tmp/empr_ipc_socket_network'

class Reader:
    def __iter__(self):
        '''Make this iterable
        '''
        return self

    def __next__(self):
        return self.read(1)

    def __enter__(self):
        '''Make a context manager
        '''
        return self

    def __exit__(self, *args):
        '''Cleanup on exiting of `with` statement
        '''
        self.close()

    def read(self, n):
        '''Read 'n' bytes from the stream
        Returning an `int` of what was read
        '''
        raise NotImplementedError

    def read_int(self):
        ''' Read a 32bit Integer
        '''
        i = self.read(4)
        return struct.unpack('I', i)[0]

    def read_byte(self):
        b = self.read(1)
        return struct.unpack('B', b)[0]

    def close(self):
        raise NotImplementedError

# b-mode i-angle i-value 
SCAN_DO = 10
MODE = lambda x, y, z: struct.pack('B', x) + struct.pack('I', y) + struct.pack('I', z)
BYTE_STR = MODE(SCAN_DO, 5, 10) + MODE(SCAN_DO, 10, 15) + MODE(SCAN_DO, 15, 20)

class MockReader(Reader):
    def __init__(self, data=None):
        self._data = data or BYTE_STR
        self._data_iter = iter(self._data)

    def read(self, n):
        l = []
        for i in range(n):
            try:
                b = next(self._data_iter)
            except StopIteration:
                self._data_iter = iter(self._data)
                b = next(self._data_iter)
            l.append(b)

        return bytearray(l)

    def close(self):
        pass

    def __iter__(self):
        return iter(self._data)

def monitor(unix_reader):
    '''Monitors a UNIX Socket
    for IPC with debug build
    '''

    if os.path.exists(SOCK_ADDR):
        os.remove(SOCK_ADDR)

    processor = process(unix_reader)
    next(processor)

    with closing(socket(AF_UNIX, SOCK_DGRAM)) as sock:
        sock.bind(SOCK_ADDR)

        while True:
            v = sock.recv(1024)

            if not v:
                break

            processor.send(v)

    os.remove(SOCK_ADDR)

def process(reader):
    while True:
        v = yield
        reader.Q.extend(v)
        time.sleep(0.01)

class UnixReader(Reader):
    def __init__(self, app):
        self.app = app
        self.Q = collections.deque()

    def read(self, n):
        i = n
        L = []

        while i > 0:
            while not self.Q:
                pass

            L.append(self.Q.popleft())
            i -= 1
        
        return bytearray(L)

    def close(self):
        pass

    def __iter__(self):
        return self
