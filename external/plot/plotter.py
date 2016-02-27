import collections

class Plotter:
    '''Uses Tkinter to plot incoming data
    to a tkinter board thingy
    '''
    def __init__(self, *dimensions):
        self.max_x = None
        self.max_y = None
        self.dimensions = dimensions

    def update(self, *data):
        '''Update the Plotter with some new data
        '''
        raise NotImplemenetedError

class DefaultPlotter(Plotter):
    def __init__(self, *dimensions):
        Plotter.__init__(self, *dimensions)
        self.xs = []
        self.ys = []

    @property
    def x(self):
        return self.xs

    @property
    def y(self):
        return self.ys

    def update(self, *data):
        x, y = data

        self.xs.append(x)
        self.ys.append(y)

class MeasurePlotter(Plotter):
    def __init__(self, w, h):
        Plotter.__init__(self, w, h)
        self.max_x = None
        self.max_y = int(1e6)
        self.xs = []
        self.ys = []
        self.bound = w
        self.x_offs = 0

    @property
    def x(self):
        return map(lambda x: x - self.x_offs, self.xs)

    @property
    def y(self):
        return self.ys

    def update(self, *data):
        x, y = data

        if len(self.xs) >= self.bound:
            self.xs = self.xs[1:]
            self.ys = self.ys[1:]
            self.x_offs = self.xs[0]

        self.xs.append(x)
        self.ys.append(y)

class ScanPlotter(Plotter):
    def __init__(self, *dimensions):
        Plotter.__init__(self, *dimensions)
        self.max_x = 270
        self.max_y = int(1e6)
        self.values = {}

    @property
    def x(self):
        vals = self.values.items()
        in_order = sorted(vals, key=lambda x: x[0])
        return list(map(lambda x: x[0], in_order))

    @property
    def y(self):
        vals = self.values.items()
        in_order = sorted(vals, key=lambda x: x[0])
        return list(map(lambda x: x[1], in_order))

    def update(self, *data):
        x, y = data
        self.values[x] = y
