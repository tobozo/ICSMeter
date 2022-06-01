#!/usr/bin/env python3

from http.server import BaseHTTPRequestHandler, HTTPServer
import os, sys, socket
import websocket
import logging
import cgi
import serial
import random
import json
import threading
import time

name = "ICUSBProxy"
version = "0.1.0"
client_timeout = 0.01
server_verbose = 0
demo_mode = 1 # use this when no IC is actually connected, will send dummy data
uart_count = 0

def demo_response( clt_address, msg ):
    if   msg == '03fd':   # CIV_CHECK (ping)
        return 'fefee0' + clt_address + '030050973404fd' # pong !
    elif msg == '1c00fd': # CIV_GET_TX // Send/read the transceiver’s status (00=RX, 01=TX)
        #    1c00fd --> fefee0' + clt_address + '1c0000fd (TX OFF)
        #    1c00fd --> fefee0' + clt_address + '1c0001fd (TX ON)
        return 'fefee0' + clt_address + '1c0001fd'
    elif msg == 'e004fd': # CIV_GET_MOD
        #    e004fd --> fefee0' + clt_address + '040501fd (FIL1, FM)
        #    e004fd --> fefee0' + clt_address + '040502fd (FIL2, FM)
        #    e004fd --> fefee0' + clt_address + '040103fd (FIL3, USB)
        mod = random.choice(['040501fd', '040502fd', '040103fd'])
        return 'fefee0' + clt_address + mod
    elif msg == 'e003fd': # CIV_GET_FRQ
        #    e003fd --> fefee0' + clt_address + '030050973404fd (434.975.000)
        #    e003fd --> fefee0' + clt_address + '030000504501fd (145.500.000)
        freq =  '0300{0:0{1}x}fd'.format( int( random.uniform( 0x00504501, 0x50973404 ) ), 8 )
        return 'fefee0' + clt_address + freq
    elif msg == '1502fd': # CIV_GET_SMETER // Read S-meter level (0000=S0, 0120=S9, 0241=S9+60 dB)
        #    1502fd --> fefee0' + clt_address + '15020000fd (valeur du SMetre --> 0000)
        #    1502fd --> fefee0' + clt_address + '15020192fd (valeur du SMetre --> 0192)
        smeter = '15020{0:0{1}x}fd'.format( int( random.uniform( 0x00, 0x192 ) ), 3 )
        return 'fefee0' + clt_address + smeter
    elif msg == '1511fd': # CIV_GET_PWR // Read the Po meter level (0000=0% ~ 0143=50% ~ 0213=100%)
        #    1511fd --> fefee0' + clt_address + '15110000fd (valeur du PWR --> 0000)
        #    1511fd --> fefee0' + clt_address + '15110074fd (valeur du PWR --> 0074)
        pwr = '151100{0:0{1}x}fd'.format( int( random.uniform( 0x00, 0x74 ) ), 2 )
        return 'fefee0' + clt_address + pwr
    elif msg == '1512fd': # CIV_GET_SWR // Read SWR meter level (0000=SWR1.0, 0048=SWR1.5, 0080=SWR2.0, 0120=SWR3.0)
        #    1512fd --> fefee0' + clt_address + '15120000fd (valeur du SWR --> 0000)
        #    1512fd --> fefee0' + clt_address + '15120027fd (valeur du SWR --> 0027)
        swr = '151200{0:0{1}x}fd'.format( int( random.uniform( 0x00, 0x27 ) ), 2 )
        return 'fefee0' + clt_address + '15120000fd'
    elif msg == '1a06fd': # CIV_GET_DATA_MODE // Send/read the DATA mode setting
        #    1a06fd --> fefee0' + clt_address + '1a060000fd (Mode Data désacté)
        #    1a06fd --> fefee0' + clt_address + '1a060101fd (Mod Data activé)
        return 'fefee0' + clt_address + '1a060101fd'
    else:
        return '??????fd'

class UART():
    def __init__(self):
        self.id     = None
        self.tty    = None # /dev/ttyUSBxx
        self.bauds  = None # 115200
        self.serial = None # serial.Serial(tty, bauds, timeout=client_timeout)

class Subscription():
    def __init__(self):
        self.cmd   = None # CIV command
        self.freq  = None # Polling frequency
        self.uart  = None # UART() object
        self.last_poll     = None
        self.last_response = None


class M5Socket():
    def __init__(self):
        self.host          = None
        self.subscriptions = None
        self.ws            = None

M5Clients = []
UARTS     = []




def initSerial( tty, bauds ):
    try:
        uart = serial.Serial(tty, bauds, timeout=client_timeout)
        return uart
    except:
        print(tty + " not reachable")
        return None



