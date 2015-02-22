#include <bits/stdc++.h>
#define MAX_DISK_PAGE_SIZE 65536 
#define eps 0.000001

using namespace std;
typedef chrono::high_resolution_clock myclock;

int M = 5;
int diskPageSize = 2048;
int d = 2;

bool isLeaf(string s)
{
	return (s[0]=='L');
}
void setValue(double* k, int n, double value)
{
	for(int i=0; i<n; i++)
		k[i] = value;
}

#include "node.h"
#include "leaf.h"
#include "metadata.h"
#include "mymath.h"

struct myData
{
	double *key;
	string object;
	myData()
	{
		key = new double[d];
	}
};

void insertInNode(string, string, double*, double*);

void updateKeyOfParent(string parent, string child, double* minKey, double* maxKey)
{
	int position; // 1-indexed
	node n;
	n.readFromFile(parent);
	for(position=1; position<=n.getKeyCount(); position++)
	{
		if(n.getPtr(position) == child)
			break;
	}
	assert(position<=n.getKeyCount());
	n.setMinKey(position, minKey);
	n.setMaxKey(position, maxKey);
	n.printToFile(parent);

	minKey = findMinKeyInNode(n);
	maxKey = findMaxKeyInNode(n);
	if(n.getParent()!="unoccupied")
		updateKeyOfParent(n.getParent(), parent, minKey, maxKey);
}

string findLeafToInsert(double *key)
{
	vector<string> candidates;
	candidates.push_back(root);
	node n; leaf l;
	while(not isLeaf(candidates[0]))
	{
		vector<string> temp;
		for(int i=0; i<candidates.size(); i++)
		{
			n.readFromFile(candidates[i]);
			for(int i=1; i<=n.getKeyCount(); i++)
			{
				if(rectangleContainsPoint(n.getMinKey(i), n.getMaxKey(i), key))
					temp.push_back(n.getPtr(i));
			}
		}
		if(temp.size()==0)
			break;
		else
		{
			candidates.clear();
			candidates = temp;
		}
	}
	if(not isLeaf(candidates[0]))
	{
		// traverse to leaf level, collect all leaves.
		while(not isLeaf(candidates[0]))
		{
			vector<string> temp;
			for(int i=0; i<candidates.size(); i++)
			{
				n.readFromFile(candidates[i]);
				for(int i=1; i<=n.getKeyCount(); i++)
				{
					temp.push_back(n.getPtr(i));
				}
			}
			candidates.clear();
			candidates = temp;
		}
	}
	// now candidates has all leaves possible. Find the best one.
	assert(candidates.size()>0);
	double leastVolInc = 1.0;
	string bestLeaf = candidates[0];
	double presentVolume, newVolume;
	double* minKey;
	double* maxKey;
	int keyCountOfBestLeaf=100;
	int volumeOfBestLeaf=1.0;
	for(int i=0; i<candidates.size(); i++)
	{
		l.readFromFile(candidates[i]);
		minKey = findMinKeyInLeaf(l);
		maxKey = findMaxKeyInLeaf(l);
		presentVolume = findVolume(minKey, maxKey);
		newVolume = findVolume(arrayMin(minKey, key), arrayMax(maxKey, key));

		if(newVolume - presentVolume < leastVolInc)
		{
			bestLeaf = candidates[i];
			leastVolInc = newVolume - presentVolume;
			keyCountOfBestLeaf = l.getKeyCount();
			volumeOfBestLeaf = presentVolume;
		}
		else if(newVolume - presentVolume == leastVolInc)
		{
			if(presentVolume < volumeOfBestLeaf)
			{
				bestLeaf = candidates[i];
				keyCountOfBestLeaf = l.getKeyCount();
				volumeOfBestLeaf = presentVolume;
			}
			else if(presentVolume == volumeOfBestLeaf)
			{
				if(l.getKeyCount()<keyCountOfBestLeaf)
				{
					bestLeaf = candidates[i];
					keyCountOfBestLeaf = l.getKeyCount();
				}
			}
		}
	}

	return bestLeaf;
}

