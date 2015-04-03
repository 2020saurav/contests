import time
import numpy as np
# inputFile = 'assgn6_data_unif.txt'
vaFile = 'va-file'
# queryFile = 'assgn6_querysample_unif.txt'
inputFile = "small.in"
queryFile = "small.qr"
b = 0
d = 0
alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

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
			print response
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
