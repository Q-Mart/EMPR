import struct

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
        i = self.read(4)
        return struct.unpack('i', i)[0]

    def read_byte(self):
        b = self.read(1)
        return struct.unpack('b', b)[0]

    def close(self):
        raise NotImplementedError
    
BYTE_STR = b'\x01\x01\x02\x03\x04\x01\x02\x03\x04'
class MockReader(Reader):
    def __init__(self, data=None):
        self._data = data or BYTE_STR
        self._data_iter = iter(self._data)

    def read(self, n):
        try:
            return bytes([next(self._data_iter)] * n)
        except StopIteration:
            self._data_iter = iter(self._data)
            return self.read(n)

    def close(self):
        pass

    def __iter__(self):
        return iter(self._data)
