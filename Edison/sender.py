import os.path
import time
import socket
file_name = 'temp.txt'
port = 9090
ip = '192.168.1.55'

def connect(ip, port):
    soc = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    print "Try connect"
    while True:
        try:
            soc.connect((ip, port))
            print "Connected"
            break
        except socket.error as msq:
            print "Can't connect"
            time.sleep(5)
            continue

    return soc
soc = connect(ip, port)

while True:
    if os.path.exists(file_name):
        f = open(file_name)
        str = f.read()
        #print [int(d) for d in str.split('-') if d != '']
        while True:
            try:
                soc.send(str)
                print "Send data"
                break
            except socket.error as msq:
                print "Can't send"
                soc.close()
                soc = connect(ip, port)
                continue

        os.remove(file_name)
        
