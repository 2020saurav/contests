import time
import numpy as np
import math
vaFile = 'va-file'
inputFile = 'assgn6_data_unif.txt'
queryFile = 'assgn6_querysample_unif.txt'
# inputFile = "small.in"
# queryFile = "small.qr"
b = 0
d = 0
alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
vaFileLines = ''
# To keep this structure in RAM

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
	for line in vaFileLines:
		line = line.strip()
		if line == quantizedValue:
			filterList.append(index)
		index += 1
	# refine
	lines = open(inputFile).readlines()
	refinedList = []
	for data in filterList:
		line = lines[data].strip()
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
		dist = dist + ((x[i] - y[i]) * (x[i] - y[i]))
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
	for line in vaFileLines:
		line = line.strip()
		point = binpad(base62_decode(line), d*b)
		pointArray = chunkstring(point, b)
		pointArray = [int(x,2) for x in pointArray]
		if minDistance(qcArray, pointArray) <= scaledRadius:
			filterList.append(index)
		index += 1
	# refine
	lines = open(inputFile).readlines()
	refinedList = []
	for data in filterList:
		line = lines[data].strip()
		line = line.split('\t')
		point = line[:d]
		if actualDistance(center, point) < radius:
			refinedList.append(line)
	return refinedList


def kNNQuery(center, k):
	# use priority queue
	pass

def runQuery(queryFile):
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
			if len(response) > 0:
				print response
			end = timer()
			statsPointQuery.append(end-start)
		
		elif query[0] == '2':
			# Range Query
			center = query[1:d+1]
			radius = float(query[d+1])
			start = timer()
			response = rangeQuery(center, radius)
			end = timer()
			statsRangeQuery.append(end-start)
		
		elif query[0] == '3':
			# KNN Query
			center = query[1:d+1]
			k = query[d+1]
			start = timer()
			response = kNNQuery(center, k)
			end = timer()
			statsRangeQuery.append(end-start)

if __name__=="__main__":
	f = open('vafile.config').readlines()
	b = int(f[0])
	d = int(f[1])
	generateVAFile(inputFile, vaFile)
	vaFileLines = open(vaFile).readlines()
	runQuery(queryFile)
