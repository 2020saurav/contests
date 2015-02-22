class leaf
{
	private:
		int keyCount;
		double *key;
		string *ptr;
		string parent;
		string left;
		string right;
	public:
		leaf();
		void printToFile(string);
		void readFromFile(string);
		int getKeyCount();
		void setKeyCount(int);
		double getKey(int);
		string getPtr(int);
		void setKey(int, double);
		void setPtr(int, string);
		string getParent();
		void setParent(string);
		void setLeft(string);
		void setRight(string);
		string getLeft();
		string getRight();
		string getObjectContent(int);
		void printNode();
};

leaf::leaf()
{
	keyCount = 0;
	key = new double[M];
	ptr = new string[M];
	for(int i=0; i<M; i++)
		key[i] = 2.000000;		// 8 bytes
	for(int i=0; i<M; i++)
		ptr[i] = "unoccupied"; // 10 bytes
	parent = "unoccupied";
	left = "unoccupied";
	right = "unoccupied";
}

void leaf::setLeft(string s)
{
	left = s;
}
void leaf::setRight(string s)
{
	right = s;
}
string leaf::getRight()
{
	return right;
}
string leaf::getLeft()
{
	return left;
}

void leaf::printToFile(string filename)
{
	char buffer[MAX_DISK_PAGE_SIZE];
	sprintf(buffer,"%d\n",keyCount);
	for(int i=0; i<M; i++)
		sprintf(buffer + strlen(buffer), "%.6lf\t",key[i]);
	sprintf(buffer+strlen(buffer),"\n");
	for(int i=0; i<M; i++)
		sprintf(buffer+strlen(buffer), "%s\t", ptr[i].c_str());
	sprintf(buffer+strlen(buffer),"\n");
	sprintf(buffer+strlen(buffer),"%s\n",parent.c_str());
	sprintf(buffer+strlen(buffer),"%s\n",left.c_str());
	sprintf(buffer+strlen(buffer),"%s\n",right.c_str());

	FILE *outFile;
	outFile = fopen(filename.c_str(),"wb"); // BINARY MODE
	fwrite(buffer,1,diskPageSize,outFile);
	fclose(outFile);
	/*
	ofstream outputFile(filename);
	outputFile<<keyCount<<endl;
	for(int i=0; i<M; i++)
		outputFile<<fixed<<key[i]<<" ";
	outputFile<<endl;
	for(int i=0; i<M; i++)
		outputFile<<ptr[i]<<" ";
	outputFile<<endl;
	outputFile<<parent<<endl;
	outputFile<<left<<endl;
	outputFile<<right<<endl;
	// padding
	int sz = filesize(filename);
	string s = "\n";
	s.insert(s.end(),2047-sz,'0');
	ofstream outputFile2(filename, fstream::app|fstream::out);
	outputFile2<<s;
	*/
}

void leaf::readFromFile(string filename)
{
	ifstream inputFile(filename);
	inputFile>>keyCount;
	for(int i=0; i<M; i++)
		inputFile>>key[i];
	for(int i=0; i<M; i++)
		inputFile>>ptr[i];
	inputFile>>parent;
	inputFile>>left;
	inputFile>>right;
}

void leaf::printNode()
{
	cout<<keyCount<<endl;
	for(int i=0; i<M; i++)
		cout<<fixed<<key[i]<<" ";
	cout<<endl;
	for(int i=0; i<M; i++)
		cout<<ptr[i]<<" ";
	cout<<endl;
	cout<<parent<<endl;
	cout<<left<<endl;
	cout<<right<<endl;
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

double leaf::getKey(int i)
{
	return key[i];
}

string leaf::getPtr(int i)
{
	return ptr[i];
}
void leaf::setKey(int i, double newKey)
{
	key[i] = newKey;
}
void leaf::setPtr(int i, string newPtr)
{
	ptr[i] = newPtr;
}

string leaf::getObjectContent(int i)
{
	ifstream inputFile(ptr[i]);
	string content;
	inputFile>>content;
	return content;
}