void splitNode(string myNode)
{
	node n;
	n.readFromFile(myNode); // n has to be split
	int a, b;
	double maxVolWaste = -10.0;
	double volU, vola, volb;
	double* leftBottom; double* topRight;
	for(int i=1; i<=n.getKeyCount(); i++)
	{
		for(int j=i+1; j<=n.getKeyCount(); j++)
		{
			leftBottom = arrayMin(n.getMinKey(i), n.getMinKey(j));
			topRight = arrayMax(n.getMaxKey(i), n.getMaxKey(j));
			volU = findVolume(leftBottom, topRight);
			vola = findVolume(n.getMinKey(i), n.getMaxKey(i));
			volb = findVolume(n.getMinKey(j), n.getMaxKey(j));
			if(volU - vola - volb > maxVolWaste)
			{
				a = i;
				b = j;
				maxVolWaste = volU - vola - volb;
			}
		}
	}

	int i;
	node n1, n2, newRoot;
	int position;
	double waste1, waste2;
	double* n1LeftBottom;
	double* n1TopRight;
	double* n2LeftBottom;
	double* n2TopRight;

	n1.readFromFile(myNode);
	string sibling = getNewNode();
	n2.readFromFile(sibling);

	n1.setMinKey(1, n.getMinKey(a));
	n1.setMaxKey(1, n.getMaxKey(a));
	n1.setPtr(1, n.getPtr(a));
	n1.setKeyCount(1);
	n1LeftBottom = findMinKeyInNode(n1);
	n1TopRight = findMaxKeyInNode(n1);

	n2.setMinKey(1, n.getMinKey(b));
	n2.setMaxKey(1, n.getMaxKey(b));
	n2.setPtr(1, n.getPtr(b));
	n2.setKeyCount(1);
	n2LeftBottom = findMinKeyInNode(n2);
	n2TopRight = findMaxKeyInNode(n2);

	for(i=1; i<=n.getKeyCount() and n1.getKeyCount()<M/2 and n2.getKeyCount()<M/2; i++)
	{
		if(i==a or i==b)
			continue;
		volU = findVolume(arrayMin(n1LeftBottom, n.getMinKey(i)), arrayMax(n1TopRight, n.getMaxKey(i)));
		vola = findVolume(n1LeftBottom, n1TopRight);
		volb = 0.0;
		waste1 = volU - vola - volb;

		volU = findVolume(arrayMin(n2LeftBottom, n.getMinKey(i)), arrayMax(n2TopRight, n.getMaxKey(i)));
		vola = findVolume(n2LeftBottom, n2TopRight);
		volb = 0.0;
		waste2 = volU - vola - volb;

		if(waste1 < waste2)
		{
			position = n1.getKeyCount() + 1;
			n1.setKeyCount(position);
			n1.setMinKey(position, n.getMinKey(i));
			n1.setMaxKey(position, n.getMaxKey(i));
			n1.setPtr(position, n.getPtr(i));
			n1LeftBottom = findMinKeyInNode(n1);
			n1TopRight = findMaxKeyInNode(n1);
		}
		else if(waste1 == waste2)
		{
			double vol1 = findVolume(n1LeftBottom, n1TopRight);
			double vol2 = findVolume(n2LeftBottom, n2TopRight);
			if(vol1 < vol2)
			{
				position = n1.getKeyCount() + 1;
				n1.setKeyCount(position);
				n1.setMinKey(position, n.getMinKey(i));
				n1.setMaxKey(position, n.getMaxKey(i));
				n1.setPtr(position, n.getPtr(i));
				n1LeftBottom = findMinKeyInNode(n1);
				n1TopRight = findMaxKeyInNode(n1);
			}
			else if(vol1 == vol2)
			{
				if(n1.getKeyCount() < n2.getKeyCount())
				{
					position = n1.getKeyCount() + 1;
					n1.setKeyCount(position);
					n1.setMinKey(position, n.getMinKey(i));
					n1.setMaxKey(position, n.getMaxKey(i));
					n1.setPtr(position, n.getPtr(i));
					n1LeftBottom = findMinKeyInNode(n1);
					n1TopRight = findMaxKeyInNode(n1);					
				}
				else
				{
					position = n2.getKeyCount() + 1;
					n2.setKeyCount(position);
					n2.setMinKey(position, n.getMinKey(i));
					n2.setMaxKey(position, n.getMaxKey(i));
					n2.setPtr(position, n.getPtr(i));
					n2LeftBottom = findMinKeyInNode(n2);
					n2TopRight = findMaxKeyInNode(n2);
				}
			}
			else
			{
				position = n2.getKeyCount() + 1;
				n2.setKeyCount(position);
				n2.setMinKey(position, n.getMinKey(i));
				n2.setMaxKey(position, n.getMaxKey(i));
				n2.setPtr(position, n.getPtr(i));
				n2LeftBottom = findMinKeyInNode(n2);
				n2TopRight = findMaxKeyInNode(n2);				
			}
		}
		else
		{
			position = n2.getKeyCount() + 1;
			n2.setKeyCount(position);
			n2.setMinKey(position, n.getMinKey(i));
			n2.setMaxKey(position, n.getMaxKey(i));
			n2.setPtr(position, n.getPtr(i));
			n2LeftBottom = findMinKeyInNode(n2);
			n2TopRight = findMaxKeyInNode(n2);
		}
	}

	if(n1.getKeyCount() == M/2)
	{
		for( ; i<=n.getKeyCount(); i++)
		{
			if(i==a or i==b)
				continue;
			position = n2.getKeyCount() + 1;
			n2.setKeyCount(position);
			n2.setMinKey(position, n.getMinKey(i));
			n2.setMaxKey(position, n.getMaxKey(i));
			n2.setPtr(position, n.getPtr(i));
		}
	}
	else
	{
		for( ; i<=n.getKeyCount(); i++)
		{
			if(i==a or i==b)
				continue;
			position = n1.getKeyCount() + 1;
			n1.setKeyCount(position);
			n1.setMinKey(position, n.getMinKey(i));
			n1.setMaxKey(position, n.getMaxKey(i));
			n1.setPtr(position, n.getPtr(i));
		}	
	}

	n1LeftBottom = findMinKeyInNode(n1);
	n1TopRight = findMaxKeyInNode(n1);
	n2LeftBottom = findMinKeyInNode(n2);
	n2TopRight = findMaxKeyInNode(n2);

	node nodeChild;
	leaf leafChild;

	// change the parent pointer if sibling's children. new Parent
	if(isLeaf(n2.getPtr(1))) // this level is just above leaves
	{
		for(int i=1; i<=n2.getKeyCount(); i++)
		{
			string childname = n2.getPtr(i);
			if(childname!="unoccupied")
			{
				leafChild.readFromFile(childname);
				leafChild.setParent(sibling);
				leafChild.printToFile(childname);
			}
		}
	}
	else // this level has nodes as children
	{
		for(int i=1; i<=n2.getKeyCount(); i++)
		{
			string childname = n2.getPtr(i);
			if(childname!="unoccupied")
			{
				nodeChild.readFromFile(childname);
				nodeChild.setParent(sibling);
				nodeChild.printToFile(childname);
			}
		}
	}

	if(myNode == root)
	{
		root = getNewNode();
		newRoot.readFromFile(root);
		newRoot.setMinKey(1, n1LeftBottom);
		newRoot.setMaxKey(1, n1TopRight);
		newRoot.setPtr(1, myNode);

		newRoot.setMinKey(2, n2LeftBottom);
		newRoot.setMaxKey(2, n2TopRight);
		newRoot.setPtr(2, sibling);
		
		newRoot.setKeyCount(2);
		n1.setParent(root);
		n2.setParent(root);

		newRoot.printToFile(root);
		n1.printToFile(myNode);
		n2.printToFile(sibling);
	}
	else
	{
		n2.setParent(n1.getParent());
		n1.printToFile(myNode);
		n2.printToFile(sibling);
		
		// parent definitely exists
		updateKeyOfParent(n.getParent(), myNode, n1LeftBottom, n1TopRight);
		insertInNode(n.getParent(), sibling, n2LeftBottom, n2TopRight);
	}
}

