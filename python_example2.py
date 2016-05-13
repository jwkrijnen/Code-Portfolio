""" Josh Krijnen
    04/06/2015
    Graham Scan algorithm evaluation
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
    return x

#Finds coord with minimum y value
def findminycoord(coordlist):
    minycoord = coordlist[0]
    for coordpair in coordlist:
        if coordpair[1] < minycoord[1]:
            minycoord = coordpair
        elif coordpair[1] == minycoord[1]:
            if coordpair[0] > minycoord[0]:
                minycoord = coordpair
    return minycoord

#Used by isCCW to determine if a point is a right or left turn from line AB
def lineFn(ptA, ptB, ptC):
    return (ptB[0]-ptA[0])*(ptC[1]-ptA[1]) - (ptB[1]-ptA[1])*(ptC[0]-ptA[0])

#Determines if point C forms a counter clockwise turn from line AB
def isCCW(ptA, ptB, ptC):
    return lineFn(ptA, ptB, ptC) >= 0

#Sorts array by angle from starting point.
def sortbyangle(array,pos):
    array.remove(pos)
    for coord in array:
        coord.append(theta(pos,coord))
    sortedarray = sorted(array, key = lambda x: x[2])
    for coord in sortedarray:
        coord.remove(coord[2])
    sortedarray.insert(0,pos)
    return sortedarray

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

#Setup
def grahamscan(shape, n):
    if shape == "Square": 
        coordlist = generatesquare(n)
    elif shape == "Circle":
        coordlist = generatecircle(n)  
    originalcoordlist = coordlist[:]
    size = len(coordlist)-1
    minycoord = findminycoord(coordlist)
    coordlist = sortbyangle(coordlist,minycoord)
    
    #Initializing variables
    stack = []
    outputarray = []
    start_time = time.time()
    
    #Find graham scan convex hull
    for i in range(0,3):
        x = coordlist[0]
        stack.append(x)
        coordlist.remove(x)
    
    for i in range(0, size-3):
        while not isCCW(stack[-2],stack[-1],coordlist[i]):
            stack.pop()
        stack.append(coordlist[i])  
        
    
    #Formats output
    return ("--- %s point %s takes %s seconds ---" % (n, shape, (time.time() - start_time)))    
    
for i in range (1000,26000,1000):    
    print (grahamscan("Square", i))
for i in range (1000,26000,1000):    
    print (grahamscan("Circle", i)) 