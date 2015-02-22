#include <bits/stdc++.h>
#define MAX_DISK_PAGE_SIZE 65536 
using namespace std;
typedef chrono::high_resolution_clock myclock;

// these are just some values, actual value is computed in main() and used in the execution
int M = 99;
int diskPageSize = 2048;

#include "node.h"
#include "leaf.h"
#include "metadata.h"
#define eps 0.000001
void insertInNode(string, double, string);
bool isLeaf(string s)
{
	return (s[0]=='L');
}

struct myData
{
	double key;
	string object;
};
bool myCompare(pair<double,myData>a, pair<double,myData>b)
{
	return a.first <= b.first;
}

string findLeaf(double key)
{
	string current = root;
	if(isLeaf(current))
		return current;
	node n;
	n.readFromFile(current);
	while(not isLeaf(current))
	{
		for(int i=0; i<=n.getKeyCount(); i++)
		{
			if(i==n.getKeyCount() or key <= n.getKey(i))
			{
				current = n.getPtr(i);
				n.readFromFile(current);
				break;
			}
		}
	}
	return current;
}

void splitNode(string myNode)
{
	node n, sib, nextNode, newRoot, nodeChild;
	leaf leafChild;
	n.readFromFile(myNode);
	string sibling = getNewNode();

	double midKey = n.getKey(M/2);
	n.setKeyCount(M/2);
	sib.readFromFile(sibling);
	sib.setKeyCount(M - M/2 - 1);
	for(int i=0; i<sib.getKeyCount(); i++)
	{
		sib.setKey(i,n.getKey(i + M/2 + 1));
		sib.setPtr(i,n.getPtr(i + M/2 + 1));
	}
	sib.setPtr(sib.getKeyCount(),n.getPtr(M));
	n.printToFile(myNode);
	sib.printToFile(sibling);
	// change the parent pointer of sibling's children. they have new parent now.
	if(isLeaf(sib.getPtr(0))) // this node is just above leaves, children are leaves
	{
		for(int i=0; i<sib.getKeyCount()+1; i++)
		{
			string childname = sib.getPtr(i);
			if(childname!="unoccupied")
			{
				leafChild.readFromFile(childname);
				leafChild.setParent(sibling);
				leafChild.printToFile(childname);
			}
		}
	}
	else // children are nodes
	{
		for(int i=0; i<sib.getKeyCount()+1; i++)
		{
			string childname = sib.getPtr(i);
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
		newRoot.setKey(0,midKey);
		newRoot.setPtr(0,myNode);
		newRoot.setPtr(1,sibling);
		newRoot.setKeyCount(1);
		n.setParent(root);
		sib.setParent(root);
		newRoot.printToFile(root);
		n.printToFile(myNode);
		sib.printToFile(sibling);
	}
	else
	{
		sib.setParent(n.getParent());
		n.printToFile(myNode);
		sib.printToFile(sibling);
		insertInNode(n.getParent(),midKey,sibling);
	}
}

void insertInNode(string mNode, double key, string subtree)
{
	node n;
	n.readFromFile(mNode);
	int position = 0;
	while(position<n.getKeyCount() and n.getKey(position) <= key)
		position++;
	for(int i=n.getKeyCount(); i>position; i--)
	{
		n.setKey(i,n.getKey(i-1));
		n.setPtr(i+1,n.getPtr(i));
	}
	n.setKey(position, key);
	n.setPtr(position+1,subtree);
	n.setKeyCount(n.getKeyCount()+1);
	n.printToFile(mNode);
	if(n.getKeyCount()==M)
		splitNode(mNode);
}

void splitLeaf(string myLeaf)
{
	leaf n,sib,nextLeaf; 
	node newRoot; // will be used if needed
	n.readFromFile(myLeaf);
	string sibling = getNewLeaf();

	double midKey = n.getKey(M/2);
	n.setKeyCount(M/2);
	sib.readFromFile(sibling);
	sib.setKeyCount(M - M/2);
	for(int i=0; i<sib.getKeyCount(); i++)
	{
		sib.setKey(i,n.getKey(i + M/2));
		sib.setPtr(i,n.getPtr(i + M/2));
	}
	for(int i=n.getKeyCount(); i<M; i++)
	{
		n.setKey(i,2.000000);
		n.setPtr(i,"unoccupied");
	}
	string next = n.getRight();
	if(next!="unoccupied")
	{
		nextLeaf.readFromFile(next);
		nextLeaf.setLeft(sibling);
		sib.setRight(next);
		nextLeaf.printToFile(next);
	}
	n.setRight(sibling);
	sib.setLeft(myLeaf);
	if(myLeaf == root)
	{
		root = getNewNode();
		newRoot.readFromFile(root);
		newRoot.setKey(0,midKey);
		newRoot.setPtr(0,myLeaf);
		newRoot.setPtr(1,sibling);
		newRoot.setKeyCount(1);
		n.setParent(root);
		sib.setParent(root);
		newRoot.printToFile(root);
		n.printToFile(myLeaf);
		sib.printToFile(sibling);
	}
	else
	{
		sib.setParent(n.getParent());
		n.printToFile(myLeaf);
		sib.printToFile(sibling);
		insertInNode(n.getParent(),midKey,sibling);
	}
}

void insertInLeaf(string myLeaf, myData d)
{
	leaf n;

	n.readFromFile(myLeaf);
	int position = 0;
	while(position<n.getKeyCount() and n.getKey(position) <= d.key)
		position++;
	for(int i=n.getKeyCount(); i>position; i--)
	{
		n.setKey(i,n.getKey(i-1));
		n.setPtr(i,n.getPtr(i-1));
	}
	n.setKey(position,d.key);
	string objectname = getNewObject(d.object);
	n.setPtr(position,objectname);
	n.setKeyCount(n.getKeyCount()+1);
	n.printToFile(myLeaf);
	if(n.getKeyCount() == M)
		splitLeaf(myLeaf);
}

void insertInTree(myData d, string subtree)
{
	string myLeaf = findLeaf(d.key);
	insertInLeaf(myLeaf, d);
}

vector <myData> windowQuery(double left, double right) // left is included, right is not
{
	vector<myData> ans;
	myData d;
	string current = findLeaf(left);
	leaf n;
	bool flag = true;
	n.readFromFile(current);
	int start = 0;
	while(n.getKey(start) < left and start < n.getKeyCount())
		start++;
	while(flag)
	{
		for(int i = start; i<n.getKeyCount() and flag; i++)
		{
			d.key = n.getKey(i);
			d.object = n.getObjectContent(i);
			if(d.key < right)
				ans.push_back(d);
			else
				flag = false;
		}
		if(flag)
		{
			string nextLeaf = n.getRight();
			if(nextLeaf != "unoccupied")
				n.readFromFile(nextLeaf);
			else
				flag = false;
			start = 0;
		}
	}
	return ans;
}

vector<myData> kNNQuery(double center, int k)
{
	vector<pair<double,myData> > v;
	vector<myData> ans;
	myData d;
	int count = k;
	string current = findLeaf(center);
	leaf n;
	bool flag = true;
	int start = 0;
	n.readFromFile(current);
	while(n.getKey(start) <= center and start < n.getKeyCount())
		start++;
	// go right first, collect k points
	while(flag and count>0)
	{
		for(int i=start; i<n.getKeyCount() and flag and count>0; i++)
		{
			d.key = n.getKey(i);
			d.object = n.getObjectContent(i);
			v.push_back(make_pair(fabs(center-d.key),d));
			count--;
		}
		if(flag and count>0)
		{
			string nextLeaf = n.getRight();
			if(nextLeaf != "unoccupied")
				n.readFromFile(nextLeaf);
			else
				flag = false;
			start = 0;
		}
	}
	// now go left
	count = k;
	start = 0;
	flag = true;
	n.readFromFile(current);
	// including A as A's neighbour.
	while(n.getKey(start)<=center and start < n.getKeyCount())
		start++;
	while(flag and count>0)
	{
		for(int i=start-1; i>=0 and flag and count>0; i--) // init : start counted above. loop : start is set as keyCount.(so -1)
		{
			d.key = n.getKey(i);
			d.object = n.getObjectContent(i);
			v.push_back(make_pair(fabs(center-d.key),d));
			count--;
		}
		if(flag and count>0)
		{
			string prevLeaf = n.getLeft();
			if(prevLeaf != "unoccupied")
			{
				n.readFromFile(prevLeaf);
				start = n.getKeyCount();
			}
			else
				flag = false;
		}
	}
	sort(v.begin(), v.end(), myCompare);
	for(int i=0; i<k and i<v.size(); i++)
		ans.push_back(v[i].second);
	return ans;
}

vector<myData> rangeQuery(double center, double radius)
{
	double left = center - radius;
	double right = center + radius + eps;
	return windowQuery(left, right);
}

vector<myData> pointQuery(double point)
{
	double left = point;
	double right = point + eps;
	return windowQuery(left, right);
}

void printVector(vector<myData> v)
{
	// cout<<v.size()<<endl;
	for(int i=0; i<v.size(); i++)
		cout<<fixed<<v[i].key<<" "<<v[i].object<<endl;
}

int main()
{

	bool build = true;
	bool query = false;
	bool printStatsAtTheEnd = true;

	string buildFile = "small.in";
	string queryFile = "query.in";

	ifstream configFile("bplustree.config");
	configFile>>diskPageSize;
	configFile.close();
	if(diskPageSize>MAX_DISK_PAGE_SIZE)
	{
		cout<<"DISK PAGE SIZE in configuration file is greater than MAX VALUE\nAborting\n";
		return 0;
	}
	M = (diskPageSize-49)/20; // explained in report


	myData d;
	vector<myData> v;
	double center, radius, left, right;
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
		while(inputData>>d.key>>d.object)
		{
			insertInTree(d,"unoccupied");
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
						queryData>>d.key>>d.object;
						cout<<type<<" "<<fixed<<d.key<<" "<<d.object<<endl;
						start = myclock::now();
						insertInTree(d,"unoccupied");
						end = myclock::now();
						statsInsert.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						cout<<endl;
						break;

				case 1:	// POINT QUERY(key)
						queryData>>d.key;
						cout<<type<<" "<<d.key<<endl;
						start = myclock::now();
						v = pointQuery(d.key);
						end = myclock::now();
						statsPointQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						printVector(v);
						cout<<endl;
						break;

				case 2:	// RANGE QUERY(center, radius)
						queryData>>center>>radius;
						cout<<type<<" "<<fixed<<center<<" "<<fixed<<radius<<endl;
						start = myclock::now();
						v = rangeQuery(center, radius);
						end = myclock::now();
						statsRangeQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						printVector(v);
						cout<<endl;
						break;

				case 3:	// kNN Query(center, k)
						queryData>>center>>k;
						cout<<type<<" "<<fixed<<center<<" "<<k<<endl;
						start = myclock::now();
						v = kNNQuery(center, k);
						end = myclock::now();
						statsKNNQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
						printVector(v);
						cout<<endl;
						break;

				case 4: // WINDOW QUERY(left, right)
						queryData>>left>>right;
						cout<<type<<" "<<fixed<<left<<" "<<fixed<<right<<endl;
						start = myclock::now();
						v = windowQuery(left, right+eps);
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