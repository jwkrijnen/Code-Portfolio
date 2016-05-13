import os.path
import socket
import select
import time
import threading
import sys

from random import uniform
from python_example3 import RipPacket
from copy import deepcopy

UDP_IP = "127.0.0.1"
out_socket = None
triggered_update_blocking = False
triggered_update_queued = False

class Router():
    #TODO Docstring
    def __init__(self, ID, input_ports, outputs, timers):
            
        self.ID = ID 
        self.input_ports = input_ports #List of input ports
        self.outputs = outputs #Dictionary of neighbour information
        self.routing_table = {ID: {'Metric': 0,
                                   'NextHop': ID,
                                   'RouteChangeFlag': False,
                                   'Timer': 0}}
        self.scheduled_timer_period = timers[0]
        self.timeout_period = timers[1]
        self.garbage_collection_period = timers[2]
        self.garbage_bin = {}
        self.timers = {}

    #TODO: Functions to process routing table

'''Init Functions'''
#----------------------------------------------------------------------#
def readFile(file):
    lines = []
    with open(file) as open_file:
        for line in open_file:
            lines.append(line)
    return lines
            
def processConfigInfo(config_file):
    rawinput = readFile(config_file)
    processed_input = []
    for line in rawinput:
        processed_input.append(line.replace(",","").rstrip().split())
    
    if processed_input[0][0] != 'router-id':
        raise Exception, "Could not find router IDs"
    if processed_input[1][0] != 'input-ports':
        raise Exception, "Could not find input ports"     
    if processed_input[2][0] != 'outputs':
        raise Exception, "Could not find outputs"   
    
    router_id = int(processed_input[0][1])
    if router_id > 64000:
        raise Exception, "Router ID value is too large" 
    if router_id < 1:
        raise Exception, "Router ID value is too small"     
    
    router_input_ports = []
    for iport in processed_input[1][1:]:
        if int(iport) > 64000:
            raise Exception, "A router input port number is too large"
        if int(iport) < 1024:
            raise Exception, "A router input port number is too small"        
        router_input_ports.append(int(iport))
        
    router_outputs = {}
    for out in processed_input[2][1:]:
        neighbour_info = {}
        neighbour_list = out.split("-")
        if int(neighbour_list[0]) > 64000:
            raise Exception, "A router output port number is too large"
        if int(neighbour_list[0]) < 1024:
            raise Exception, "A router output port number is too small"
        neighbour_info['Outport'] = int(neighbour_list[0])
        if int(neighbour_list[1]) < 0:
            raise Exception, "A neighbour has a negative metric"        
        neighbour_info['Metric'] = int(neighbour_list[1])
        if int(neighbour_list[2]) > 64000:
            raise Exception, "A neighbour router ID value is too large" 
        if int(neighbour_list[2]) < 1:
            raise Exception, "A neighbour router ID value is too small"           
        router_outputs[int(neighbour_list[2])] = neighbour_info
        
    router_timers = []
    for timer in processed_input[3][1:]:
        router_timers.append(int(timer))
    if int(router_timers[0]) * 6 != int(router_timers[1]):
        raise Exception, "First or second timer value is incorrect" 
    if int(router_timers[0]) * 4 != int(router_timers[2]):
        raise Exception, "First or third timer value is incorrect"        
        

    this_router = Router(router_id, router_input_ports, router_outputs, router_timers)
    print(this_router.outputs)

    return this_router

def initialiseRouter():
    '''Get config file name from stdin'''
    file_read = False

    while not file_read:
        config_file = raw_input('Please enter config file name: ')
        try:
            open(config_file)
        except IOError:
            print("File doesn't exist, try again")
            continue
        else:
            router = processConfigInfo(config_file)
            file_read = True
    return router

def initialiseSockets(input_ports):
    '''Create sockets given from config file'''
    global UDP_IP
    sockets = []

    for port in input_ports:
        addr = (UDP_IP, port)
        input_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        input_socket.bind(addr)
        sockets.append(input_socket)

    return sockets


'''Packet Functions'''
#----------------------------------------------------------------------#
def processRipPacket(socket, router):
    data,addr = socket.recvfrom(8000)

    my_bytes = bytearray(data)
    packet = RipPacket()
    packet.fromBytes(my_bytes)

    router = updateRoutingTable(packet, router)
    for destinationID in router.routing_table: 
        resetTimeoutTimer(router, destinationID, packet)

    print("--------------------ROUTING TABLE UPDATED--------------------")
    print("Source: " + str(packet.routerID))
    for destination, information in router.routing_table.iteritems():
        # print("Destination: " + str(destination))
        print("Information: " + str(information))

    return router

'''Update Functions'''
#----------------------------------------------------------------------#
def resetTimeoutTimer(router, destinationID, packet):
    if router.routing_table[destinationID]['NextHop'] == packet.routerID:
        router.routing_table[destinationID]['Timer'] = 0

def scheduledUpdate(router):
    global out_socket
    print "Update going out now"

    timer_value = uniform(0.8, 1.2) * router.scheduled_timer_period

    timer = threading.Timer(timer_value, scheduledUpdate, [router])
    timer.daemon = True
    timer.start()

    for neighbour_id, neighbour_info in router.outputs.iteritems():
        temp_changes = {}
        for dest_id, dest_info in router.routing_table.iteritems():
            if dest_info['NextHop'] == neighbour_id:
                temp_changes[dest_id] = router.routing_table[dest_id]['Metric']
                router.routing_table[dest_id]['Metric'] = 16
                
        ripPacket = RipPacket().toBytes(router.ID, router.routing_table)
        out_socket.sendto(ripPacket, (UDP_IP, neighbour_info['Outport']))

        for dest_id, metric in temp_changes.iteritems():
            router.routing_table[dest_id]['Metric'] = metric

