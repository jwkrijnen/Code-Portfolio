""" Josh Krijnen
    04/06/2015
    Gift Wrapping algorithm evaluation
"""

import random
import sys
import time

#Calculates angle between two lines (one between point a and b, one horizontal from a)
def theta(pointa, pointb):
    dx = pointb[0] - pointa[0]
    dy = pointb[1] - pointa[1]
    if abs(dx) < 1.e-6 and abs(dy) < 1.e-6:
        t = 0
    else:
        t = dy/(abs(dx) + abs(dy))
    if dx < 0:
        t = 2 - t
    elif dy < 0:
        t = 4 + t
    x = t*90
    if x == 0:
        x = 360
    return float(x)

#Swaps two coordinates in a list    
def swapcoords(coordlist, indexa, indexb):
    temp = coordlist[indexb]
    coordlist[indexb] = coordlist[indexa]
    coordlist[indexa] = temp 
    return coordlist

#Finds coord with minimum y value
def findminycoord(coordlist):
    minycoord = coordlist[0]
    for coordpair in coordlist:
        if coordpair[1] < minycoord[1]:
            minycoord = coordpair
        elif coordpair[1] == minycoord[1]:
            if coordpair[0] > minycoord[0]:
                minycoord = coordpair
    minyindex = coordlist.index(minycoord)
    coordlist.append(minycoord)
    swapcoords(coordlist,0,minyindex)
    return minycoord

#Generates a random coordinates in the shape of a square.
def generatesquare(reps):
    square = []
    for i in range(0, reps):
        x = random.randint(0,999)
        y = random.randint(0,999)
        square.append([x,y])
    return square

#Generates a random coordinates in the shape of a circle.
def generatecircle(reps):
    circle = []
    while len(circle) < reps:
        x = random.randint(0,999)
        y = random.randint(0,999)
        if ((x-500)**2) + ((y-500)**2) < 250000:
            circle.append([x,y])
    return circle

def giftwrap(shape, n):
    #Setup
    if shape == "Square": 
        coordlist = generatesquare(n)
    elif shape == "Circle":
        coordlist = generatecircle(n)    
    originalcoordlist = coordlist[:]
    findminycoord(coordlist)
    size = len(coordlist)-1
    
    #Initializing variables
    currentpoint = 0
    pastminangle = 0
    count = 0
    total = 0
    pasttotal = 0
    returnarray = []
    outputarray = []
    start_time = time.time()
    
    #Finds giftwrap convex hull
    while currentpoint < size and count < size:
        minAngle = 361
        for coord in range(count+1, size+1):
            angle = theta(coordlist[count], coordlist[coord])
            if angle < minAngle and angle > pastminangle and coordlist[coord] != coordlist[count]:
                currentpoint = coord
                minAngle = angle
                total = coordlist[coord][0] + coordlist[coord][1]
            elif angle == minAngle and angle > pastminangle and coordlist[coord] != coordlist[count]: #If 2 points are on the same line, finds whcih is further away
                if abs(total-pasttotal) < abs((coordlist[coord][0]+coordlist[coord][1])-pasttotal):
                    currentpoint = coord
                    minAngle = angle
                    total = coordlist[coord][0] + coordlist[coord][1]                
        count += 1
        coordlist = swapcoords(coordlist, count, currentpoint)
        pasttotal = total
        pastminangle = minAngle
    
    #Formats output
    return ("--- %s point %s takes %s seconds ---" % (n, shape, (time.time() - start_time)))

for i in range (1000,26000,1000):    
    print (giftwrap("Square", i))
for i in range (1000,26000,1000):    
    print (giftwrap("Circle", i))    


