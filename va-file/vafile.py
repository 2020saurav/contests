import time
import numpy as np
import math
import Queue
import sys

vaFile = 'va-file'
inputFile = 'assgn6_data_exp.txt'
queryFile = 'assgn6_querysample_exp.txt'
# inputFile = "small.in"
# queryFile = "small.qr"
b = 0
d = 0
pageSize = 2048
RAM = 2.0
inRAM = True
N = 0
alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
vaFileLines = '' # use when data is too large
vaFileArray = [] # use when data is too small

def getVAFileContent(lineno):
	if inRAM:
		return vaFileArray[lineno]
	else:
		line = vaFileLines[lineno].strip()
		return line

def base62_encode(num):
	# Base dec(10) num to dec(62) string
	if (num == 0):
		return alphabet[0]
	arr = []
	base = len(alphabet)
	while num:
		rem = num % base
		num = num // base
		arr.append(alphabet[rem])
	arr.reverse()
	return ''.join(arr)

def base62_decode(string):
	# Base dec(62) string to dec(10) num
	base = len(alphabet)
	strlen = len(string)
	num = 0
	idx = 0
	for char in string:
		power = (strlen - (idx + 1))
		num += alphabet.index(char) * (base ** power)
		idx += 1
	return num

def chunkstring(string, length):
	return list((string[0+i:length+i] for i in range(0, len(string), length)))

def timer():
	return float("%.6f" % time.time())

def binpad(num, width):
	# converts number to equivalent binary string of req width by padding
	v = bin(num)
	v = v[2:] # remove '0b'
	v = '0'*(width-len(v)) + v
	return v

def getQuantum(data, threshold):
	value = ''
	for index in range(0, d):
		i = 0
		while (i+1)*threshold < float(data[index]):
			i+=1
		value += binpad(i, b)
	value = base62_encode(int(value,2))
	return value

def generateVAFile(source, destination):
	output = []
	lines = open(source).readlines()
	threshold = 1.0/(2**b)
	for line in lines:
		data =  line.split('\t')
		value = getQuantum(data, threshold)
		output.append(value)
	f = open(destination,'w+')
	for item in output:
		print>>f, item

def pointQuery(point):
	threshold = 1.0/(2**b)
	quantizedValue = getQuantum(point, threshold)
	filterList = []
	index = 0
	for lineno in range(0, N):
		line = getVAFileContent(lineno)
		if line == quantizedValue:
			filterList.append(index)
		index += 1
	# refine
	lines = open(inputFile).readlines()
	refinedList = []
	for index in filterList:
		line = lines[index].strip()
		line = line.split('\t')
		actualPoint = line[:d]
		if actualPoint == point:
			refinedList.append(line)
	return refinedList			

def minDistance(x, y):
	dist = 0
	for i in range(0, len(x)):
		dist = dist + ((x[i] - y[i]) * (x[i] - y[i]))
	return math.sqrt(dist) - 1
	# Loose lower bound on this distance using triangle inequality

def actualDistance(x, y):
	dist = 0
	for i in range(0, len(x)):
		dist = dist + ((float(x[i]) - float(y[i])) * (float(x[i]) - float(y[i])))
	return math.sqrt(dist)

def rangeQuery(center, radius):
	scaledRadius = radius * (2**b)
	threshold = 1.0/(2**b)
	qCenter = getQuantum(center, threshold)
	qCenter = binpad(base62_decode(qCenter), d*b)
	qcArray = chunkstring(qCenter, b)
	qcArray = [int(x,2) for x in qcArray]
	filterList = []
	index = 0
	# filter
	for lineno in range(0, N):
		line = getVAFileContent(lineno)
		point = binpad(base62_decode(line), d*b)
		pointArray = chunkstring(point, b)
		pointArray = [int(x,2) for x in pointArray]
		if minDistance(qcArray, pointArray) <= scaledRadius:
			filterList.append(index)
		index += 1
	# refine
	lines = open(inputFile).readlines()
	refinedList = []
	for index in filterList:
		line = lines[index].strip()
		line = line.split('\t')
		point = line[:d]
		if actualDistance(center, point) < radius:
			refinedList.append(line)
	return refinedList

class maxPQ(object):
	def __init__(self, k=0):
		self.Q = Queue.PriorityQueue(k)
	def push(self, priority, value):
		self.Q.put((-priority, value))
	def size(self):
		return self.Q.qsize()
	def pop(self):
		return self.Q.get()
	def empty(self):
		return self.Q.empty()

