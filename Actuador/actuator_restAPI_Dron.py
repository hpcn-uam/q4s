import importlib
import socketserver
import http.client
import socket
import math
import sys
import getopt
import time
import urllib
import os
import time
def main(): 
    port_number= 27017
    coder_ip = '127.0.0.1'
    coder_port= 55555  
    
    
    print('HELLO')
    try:

        #with open("../test/measured/dynamic_actuator.txt", "w") as text_file:
        #    ts= time.time()
        #   text_file.write("%f,0,0,0,0,0,0\n"%ts)
        server = socketserver.UDPServer(('', port_number), UDPHandler)
        connHTTP = http.client.HTTPConnection("127.0.0.1:5050")
        server.level= 0
        server.my_ip=socket.gethostbyname(socket.gethostname())
        server.client_ip="192.168.1.102"
        server.flag_p_size= True
        server.flag_Continuity=False
        server.QoSlevel= 0
        server.packet_size=2
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
        flag_Continuity= self.server.flag_Continuity
        level = self.server.level
        flag_p_size= self.server.flag_p_size
        packet_size=self.server.packet_size
        QoSlevel = self.server.QoSlevel
        data = self.request[0].strip()
        data = data.decode("utf-8")
        coder_ip = self.server.coder_direction[0]
        coder_port = self.server.coder_direction[1]
        latency, jitter, bandwidth, packetloss, flag_Termination,flag_Continuity = parse_metrics(data)
        if flag_Continuity:
            command_Decodec="ssh hpcn@%s 'cd Repos/Packetization-LHE;screen -S screenDecodec -d -m ./LHEPacketizer.out -rc %s --pipe /home/hpcn/Repos/Packetization-LHE/dummy'"%(self.server.client_ip, self.server.my_ip)
            os.system(command_Decodec)
            time.sleep(1)

        
        flag_p_size, level, QoSlevel,packet_size,ts,height,width,fps,rs,clr = calculate_parameters(latency, jitter, bandwidth, packetloss, level, flag_p_size, QoSlevel, packet_size,flag_Continuity)
        self.server.level= level
        self.server.flag_p_size= flag_p_size
        self.server.flag_Continuity=flag_Continuity
        self.server.QoSlevel= QoSlevel
        self.server.packet_size=packet_size
        if QoSlevel>10:
            self.server.QoSlevel=0
            self.server.level=0
            self.server.packet_size=2
            self.server.flag_p_size=True
        elif QoSlevel<=0: 
            self.server.QoSlevel=0            
            self.server.level=0
            self.server.packet_size=2
            self.server.flag_p_size= True
        if flag_Termination:
            command="screen -S screenDecodec -X at '#' stuff S^M"
            command2= 'ssh hpcn@%s "%s"'%(self.server.client_ip,command)

            os.system(command2)
            self.server.QoSlevel=0
            QoSlevel= 0
            self.server.level=0
            self.server.packet_size=2
            self.server.flag_p_size=True
            height= 0
            width= 0
            fps= 0
            rs= 0
            clr=0 
        if fps>0:
            fps_curl= 110/int(fps) 
            command_curl= "curl -i -XPOST 'http://%s:8086/write?db=racing_drones&precision=ms' --data-binary 'actuator_Dron QoS_level=%d,fps=%d,clr=%d,heigth=%d,width=%d'"%(self.server.client_ip,QoSlevel,fps_curl,int(clr),int(height), int(width))
            os.system(command_curl)

        #print("Imprimir:",height,width,fps,rs,clr,self.server.QoSlevel)
        """
        with open("../test/measured/dynamic_actuator.txt", "a") as text_file:
            text_file.write("%f,%d,%d,%d,%d,%d,%d\n"%(ts,height,width,fps,rs,clr,self.server.QoSlevel))
        print "Imprimir:",height,width,fps,rs,clr,self.server.QoSlevel
        #print latency, jitter, bandwidth, packetloss
        """      
        return



