#!/usr/bin/env python
'''Main program

Usage:
    main
    main -d | --debug
    main -r | --record
    main -h | --help

Options:
    -d --debug  Debug Mode
    -h --help   Show this text
'''

BASE_DIR = '../../'
RECORD_FILE = BASE_DIR + 'records/record'

import reader

try:
    from serial_reader import SerialReader
except ImportError:
    print('Warning: missing module `pyserial`')
    print('... defauling to MockReader for testing')
    SerialReader = reader.MockReader

import time
import threading
import plotter

try:
    import tkinter
except ImportError:
   import Tkinter as tkinter

from docopt import docopt

class PlotCanvas(tkinter.Canvas):
    def __init__(self, parent, width, height):
        tkinter.Canvas.__init__(self, parent, width=width, height=height)
        self.width = width
        self.height = height
        self.lines = []
        self.parent = parent

    def plot(self, xs, ys):
        '''Plot some graph
        '''
        if len(xs) != len(ys):
            raise ValueError('Mismatch of argc')

        w = self.width
        h = self.height

        self.draw_border(w, h)

        if len(xs) != 0:
            self.draw_graph(w, h, xs, ys)

    def line(self, *args, **kwargs):
        self.lines.append(self.create_line(*args, **kwargs))

    def draw_graph(self, w, h, xs, ys):
        max_x = self.parent.plotter.max_x or max(xs)
        max_y = self.parent.plotter.max_y or max(ys)

        tx = w / float(max_x)
        ty = h / float(max_y)

        x0, y0 = 0, 0
        for x, y in zip(xs, ys):
            x, y = int(tx * x), h - int(ty * y)
            self.line(x0, y0, x, y)

            #self.line(x-s, y-s, x+s, y+s, fill='red')
            #self.line(x+s, y-s, x-s, y+s, fill='red')
            x0, y0 = x, y

    def draw_border(self, w, h):
        self.line(1, h, w, h, fill='black')
        self.line(1, 1, 1, h, fill='black')

    def clear(self):
        self.delete(*self.lines)
        self.lines = []

class Mode:
    '''These mode values come from the `state_t` enum
    in the C source
    '''
    CALIBRATE = 0
    SCAN = 1
    MEASURE = 2
    MULTI = 3
    CALIBRATE_NEAR_DONE = 4
    CALIBRATE_DONE = 5
    SCAN_PARAMETERS = 6
    SCAN_PARAMATER_1 = 7
    SCAN_PARAMATER_2 = 8
    SCAN_PARAMATER_3 = 9
    SCAN_DO = 10
    MEASURE_PARAMETERS = 11
    MEASURE_PARAMATER_1 = 12
    MEASURE_PARAMATER_2 = 13
    MEASURE_PARAMATER_3 = 14
    MEASURE_DO = 15
    MULTI_PARAMETERS = 16
    MULTI_DO_STAGE_1 = 17
    MULTI_DO_STAGE_2 = 18
    MULTI_DO_STAGE_3 = 19
    MULTI_DO_STAGE_4 = 20
    MULTI_DONE = 21
    ANY = 22

def monitor(frame):
    with SerialReader() as r:
        read_live(frame, r)

def append_record(b):
    with open(RECORD_FILE, 'ab') as f:
        f.write(b)

def read_record(r):
    while True:
        header = r.read_byte()

        if header == 0x03:
            vals = [r.read_byte() for _ in range(16)]
            print('keypad:', vals)
            append_record(bytearray([header]))
            append_record(bytearray(map(lambda x: 0 if x < 0 else x, vals)))
        elif header == 0x04:
            val = r.read(4)  
            print('ir:', val)
            append_record(bytearray([header]))
            append_record(bytearray(val))
        elif header == 0x06:
            val = r.read(4)
            print('ultra:', val)
            append_record(bytearray([header]))
            append_record(bytearray(val))
        # sinks for others
        elif header == 0x01:
            r.read(4)
        elif header == 0x05:
            val = r.read(2) # servo sends a uint16_t
            print('servo:')
            append_record(bytearray([header]))
            append_record(bytearray(val))

def read_live(frame, r):
    t = 1
    while True:
        mode = r.read_byte()

        if mode == Mode.SCAN_DO:
            angle = r.read_int()
            value = r.read_int()
    
            frame.plotter.update(angle, value)
            frame.draw()
        elif mode == Mode.MEASURE_DO:
            value = r.read_int()
            t += 1
            frame.plotter.update(t, value)
            frame.draw()
        elif mode == Mode.MEASURE:
            frame.plotter = plotter.MeasurePlotter()
        elif mode == Mode.SCAN:
            frame.plotter = plotter.ScanPlotter()
        elif mode == Mode.MULTI:
            pass

class AppFrame(tkinter.Frame):
    def __init__(self, parent):
        tkinter.Frame.__init__(self, parent)
        self.plotter = plotter.DefaultPlotter()

        self.pack()
        self.init()

        self.monitor_t = threading.Thread(target=monitor, args=(self,))
        self.monitor_t.daemon = True
        self.monitor_t.start()

    def init(self):
        self.graph_canvas = PlotCanvas(self, 400, 600)
        self.graph_canvas.pack(side='top')

        quit = tkinter.Button(text='Quit', command=tk.destroy)
        quit.pack(side='bottom')

        self.draw()

    def draw(self):
        xs, ys = self.plotter.x, self.plotter.y

        self.graph_canvas.clear()
        self.graph_canvas.plot(xs, ys)

if __name__ == '__main__':
    global DEBUG
    args = docopt(__doc__)

    if args['--record']:
        with open(RECORD_FILE, 'wb') as f:
            f.write('')

        with SerialReader() as r:
            read_record(r)
    else:
        DEBUG = False
        if args['--debug']:
            DEBUG = True

        if DEBUG:
            print('Running in DEBUG mode, defauling to MockReader for testing')
            SerialReader = reader.MockReader


        tk = tkinter.Tk()
        app = AppFrame(parent=tk)
        app.mainloop()
