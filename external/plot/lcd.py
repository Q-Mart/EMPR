#!/usr/bin/env python
from socket import AF_UNIX, SOCK_DGRAM, socket
from contextlib import closing
import threading
import time
import struct
import os
import collections
import functools

try:
    import tkinter
except ImportError:
   import Tkinter as tkinter

SOCK_ADDR = '/tmp/empr_ipc_socket_lcd'

def monitor(frame):
    '''Monitors a UNIX Socket
    for IPC with debug build
    '''

    if os.path.exists(SOCK_ADDR):
        os.remove(SOCK_ADDR)

    processor = process(frame)
    next(processor)
    with closing(socket(AF_UNIX, SOCK_DGRAM)) as sock:
        sock.bind(SOCK_ADDR)

        while True:
            v = sock.recv(1024)

            if not v:
                break

            processor.send(v)

    os.remove(SOCK_ADDR)

def process(frame):
    '''Try process `buffer`
    '''
    strs = collections.deque()
    buf = collections.deque()

    top, bot = "", ""

    while True:
        v = yield
        buf.extend(v) 

        if b'\0' in buf:
            line, s = get_str(buf)
            if line == 1:
                top = s
            else:
                bot = s

            frame.update(top, bot)

def get_str(buf):
    line, = struct.unpack('B', buf.popleft())
    s = ""
    for x in iter(buf.popleft, b'\0'):
        s += x
    return line, s

class LcdCanvas(tkinter.Canvas):
    def __init__(self, parent, width=None, height=None):
        if not width or not height:
            raise ValueError

        tkinter.Canvas.__init__(self, parent, width=width, height=height)

        self.lines = [None, None]

        top = tkinter.StringVar()
        bot = tkinter.StringVar()
        tkinter.Label(parent, textvariable=top).pack()
        tkinter.Label(parent, textvariable=bot).pack()
        self._lines = [top, bot]

    def update(self, *lines):
        self.lines = lines

    def draw(self):
        list(map(lambda t, s: t.set(s), self._lines, self.lines))
        
class AppFrame(tkinter.Frame):
    def __init__(self, parent):
        tkinter.Frame.__init__(self, parent)

        self.pack()
        self.lcd = LcdCanvas(self, 10, 5)
        self.lcd.pack()

        monitor_t = threading.Thread(target=monitor, args=(self,))
        monitor_t.daemon = True
        monitor_t.start()

    def update(self, *lines):
        self.lcd.update(*lines)
        self.lcd.draw()

def run_lcd():
    tk = tkinter.Tk()

    W, H = 150, 50
    tk.minsize(width=W, height=H)
    tk.maxsize(width=W, height=H)
    tk.resizable(width=tkinter.FALSE, height=tkinter.FALSE)

    app = AppFrame(parent=tk)
    app.pack()

    time.sleep(0.1)

    app.mainloop()

if __name__ == '__main__':
    run_lcd()
