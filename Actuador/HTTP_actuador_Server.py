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
        #   text_file.write("%f,0,0,0,0,0,0\n"%ts)
        with open("ActuatorConfigFile.txt","r") as text_file:
            line=text_file.readline()
            while line:
                text=line.rsplit("=")
                for index, word in enumerate(text[:-1]):
                    if word== "Q4S_IP":
                        server.client_ip=text[index+1].rstrip()
                    
                    elif word== "FLAG_IDB":
                        if int(text[index+1])==1:
                            server.flag_IDB=True
                        else:
                            server.flag_IDB=False
                    line=text_file.readline()
            text_file.close()
        server.serve_forever()
        return 1

class myHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        self.send_response(200)
        self.send_header('Content-type','text/plain')
        self.end_headers()
        content_len = int(self.headers.get('content-length', 0))
        post_body = self.rfile.read(content_len).decode()
        fields = urllib.parse.parse_qs(post_body)
        #print(post_body)
        #print(fields['fps'])
        hp= list(map(int, fields['hp']))
        rs= list(map(int, fields['rs']))
        fps=list(map(int, fields['fps']))
        clr= list(map(int, fields['clr']))
        wp=list(map(int, fields['wp']))
        command="lhe_config -f %d -c %d -h %d -w %d" %(fps[0], clr[0],hp[0],wp[0])
        BW_up=10000
        BW_down=10000
        jitter_up=20
        jitter_down=20
        latency=41
        #print(command)
        os.system(command)
        change_flag=True
        if fps[0]==2:
            if clr[0]==0:
                BW_up=48000
                BW_down=48000 
                latency=41
                jitter_up=20
                jitter_down=20
            else:
                BW_up=48000
                BW_down=48000
                latency=41
                jitter_up=20
                jitter_down=20
        elif fps[0]==3:
            if clr[0]==0:
                BW_up=32000
                BW_down=32000
                latency=36
                jitter_up=36
                jitter_down=36
            else:
                BW_up=32000
                BW_down=32000
                latency=36
                jitter_up=36
                jitter_down=36

        elif fps[0]==4:
            if clr[0]==0:
                BW_up=20000
                BW_down=20000  
                latency=32
                jitter_up=32
                jitter_down=32
            else:
                BW_up=24800
                BW_down=24800 
                latency=32
                jitter_up=32
                jitter_down=32
        elif fps[0]==5:
            if clr[0]==0:
                BW_up=16000
                BW_down=16000 
                latency=27
                jitter_up=27
                jitter_down=27
            else:
                BW_up=19000
                BW_down=19000 
                latency=27
                jitter_up=27
                jitter_down=27
        elif fps[0]==6:
            if clr[0]==0:
                BW_up=13000
                BW_down=13000 
                latency=23
                jitter=23
                jitter_down=23
            else:
                BW_up=16000
                BW_down=16000  
                latency=23
                jitter_up=23
                jitter_down=23
        elif fps[0]==8:
            if clr[0]==0:
                BW_up=10500
                BW_down=10500 
                latency=14
                jitter_up=14
                jitter_down=14
            else:
                BW_up=13000
                BW_down=13000 
                latency=14
                jitter_up=14
                jitter_down=14
        elif fps[0]==12:
            if clr[0]==0:
                BW_up=6800
                BW_down=6800  
                latency=5
                jitter_up=5
                jitter_down=5
            else:
                BW_up=6800
                BW_down=6800
                latency=5
                jitter_up=5
                jitter_down=5
        else: 
            change_flag=False

        if hp[0]==1:
            if wp[0]==320:
                packet_size=750
            else:
                packet_size=980
        elif hp[0]==6:
            packet_size=1240
        elif hp[0]==8:
            packet_size=1400
        else:
            change_flag=False
        if self.server.flag_IDB:
            command_curl= "curl -i -XPOST 'http://%s:8086/write?db=racing_drones&precision=ms' --data-binary 'App_web BW_Up=%d,BW_Down=%d'"%(self.server.client_ip,int(BW_up),int(BW_down))
            os.system(command_curl)
        if change_flag:
            with open("../Q4S_client-server/q4sServer/dynamic_params.txt", "w") as text_file:
                text_file.write("BW_up=%d\nBW_down=%d\nlatency=%f\njitter_up=%f\njitter_down=%f\npacket_length=%d\n"%(int(BW_up), int(BW_down),float(latency),float(jitter_up),float(jitter_down),int(packet_size)))
                text_file.close()

class main:
    def __init__(self):
        try:
            server = http_server()


        except KeyboardInterrupt:
            print('\033[94m', 'INFO: ^C received, shutting down UDP server', '\033[0m')
            with open("../Q4S_client-server/q4sServer/dynamic_params.txt","w") as text_file:
                text_file.write("BW_up=%d\nBW_down=%d\nlatency=%f\njitter_up=%f\njitter_down=%f\npacket_length=%d\n"%(int(48000), int(48000), float(41),float(20),float(20), int(1420)))
                text_file.close()
            self.server.shutdown()
 
if __name__ == '__main__':
    m = main()
