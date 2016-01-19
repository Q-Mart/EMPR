class DataReader:
    def __init__(self):
        '''Initialise a new :class:`DataReader` which reads incoming data
        on the serial USB line from the mbed.
        '''

        self.device = serial.Serial('/dev/ttyACM0')

    def __next__(self):
        '''Make this an iterator
        '''
        if self.device:
            return self.device.read(1)

        raise StopIteration

    def __iter__(self):
        '''Make this iterable
        '''
        return self

    def __enter__(self):
        '''Make a context manager
        '''
        return self

    def __exit__(self):
        '''Cleanup on exiting of `with` statement
        '''
        self.close()

    def close():
        self.device.close()

BYTE_STR = b'1234'
class MockReader(DataReader):
    def __init__(self, data=None):
        self._data = data or BYTE_STR
        self._data_iter = iter(self._data)

    def __next__(self):
        return next(self._data_iter)

    def __iter__(self):
        return iter(self._data)
