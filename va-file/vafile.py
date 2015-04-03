import time
import numpy as np
inputFile = 'assgn6_data_unif.txt'
vaFile = 'va-file'
queryFile = 'assgn6_querysample_unif.txt'
# inputFile = "small.in"
# queryFile = "small.qr"
b = 0
d = 0

def timer():
	return float("%.6f" % time.time())

def binpad(num, width):
	v = bin(num)
	v = v[2:]
	v = '0'*(width-len(v)) + v
	# TODO swtich to higher base here
	return v

def getQuantum(data, threshold):
	value = ''
	for index in range(0, d):
		i = 0
		while (i+1)*threshold < float(data[index]):
			i+=1
		value += binpad(i, b)
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

# def in

def pointQuery(point):
	threshold = 1.0/(2**b)
	quantizedValue = getQuantum(point, threshold)
	filterList = []
	lines = open(vaFile).readlines()
	index = 0
	for line in lines:
		line = line.strip()
		if line == quantizedValue:
			filterList.append(index)
		index += 1
	# refine
	lines = open(inputFile).readlines()
	refinedList = []
	for data in filterList:
		line = lines[data].strip()
		# print line
		line = line.split('\t')
		actualPoint = line[:d]
		if actualPoint == point:
			refinedList.append(line)
	return refinedList			

def rangeQuery(center, radius):
	pass


def kNNQuery(center, k):
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
			point = query[1:]
			start = timer()
			response = pointQuery(point)
			end = timer()
			statsPointQuery.append(end-start)
		
		elif query[0] == '2':
			# Range Query
			center = query[1:26]
			radius = query[26]
			start = timer()
			response = rangeQuery(center, radius)
			end = timer()
			statsRangeQuery.append(end-start)
		
		elif query[0] == '3':
			# KNN Query
			center = query[1:26]
			k = query[26]
			start = timer()
			response = kNNQuery(center, k)
			end = timer()
			statsRangeQuery.append(end-start)

if __name__=="__main__":
	f = open('vafile.config').readlines()
	b = int(f[0])
	d = int(f[1])
	generateVAFile(inputFile, vaFile)
	runQuery(queryFile)
