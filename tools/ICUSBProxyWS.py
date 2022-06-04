#!/usr/bin/env python3

#
#  ICUSBProxyWS.py by @tobozo copyleft (c+) 2022
#
#  Scope: behaves like @armel's ICUSBProxy.py while providing some added features:
#
#    - Threading model
#      - Main thread = web server (legacy)
#      - PortsEnumerator = scans for changes on tty/COM connected devices
#      - Poll = runs subscriptions and publishes if needed
#    - Extended support for shared UART
#      - Ports enumeration
#      - Port sharing
#      - Persistence
#      - Concurrent access
#    - Inverted subscribe/publish data flow model
#      - M5 device subscribes via POST (devive, command, poll_delay, publish address)
#      - Proxy thread publishes to M5 device WebSocket when new data needs to be sent
#    - Demo/test mode based for IC705
#


import websocket
import random
import time
import json
import serial
import serial.tools.list_ports
import threading
import logging
from threading import Thread, Lock
from http.server import BaseHTTPRequestHandler, HTTPServer

name           = "ICUSBProxy"
version        = "0.1.0"
client_timeout = 0.01
server_verbose = 1
demo_mode      = 0 # use this when no IC is actually connected, will send dummy data

UARTS      = [] # UART's are shared between HTTP Server and WebSockets/Serial thread, but also across M5 Devices
uart_count = 0
M5Clients  = [] # M5Stack devices with registered subscriptions
connected_serial_ports = [] # currently connected COM/tty ports, repopulated every second
last_message = "" # to avoid repeated messages in the console




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


last_error = ''
def ConsolePrintError( e ):
    global last_error
    if last_error != e:
      print( e )
      last_error = e



last_message = ''
def ConsolePrintMessage( msg, error=None ):
    global last_message
    if error!=None:
      ConsolePrintError( error )
    if server_verbose == 1 and last_message != msg:
        print( msg )
        last_message = msg




class UART():
    def __init__(self):
        self.id     = None # unique id (incremented)
        self.tty    = None # tty name e.g. /dev/ttyUSBxx
        self.bauds  = None # baud rate e.g. 115200
        self.serial = None # serial instance, e.g. self.serial = serial.Serial(tty, bauds, timeout=client_timeout)
        self.mutex  = None # mutex for shared access, e.g. self.mutex = threading.Lock()



class M5Socket():
    def __init__(self):
        self.host          = None # Hostname/IP of the WS server on the M5Stack
        self.subscriptions = None # Array of subscriptions
        self.ws            = None # WebSocket instance, e.g. self.ws = websocket.WebSocket()



# Subscriptions are stored in in M5Socket().subscriptions as an array
class Subscription():
    def __init__(self):
        self.cmd           = None # CIV command, e.g. 'fefea4e01502fd'
        self.uart          = None # UART() shared object
        self.freq          = None # Polling frequency
        self.last_poll     = None # Last polling time
        self.last_response = None # Cache for last response




def initSerial( tty, bauds ):
    try:
        uart = serial.Serial(tty, bauds, timeout=client_timeout)
        return uart
    except Exception as e:
        ConsolePrintMessage(tty + " not reachable")
        return None



def GetCIVResponse( request, response_data ):
    response = ''
    for value in data:
        response += '{:02x}'.format(value)

    # test: 2 first bytes must match, bytes 3 and 4 are swapped
    if response[:4]!=request[:4] or response[4:2]!=request[6:2] or response[6:2]!=request[4:2]:
        ConsolePrintMessage( 'invalid packet')
    elif len(data) > 0 and data[-1] == 0xfd: # packet terminator
        return response
    else:
        ConsolePrintMessage( 'unterminated packet')
    return ''



def WSEmit( client, message ):
    try:
        client.ws.send( message )
    except Exception as e:
        ConsolePrintMessage("Reconnecting")
        try:
            client.ws.connect("ws://"+client.host+"/ws", origin="icusbproxy.local")
        except Exception as e:
            ConsolePrintMessage("WS Connection lost")






