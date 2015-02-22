string root;
int leafCount;
int nodeCount;
int objectCount;

void readMetaData()
{
	ifstream inputFile("metadata");
	inputFile>>root>>leafCount>>nodeCount>>objectCount;
}
void writeMetaData()
{
	ofstream outputFile("metadata");
	outputFile<<root<<endl<<leafCount<<endl<<nodeCount<<endl<<objectCount;
}

string getNewLeaf()
{
	leafCount++;
	writeMetaData();
	string s = to_string(leafCount);
	s.insert(s.begin(),9-s.size(),'0');
	s.insert(s.begin(),'L');
	leaf *n = new leaf();
	n->printToFile(s);
	return s;
}

string getNewNode()
{
	nodeCount++;
	writeMetaData();
	string s = to_string(nodeCount);
	s.insert(s.begin(),9-s.size(),'0');
	s.insert(s.begin(),'N');
	node *n = new node();
	n->printToFile(s);
	return s;
}

string getNewObject(string content)
{
	objectCount++;
	writeMetaData();
	string s = to_string(objectCount);
	s.insert(s.begin(),9-s.size(),'0');
	s.insert(s.begin(),'O');
	ofstream outputFile(s);
	outputFile<<content;
	return s;
}