def kNNQuery(center, k):
	q = maxPQ() # max priority queue
	threshold = 1.0/(2**b)
	qCenter = getQuantum(center, threshold)
	qCenter = binpad(base62_decode(qCenter), d*b)
	qcArray = chunkstring(qCenter, b)
	qcArray = [int(x,2) for x in qcArray]
	index = 0
	# filter
	for lineno in range(0, N):
		line = getVAFileContent(lineno)
		point = binpad(base62_decode(line), d*b)
		pointArray = chunkstring(point, b)
		pointArray = [int(x,2) for x in pointArray]
		distance = actualDistance(qcArray, pointArray)

		if q.size() < k:
			q.push(distance, index)
		else:
			top = q.pop()
			if top[0] == -distance:
				q.push(distance, top[1])
				q.push(distance, index)
			else:
				if -top[0] < distance:
					q.push(-top[0], top[1])
				else:
					q.push(distance, index)
		index += 1
	filterList = []
	while not q.empty():
		top = q.pop()
		filterList.append(top[1])

	r = maxPQ()
	lines = open(inputFile).readlines()
	refinedList = []
	for index in filterList:
		line = lines[index].strip()
		line = line.split('\t')
		point = line[:d]
		r.push(actualDistance(center, point), line)
	k = min(k, r.size())
	for i in range(0, k):
		refinedList.append(r.pop()[1])
	return refinedList

def printArray(array):
	for line in array:
		for data in line:
			print data,
		print
	print

def printQuery(query):
	for data in query:
		print data,
	print

def runQuery(printStats = False):
	statsPointQuery = []
	statsRangeQuery = []
	statsKNNQuery = []
	lines = open(queryFile).readlines()
	for line in lines:
		query = line.strip().split('\t')
		
		if query[0] == '1':
			# Point Query
			point = query[1:d+1]
			start = timer()
			response = pointQuery(point)
			end = timer()
			printQuery(query)
			printArray(response)
			statsPointQuery.append(end-start)
		
		elif query[0] == '2':
			# Range Query
			center = query[1:d+1]
			radius = float(query[d+1])
			start = timer()
			response = rangeQuery(center, radius)
			end = timer()
			printQuery(query)
			printArray(response)
			statsRangeQuery.append(end-start)

		elif query[0] == '3':
			# KNN Query
			center = query[1:d+1]
			k = int(query[d+1])
			start = timer()
			response = kNNQuery(center, k)
			end = timer()
			printQuery(query)
			printArray(response)
			statsKNNQuery.append(end-start)

	if printStats:
		print "\nPOINT QUERY\n------------------"
		print "Maximum: \t", np.amax(statsPointQuery)
		print "Minimum: \t", np.amin(statsPointQuery)
		print "Mean: \t\t",  np.mean(statsPointQuery)
		print "Std Dev: \t", np.std( statsPointQuery)
		print "\nRANGE QUERY\n------------------"
		print "Maximum: \t", np.amax(statsRangeQuery)
		print "Minimum: \t", np.amin(statsRangeQuery)
		print "Mean: \t\t",  np.mean(statsRangeQuery)
		print "Std Dev: \t", np.std( statsRangeQuery)
		print "\nKNN QUERY\n------------------"
		print "Maximum: \t", np.amax(statsKNNQuery)
		print "Minimum: \t", np.amin(statsKNNQuery)
		print "Mean: \t\t",  np.mean(statsKNNQuery)
		print "Std Dev: \t", np.std( statsKNNQuery)

if __name__=="__main__":
	f = open('vafile.config').readlines()
	b = int(f[0])
	d = int(f[1])
	pageSize = int(f[2])
	RAM = float(f[3])*(2**30)
	generateVAFile(inputFile, vaFile)
	vaFileLines = open(vaFile).readlines()
	N = len(vaFileLines)
	size = 0
	lineno = 0
	for line in vaFileLines:
		line = line.strip()
		size = max(size, sys.getsizeof(line))
		lineno += 1
	if RAM >= size*lineno: # Threshold = 0.00042 for my system of encoding VA File
		print "Loading in RAM"
		inRAM = True
		for line in vaFileLines:
			line = line.strip()
			vaFileArray.append(line)
	else:
		print "Can't load in RAM"
		inRAM = False
	runQuery(printStats = True)
