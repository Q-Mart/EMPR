import reader
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

    def draw_graph(self, w, h, xs, ys):
        tx = w / max(xs)
        ty = h / max(ys)

        s = 2
        x0, y0 = 0, 0
        for x, y in zip(xs, ys):
            x, y = int(tx * x), h - int(ty * y)
            #self.create_line(x0, y0, x, y)

            self.create_line(x-s, y-s, x+s, y+s, fill='red')
            self.create_line(x+s, y-s, x-s, y+s, fill='red')
            x0, y0 = x, y
        


    def draw_border(self, w, h):
        self.create_line(1, h, w, h, fill='black')
        self.create_line(1, 1, 1, h, fill='black')

        
class Mode:
    SCAN_DO = 1

def monitor(frame):
    with reader.MockReader() as r:
        while True:
            time.sleep(1)
            mode = r.read_byte()

            if mode == Mode.SCAN_DO:
                angle = r.read_int()
                value = r.read_int()
                frame.plotter.update(angle, value)
                frame.update()

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

        x, y = self.plotter.x, self.plotter.y
        self.graph_canvas.plot(x, y)

        quit = tkinter.Button(text='Quit', command=tk.destroy)
        quit.pack(side='bottom')

if __name__ == '__main__':
    tk = tkinter.Tk()
    app = AppFrame(parent=tk)
    app.mainloop()
