import collections

class Plotter:
    '''Uses Tkinter to plot incoming data
    to a tkinter board thingy
    '''
    def __init__(self):
        self.max_x = None
        self.max_y = None

    def update(self, *data):
        '''Update the Plotter with some new data
        '''
        raise NotImplemenetedError

class DefaultPlotter(Plotter):
    def __init__(self):
        super(DefaultPlotter, self).__init__()
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
    def __init__(self):
        super(DefaultPlotter, self).__init__()
        self.max_x = None
        self.max_y = 4096
        self.xs = []
        self.ys = []
        self.bound = 100
        self.x_offs = 0

    @property
    def x(self):
        return map(lambda x: x-self.x_offs, self.xs)

    @property
    def y(self):
        return self.ys

    def update(self, *data):
        x, y = data

        if len(self.xs) >= self.bound:
            self.xs = self.xs[1:]
            self.ys = self.ys[1:]
            self.x_offs += 1

        self.xs.append(x)
        self.ys.append(y)

class ScanPlotter(Plotter):
    def __init__(self):
        super(DefaultPlotter, self).__init__()
        self.max_x = 270
        self.max_y = 4096
        self.values = {}

    @property
    def x(self):
        return map(lambda x: x[1], sorted(self.values.values(), key=lambda x: x[0]))

    @property
    def y(self):
        return map(lambda x: x[0], sorted(self.values.values(), key=lambda x: x[0]))

    def update(self, *data):
        x, y = data

        self.values[x] = y
