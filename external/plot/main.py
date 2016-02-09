DEBUG = False

import reader

try:
    from serial_reader import SerialReader
except ImportError:
    print('Warning: missing module `pyserial`')
    print('... defauling to MockReader for testing')
    SerialReader = reader.MockReader

if DEBUG:
    print('Running in DEBUG mode, defauling to MockReader for testing')
    SerialReader = reader.MockReader

import time
import threading
import plotter

try:
    import tkinter
except ImportError:
    import Tkinter as tkinter

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

    CALIBRATE_NEAR = 4
    CALIBRATE_NEAR_DONE = 5
    CALIBRATE_DONE = 6

    SCAN_PARAMETERS = 7
    SCAN_PARAMETER_1 = 8 
    SCAN_PARAMETER_2 = 9
    SCAN_PARAMETER_3 = 10
    SCAN_PARAMETER_4 = 11
    SCAN_DO = 12

    MEASURE_DO = 13

    MULTI_PARAMETERS = 14
    MULTI_DO = 15
    MULTI_DO_STAGE_1 = 16
    MULTI_DO_STAGE_2 = 17
    MULTI_DO_STAGE_3 = 18
    MULTI_DO_STAGE_4 = 19

    MULTI_DONE = 20
    ANY = 21

def monitor(frame):
    with SerialReader() as r:
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
    tk = tkinter.Tk()
    app = AppFrame(parent=tk)
    app.mainloop()
