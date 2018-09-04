import importlib
import socketserver
import http.client
import math
import sys
import getopt
import time
import urllib
import os
def main(): 
    port_number= 27017
    coder_ip = '127.0.0.1'
    coder_port= 3000  
    
    
    print('HELLO')
    try:

        #with open("../test/measured/dynamic_actuator.txt", "w") as text_file:
        #    ts= time.time()
        #   text_file.write("%f,0,0,0,0,0,0\n"%ts)
        server = socketserver.UDPServer(('', port_number), UDPHandler)
        connHTTP = http.client.HTTPConnection("127.0.0.1:3000")
        server.QoSlevel=0
        server.coder_direction = (coder_ip, coder_port)
        print('INFO: Started UDP server on port ', port_number)
        server.serve_forever()
    except KeyboardInterrupt:
        print('INFO: ^C received, shutting down UDP server')
        server.shutdown()
    return


class UDPHandler(socketserver.BaseRequestHandler):
    """Handler class for the UDP server"""

    def handle(self):
        flag_Termination= False
        QoSlevel = self.server.QoSlevel
        data = self.request[0].strip()
        data = data.decode("utf-8")
        coder_ip = self.server.coder_direction[0]
        coder_port = self.server.coder_direction[1]
        latency, jitter, bandwidth, packetloss, flag_Termination = parse_metrics(data)
        QoSlevel = calculate_parameters(latency, jitter, bandwidth, packetloss, level, flag_p_size, QoSlevel, packet_size)
        self.server.QoSlevel= QoSlevel
        if QoSlevel>10:
            self.server.QoSlevel=0
            self.server.flag_p_size=True
        elif QoSlevel<=0: 
            self.server.QoSlevel=0
        if flag_Termination:
            self.server.QoSlevel=0
            QoSlevel= 0
        """
        with open("../test/measured/dynamic_actuator.txt", "a") as text_file:
            text_file.write("%f,%d,%d,%d,%d,%d,%d\n"%(ts,height,width,fps,rs,clr,self.server.QoSlevel))
        print "Imprimir:",height,width,fps,rs,clr,self.server.QoSlevel
        #print latency, jitter, bandwidth, packetloss
        """      
        return



def calculate_parameters(latency, jitter, bandwidth, packetloss,level, flag_p_size,QoSlevel, packet_size):
    """ From the network Q4S parameters generats the coder options."""
    #pylint: disable=unused-argument
    pl_flag=False
    lt_flag=False
    BW_flag=False
    jt_flag=False 

    with open("../Q4S_client-server/q4sServer/ejemplo.txt", "r") as text_file:
            BW_up = text_file.readline()
            BW_down = text_file.readline()
            text_file.close()
    discard=0   
    skip=0

    """""""""""""""""""""""""""""""""""""""""
    AQUI HAY QUE PONER UMBRALES REALES
    """""""""""""""""""""""""""""""""""""""""
    print ("Jitter calculate_parameters:", jitter)
    if packetloss > 5:
        pl_flag=True  
    if latency>5.5:
        lt_flag=True
    if bandwidth < float(BW_up):
        BW_flag=True
    if jitter > 1.1:
        jt_flag=True

    if (not BW_flag) & (not pl_flag) & (not lt_flag)& (not jt_flag):
        QoSlevel-=1
    else:
        QoSlevel+=1


    if QoSlevel<=0: 
        discard=0
        skip=0
        BW=9650
    elif QoSlevel== 1:
        discard=1
        skip=0
        BW=9600
    elif QoSlevel== 2:
        discard=2
        skip=0
        BW=9400
    elif QoSlevel== 3:
        discard=3
        skip=0
        BW=8500
    elif QoSlevel== 4:
        discard=4
        skip=0
        BW=7000
    elif QoSlevel== 5:
        discard=5
        skip=0
        BW=5000
    elif QoSlevel== 6:
        discard=5
        skip=1
        BW=5000
    elif QoSlevel== 7:
        discard=5
        skip=2
        BW=5000
    elif QoSlevel== 8:
        discard=5
        skip=3
        BW=5000
    elif QoSlevel== 9:
        discard=5
        skip=4
        BW=5000
    elif QoSlevel>= 10: #LO MAS BAJO POSIBLE, ES EL ULTIMO NIVEL
        discard=5
        skip=5
        BW=5000
    
    with open("../Q4S_client-server/q4sServer/ejemplo.txt","w") as text_file:
        text_file.write("%d\n%d"%(BW,BW))
    #with open("../test/measured/dynamic_actuator.txt", "a") as text_file:
    #        text_file.write("%f,%d,%d,%d,%d,%d,%d\n"%(ts,dim_packet[packet_size][0],dim_packet[packet_size][1],fps,rs,clr,QoSlevel))
    headers = {"Content-type": "","Accept": "text/plain"}

    conn = http.client.HTTPConnection('127.0.0.1',3000)            
    conn.request('POST', '/' + 'discard'+ '/' + str(discard))
    conn = http.client.HTTPConnection('127.0.0.1',3000)  
    conn.request('POST', '/' + 'skip'+ '/' + str(skip))

    res = conn.getresponse()

    return QoSlevel


def parse_metrics(text):
    """ Parses the Q4S message into the corresponding metrics"""
    latency = float('nan')
    jitter = float('nan')
    bandwidth = float('nan')
    bandwidth = 100000
    packetloss = float('nan')
    flag_Termination=False
    text = text.split()
    if text[1]=="Termination":
        flag_Termination=True
    print (text)
    for index, word in enumerate(text[:-1]):
        if word == "Latency:":
            try:
                latency = float(text[index+1])
            except ValueError:
                latency = float('nan')
                
        elif word == "Jitter:":
            try:
                jitter = float(text[index+1])
            except ValueError:
                jitter = float('nan')                
        elif word == "PacketLoss:":
            try:
                packetloss = float(text[index+1])
            except ValueError:
                packetloss = float('nan')
        elif word == "BandWidth:":
            try:
                bandwidth = float(text[index+1])
            except ValueError:
                bandwidth = float('nan')
    return latency, jitter, bandwidth, packetloss, flag_Termination


def parse_arguments(argv):
    """ Parses the program arguments in order to ensure that everthing is alright"""

    port_number = -1
    coder_ip = ""
    coder_port = -1
    try:
        optlist, dummy = getopt.getopt(
            argv[1:], "p:c:h", ["port=", "coder=", "help"])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)
    for option, value in optlist:
        if option in ("-p", "--port"):
            try:
                port_number = int(value)
            except ValueError:
                print("ERROR: The port is not a integer")
                usage()
                sys.exit(2)
        elif option in ("-c", "--coder"):
            value = value.split(":")
            if len(value) != 2:
                print("ERROR: The coder direction is not ok", len(value))
                usage()
                sys.exit(2)
            coder_ip = value[0]
            try:
                coder_port = int(value[1])
            except ValueError:
                print("ERROR: The port of the coder is not a integer")
                usage()
                sys.exit(2)
        elif option in ("-h", "--help"):
            usage()
            sys.exit(0)
    if port_number == -1 or coder_ip == "" or coder_port == -1:
        print("ERROR: The required parameters are not supplied")
        usage()
        sys.exit(2)
    return port_number, coder_ip, coder_port

if __name__ == "__main__":
    main()



