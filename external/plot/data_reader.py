from reader import Reader
import serial

class DataReader(Reader):
    def __init__(self):
        '''Initialise a new :class:`DataReader` which reads incoming data
        on the serial USB line from the mbed.
        '''

        self.device = serial.Serial('/dev/ttyACM0')

    def read(self, n):
        return self.device.read(n)

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

    def close(self):
        self.device.close()
