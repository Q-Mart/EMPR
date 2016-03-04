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

FRAME_WIDTH  = 4*200
FRAME_HEIGHT = 3*200

CANVAS_WIDTH  = 4*150
CANVAS_HEIGHT = 3*150

COL = 'blue'

import reader

try:
    from serial_reader import SerialReader
except ImportError:
    print('Warning: missing module `pyserial`')
    print('... defauling to MockReader for testing')
    SerialReader = reader.MockReader

import time
import threading
import math
import functools
import plotter

try:
    import tkinter
except ImportError:
   import Tkinter as tkinter

from docopt import docopt

import lcd

class PlotCanvas(tkinter.Canvas):
    def __init__(self, parent, width, height):
        tkinter.Canvas.__init__(self, parent)
        self.lines = []
        self.parent = parent

        self.graph = tkinter.Canvas(self, width=width, height=height)
        self.graph.grid(row=0, column=1, rowspan=10, columnspan=10)

        self.x_labels = []
        self.y_labels = []
        for i in range(10):
            var = tkinter.StringVar()
            vary = tkinter.StringVar()
            var.set('!')
            vary.set('&')
            self.x_labels.append(var)
            self.y_labels.append(vary)
            tkinter.Label(self, textvariable=var).grid(row=10, column=i+1)
            tkinter.Label(self, textvariable=vary).grid(row=i, column=0)

    def plot(self, xs, ys):
        '''Plot some graph
        '''
        if len(xs) != len(ys):
            raise ValueError('Mismatch of argc')

        w = self.graph.winfo_width()
        h = self.graph.winfo_height()

        self.draw_border(w, h)

        if len(xs) != 0:
            self.draw_graph(w, h, xs, ys)
        else:
            for i in range(10):
                plot = self.parent.plotter
                if plot.mode & plotter.Plotter.NOLABELS == 0:
                    self.x_labels[i].set(' ')
                    self.y_labels[9-i].set(' ')

    def line(self, *args, **kwargs):
        self.lines.append(self.graph.create_line(*args, **kwargs))

    def draw_graph(self, w, h, xs, ys):
        plot = self.parent.plotter

        max_x = plot.max_x or max(xs)
        max_y = plot.max_y or max(ys)

        tx = float(w) / float(max_x)
        ty = float(h) / float(max_y)

        s = 2

        for i in range(10):
            if plot.mode & plotter.Plotter.NOLABELS != 0:
                self.x_labels[i].set(' ')
                self.y_labels[9-i].set(' ')
            else:
                self.x_labels[i].set(str((i+1)*max_x // 10))
                self.y_labels[9-i].set(str((i+1)*max_y // 10))

        x0, y0 = 0, h
        for xp, yp in zip(xs, ys):
            x, y = int(tx * xp), h - int(ty * yp)
            
            # perform rotation
            if (xp, yp) in plot.rotations:
                t, cx, cy = plot.rotations[(xp, yp)]
                cx, cy = int(tx * cx), h - int(ty * cy)
                cos_t = math.cos(t)
                sin_t = math.sin(t)
                x, y = x - cx, y - cy
                x, y = (x*cos_t) - (y*sin_t), (x*sin_t) + (y*cos_t)
                x, y = x + cx, y + cy

            if plot.mode & plotter.Plotter.POLAR != 0:
                # x is angle between 90-270
                # so flatten it to between 0-180(?)
                # y is euclidian distance
                cos_x = math.cos(x)
                sin_x = math.sin(x)
                x, y = y*cos_x, y*sin_x
                x, y = x + (max_x / 2.0), y

            if plot.mode & plotter.Plotter.NOLINE == 0:
                self.line(x0, y0, x, y)

            self.line(x-s, y-s, x+s, y+s, fill=COL)
            self.line(x+s, y-s, x-s, y+s, fill=COL)
            x0, y0 = x, y

    def draw_border(self, w, h):
        self.line(1, h-1, w, h-1, fill='black', width=3)
        self.line(0, 0, 0, h, fill='black', width=3)

        if self.parent.plotter.mode & plotter.Plotter.POLAR != 0:
            pass

    def re_draw(self, xs, ys):
        self.old_lines = self.lines
        self.lines = []
        self.plot(xs, ys)
        self.graph.delete(*self.old_lines)

    def on_resize(self, event):
        '''TODO: This
        '''
        self.width = self.winfo_width()
        self.height = self.winfo_height()

        scale = (event.width / self.width, event.height / self.height)
        self.width = event.width
        self.height = event.height
        self.config(width=self.width, height=self.height)
        self.scale("all", 0, 0, *scale)

class Mode:
    '''These mode values come from the `state_t` enum
    in the C source
    '''
    CALIBRATE = 0
    SCAN = 1
    MEASURE = 2
    MULTI = 3

    CALIBRATE_NEAR = 4
    CALIBRATE_NEAR_DONE = 5
    CALIBRATE_DONE = 6

    SCAN_PARAMETERS = 7
    SCAN_PARAMETER_1 = 8 # sweep speed
    SCAN_PARAMETER_2 = 9 # start position
    SCAN_PARAMETER_3 = 10 # stop position
    SCAN_PARAMETER_4 = 11 # unused
    SCAN_DO = 12

    MEASURE_DO = 13

    MULTI_PARAMETERS = 14
    MULTI_SWEEP = 15
    MULTI_WAIT = 16
    MULTI_SWEEP_NUMBER = 17
    MULTI_MIN_ANGLE = 18
    MULTI_MAX_ANGLE = 19
    MULTI_DONE = 20
    ANY = 21

def monitor(frame, r):
    def _update():
        while True:
            time.sleep(1.0 / 10.0)
            frame.draw()

    t1 = threading.Thread(target=_update)
    t1.daemon = True
    t1.start()

    t = 0
    while True:
        mode = r.read_byte()

        if mode == Mode.SCAN_DO:
            angle = r.read_int()
            value = r.read_int()
    
            frame.plotter.update(angle, value)
        elif mode == Mode.MEASURE_DO:
            value = r.read_int()
            t += 1
            frame.plotter.update(t, value)
        elif mode == Mode.MULTI_SWEEP:
            angle = r.read_int()
            value = r.read_int()

            frame.plotter.update(plotter.MultiPlotter.SWEEP, (angle, value))
        elif mode == Mode.MULTI_WAIT:
            frame.plotter.update(plotter.MultiPlotter.NEXT, None)
        elif mode == Mode.MULTI_PARAMETERS:
            scan_number = r.read_int()
            min_angle = r.read_int()
            max_angle = r.read_int()
            frame.plotter.update(plotter.MultiPlotter.PARAMS, (scan_number, min_angle, max_angle))
        elif mode == Mode.MEASURE:
            t = 1
            frame.plotter = plotter.MeasurePlotter(*frame.dimensions)
        elif mode == Mode.SCAN:
            frame.plotter = plotter.ScanPlotter(*frame.dimensions)
        elif mode == Mode.MULTI:
            frame.plotter = plotter.MultiPlotter(*frame.dimensions)

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

class AppFrame(tkinter.Frame):
    def __init__(self, parent):
        tkinter.Frame.__init__(self, parent)

        if DEBUG:
            global SerialReader
            SerialReader = functools.partial(reader.UnixReader, self)

        self.init()
        self.plotter = plotter.DefaultPlotter(*self.dimensions)

        self.init_monitors()
        self.draw()

    @property
    def dimensions(self):
        return self.graph_canvas.winfo_width(), self.graph_canvas.winfo_height()

    def init_monitors(self):
        unix_reader = [None]
        def _monitor():
            with SerialReader() as r:
                unix_reader[0] = r
                monitor(self, r)

        t = threading.Thread(target=_monitor)
        t.daemon = True
        t.start()

        if DEBUG:
            t2 = threading.Thread(target=reader.monitor, args=(unix_reader[0],))
            t2.daemon = True
            t2.start()

            t3 = threading.Thread(target=lcd.monitor, args=(self.lcd_canvas,))
            t3.daemon = True
            t3.start()

    def init(self):
        if DEBUG:
            self.lcd_canvas = lcd.LcdCanvas(self, CANVAS_WIDTH, 75)
            self.lcd_canvas.grid(row=3, column=1, rowspan=2)

        self.graph_canvas = PlotCanvas(self, CANVAS_WIDTH, CANVAS_HEIGHT)
        left = tkinter.StringVar()
        bot = tkinter.StringVar()
        left.set('Left!')
        bot.set('Bot!')

        t_left = tkinter.Label(self, textvariable=left)
        t_bot = tkinter.Label(self, textvariable=bot)

        self.axes_labels = (left, bot)
        quit = tkinter.Button(self, text='Quit', command=tk.destroy)

        self.graph_canvas.grid(row=0, column=1)
        t_left.grid(row=0)
        t_bot.grid(row=1, columnspan=2)
        quit.grid(row=2, columnspan=2)
        self.pack() 

    def draw(self):
        xs, ys = self.plotter.x, self.plotter.y
        left, bot = self.axes_labels

        self.graph_canvas.re_draw(xs, ys)

        left.set(self.plotter.label_y)
        bot.set(self.plotter.label_x)

        if DEBUG:
            self.lcd_canvas.draw()

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

        tk = tkinter.Tk()
        tk.minsize(width=FRAME_WIDTH, height=FRAME_HEIGHT)
        tk.maxsize(width=FRAME_WIDTH, height=FRAME_HEIGHT)
        tk.resizable(width=tkinter.FALSE, height=tkinter.FALSE)

        app = AppFrame(parent=tk)

        if DEBUG:
            print('Running in DEBUG mode, reading from UNIX Socket')
            SerialReader = functools.partial(reader.UnixReader, app)

        app.mainloop()
