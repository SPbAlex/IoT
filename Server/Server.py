import os
import subprocess
import httplib
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.dates import DateFormatter
import os.path
from Reciver import Reciver
from flask import Flask, render_template, url_for, request
app = Flask(__name__)

plot_id = {'id': 0}
nxt_id = 0
prv_id = 0
#last plot

@app.route('/')
def index():

    plot = 'plot-{}.png'.format(plot_id['id'])
    return render_template('index.html', plot=plot)

Reciver(plot_id)

if __name__ == '__main__':
    app.run()

print 'hellow'