void splitLeaf(string myLeaf)
{
	leaf l; node newRoot;
	l.readFromFile(myLeaf);
	// l has to be split into 2 parts, using Quadratic Split.
	// treat each object key as a rectangle of 0 length in each dimension. Used in calculating waste
	int a, b; // most wasteful object's indexes
	double maxVolWaste = 0.0;
	for(int i=1; i<=l.getKeyCount(); i++)
	{
		for(int j=i+1; j<=l.getKeyCount(); j++)
		{
			if(findVolume(l.getKey(i), l.getKey(j)) > maxVolWaste) // area of point objects = 0
			{
				a = i;
				b = j;
				maxVolWaste = findVolume(l.getKey(a), l.getKey(b));
			}
		}
	}
	
	int i;
	leaf l1, l2;
	double* l1LeftBottom;
	double* l1TopRight;
	double* l2LeftBottom;
	double* l2TopRight;
	double waste1, waste2;
	double volU, vola, volb;
	int position;

	string sibling = getNewLeaf();

	l1.readFromFile(myLeaf);
	l1.setKeyCount(0);
	l2.readFromFile(sibling);

	l1.setKey(1, l.getKey(a));
	l1.setPtr(1, l.getPtr(a));
	l1.setKeyCount(1);
	l1LeftBottom = l.getKey(a);
	l1TopRight = l.getKey(a);

	l2.setKey(1, l.getKey(b));
	l2.setPtr(1, l.getPtr(b));
	l2.setKeyCount(1);
	l2LeftBottom = l.getKey(b);
	l2TopRight = l.getKey(b);

	for(i=1; i<=l.getKeyCount() and l1.getKeyCount()<M/2 and l2.getKeyCount()<M/2; i++)
	{
		if(i==a or i==b)
			continue;
		//else : find the best leaf (a's or b's) for ith object
		volU = findVolume(arrayMin(l1LeftBottom, l.getKey(i)), arrayMax(l1TopRight, l.getKey(i)));
		vola = findVolume(l1LeftBottom, l1TopRight);
		volb = 0.0; // volume of a point
		waste1 = volU - vola - volb;
		volU = findVolume(arrayMin(l2LeftBottom, l.getKey(i)), arrayMax(l2TopRight, l.getKey(i)));
		vola = findVolume(l2LeftBottom, l2TopRight);
		volb = 0.0;
		waste2 = volU - vola - volb;
		if(waste1 < waste2)
		{
			position = l1.getKeyCount() + 1;
			l1.setKeyCount(position);
			l1.setKey(position, l.getKey(i));
			l1.setPtr(position, l.getPtr(i));
			l1LeftBottom = findMinKeyInLeaf(l1);
			l1TopRight = findMaxKeyInLeaf(l1); 
		}
		else if(waste1 == waste2)
		{
			double vol1 = findVolume(l1LeftBottom, l1TopRight);
			double vol2 = findVolume(l2LeftBottom, l2TopRight);
			if(vol1 < vol2)
			{
				// insert in l1
				position = l1.getKeyCount() + 1;
				l1.setKeyCount(position);
				l1.setKey(position, l.getKey(i));
				l1.setPtr(position, l.getPtr(i));
				l1LeftBottom = findMinKeyInLeaf(l1);
				l1TopRight = findMaxKeyInLeaf(l1);
			}
			else if(vol1==vol2)
			{
				if(l1.getKeyCount()<l2.getKeyCount())
				{
					position = l1.getKeyCount() + 1;
					l1.setKeyCount(position);
					l1.setKey(position, l.getKey(i));
					l1.setPtr(position, l.getPtr(i));
					l1LeftBottom = findMinKeyInLeaf(l1);
					l1TopRight = findMaxKeyInLeaf(l1);
				}
				else
				{
					position = l2.getKeyCount() + 1;
					l2.setKeyCount(position);
					l2.setKey(position, l.getKey(i));
					l2.setPtr(position, l.getPtr(i));
					l2LeftBottom = findMinKeyInLeaf(l2);
					l2TopRight = findMaxKeyInLeaf(l2);
				}
			}
			else
			{
				position = l2.getKeyCount() + 1;
				l2.setKeyCount(position);
				l2.setKey(position, l.getKey(i));
				l2.setPtr(position, l.getPtr(i));
				l2LeftBottom = findMinKeyInLeaf(l2);
				l2TopRight = findMaxKeyInLeaf(l2);
			}
		}
		else
		{
			position = l2.getKeyCount() + 1;
			l2.setKeyCount(position);
			l2.setKey(position, l.getKey(i));
			l2.setPtr(position, l.getPtr(i));
			l2LeftBottom = findMinKeyInLeaf(l2);
			l2TopRight = findMaxKeyInLeaf(l2); 
		}
	}

	if(l1.getKeyCount() == M/2)
	{
		for( ; i<=l.getKeyCount(); i++)
		{
			if(i==a or i==b)
				continue;
			position = l2.getKeyCount() + 1;
			l2.setKeyCount(position);
			l2.setKey(position, l.getKey(i));
			l2.setPtr(position, l.getPtr(i));
		}
	}
	else
	{
		for( ; i<=l.getKeyCount(); i++)
		{
			if(i==a or i==b)
				continue;
			position = l1.getKeyCount() + 1;
			l1.setKeyCount(position);
			l1.setKey(position, l.getKey(i));
			l1.setPtr(position, l.getPtr(i));
		}
	}
	l1LeftBottom = findMinKeyInLeaf(l1);
	l1TopRight = findMaxKeyInLeaf(l1); 
	l2LeftBottom = findMinKeyInLeaf(l2);
	l2TopRight = findMaxKeyInLeaf(l2); 

	if(myLeaf == root)
	{
		root = getNewNode();
		newRoot.readFromFile(root);
		newRoot.setMinKey(1, l1LeftBottom);
		newRoot.setMaxKey(1, l1TopRight);
		newRoot.setPtr(1, myLeaf);

		newRoot.setMinKey(2, l2LeftBottom);
		newRoot.setMaxKey(2, l2TopRight);
		newRoot.setPtr(2, sibling);

		newRoot.setKeyCount(2);
		l1.setParent(root);
		l2.setParent(root);

		newRoot.printToFile(root);
		l1.printToFile(myLeaf);
		l2.printToFile(sibling);
	}
	else
	{
		l2.setParent(l1.getParent());
		l1.printToFile(myLeaf);
		l2.printToFile(sibling);
		// parent definitely exists, correct child rectangle dimensions
		updateKeyOfParent(l1.getParent(), myLeaf, l1LeftBottom, l1TopRight);
		insertInNode(l2.getParent(), sibling, l2LeftBottom, l2TopRight);
	}
}

