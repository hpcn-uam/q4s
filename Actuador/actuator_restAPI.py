import getopt, sys, os, errno  
import getpass    
import requests
import socket
from requests.auth import HTTPBasicAuth

def main(): 
	port_number= 27017
	coder_ip = '127.0.0.1'
	coder_port= 55555

		sock = createListenAlertSocket(port_number)

		
		print 'INFO: Started UDP server on port ', port_number
		server.serve_forever()
	




def createListenAlertSocket(port_number):
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.bind(("", port_number))
	return sock


def parse_metrics(text):
    """ Parses the Q4S message into the corresponding metrics"""
    latency = float('nan')
    jitter = float('nan')
    bandwidth = float('nan')
    packetloss = float('nan')

    text = text.split()
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
    return latency, jitter, bandwidth, packetloss