def triggeredUpdate(router):
    global out_socket
    global triggered_update_blocking
    update_info = {}

    triggered_update_queued = False

    print "Triggered update going out"
    for dest_id, dest_info in router.routing_table.iteritems():
        if dest_info['RouteChangeFlag'] == True:
            update_info[dest_id] = dest_info
            router.routing_table[dest_id]['RouteChangeFlag'] = False

    for neighbour_id, neighbour_info in router.outputs.iteritems():
        routing_info = dict((k, v) for k, v in update_info.iteritems())
        temp_changes = {}

        for dest_id, dest_info in update_info.iteritems():
            if dest_info['NextHop'] == neighbour_id:
                temp_changes[dest_id] = update_info[dest_id]['Metric']
                update_info[dest_id]['Metric'] = 16
        ripPacket = RipPacket().toBytes(router.ID, update_info)
        out_socket.sendto(ripPacket, (UDP_IP, neighbour_info['Outport']))

        for dest_id, metric in temp_changes.iteritems():
            update_info[dest_id]['Metric'] = metric

    timer_value = uniform(1, 5)
    timer = threading.Timer(timer_value, triggeredUpdateBlockingEnd, [router])
    timer.daemon = True
    timer.start()

    triggered_update_blocked = True

def triggeredUpdateBlockingEnd(router):
    global triggered_update_blocking
    global triggered_update_queued

    triggered_update_blocking = False

    if triggered_update_queued:
        triggeredUpdate(router)

#ENTRY POINT
def processTriggeredUpdate(router):
    global triggered_update_blocking
    global triggered_update_queued

    if triggered_update_blocking:
        triggered_update_queued = True
    else:
        triggeredUpdate(router)

'''Distance Vector Algorithm Functions'''
#----------------------------------------------------------------------#

def reuseGarbage(router, destinationID):
    router.garbage_bin[destinationID].cancel()
    router.garbage_bin.pop(destinationID, None)

def collectGarbage(router, destinationID):
    print "-------------------Collecting the Trash----------------------------"
    print destinationID
    router.garbage_bin.pop(destinationID, None)
    router.routing_table.pop(destinationID, None)

def addGarbage(router, destinationID):
    timer = threading.Timer(router.garbage_collection_period, collectGarbage, [router, destinationID])
    timer.daemon = True
    timer.start()
    router.garbage_bin[destinationID] = timer

def expire(destinationID, router):
    router.routing_table[destinationID]['Timer'] += 1
    #Start garbage collection timer
    if router.routing_table[destinationID]['Timer'] >= router.timeout_period:
        if destinationID not in router.garbage_bin:
            addGarbage(router, destinationID)
            router.routing_table[destinationID]['Metric'] = 16
            router.routing_table[destinationID]['RouteChangeFlag'] = True
            processTriggeredUpdate(router)
    else:
        timer = threading.Timer(1, expire, [destinationID, router])
        timer.daemon = True
        timer.start()
        router.timers[destinationID] = timer        

def shouldRouteChange(current_destinationID_metric, received_destinationID_metric, nexthop, originatingID):
    if (int(received_destinationID_metric) < int(current_destinationID_metric)):
        return True
    if (int(received_destinationID_metric) > int(current_destinationID_metric) and nexthop == originatingID):
        return True

def updateRoutingTable(rip_packet, router):
    originatingID = rip_packet.routerID
    
    for destinationID, cost in rip_packet.rtePayloads.iteritems():
        current_destinationID_metric = 16 #From the specification, if metric is > 16, use 16
        nexthop = None

        destinationID = int(destinationID)

        if (destinationID in router.routing_table):
            nexthop = router.routing_table[destinationID]['NextHop']
            current_destinationID_metric = int(router.routing_table[destinationID]['Metric'])

        received_destinationID_metric = min((int(router.outputs[originatingID]['Metric']) + int(cost)), 16)

        if (shouldRouteChange(current_destinationID_metric, received_destinationID_metric, nexthop, originatingID)):
            new_route = {'Metric': received_destinationID_metric,
                         'NextHop': originatingID,
                         'RouteChangeFlag': True,
                         'Timer': 0}
            if destinationID in router.garbage_bin and received_destinationID_metric != 16:
                reuseGarbage(router, destinationID)

                router.routing_table[destinationID] = new_route
                if destinationID in router.timers:
                    router.timers[destinationID].cancel()
                expire(destinationID, router)
            elif received_destinationID_metric == 16 and destinationID not in router.garbage_bin:
                addGarbage(router, destinationID)
                processTriggeredUpdate(router)   

                router.routing_table[destinationID] = new_route
                if destinationID in router.timers:
                    router.timers[destinationID].cancel()
                
            elif received_destinationID_metric != 16:
                router.routing_table[destinationID] = new_route
                if destinationID in router.timers:
                    router.timers[destinationID].cancel()
                expire(destinationID, router)

    return router

def main():
    #Initialise
    global out_socket
    router = initialiseRouter()
    in_sockets = initialiseSockets(router.input_ports)
    out_socket = in_sockets[0]

    scheduledUpdate(router)

    while True:
        try:
            readable, _, _ = select.select(in_sockets, [], [])
            for s in readable:
                if s in in_sockets:
                    router = processRipPacket(s, router)

        except select.error, v:
            continue

if __name__ == "__main__":
    main()
    