void insertInNode(string myNode, string child, double* minKey, double* maxKey)
{
	node n;
	n.readFromFile(myNode);
	int position = n.getKeyCount() + 1;
	n.setMinKey(position, minKey);
	n.setMaxKey(position, maxKey);
	n.setPtr(position, child);
	n.setKeyCount(n.getKeyCount() + 1);
	n.printToFile(myNode);

	minKey = findMinKeyInNode(n);
	maxKey = findMaxKeyInNode(n);
	if(n.getParent()!="unoccupied") // required?
		updateKeyOfParent(n.getParent(), myNode, minKey, maxKey);
		
	if(n.getKeyCount()==M)
		splitNode(myNode);
}

void insertInLeaf(string myLeaf, myData m)
{
	leaf n;
	n.readFromFile(myLeaf);
	int position = n.getKeyCount() + 1; // 1-indexed for front end
	n.setKey(position, m.key);
	string objectname = getNewObject(m.object);
	n.setPtr(position, objectname);
	n.setKeyCount(n.getKeyCount() + 1);
	n.printToFile(myLeaf);

	double* minKey = findMinKeyInLeaf(n);
	double* maxKey = findMaxKeyInLeaf(n);
	if(n.getParent()!="unoccupied")
		updateKeyOfParent(n.getParent(), myLeaf, minKey, maxKey);
		
	if(n.getKeyCount() == M)
		splitLeaf(myLeaf);
}

