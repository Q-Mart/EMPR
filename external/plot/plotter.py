import collections
import math

MAX = int(6e5)

Point = collections.namedtuple('Point', ['x', 'y', 'col', 'rot_x', 'rot_y', 'rot_theta'])

DEFAULT_COLOR = 'blue'
COLORS = ['blue', 'gray62', 'gray87'] # show 3 sweeps (current and 2 history)

class Plotter:
    '''Uses Tkinter to plot incoming data
    to a tkinter board thingy
    '''

    NONE = 0b0000
    NOLINE = 0b0001
    POLAR = 0b0010
    NOXLABELS = 0b0100
    NOYLABELS = 0b1000
    NOLABELS = 0b1100

    def __init__(self, flags, x_label, y_label, *dimensions):
        self.max_x = None
        self.max_y = None
        self.mode = flags
        self.dimensions = dimensions
        self.label_x = x_label
        self.label_y = y_label
        self.points = []

    def plot(self, x, y, col=DEFAULT_COLOR, rot_theta=0, rot_x=0, rot_y=0):
        p = Point(x, y, col, rot_x, rot_y, rot_theta)
        self.points.append(p)
        return p

    def update(self, *data):
        '''Update the Plotter with some new data
        '''
        raise NotImplemenetedError

    @property
    def x(self):
        return map(lambda p: p.x, self.points)

    @property
    def y(self):
        return map(lambda p: p.y, self.points)

class DefaultPlotter(Plotter):
    def __init__(self, *dimensions):
        Plotter.__init__(self, Plotter.NONE, 'Default', 'Default', *dimensions)

    def update(self, *data):
        x, y = data
        self.plot(x, y)

class MeasurePlotter(Plotter):
    def __init__(self, w, h):
        Plotter.__init__(self, Plotter.NONE, 'Time', 'Distance', w, h)
        self.max_x = w
        self.max_y = MAX

        self.n = 30

    def update(self, *data):
        print('MeasurePlotter, update(x={}, y={})'.format(*data))
        _, y = data
        x = self.max_x - 1

        if len(self.points) > 0:
            dt = int(float(self.max_x) / float(self.n))

            for i, p in enumerate(self.points):
                self.points[i] = p._replace(x=p.x - dt)

        self.plot(x, y)

        while self.points[0].x < 0:
            self.points = self.points[1:]

class ScanPlotter(Plotter):
    def __init__(self, *dimensions):
        Plotter.__init__(self, Plotter.NOLABELS | Plotter.POLAR, 'Angle', 'Distance', *dimensions)
        self.max_x = 270
        self.max_y = MAX
        self.values = collections.defaultdict(list)

    def update(self, *data):
        x, y = data
        
        if len(self.values[x]) >= len(COLORS):
            p = self.values[x][0]
            self.values[x] = self.values[x][1:]
            self.points = self.points[1:]

        for i, p in enumerate(self.values[x]):
            j = COLORS.index(p.col)
            p2 = p._replace(col=COLORS[j+1])
            k = self.points.index(p)

            self.values[x][i] = p2
            self.points[k] = p2
            
        p = self.plot(x, y, col=COLORS[0])
        self.values[x].append(p)

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
        self.plot(x, y, rot_theta=t, rot_x=self.centre_x, rot_y=self.centre_y)

    def update(self, *data):
        msg, value = data
        if msg == MultiPlotter.SWEEP:
            x, y = value
            self._append(x, y)
        elif msg == MultiPlotter.NEXT:
            # rotate
            self._current += 1
        elif msg == MultiPlotter.PARAMS:
            scan_number, amin, amax = value
            self._number = scan_number
            self._angle = 2*math.pi / float(scan_number)
            self._limits = (amin, amax)