def UARTPoller():
    while True:
        global M5Clients, UARTS
        for M5Client in M5Clients:
            for idx, subscription in enumerate(M5Client.subscriptions):
                now = time.time()
                if( subscription.last_poll + int(subscription.freq) < int(now) ):
                    print( "polling" )
                    subscription.last_poll = now
                    if subscription.uart.serial == None:
                        print( "opening uart" )
                        subscription.uart.serial = initSerial(  subscription.uart.tty, subscription.uart.bauds )
                    if subscription.uart.serial != None:
                        print( "Running cmd " + subscription.cmd )
                        try:
                            packet = bytearray.fromhex(subscription.cmd)
                            subscription.uart.serial.write( packet )
                            data = subscription.uart.serial.read(size=16) # Set size to something high
                            response = ''
                            for value in data:
                                response += '{:02x}'.format(value)
                            if response != subscription.last_response:
                                subscription.last_response = response
                                is_valid_packet = True
                                if response[:4]!=subscription.cmd[:4] or response[5:1]!=subscription.cmd[6:1] or response[6:1]==subscription.cmd[5:1]:
                                    is_valid_packet = False
                                elif len(data) > 0 and data[-1] == 0xfd:
                                    is_valid_packet = True
                                else:
                                    is_valid_packet = False

                                if is_valid_packet == True:
                                    print( "new data: ", data )
                                    M5Client.ws.send( response )

                        except:
                            print("UART disconnected")
                            M5Client.ws.send( "disconnected" )
                    else:
                        print("UART offline")
                        M5Client.ws.send( "offline" )

                    if subscription.freq == 0: # one time subscription self-deletes after use
                        M5Client.subscriptions.remove(idx)
                #else:
                    #print( subscription.last_poll, int(subscription.freq), int(now) )
        time.sleep(1) # avoid loopbacks
        # print( len( M5Clients ) )



class S(BaseHTTPRequestHandler):

    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def _set_error(self):
        self.send_response(404)
        self.send_header('Content-type', 'text/html')
        self.end_headers()


    def do_OPTIONS(self):
        # handle CORS requests
        self.send_response(200, "ok")
        self.send_header('Access-Control-Allow-Origin', self.headers['Origin'])
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header("Access-Control-Allow-Headers", "X-Requested-With")

    def do_POST(self):

        global uart_count

        M5Client = None
        ICUART   = None

        varLen = int(self.headers['Content-Length'])
        postContent = self.rfile.read(varLen)
        jsonContent = json.loads( postContent )

        sub = Subscription()
        sub.cmd   = jsonContent["command"]
        sub.freq  = jsonContent["polling_rate"]
        M5Host    = jsonContent["host"]
        tty       = jsonContent["tty"]
        bauds     = jsonContent["baud_rate"]

        if M5Host == None or sub.cmd == None or tty == None or bauds == None or sub.freq == None:
            print('Incomplete subscription')
            self._set_error()
            return

        for uart in UARTS:
            if uart.tty == tty and uart.bauds == bauds:
                sub.uart = uart

        if sub.uart == None:
            sub.uart       = UART()
            sub.uart.tty   = tty
            sub.uart.bauds = bauds
            sub.uart.id    = uart_count
            sub.last_poll  = time.time()
            uart_count     = uart_count+1
            UARTS.append( sub.uart )

        for client in M5Clients:
            if M5Host == client.host:
                M5Client = client

        if M5Client == None:
            M5Client = M5Socket()
            M5Client.host = M5Host
            M5Client.subscriptions = []
            M5Client.ws = websocket.WebSocket()
            M5Client.ws.connect("ws://"+M5Host+"/ws", origin="icusbproxy.local")
            M5Client.ws.send("ping")
            print(M5Client.ws.recv())
            M5Clients.append( M5Client )


        if self.path == '/subscribe':
            found = False
            for subscription in M5Client.subscriptions:
                if subscription.uart.id == sub.uart.id and subscription.cmd == sub.cmd:
                    found = True
                    break
            if found == False:
                self._set_response()
                M5Client.subscriptions.append( sub )
                print("Added subscription")
                return
            print("Subscription already exists!")
            self._set_error()

        elif self.path == '/unsubscribe':
            for idx, subscription in enumerate(M5Client.subscriptions):
                if subscription.uart.id == sub.uart.id and subscription.cmd == sub.cmd:
                    self._set_response()
                    M5Client.ws.close()
                    M5Client.subscriptions.uart.serial.close()
                    M5Client.subscriptions.remove( idx )
                    print("Removed subscription")
                    return
            print("Subscription didn't exist!")
            self._set_error()

        else:
            print("Unrecognized POST request: " + self.path )
            self._set_error()


    def do_GET(self):
        civ = str(self.path).split('=')
        response = ''
        request = civ[0]

        if(len(civ) == 2):
            civ = civ[1]

            civStr = civ
            civ = civ.split(',')

            client_serial   = civ.pop()
            client_baudrate = civ.pop()
            clt_address  = civ[2]

            self._set_response()
            civ_msg = ''
            civ.reverse()
            civ.pop()
            civ.pop()
            civ.pop()
            civ.pop()
            civ.reverse()

            for value in civ:
                civ_msg += '{:02x}'.format(int(value, 16))

            response = demo_response( clt_address, civ_msg )

            if server_verbose > 0:
                print('<< Received CIV packet: ' +civ_msg )
                print('>> Sending response: ' +response)
            self.wfile.write("{}".format( response ).encode('utf-8'))

        else:
            if server_verbose > 0:
                print('Bad request ' + request)
            self._set_error()
            self.wfile.write("{}".format("BAD REQUEST").encode('utf-8'))



def run(server_class=HTTPServer, handler_class=S, port=1234):
    if server_verbose > 1:
        logging.basicConfig(level=logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print('Starting ' + name + ' v' + version + ' HTTPD on Port', port)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print('Stopping ' + name + ' v' + version + ' HTTPD...\n')


if __name__ == "__main__":

    d = threading.Thread(target=UARTPoller, name='Daemon')
    d.setDaemon(True)
    d.start()

    from sys import argv

    if len(argv) == 2:
        server_verbose = 0
        run(port=int(argv[1]))
    elif len(argv) == 3:
        server_verbose = int(argv[2])
        run(port=int(argv[1]))
    else:
        run()

