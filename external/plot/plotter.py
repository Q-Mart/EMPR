import collections

class Plotter:
    '''Uses Tkinter to plot incoming data
    to a tkinter board thingy
    '''
    def __init__(self, x_label, y_label, *dimensions):
        self.max_x = None
        self.max_y = None
        self.dimensions = dimensions
        self.label_x = x_label
        self.label_y = y_label

    def update(self, *data):
        '''Update the Plotter with some new data
        '''
        raise NotImplemenetedError

class DefaultPlotter(Plotter):
    def __init__(self, *dimensions):
        Plotter.__init__(self, 'Default', 'Default', *dimensions)
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
        #print('got (x, y) = (%d, %d)' % (x,y))

        self.xs.append(x)
        self.ys.append(y)

class MeasurePlotter(Plotter):
    def __init__(self, w, h):
        Plotter.__init__(self, 'Time', 'Distance', w, h)
        self.max_x = w
        self.max_y = int(1e6)
        self.xs = []
        self.ys = []

        self.n = 30

    @property
    def x(self):
        return self.xs

    @property
    def y(self):
        return self.ys

    def update(self, *data):
        print('MeasurePlotter, update(x={}, y={})'.format(*data))
        _, y = data
        x = self.max_x - 1

        if len(self.xs) > 0:
            dt = int(float(self.max_x) / float(self.n))

            for i,xp in enumerate(self.xs):
                self.xs[i] = xp - dt

        self.xs.append(x)
        self.ys.append(y)

        while self.xs[0] < 0:
            self.xs = self.xs[1:]
            self.ys = self.ys[1:]

class ScanPlotter(Plotter):
    def __init__(self, *dimensions):
        Plotter.__init__(self, 'Angle', 'Distance', *dimensions)
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
