import collections
import math

MAX = int(6e5)

class Plotter:
    '''Uses Tkinter to plot incoming data
    to a tkinter board thingy
    '''

    NONE = 0b000
    NOLINE = 0b001
    POLAR = 0b010
    NOLABELS = 0b100

    def __init__(self, flags, x_label, y_label, *dimensions):
        self.max_x = None
        self.max_y = None
        self.mode = flags
        self.dimensions = dimensions
        self.label_x = x_label
        self.label_y = y_label
        self.rotations = {}

    def rotate(self, x, theta, centre_x, centre_y):
        self.rotations[x] = (theta, centre_x, centre_y)

    def update(self, *data):
        '''Update the Plotter with some new data
        '''
        raise NotImplemenetedError

class DefaultPlotter(Plotter):
    def __init__(self, *dimensions):
        Plotter.__init__(self, Plotter.NONE, 'Default', 'Default', *dimensions)
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
        Plotter.__init__(self, Plotter.NONE, 'Time', 'Distance', w, h)
        self.max_x = w
        self.max_y = MAX
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
        Plotter.__init__(self, Plotter.NONE | Plotter.POLAR, 'Angle', 'Distance', *dimensions)
        self.max_x = 270
        self.max_y = MAX
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
        Plotter.__init__(self, Plotter.NOLINE | Plotter.NOLABELS, 'Angle', 'Shape', *dimensions)
        self.max_x = 270
        self.max_y = MAX
        self.x = []
        self.y = []
        self.centre_x = 160
        self.centre_y = int(150000)
        self._current = 0

    def _append(self, x, y):
        t = (self._current) * self._angle
        self.rotate((x, y), t, self.centre_x, self.centre_y)
        self.x.append(x)
        self.y.append(y)

    def update(self, *data):
        msg, value = data
        if msg == MultiPlotter.SWEEP:
            x, y = value
            self._append(x, y)
        elif msg == MultiPlotter.NEXT:
            print "Next"
            # rotate
            self._current += 1
        elif msg == MultiPlotter.PARAMS:
            scan_number, amin, amax = value
            self._number = scan_number
            self._angle = 2*math.pi / float(scan_number)
            self._limits = (amin, amax)
            print "Parameters: ", self._number, self._angle
