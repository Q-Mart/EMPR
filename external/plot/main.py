import serial

class DataReader:
    def __init__(self);
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

    def close():
        self.device.close()

if __name__ == '__main__':
    print('Data!')
    reader = DataReader()

    for b in reader:
        print(b)

    reader.close()