def Poll( subscription ):

    if subscription.uart.serial == None:
        ConsolePrintMessage( "opening uart" )
        try:
            subscription.uart.serial = initSerial(  subscription.uart.tty, subscription.uart.bauds )
        except Exception as e:
            ConsolePrintMessage("Opening UART failed")
            return False

    # previous attempt to connect to serial failed, no need to go further
    # TODO: add
    if subscription.uart.serial == None:
        return False

    try:
        packet = bytearray.fromhex(subscription.cmd)
        subscription.uart.serial.write( packet )
        data = subscription.uart.serial.read(size=16) # Set size to something high
        response = GetCIVResponse( subscription.cmd, data )
        if response == '' or response == None:
            ConsolePrintMessage(['Invalid data: ', data])
            return False
        else:
            if subscription.last_response != response:
                subscription.last_response = response
                ConsolePrintMessage(["New data: ", data])
                WSEmit( M5Client, response )
            return True
    except Exception as e:
        subscription.uart.serial = None
        ConsolePrintMessage("UART disconnected")
        WSEmit( M5Client, "UART DOWN" )
        return False




# daemon
def PortsEnumerator():
    global connected_serial_ports
    while True:
        tmp_ports = [tuple(p) for p in list(serial.tools.list_ports.comports())]
        if len(tmp_ports)>len(connected_serial_ports):
            new_device = list(set(tmp_ports) - set(connected_serial_ports))
            ConsolePrintMessage(["New Device(s): ", new_device] )
        elif len(tmp_ports)<len(connected_serial_ports):
            old_device = list(set(connected_serial_ports) - set(tmp_ports))
            ConsolePrintMessage(["Device(s) removed: ", old_device] )
        connected_serial_ports = tmp_ports
        time.sleep(1)







# daemon
def UARTPoller():
    while True:
        global M5Clients, UARTS, connected_serial_ports, uart_message
        sleep_time = 1
        for M5Client in M5Clients:
            for subscription in M5Client.subscriptions:
                sleep_time = min( subscription.freq, sleep_time )
                now = time.time()
                if subscription.uart.tty in connected_serial_ports:
                    if subscription.last_poll + subscription.freq < now:

                        # get mutex to avoid collision with the main thread
                        subscription.uart.mutex.acquire()

                        ConsolePrintMessage( "polling" )
                        if Poll( subscription ) != True:
                            ConsolePrintMessage("UART offline")
                            WSEmit( M5Client, "UART DOWN" )

                        subscription.last_poll = now

                        subscription.uart.mutex.release()

                        if subscription.freq == 0: # one time subscription self-deletes after use
                            M5Client.subscriptions.remove( subscription )
                else:
                    ConsolePrintMessage( subscription.uart.tty + " is not available" )
                    WSEmit( M5Client, "UART DOWN" )

        time.sleep( sleep_time ) # avoid loopbacks





