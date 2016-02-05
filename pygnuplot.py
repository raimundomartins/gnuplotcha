import os, sys, subprocess, struct, itertools
from subprocess import PIPE
from math import sin,pi

class pyplotcha():
    def __init__(self, cmd="plot", xrange="[*:*]", yrange="[*:*]"):
        self.gnu = subprocess.Popen(["gnuplot", "-persist"], stdin=PIPE, stdout=PIPE, universal_newlines=False)
        self.plots, self.cmd = [], cmd
        self.xrange, self.yrange = xrange, yrange

    def addplot(self, data, pwith="points", cmd=''):
        undata = list(itertools.chain(*data))
        bin = struct.pack("d"*len(undata), *undata)
        self.plots.append(("'-' binary record=(1,%d) format='%s' %s with %s" % (len(data[0]),'%lf'*len(data),cmd,pwith),bin))

    def writeline(self, data):
        if sys.version_info[0] is 2:
                self.gnu.stdin.write(data+os.linesep)
        elif sys.version_info[0] is 3:
                self.gnu.stdin.write(bytes(data+os.linesep, "UTF-8"))

    def draw(self):
        if self.gnu.poll() is not None:
                return False
        self.writeline("set xrange %s"%self.xrange)
        self.writeline("set yrange %s"%self.yrange)
        self.writeline("".join(["%s "%self.cmd, ", ".join([x[0] for x in self.plots])]))
        for x in self.plots: self.gnu.stdin.write(x[1])
        self.gnu.stdin.flush()

    def mkvid(self, filename, size, plotframe_func, setupplot_func=None, fps=25):
        self.gnu.stdout=PIPE
        args = ["ffmpeg", "-y", "-f", "image2pipe", "-r", str(fps), "-vcodec", "png", "-i", "-", "-vcodec", "png", "-s", "%dx%d"%size, "-r", str(fps), filename]
        ffmpeg = subprocess.Popen(args, stdin=self.gnu.stdout)
        self.writeline("set t png size %d,%d"%size)
        if setupplot_func is not None: setupplot_func()
        frame = 1
        while plotframe_func(frame) is True: frame += 1
        self.gnu.stdout=None

    def close(self):
        self.gnu.stdin.close()
        if self.gnu.stdout is not None: self.gnu.stdout.close()
        if self.gnu.stderr is not None: self.gnu.stderr.close()

if __name__ == "__main__":
    def plotframe(plotter, frame):
        plotter.plots = []
        x = list(range(50))
        y = list(map(lambda x: sin(x/50.0*pi*2)*frame, x))
        plotter.addplot(x, y)
        plotter.draw()
        return frame < 20

    from math import sin, pi
    #example
    p = pyplotcha()
    p.xrange = "[0:9]"
    p.yrange = "[-1:26]"
    p.mkvid("test.avi", (600,400), lambda f: plotframe(p, f), fps=10)
    p.close()
