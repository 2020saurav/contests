import time
inputFile = 'assgn6_data_unif.txt'
vaFile = 'va-file'
queryFile = 'assgn6_querysample_unif.txt'

def timer():
	return float("%.6f" % time.time())

def binpad(num, width):
	v = bin(num)
	v = v[2:]
	v = '0'*(width-len(v)) + v
	return v

def generateVAFile(source, destination, b, d):
	output = []
	lines = open(source).readlines()
	threshold = 1.0/(2**b)
	for line in lines:
		data =  line.split('\t')
		value = ''
		for index in range(0, d):
			i = 0
			while (i+1)*threshold < float(data[index]):
				i+=1
			value += binpad(i, b)
		output.append(value)
	f = open(destination,'w+')
	for item in output:
		print>>f, item

if __name__=="__main__":

	f = open('vafile.config').readlines()
	b = int(f[0])
	d = int(f[1])
	statsPointQuery = []
	statsRangeQuery = []
	statsKNNQuery = []


	generateVAFile(inputFile, vaFile, b, d)
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
			K = query[26]
			start = timer()
			response = kNNQuery(center, K)
			end = timer()
			statsRangeQuery.append(end-start)