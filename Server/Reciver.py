import matplotlib.pyplot as plt
import threading
import socket


class Reciver(threading.Thread):
    def __init__(self, plot_id):
        threading.Thread.__init__(self)
        self.daemon = True
        self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.id = plot_id
        open('log.txt', 'w').close()
        self.start()

    def connect(self):
        self.sock.close()
        self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.sock.bind(('', 9090))
        self.sock.listen(1)

        return self.sock.accept()

    def sv_fig(self, data):
        y = data
        x = range(len(data))
        fig = plt.figure()
        plt.plot(x, y, )
        plt.title('Speed')
        plt.grid()
        plt.xlabel('Counts')
        plt.ylabel('Speed (m/s)')
        # plt.show()
        self.id['id'] += 1
        fig.savefig('static/plot-{}.png'.format(self.id['id']))



    def run(self):
        print 'Try connection'
        conn, addr = self.connect()
        print 'Connected'

        while True:
            while True:
                try:
                    data = conn.recv(1024)
                    break
                except socket.error as msq:
                    print "Connection problems"
                    conn, addr = self.connect()
                    continue
            print 'Recive data -- plot-{}'.format(self.id['id'])

            f = open('log.txt', 'a')
            f.write(data)
            f.close()
            print data
            data = [int(d) for d in data.split('-') if d != '']
            data = [25120000.0 / d for d in data]

            if len(data) > 3:
                self.sv_fig(data)