void insertInTree(myData m)
{
	string myLeaf = findLeafToInsert(m.key);
	insertInLeaf(myLeaf, m);
}

vector<myData> windowQuery(double* leftBottom, double* topRight)
{
	vector<string> candidates, temp;
	vector<myData> ans;
	leaf l; node n;
	myData m;
	candidates.push_back(root);
	while(true)
	{
		if(isLeaf(candidates[0]))
		{
			for(int i=0; i<candidates.size(); i++)
			{
				l.readFromFile(candidates[i]);
				for(int j=1; j<=l.getKeyCount(); j++)
				{
					if(rectangleContainsPoint(leftBottom, topRight, l.getKey(j)))
					{
						m.key = l.getKey(j);
						m.object = l.getObjectContent(j);
						ans.push_back(m);
					}
				}
			}
			break;
		}
		else // still at some node level
		{
			for(int i=0; i<candidates.size(); i++)
			{
				n.readFromFile(candidates[i]);
				for(int j=1; j<=n.getKeyCount(); j++)
				{
					if(rectangleIntersectsRectangle(leftBottom, topRight, n.getMinKey(j), n.getMaxKey(j)))
					{
						temp.push_back(n.getPtr(j));
					}
				}
			}
			candidates.clear();
			candidates = temp;
			temp.clear();
		}

		if(candidates.size()==0)
			break;
	}

	return ans;
}

