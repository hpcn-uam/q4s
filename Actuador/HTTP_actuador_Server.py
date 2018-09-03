import math
import sys
import getopt
import time
import os
import importlib
import socketserver
import urllib
from http.server import BaseHTTPRequestHandler, HTTPServer


PORT = 5050

class http_server:
    def __init__(self):

        server = HTTPServer(('', PORT), myHandler)
        server.serve_forever()

class myHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        self.send_response(200)
        self.send_header('Content-type','text/plain')
        self.end_headers()
        content_len = int(self.headers.get('content-length', 0))
        post_body = self.rfile.read(content_len).decode()
        fields = urllib.parse.parse_qs(post_body)
        print(post_body)
        print(fields['fps'])
        hp= list(map(int, fields['hp']))
        rs= list(map(int, fields['rs']))
        fps=list(map(int, fields['fps']))
        clr= list(map(int, fields['clr']))
        wp=list(map(int, fields['wp']))
        command="lhe_config -f %d -c %d -h %d -w %d" %(fps[0], clr[0],hp[0],wp[0])
        BW_up=10000
        BW_down=10000 
        print(command)
        os.system(command)
        if fps[0]==2:
            if clr[0]==0:
                BW_up=20000
                BW_down=20000 
            elif clr[0]==1:
                BW_up=30001
                BW_down=30001 
        elif fps[0]==3:
            if clr[0]==0:
                BW_up=20000
                BW_down=20000  
            elif clr[0]==1:
                BW_up=20000
                BW_down=20000   
        elif fps[0]==4:
            if clr[0]==0:
                BW_up=20000
                BW_down=20000  
            elif clr[0]==1:
                BW_up=20000
                BW_down=20000  
        elif fps[0]==5:
            if clr[0]==0:
                BW_up=20000
                BW_down=20000  
            elif clr[0]==1:
                BW_up=20000
                BW_down=20000  
        elif fps[0]==6:
            if clr[0]==0:
                BW_up=20000
                BW_down=20000  
            elif clr[0]==1:
                BW_up=20000
                BW_down=20000  
        elif fps[0]==8:
            if clr[0]==0:
                BW_up=20000
                BW_down=20000  
            elif clr[0]==1:
                BW_up=20000
                BW_down=20000  
        elif fps[0]==12:
            if clr[0]==0:
                BW_up=20000
                BW_down=20000  
            elif clr[0]==1:
                BW_up=20000
                BW_down=20000  
        with open("../Q4S_client-server/q4sServer/ejemplo.txt", "r") as text_file: 
            BW_aux=text_file.readline()
            BW_aux=text_file.readline()
            packet_size=text_file.readline()
            text_file.close()
        with open("../Q4S_client-server/q4sServer/ejemplo.txt", "w") as text_file:
            text_file.write("%d\n%d\n%d"%(int(BW_up), int(BW_down),int(packet_size)))
            text_file.close()

class main:
    def __init__(self):
 
        self.server = http_server()
 
if __name__ == '__main__':
    m = main()
