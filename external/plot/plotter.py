import collections
import math

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

        self.xs.append(x)
        self.ys.append(y)

class MeasurePlotter(Plotter):
    def __init__(self, w, h):
        Plotter.__init__(self, 'Time', 'Distance', w, h)
        self.max_x = w
        self.max_y = int(3e7)
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
        self.max_y = int(3e7)
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

class MultiPlotter(Plotter):
    SWEEP = 1
    NEXT = 2
    PARAMS = 3

    def __init__(self, *dimensions):
        Plotter.__init__(self, 'Angle', 'Distance', *dimensions)
        self.max_x = 270
        self.max_y = int(3e7)
        self.xs = []
        self.ys = []

    @property
    def x(self):
        return self.xs

    @property
    def y(self):
        return self.ys

    def _rotate(self):
        print('_rotate')
        new = []
        cos_t = math.cos(self._angle)
        sin_t = math.sin(self._angle)
        for x, y in zip(self.xs, self.ys):
            # rotate by _angle
            n_x = x*cos_t - y*sin_t
            n_y = x*sin_t + y*cos_t
            new.append((n_x, n_y))

        new = sorted(new)
        self.xs = []
        self.ys = []
        for (x, y) in new:
            self.xs.append(int(x))
            self.ys.append(int(y))
        print('_rotate, done.')

    def update(self, *data):
        msg, value = data
        if msg == MultiPlotter.SWEEP:
            x, y = value
            self.xs.append(x)
            self.ys.append(y)
        elif msg == MultiPlotter.NEXT:
            # rotate
            self._rotate()
        elif msg == MultiPlotter.PARAMS:
            scan_number, _, _ = value
            self._number = scan_number
            self._angle = math.pi / float(scan_number)
