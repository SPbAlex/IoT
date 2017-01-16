import os.path
import requests

file_name = 'temp.txt'
port = 5000
ip = 'http://127.0.0.1'


def get_data(file_name):
    f = open(file_name, 'r')
    return f.read()

while True:
    if os.path.exists(file_name):
        data = get_data(file_name)
        print data

        r = requests.post("{}:{}/req".format(ip, port), data={'data': data})
        print(r.status_code, r.reason)

        os.remove(file_name)