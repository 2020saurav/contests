class leaf
{
	private:
		int keyCount;
		double *key;
		string *ptr;
		string parent;
	public:
		leaf();
		void printToFile(string);
		void readFromFile(string);
		int getKeyCount();
		void setKeyCount(int);
		double* getKey(int);
		string getPtr(int);
		void setKey(int, double*);
		void setPtr(int, string);
		string getParent();
		void setParent(string);
		string getObjectContent(int);
		void printNode();
};

leaf::leaf()
{
	keyCount = 0;
	key = new double[M*d];
	ptr = new string[M];
	for(int i=0; i<M*d; i++)
		key[i] = 2.000000;		// 8 bytes
	for(int i=0; i<M; i++)
		ptr[i] = "unoccupied"; // 10 bytes
	parent = "unoccupied";
}

void leaf::printToFile(string filename)
{
	char buffer[MAX_DISK_PAGE_SIZE];
	sprintf(buffer,"%d\n",keyCount);
	for(int i=0; i<M*d; i++)
		sprintf(buffer + strlen(buffer), "%.6lf\t",key[i]);
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

void leaf::readFromFile(string filename)
{
	ifstream inputFile(filename);
	inputFile>>keyCount;
	for(int i=0; i<M*d; i++)
		inputFile>>key[i];
	for(int i=0; i<M; i++)
		inputFile>>ptr[i];
	inputFile>>parent;
}

void leaf::printNode()
{
	cout<<keyCount<<endl;
	for(int i=0; i<M*d; i++)
		cout<<fixed<<key[i]<<" ";
	cout<<endl;
	for(int i=0; i<M; i++)
		cout<<ptr[i]<<" ";
	cout<<endl;
	cout<<parent<<endl;
}

void leaf::setParent(string s)
{
	parent = s;
}

string leaf::getParent()
{
	return parent;
}

int leaf::getKeyCount()
{
	return keyCount;
}

void leaf::setKeyCount(int newCount)
{
	keyCount = newCount;
}

// array index access from caller will be 1-indexed
double * leaf::getKey(int n)
{
	double *k = new double[d];
	for(int i=0; i<d; i++)
		k[i] = key[(n-1)*d + i];
	return k;
}

string leaf::getPtr(int i)
{
	return ptr[i-1];
}

void leaf::setKey(int n, double* newKey)
{
	for(int i=0; i<d; i++)
		key[(n-1)*d + i] = newKey[i];
}

void leaf::setPtr(int i, string newPtr)
{
	ptr[i-1] = newPtr;
}

string leaf::getObjectContent(int i)
{
	ifstream inputFile(ptr[i-1]);
	string content;
	inputFile>>content;
	return content;
}

double* findMinKeyInLeaf(leaf n)
{
	double* minKey = new double[d];
	setValue(minKey, d, 1.000000);
	for(int child=1; child<=n.getKeyCount(); child++)
	{
		double* key = n.getKey(child);
		for(int dim=0; dim<d; dim++)
		{
			minKey[dim] = min(minKey[dim], key[dim]);
		}
	}
	return minKey;
}

double* findMaxKeyInLeaf(leaf n)
{
	double* maxKey = new double[d];
	setValue(maxKey, d, 0.000000);
	for(int child=1; child<=n.getKeyCount(); child++)
	{
		double* key = n.getKey(child);
		for(int dim=0; dim<d; dim++)
		{
			maxKey[dim] = max(maxKey[dim], key[dim]);
		}
	}
	return maxKey;
}