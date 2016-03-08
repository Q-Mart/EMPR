#!/usr/bin/env python
'''Main program

Usage:
    main.py [-hbd]
    main.py --record

Options:
    -d --debug      Debug Mode
    -h --help       Show this text
    -r --record     Record into records
    -b              with Ben's Personal Project
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
import struct

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
            var.set(' ')
            vary.set(' ')
            self.x_labels.append(var)
            self.y_labels.append(vary)
            tkinter.Label(self, textvariable=var).grid(row=10, column=i+1)
            tkinter.Label(self, textvariable=vary).grid(row=i, column=0)

    def plot(self, points):
        '''Plot some graph
        '''
        w = self.graph.winfo_width()
        h = self.graph.winfo_height()

        self.draw_border(w, h)

        if len(points) != 0:
            self.draw_graph(w, h, points)
        else:
            for i in range(10):
                plot = self.parent.plotter
                if plot.mode & plotter.Plotter.NOXLABELS != 0:
                    self.x_labels[i].set(' ')

                if plot.mode & plotter.Plotter.NOYLABELS != 0:
                    self.y_labels[9-i].set(' ')

    def line(self, *args, **kwargs):
        self.lines.append(self.graph.create_line(*args, **kwargs))

    def draw_graph(self, w, h, points):
        plot = self.parent.plotter

        max_x = plot.max_x or max(map(lambda p: p.x, points))
        max_y = plot.max_y or max(map(lambda p: p.y, points))

        tx = float(w) / float(max_x)
        ty = float(h) / float(max_y)

        s = 2

        for i in range(10):
            if plot.mode & plotter.Plotter.NOXLABELS != 0:
                self.x_labels[i].set(' ')
            else:
                self.x_labels[i].set(str((i+1)*max_x // 10))

            if plot.mode & plotter.Plotter.NOYLABELS != 0:
                self.y_labels[9-i].set(' ')
            else:
                self.y_labels[9-i].set(str((i+1)*max_y // 10))

        x0, y0 = None, None
        cp = points
        if type(cp) is not list:
            cp = cp.__copy__()

        for p in cp:
            x, y = tx * p.x, h - ty * p.y
            _x, _y = x, y

            # ignore out-of-bounds points
            if x > w or y > h or x < 0 or y < 0:
                x0, y0 = None, None
                continue
            
            # need to wrap around (w / 2, h / 2)
            if plot.mode & plotter.Plotter.POLAR != 0:
                # y is euclidian distance
                t = (math.pi / 180.0) * (270 - p.x)
                y = (h - y) / 2.0
                x = y*math.cos(t) + w / 2.0
                y = (h / 2.0) - y*math.sin(t)

            # perform rotation
            if p.rot_theta:
                t, cx, cy = p.rot_theta, p.rot_x, p.rot_y
                cx, cy = int(tx * cx), h - int(ty * cy)
                cos_t = math.cos(t)
                sin_t = math.sin(t)
                x, y = x - cx, y - cy
                x, y = (x*cos_t) - (y*sin_t), (x*sin_t) + (y*cos_t)
                x, y = x + cx, y + cy

            if plot.mode & plotter.Plotter.NOLINE == 0 and x0:
                self.line(x0, y0, x, y, fill=p.col)

            self.line(x-s, y-s, x+s, y+s, fill=p.col)
            self.line(x+s, y-s, x-s, y+s, fill=p.col)
            x0, y0 = x, y

        #  
        if points and plot.mode & plotter.Plotter.POLAR != 0:
            t = (math.pi / 180.0) * (270 - p.x)
            x = (h / 2.0)*math.cos(t) + w / 2.0
            y = (h / 2.0) - (h / 2.0)*math.sin(t)
            self.line(w / 2.0, h / 2.0, x, y, fill='green', width=2)


    def draw_border(self, w, h):
        if self.parent.plotter.mode & plotter.Plotter.POLAR != 0:
            # centre at w/2, h
            x0, y0 = None, None
            xf, yf = None, None
            N = 30
            for i in range(1, N):
                t = (i+1)*(2.0*math.pi / float(N))
                x = (h / 2.0)*math.cos(t) + w / 2.0
                y = (h / 2.0) - (h / 2.0) * math.sin(t)
                self.line(w / 2, h / 2, x, y, fill='grey', width=1)
                if x0:
                    self.line(x0, y0, x, y, fill='grey', width=1)
                else:
                    xf, yf = x, y

                x0, y0 = x, y

            self.line(x0, y0, xf, yf, fill='grey', width=1)
            self.line(1, h-1, w, h-1, fill='grey', width=2)
            self.line(w/2, 0, w/2, h, fill='grey', width=2)
            self.line(0, 0, 0, h, fill='grey', width=2)
        else:
            self.line(1, h-1, w, h-1, fill='grey', width=2)
            self.line(0, 0, 0, h, fill='grey', width=2)

    def re_draw(self, points):
        self.old_lines = self.lines
        self.lines = []
        self.plot(points)
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

    PLATFORM = 21
    PLATFORM_SCAN = 22
    PLATFORM_WAIT = 23
    PLATFORM_DONE = 24

    ANY = 25

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
            if BPP:
                frame.plotter = plotter.ScanPlotter2(*frame.dimensions)
            else:
                frame.plotter = plotter.ScanPlotter(*frame.dimensions)
        elif mode == Mode.MULTI:
            frame.plotter = plotter.MultiPlotter(*frame.dimensions)
        elif mode == Mode.PLATFORM_SCAN:
           angle = r.read_int()
           value = r.read_int()

           frame.plotter.update(plotter.PlatformPlotter.SCAN, (angle, value))
        elif mode == Mode.PLATFORM:
            frame.plotter = plotter.PlatformPlotter(*frame.dimensions)

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
            print('ir:', val, struct.unpack('<I',val)[0])
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
        plot = self.plotter
        left, bot = self.axes_labels

        points = plot.points

        # for backwards-compatability
        if len(plot.points) == 0:
            xs, ys = self.plotter.x, self.plotter.y
            points = map(lambda x, y: plotter.Point(x, y, COL, 0, 0, 0), xs, ys)

        self.graph_canvas.re_draw(points)

        left.set(self.plotter.label_y)
        bot.set(self.plotter.label_x)

        if DEBUG:
            self.lcd_canvas.draw()

if __name__ == '__main__':
    global DEBUG, BPP
    args = docopt(__doc__)

    BPP = False
    if args['-b']:
        BPP = True

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
