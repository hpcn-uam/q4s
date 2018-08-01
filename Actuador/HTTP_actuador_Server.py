import math
import sys
import getopt
import time
import os
import requests
import urlparse
from BaseHTTPServer import  BaseHTTPRequestHandler, HTTPServer

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
        content_len = int(self.headers.getheader('content-length', 0))
        post_body = self.rfile.read(content_len)
        fields = urlparse.parse_qs(post_body)
     	print fields
        #print len(fields)
        #print fields['clr']
        hp= map(int, fields['hp'])
        print hp
        rs= map(int, fields['rs'])
        fps= map(int, fields['fps'])
        clr= map(int, fields['clr'])
        wp= map(int, fields['wp'])
        command="lhe_config -f %d -c %d -h %d -w %d" %(fps[0], clr[0],hp[0],wp[0])
        print command
class main:
    def __init__(self):
 
        self.server = http_server()
 
if __name__ == '__main__':
    m = main()