import matplotlib.pyplot as plt
from flask import Flask, render_template, request
app = Flask(__name__)

ip = '192.168.1.95'
port = 5000

plot_id = {'id': 0}

@app.route('/')
def index():

    plot = 'plot-{}.png'.format(plot_id['id'])
    return render_template('index.html', plot=plot)

def sv_fig(data):
    y = data
    x = range(len(data))
    fig = plt.figure()
    plt.plot(x, y, )
    plt.title('Speed')
    plt.grid()
    plt.xlabel('Counts')
    plt.ylabel('Speed (m/s)')
    # plt.show()
    plot_id['id'] += 1
    fig.savefig('static/plot-{}.png'.format(plot_id['id']))


def data_processing(raw_data):
    data = [int(d) for d in raw_data.split('-') if d != '']
    data = [25120000.0 / d for d in data]

    return data

@app.route('/req', methods=['POST'])
def req():
    if request.method == 'POST':
        print 'data: "{}"'.format(request.form['data'])

        data = data_processing(request.form['data'])

        if len(data) > 3:
            sv_fig(data)

    return ''

if __name__ == '__main__':
    app.run('http://{}:{}/'.format(ip, port))