vector<myData> rangeQuery(double* center, double radius)
{
	vector<myData> ans;
	double* leftBottom= new double[d];
	double* topRight = new double[d];
	double radiusSq = radius*radius;
	for(int i=0; i<d; i++)
	{
		leftBottom[i] = center[i] - radius;
		topRight[i]	= center[i] + radius;
	}
	vector<myData> v = windowQuery(leftBottom, topRight);
	for(int i=0; i<v.size(); i++)
	{
		if(euclidDistanceSq(v[i].key, center)<=radiusSq)
		{
			ans.push_back(v[i]);
		}
	}
	return ans;
}

vector<myData> kNNQuery(double* center, int k)
{
	vector<myData> ans;
	double Rmax = (sqrt(d) + eps);
	double Rmin = 0.0;
	while(Rmax-Rmin>eps)
	{
		double Rmid = (Rmax+Rmin)/2.0;
		ans = rangeQuery(center, Rmid);
		if(ans.size()<k)
			Rmin = Rmid;
		if(ans.size()>k)
			Rmax = Rmid;
		if(ans.size()==k)
			break;
	}
	return ans;
}

vector<myData> pointQuery(double* key)
{
	vector<myData> ans;
	ans = windowQuery(key, key);
	return ans;
}

void printVector(vector<myData> v)
{
	cout<<v.size()<<endl;
	for(int i=0; i<v.size(); i++)
	{
		for(int j=0; j<d; j++)
			cout<<fixed<<v[i].key[j]<<" ";
		cout<<v[i].object<<endl;
	}
}
int main()
{
	bool build = true;
	bool query = true;
	bool printStatsAtTheEnd = true;

	string buildFile = "small.in";
	string queryFile = "query.in";

	ifstream configFile("rtree.config");
	configFile>>diskPageSize>>d;
	configFile.close();

	if(diskPageSize>MAX_DISK_PAGE_SIZE)
	{
		cout<<"DISK PAGE SIZE in configuration file is greater than MAX VALUE\nAborting\n";
		return 0;
	}

	M = (diskPageSize-20)/(18*d + 11);

	myData m;
	vector<myData> v;
	double* center = new double[d];
	double radius;
	double* bottomLeft = new double[d];
	double* topRight = new double[d];
	int k;
	vector<long long> statsInsert;
	vector<long long> statsPointQuery;
	vector<long long> statsRangeQuery;
	vector<long long> statsKNNQuery;
	vector<long long> statsWindowQuery;
	auto start = myclock::now();
	auto end = myclock::now();

	readMetaData();
	if(build)
	{
		leafCount = 0;
		nodeCount = 0;
		objectCount = 0;
		system("rm -f L*");
		system("rm -f N*");
		system("rm -f O*");
		root = getNewLeaf();
		writeMetaData();
		ifstream inputData(buildFile);
		while(inputData>>m.key[0])
		{
			for(int i=1; i<d; i++)
				inputData>>m.key[i];
			inputData>>m.object;
			insertInTree(m);			
		}
		writeMetaData();
	}
	if(query)
	{
		ifstream queryData(queryFile);
		int type;
		while(queryData>>type)
		{
			v.clear();
			switch(type)
			{
				case 0:	// INSERT (key, object)
						for(int i=0; i<d; i++)
							queryData>>m.key[i];
						queryData>>m.object;
						cout<<type<<" ";
						// cout<<type<<" "<<fixed<<m.key<<" "<<m.object<<endl;
						for(int i=0; i<d; i++)
							cout<<fixed<<m.key[i]<<" ";
						cout<<m.object<<endl;
						start = myclock::now();
						insertInTree(m);
						end = myclock::now();
						statsInsert.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						cout<<endl;
						break;

				case 1:	// POINT QUERY(key)
						for(int i=0; i<d; i++)
							queryData>>m.key[i];
						cout<<type<<" ";
						// cout<<type<<" "<<m.key<<endl;
						for(int i=0; i<d; i++)
							cout<<fixed<<m.key[i]<<" ";
						cout<<endl;
						start = myclock::now();
						v = pointQuery(m.key);
						end = myclock::now();
						statsPointQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						printVector(v);
						cout<<endl;
						break;

				case 2:	// RANGE QUERY(center, radius)
						// queryData>>center>>radius;
						for(int i=0; i<d; i++)
							queryData>>center[i];
						queryData>>radius;
						cout<<type<<" ";
						for(int i=0; i<d; i++)
							cout<<fixed<<center[i]<<" ";
						cout<<fixed<<radius<<endl;
						// cout<<type<<" "<<fixed<<center<<" "<<fixed<<radius<<endl;
						start = myclock::now();
						v = rangeQuery(center, radius);
						end = myclock::now();
						statsRangeQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						printVector(v);
						cout<<endl;
						break;

				case 3:	// kNN Query(center, k)
						// queryData>>center>>k;
						for(int i=0; i<d; i++)
							queryData>>center[i];
						queryData>>k;
						cout<<type<<" ";
						for(int i=0; i<d; i++)
							cout<<fixed<<center[i]<<" ";
						cout<<k<<endl;
						// cout<<type<<" "<<fixed<<center<<" "<<k<<endl;
						start = myclock::now();
						v = kNNQuery(center, k);
						end = myclock::now();
						statsKNNQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						printVector(v);
						cout<<endl;
						break;

				case 4: // WINDOW QUERY(bottomLeft, topRight)
						// queryData>>bottomLeft>>topRight;
						for(int i=0; i<d; i++)
							queryData>>bottomLeft[i];
						for(int i=0; i<d; i++)
							queryData>>topRight[i];
						cout<<type<<" ";
						for(int i=0; i<d; i++)

							cout<<fixed<<bottomLeft[i]<<" ";
						for(int i=0; i<d; i++)
							cout<<fixed<<topRight[i]<<" ";
						cout<<endl;
						// cout<<type<<" "<<fixed<<bottomLeft<<" "<<fixed<<topRight<<endl;
						start = myclock::now();
						v = windowQuery(bottomLeft, topRight);
						end = myclock::now();
						statsWindowQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						printVector(v);
						cout<<endl;
						break;
				default:
						break;
			}
			writeMetaData();
		}
		if(printStatsAtTheEnd) // make it true to print the statistics.
		{
			cout<<"\n--------------STATISTICS-------------\n";
			long long sum; 
			double mean, sq_sum, stdev;

			if(statsInsert.size()>0)	
			{
				sum = accumulate(statsInsert.begin(), statsInsert.end(),0);
				mean = (double)sum / statsInsert.size();
				sq_sum = inner_product(statsInsert.begin(), statsInsert.end(), statsInsert.begin(),0.0);
				stdev = sqrt(sq_sum/statsInsert.size() - mean*mean);
				auto maximum = max_element(statsInsert.begin(), statsInsert.end());
				auto minimum = min_element(statsInsert.begin(), statsInsert.end());
				cout<<"\nInsert Query:\n";
				cout<<"Maximum : "<<*maximum<<" us"<<endl;
				cout<<"Minimum : "<<*minimum<<" us"<<endl;
				cout<<"Average : "<<mean<<" us"<<endl;
				cout<<"Std Dev : "<<stdev<<" us"<<endl;
			}
			
			if(statsPointQuery.size()>0)
			{
				sum = accumulate(statsPointQuery.begin(), statsPointQuery.end(),0);
				mean = (double)sum / statsPointQuery.size();
				sq_sum = inner_product(statsPointQuery.begin(), statsPointQuery.end(), statsPointQuery.begin(),0.0);
				stdev = sqrt(sq_sum/statsPointQuery.size() - mean*mean);
				auto maximum = max_element(statsPointQuery.begin(), statsPointQuery.end());
				auto minimum = min_element(statsPointQuery.begin(), statsPointQuery.end());
				cout<<"\nPoint Query:\n";
				cout<<"Maximum : "<<*maximum<<" us"<<endl;
				cout<<"Minimum : "<<*minimum<<" us"<<endl;
				cout<<"Average : "<<mean<<" us"<<endl;
				cout<<"Std Dev : "<<stdev<<" us"<<endl;
			}

			if(statsRangeQuery.size()>0)
			{
				sum = accumulate(statsRangeQuery.begin(), statsRangeQuery.end(),0);
				mean = (double)sum / statsRangeQuery.size();
				sq_sum = inner_product(statsRangeQuery.begin(), statsRangeQuery.end(), statsRangeQuery.begin(),0.0);
				stdev = sqrt(sq_sum/statsRangeQuery.size() - mean*mean);
				auto maximum = max_element(statsRangeQuery.begin(), statsRangeQuery.end());
				auto minimum = min_element(statsRangeQuery.begin(), statsRangeQuery.end());
				cout<<"\nRange Query:\n";
				cout<<"Maximum : "<<*maximum<<" us"<<endl;
				cout<<"Minimum : "<<*minimum<<" us"<<endl;
				cout<<"Average : "<<mean<<" us"<<endl;
				cout<<"Std Dev : "<<stdev<<" us"<<endl;
			}

			if(statsKNNQuery.size()>0)
			{
				sum = accumulate(statsKNNQuery.begin(), statsKNNQuery.end(),0);
				mean = (double)sum / statsKNNQuery.size();
				sq_sum = inner_product(statsKNNQuery.begin(), statsKNNQuery.end(), statsKNNQuery.begin(),0.0);
				stdev = sqrt(sq_sum/statsKNNQuery.size() - mean*mean);
				auto maximum = max_element(statsKNNQuery.begin(), statsKNNQuery.end());
				auto minimum = min_element(statsKNNQuery.begin(), statsKNNQuery.end());
				cout<<"\nkNN Query:\n";
				cout<<"Maximum : "<<*maximum<<" us"<<endl;
				cout<<"Minimum : "<<*minimum<<" us"<<endl;
				cout<<"Average : "<<mean<<" us"<<endl;
				cout<<"Std Dev : "<<stdev<<" us"<<endl;
			}

			if(statsWindowQuery.size()>0)
			{
				sum = accumulate(statsWindowQuery.begin(), statsWindowQuery.end(),0);
				mean = (double)sum / statsWindowQuery.size();
				sq_sum = inner_product(statsWindowQuery.begin(), statsWindowQuery.end(), statsWindowQuery.begin(),0.0);
				stdev = sqrt(sq_sum/statsWindowQuery.size() - mean*mean);
				auto maximum = max_element(statsWindowQuery.begin(), statsWindowQuery.end());
				auto minimum = min_element(statsWindowQuery.begin(), statsWindowQuery.end());
				cout<<"\nWindow Query:\n";
				cout<<"Maximum : "<<*maximum<<" us"<<endl;
				cout<<"Minimum : "<<*minimum<<" us"<<endl;
				cout<<"Average : "<<mean<<" us"<<endl;
				cout<<"Std Dev : "<<stdev<<" us"<<endl;		
			}
		}
	}

	return 0;
}