class S(BaseHTTPRequestHandler):


    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def _set_error(self):
        self.send_response(500)
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
        try:
            jsonContent = json.loads( postContent )
        except Exception as e:
            ConsolePrintMessage(["Malformed json", postContent] );
            self._set_error()
            self.wfile.write("{}".format( "BAD JSON" ).encode('utf-8'))
            return

        sub = Subscription()
        sub.cmd   = jsonContent["command"]
        sub.freq  = float( jsonContent["polling_rate"] )
        M5Host    = jsonContent["host"]
        tty       = jsonContent["tty"]
        bauds     = jsonContent["baud_rate"]

        if M5Host == None or sub.cmd == None or tty == None or bauds == None or sub.freq == None:
            ConsolePrintMessage('Incomplete subscription')
            self._set_error()
            return

        for uart in UARTS:
            if uart.tty == tty and uart.bauds == bauds:
                sub.uart = uart

        if sub.uart == None:
            sub.uart        = UART()
            sub.uart.tty    = tty
            sub.uart.bauds  = bauds
            sub.uart.id     = uart_count
            sub.uart.mutex  = Lock()
            sub.uart.serial = None
            uart_count      = uart_count+1
            UARTS.append( sub.uart )

        sub.last_poll   = time.time()

        for client in M5Clients:
            if M5Host == client.host:
                M5Client = client

        if M5Client == None:
            M5Client = M5Socket()
            M5Client.host = M5Host
            M5Client.subscriptions = []
            M5Client.ws = websocket.WebSocket()
            M5Client.ws.connect("ws://"+M5Client.host+"/ws", origin="icusbproxy.local")
            M5Client.ws.send("ping")
            ConsolePrintMessage(M5Client.ws.recv())
            M5Clients.append( M5Client )


        if self.path == '/subscribe':
            for subscription in M5Client.subscriptions:
                if subscription.uart.id == sub.uart.id and subscription.cmd == sub.cmd:
                    #print("Subscription already exists!")
                    self._set_response()
                    self.wfile.write("{}".format(sub.uart.id).encode('utf-8'))
                    return
            M5Client.subscriptions.append( sub )
            ConsolePrintMessage("Added subscription")
            self._set_response()
            self.wfile.write("{}".format(sub.uart.id).encode('utf-8'))

        elif self.path == '/unsubscribe':
            for idx, subscription in enumerate(M5Client.subscriptions):
                if subscription.uart.id == sub.uart.id and subscription.cmd == sub.cmd:
                    self._set_response()
                    M5Client.subscriptions.remove( subscription )
                    ConsolePrintMessage("Removed subscription")
                    return
            ConsolePrintMessage("Subscription didn't exist!")
            self._set_error()

        else:
            ConsolePrintMessage("Unrecognized POST request: " + self.path )
            self._set_error()


    def do_GET(self):
        global uart_count
        if server_verbose > 1:
            logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))

        ConsolePrintMessage(["GET request Path: ", str(self.path)])

        # Init
        civ = str(self.path).split('=')

        response = ''
        request = civ[0]

        if(len(civ) == 2):
            civ = civ[1]

            #civ = 'fe,fe,a4,e0,00,56,34,12,07,00,fd,115200,/dev/ttyUSB2'
            #civ = 'fe,fe,a4,e0,03,fd,115200,/dev/ttyUSB2'

            civStr = civ
            civ = civ.split(',')

            client_serial   = civ.pop()
            client_baudrate = civ.pop()
            clt_address  = civ[2]

            if demo_mode > 0:
                self._set_response()
                civ_msg = ''
                # civ.shift() does not exist and civ.reverse().pop().pop().pop().pop().reverse() not possible with python lists :-(
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
                    ConsolePrintMessage('<< Received CIV packet: ' +civ_msg )
                    ConsolePrintMessage('>> Sending response: ' +response)
                self.wfile.write("{}".format( response ).encode('utf-8'))
                return


            try:
                usb = None

                # command to send
                command = []

                for value in civ:
                    command.append(int(value, 16))

                for uart in UARTS:
                    if uart.tty == client_serial and uart.bauds == client_baudrate:
                        usb = uart.serial

                if usb == None:
                    uart        = UART()
                    uart.tty    = client_serial
                    uart.bauds  = client_baudrate
                    uart.id     = uart_count
                    uart.mutex  = Lock()
                    uart.serial = serial.Serial(client_serial, client_baudrate, timeout=client_timeout)
                    last_poll   = time.time()
                    uart_count  = uart_count+1
                    UARTS.append( uart )
                    usb = uart.serial

                # get mutex to avoid collision with the websocket thread
                uart.mutex.acquire()

                usb.write(serial.to_bytes(command))
                data = usb.read(size=16) # Set size to something high

                uart.mutex.release()

                for value in data:
                    response += '{:02x}'.format(value)

                # Check if bad response
                if(response == "fefee0" + clt_address + "fafd"):
                    response = ''

                if server_verbose > 0:
                    ConsolePrintMessage('Serial device ' + client_serial + ' is up...')


            except Exception as e:
                ConsolePrintMessage('Serial device ' + client_serial + ' is down...', e)
                self._set_error()
                self.wfile.write("{}".format("UART DOWN").encode('utf-8'))
                return
        else:
            ConsolePrintMessage('Bad request ' + request)
            self._set_error()
            self.wfile.write("{}".format("BAD REQUEST").encode('utf-8'))
            return

        # End properly
        try:
            self._set_response()
            self.wfile.write("{}".format(response).encode('utf-8'))
        except Exception as e:
            ConsolePrintMessage("Empty response", e)
            self._set_error()
            self.wfile.write("{}".format("EMPTY RESPONSE").encode('utf-8'))

    def log_message(self, format, *args):
        return


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

    # close all open UARTS
    for uart in UARTS:
        if uart.serial != None:
            uart.serial.close()

    print('Stopping ' + name + ' v' + version + ' HTTPD...\n')



if __name__ == "__main__":

    # spawn a new thread to scan for serial devices with an active connection
    port_controller = threading.Thread(target=PortsEnumerator, name="PortsScan")
    port_controller.setDaemon(True)
    port_controller.start()

    # spawn a new thread and attach the UART Poller
    uart_poller = threading.Thread(target=UARTPoller, name='Daemon')
    uart_poller.setDaemon(True)
    uart_poller.start()

    # start the webserver on the main thread
    from sys import argv
    if len(argv) == 2:
        server_verbose = 0
        run(port=int(argv[1]))
    elif len(argv) == 3:
        server_verbose = int(argv[2])
        run(port=int(argv[1]))
    else:
        run()