def calculate_parameters(latency, jitter, bandwidth, packetloss,level, flag_p_size,QoSlevel, packet_size,flag_Continuity):
    """ From the network Q4S parameters generats the coder options."""
    #pylint: disable=unused-argument
    pl_flag=False
    lt_flag=False
    BW_flag=False
    jt_flag=False 
    with open("../Q4S_client-server/q4sServer/dynamic_params.txt", "r") as text_file:
            aux = text_file.readline()
            BW_up=aux[6:-1]
            aux = text_file.readline()
            BW_down=aux[8:-1]
            aux = text_file.readline()
            latency_app=aux[8:-1]
            aux = text_file.readline()
            jitter_up_app=aux[10:-1]
            aux = text_file.readline()
            jitter_down_app=aux[12:-1]
            aux = text_file.readline()         
            pck_size_RTP= aux[14:-1]
            text_file.close()
    fps=0
    clr=0
    rs= 0
    dim_packet= [1,320],[1,640],[6,160],[8,128]
    packet_RTP= 750, 980, 1240, 1400

    """""""""""""""""""""""""""""""""""""""""
    AQUI HAY QUE PONER UMBRALES REALES
    """""""""""""""""""""""""""""""""""""""""
    if flag_Continuity==False:
        if latency>float(latency_app):
            lt_flag=True
            if packet_size==0:
                flag_p_size=False

        elif packetloss>30:
            if packet_size==0:
                flag_p_size=False
            pl_flag=True
        elif bandwidth < float(BW_up):
            if packet_size==3:
                flag_p_size=False
            BW_flag=True
        elif jitter > float(jitter_up_app):
            jt_flag=True
        if (lt_flag | pl_flag | jt_flag) & flag_p_size:        
            packet_size-=1
            #command="ssh root@192.168.1.101 lhe_config -h %d -w %d" %(dim_packet[packet_size][0],dim_packet[packet_size][1])
        #os.system(command)
        elif BW_flag & flag_p_size:
            packet_size+=1      
            #command="ssh root@192.168.1.101 lhe_config -h %d -w %d" %(dim_packet[packet_size][0],dim_packet[packet_size][1])
        #os.system(command)
        if (not flag_p_size)  & ((BW_flag)|(pl_flag)|(lt_flag)|(jt_flag)):
            level+=1
            flag_p_size=True

        if (not BW_flag) & (not pl_flag) & (not lt_flag)& (not jt_flag):
            QoSlevel-=1
            level-=1
            flag_p_size=True
        else:
            QoSlevel+=1
    #print("QoS level=", QoSlevel
    if level<=0: 
        fps=2
        clr=1
        rs= 0
    elif level== 1:
        fps=3
        clr=1
        rs= 0
    elif level== 2:
        #CAMBIAR RESOLUCION
        fps=4
        clr=1
        rs= 1
        #CAMBIAR FPS
    elif level== 3:
        fps=4
        clr=0
        rs= 1
    elif level== 4:
        fps=5
        clr=1
        rs= 1
    elif level== 5:
        fps=5
        clr=0
        rs= 1
    elif level== 6:
        fps=6
        clr=1
        rs= 1
    elif level== 7:
        fps=6
        clr=0
        rs= 1
    elif level== 8:
        fps=8
        clr=1
        rs= 1
    elif level== 9:
        fps=8
        clr=0
        rs= 1
    elif level>= 10: #LO MAS BAJO POSIBLE, ES EL ULTIMO NIVEL
        fps=12
        clr=0
        rs= 1
    params =urllib.parse.urlencode({'fps': fps, 'clr': clr, 'rs': rs, 'wp':dim_packet[packet_size][1] ,'hp':dim_packet[packet_size][0]})
    ts= time.time()
    #with open("../test/measured/dynamic_actuator.txt", "a") as text_file:
    #        text_file.write("%f,%d,%d,%d,%d,%d,%d\n"%(ts,dim_packet[packet_size][0],dim_packet[packet_size][1],fps,rs,clr,QoSlevel))
    headers = {"Content-type": "","Accept": "text/plain"}

    conn = http.client.HTTPConnection('127.0.0.1',5050)
    conn.request("POST","",params, headers)
    res = conn.getresponse()

    return flag_p_size, level, QoSlevel, packet_size,ts,dim_packet[packet_size][0],dim_packet[packet_size][1],fps,rs,clr


def parse_metrics(text):
    """ Parses the Q4S message into the corresponding metrics"""
    latency = float('nan')
    jitter = float('nan')
    bandwidth = float('nan')
    packetloss = float('nan')
    flag_Termination=False
    flag_Continuity=False
    text = text.split()
    if text[1]=="Termination":
        flag_Termination=True
    if text[1]=="Continuity": 
        flag_Continuity=True
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
    return latency, jitter, bandwidth, packetloss, flag_Termination, flag_Continuity


def parse_arguments(argv):
    """ Parses the program arguments in order to ensure that everthing is alright"""

    port_number = -1
    coder_ip = ""
    coder_port = -1
    try:
        optlist, dummy = getopt.getopt(
            argv[1:], "p:c:h", ["port=", "coder=", "help"])
    except getopt.GetoptError as err:
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
