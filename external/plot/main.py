DEBUG = True

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
import tkinter

class PlotCanvas(tkinter.Canvas):
    def __init__(self, parent, width, height):
        super().__init__(parent, width=width, height=height)
        self.width = width
        self.height = height
        self.plotter = None
        self.lines = []

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
        tx = w / max(xs)
        ty = h / max(ys)

        s = 2
        x0, y0 = 0, 0
        for x, y in zip(xs, ys):
            x, y = int(tx * x), h - int(ty * y)
            self.line(x0, y0, x, y)

            self.line(x-s, y-s, x+s, y+s, fill='red')
            self.line(x+s, y-s, x-s, y+s, fill='red')
            x0, y0 = x, y

    def draw_border(self, w, h):
        self.line(1, h, w, h, fill='black')
        self.line(1, 1, 1, h, fill='black')

    def clear(self):
        self.delete(*self.lines)
        self.lines = []

class Mode:
    SCAN_DO = 1

def monitor(frame):
    with SerialReader() as r:
        while True:
            mode = r.read_byte()

            if mode == Mode.SCAN_DO:
                angle = r.read_int()
                value = r.read_int()

                frame.plotter.update(angle, value)
                frame.draw()

class AppFrame(tkinter.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.plotter = plotter.DefaultPlotter()

        self.pack()
        self.init()
        
        self.monitor_t = threading.Thread(target=monitor, args=(self,), daemon=True)
        self.monitor_t.start()

    def init(self):
        self.graph_canvas = PlotCanvas(self, 400, 600)
        self.graph_canvas.pack(side='top')

        quit = tkinter.Button(text='Quit', command=tk.destroy)
        quit.pack(side='bottom')

        self.draw()

    def draw(self):
        x, y = self.plotter.x, self.plotter.y
        self.graph_canvas.clear()
        self.graph_canvas.plot(x, y)

if __name__ == '__main__':
    tk = tkinter.Tk()
    app = AppFrame(parent=tk)
    app.mainloop()
