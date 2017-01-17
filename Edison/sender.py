import os.path
import requests
import time


port = 5000
ip = '192.168.1.95'


def get_data(file_name):
    f = open(file_name, 'r')
    return f.read()
file_name = 'temp.txt'
while True:
    if os.path.exists(file_name):
        #time.sleep(1)
        data = get_data(file_name)
        print 'Send: {}'.format(data)

        r = requests.post("http://{}:{}/req".format(ip, port), data={'data': data})
        print(r.status_code, r.reason)
        try:
            os.remove(file_name)
        except OSError:
            print 'No file'