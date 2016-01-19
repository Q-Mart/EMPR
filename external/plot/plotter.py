import collections

class Plotter:
    '''Uses Tkinter to plot incoming data
    to a tkinter board thingy
    '''
    def update(self, *data):
        '''Update the Plotter with some new data
        '''
        raise NotImplemenetedError

class DefaultPlotter(Plotter):
    def __init__(self):
        self.lut = {}
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
