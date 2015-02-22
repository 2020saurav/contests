class node
{
	private:
		int keyCount;
		double *key;
		string *ptr;
		string parent;
	public:
		node();
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
		void printNode();
};

node::node()
{
	keyCount = 0;
	key = new double[M];
	ptr = new string[M+1];
	for(int i=0; i<M; i++)
		key[i] = 2.000000;		// 8 bytes
	for(int i=0; i<M+1; i++)
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

double node::getKey(int i)
{
	return key[i];
}

string node::getPtr(int i)
{
	return ptr[i];
}
void node::setKey(int i, double newKey)
{
	key[i] = newKey;
}
void node::setPtr(int i, string newPtr)
{
	ptr[i] = newPtr;
}

void node::printToFile(string filename)
{
	char buffer[MAX_DISK_PAGE_SIZE];
	sprintf(buffer,"%d\n",keyCount);
	for(int i=0; i<M; i++)
		sprintf(buffer + strlen(buffer), "%.6lf\t",key[i]);
	sprintf(buffer+strlen(buffer),"\n");
	for(int i=0; i<M+1; i++)
		sprintf(buffer+strlen(buffer), "%s\t", ptr[i].c_str());
	sprintf(buffer+strlen(buffer),"\n");
	sprintf(buffer+strlen(buffer),"%s\n",parent.c_str());

	FILE *outFile;
	outFile = fopen(filename.c_str(),"wb"); // BINARY MODE
	fwrite(buffer,1,diskPageSize,outFile);
	fclose(outFile);

	/*
	ofstream outputFile(filename);
	outputFile<<fixed<<keyCount<<endl;
	for(int i=0; i<M; i++)
		outputFile<<key[i]<<" ";
	outputFile<<endl;
	for(int i=0; i<M+1; i++)
		outputFile<<ptr[i]<<" ";
	outputFile<<endl;
	outputFile<<parent<<endl;
	outputFile.close();
	int sz = filesize(filename);
	string s = "\n";
	s.insert(s.end(),2047-sz,'0');
	ofstream outputFile2(filename, fstream::app|fstream::out);
	outputFile2<<s;
	*/
}

void node::readFromFile(string filename)
{
	ifstream inputFile(filename);
	inputFile>>keyCount;
	for(int i=0; i<M; i++)
		inputFile>>key[i];
	for(int i=0; i<M+1; i++)
		inputFile>>ptr[i];
	inputFile>>parent;
}

void node::printNode()
{
	cout<<keyCount<<endl;
	for(int i=0; i<M; i++)
		cout<<fixed<<key[i]<<" ";
	cout<<endl;
	for(int i=0; i<M+1; i++)
		cout<<ptr[i]<<" ";
	cout<<endl;
	cout<<parent<<endl;
}