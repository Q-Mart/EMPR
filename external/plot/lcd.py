#!/usr/bin/env python
from socket import AF_UNIX, SOCK_DGRAM, socket
from contextlib import closing
import threading
import time
import struct
import os

try:
    import tkinter
except ImportError:
   import Tkinter as tkinter

SOCK_ADDR = '/tmp/empr_ipc_socket'

def monitor(frame):
    '''Monitors a UNIX Socket
    for IPC with debug build
    '''

    if os.path.exists(SOCK_ADDR):
        os.remove(SOCK_ADDR)

    with closing(socket(AF_UNIX, SOCK_DGRAM)) as sock:
        sock.bind(SOCK_ADDR)

        buf = []
        while True:
            v = sock.recv(1024)

            if not v:
                break

            buf.extend(v)
            if b'\0' in buf:
                buf_i = buf.index(b'\0')
                vals, buf = buf[:buf_i], buf[buf_i+1:]
                try:
                    split_lines(frame, vals)
                except:
                    pass

    os.remove(SOCK_ADDR)
    print('Monitor dead.')

def split_lines(frame, vals):
    top, bot = [], []

    p = top
    for _ in range(2):
        val, vals = vals[0], vals[1:]
        n, = struct.unpack('B', val)
        for _ in range(n):
            val, vals = vals[0], vals[1:]
            p.append(val)
        p = bot

    top = ''.join(top)
    bot = ''.join(bot)
    frame.update(top, bot)

class LcdCanvas(tkinter.Canvas):
    def __init__(self, parent, width=None, height=None):
        if not width or not height:
            raise ValueError

        tkinter.Canvas.__init__(self, parent, width=width, height=height)

        self.lines = [None, None]

        top = tkinter.Text()
        bot = tkinter.Text()
        self._lines = [top, bot]

        top.pack(side='top')
        bot.pack(side='bottom')

    def draw(self):
        list(map(lambda t: t.delete('0.0', tkinter.END), self._lines))
        list(map(lambda t, s: t.insert(tkinter.END, s), self._lines, self.lines))
        
class AppFrame(tkinter.Frame):
    def __init__(self, parent):
        tkinter.Frame.__init__(self, parent)

        self.pack()
        self.lcd = LcdCanvas(self, 30, 10)
        self.lcd.pack(side='top')

        monitor_t = threading.Thread(target=monitor, args=(self,))
        monitor_t.daemon = True
        monitor_t.start()

    def update(self, *lines):
        self.lcd.lines = lines
        self.lcd.draw()

def run_lcd():
    tk = tkinter.Tk()
    app = AppFrame(parent=tk)
    app.pack(expand=tkinter.YES)

    time.sleep(0.1)

    app.mainloop()

if __name__ == '__main__':
    run_lcd()
