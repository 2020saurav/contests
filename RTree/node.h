class node
{
	private:
		int keyCount;
		double *minKey;
		double *maxKey;
		string *ptr;
		string parent;
	public:
		node();
		void printToFile(string);
		void readFromFile(string);
		int getKeyCount();
		void setKeyCount(int);
		double* getMinKey(int);
		double* getMaxKey(int);
		string getPtr(int);
		void setMinKey(int, double*);
		void setMaxKey(int, double*);
		void setPtr(int, string);
		string getParent();
		void setParent(string);
		void printNode();
};

node::node()
{
	keyCount = 0;
	minKey = new double[M*d];
	maxKey = new double[M*d];
	ptr = new string[M];
	for(int i=0; i<M*d; i++)
	{
		minKey[i] = 2.000000;		// 8 bytes
		maxKey[i] = 2.000000;		// 8 bytes
	}
	for(int i=0; i<M; i++)
		ptr[i] = "unoccupied"; // 10 bytes
	parent = "unoccupied";
}

void node::setParent(string s)
{
	parent = s;
}

string node::getParent()
{
	return parent;
}

int node::getKeyCount()
{
	return keyCount;
}

void node::setKeyCount(int newCount)
{
	keyCount = newCount;
}

double* node::getMinKey(int n)
{
	double* k = new double[d];
	for(int i=0; i<d; i++)
		k[i] = minKey[(n-1)*d + i];
	return k;
}

double* node::getMaxKey(int n)
{
	double* k = new double[d];
	for(int i=0; i<d; i++)
		k[i] = maxKey[(n-1)*d + i];
	return k;
}

string node::getPtr(int i)
{
	return ptr[i-1];
}

void node::setMinKey(int n, double* newKey)
{
	for(int i=0; i<d; i++)
		minKey[(n-1)*d + i] = newKey[i];
}

void node::setMaxKey(int n, double* newKey)
{
	for(int i=0; i<d; i++)
		maxKey[(n-1)*d + i] = newKey[i];
}

void node::setPtr(int i, string newPtr)
{
	ptr[i-1] = newPtr;
}

void node::printToFile(string filename)
{
	char buffer[MAX_DISK_PAGE_SIZE];
	sprintf(buffer,"%d\n",keyCount);
	for(int i=0; i<M*d; i++)
		sprintf(buffer + strlen(buffer), "%.6lf\t",minKey[i]);
	sprintf(buffer+strlen(buffer),"\n");
	
	for(int i=0; i<M*d; i++)
		sprintf(buffer + strlen(buffer), "%.6lf\t",maxKey[i]);
	sprintf(buffer+strlen(buffer),"\n");
	
	for(int i=0; i<M; i++)
		sprintf(buffer+strlen(buffer), "%s\t", ptr[i].c_str());
	sprintf(buffer+strlen(buffer),"\n");
	
	sprintf(buffer+strlen(buffer),"%s\n",parent.c_str());

	FILE *outFile;
	outFile = fopen(filename.c_str(),"wb"); // BINARY MODE
	fwrite(buffer,1,diskPageSize,outFile);
	fclose(outFile);
}

void node::readFromFile(string filename)
{
	ifstream inputFile(filename);
	inputFile>>keyCount;
	for(int i=0; i<M*d; i++)
		inputFile>>minKey[i];
	for(int i=0; i<M*d; i++)
		inputFile>>maxKey[i];
	for(int i=0; i<M; i++)
		inputFile>>ptr[i];
	inputFile>>parent;
}

void node::printNode()
{
	cout<<keyCount<<endl;
	for(int i=0; i<M*d; i++)
		cout<<fixed<<minKey[i]<<" ";
	cout<<endl;	
	for(int i=0; i<M*d; i++)
		cout<<fixed<<maxKey[i]<<" ";
	cout<<endl;
	for(int i=0; i<M; i++)
		cout<<ptr[i]<<" ";
	cout<<endl;
	cout<<parent<<endl;
}

double* findMinKeyInNode(node n)
{
	double* minKey = new double[d];
	setValue(minKey, d, 1.000000);
	for(int child=1; child<=n.getKeyCount(); child++)
	{
		double* key = n.getMinKey(child);
		for(int dim=0; dim<d; dim++)
		{
			minKey[dim] = min(minKey[dim], key[dim]);
		}
	}
	return minKey;
}

double* findMaxKeyInNode(node n)
{
	double* maxKey = new double[d];
	setValue(maxKey, d, 0.000000);
	for(int child=1; child<=n.getKeyCount(); child++)
	{
		double* key = n.getMaxKey(child);
		for(int dim=0; dim<d; dim++)
		{
			maxKey[dim] = max(maxKey[dim], key[dim]);
		}
	}
	return maxKey;
}