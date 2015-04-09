import time
import numpy as np
import math
import Queue
import sys

inputFile = 'assgn6_data_exp.txt'
queryFile = 'assgn6_querysample_exp.txt'
# inputFile = "small.in"
# queryFile = "small.qr"
d = 0

def timer():
	return float("%.6f" % time.time())

def pointQuery(point):
	lines = open(inputFile).readlines()
	N = len(lines)
	refinedList = []
	for index in range(0, N):
		line = lines[index].strip()
		line = line.split('\t')
		actualPoint = line[:d]
		if actualPoint == point:
			refinedList.append(line)
	return refinedList			

def actualDistance(x, y):
	dist = 0
	for i in range(0, len(x)):
		dist = dist + ((float(x[i]) - float(y[i])) * (float(x[i]) - float(y[i])))
	return math.sqrt(dist)

def rangeQuery(center, radius):
	lines = open(inputFile).readlines()
	N = len(lines)
	refinedList = []
	for index in range(0, N):
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
	r = maxPQ()
	lines = open(inputFile).readlines()
	N = len(lines)
	refinedList = []
	for index in range(0, N):
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
	d = int(f[1])
	runQuery(printStats = True)
