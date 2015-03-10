#include <bits/stdc++.h>
using namespace std;

#define pddi pair<pair<double,double>, int>
# define pdi pair<double,int>
#define X first.first
#define Y first.second
#define ID second
#define dist first
#define index second
#define mp make_pair
#define INF 1e9

int distCompCount;
typedef chrono::high_resolution_clock myclock;

double L2Distance(pddi a, pddi b)
{
	distCompCount++;
	return sqrt((a.X-b.X)*(a.X-b.X) + (a.Y-b.Y)*(a.Y-b.Y));
}

bool L2DistanceComp(pddi i, pddi b, pddi c)
{
	double d1sq = L2Distance(i, b);
	double d2sq = L2Distance(i, c);
	return (d1sq < d2sq);
}

struct Node
{
	int index;
	double threshold;
	Node* left;
	Node* right;
	Node():
		index(0), threshold(0.0), left(0), right(0){}
};

struct DC
{
	pddi item;
	DC(pddi item): item(item) {}
	bool operator()(pddi a, pddi b )
	{
		return L2DistanceComp(item,a,b);
	}
};

class VPTree
{
	public:
		VPTree() : root(0){}
		void create(vector<pddi>& items);
		void print();
		vector<pddi> kNNQuery(pddi center, int k);
		vector<pddi> rangeQuery(pddi center, double radius);

	private:
		Node* root;
		vector<pddi> mItems;
		double tau;
		Node* buildSubtree(int lower, int upper);
		void kNNSearch(Node* node, pddi center, int k, priority_queue<pdi>& heap);
		void rangeSearch(Node* node, pddi center, double radius, priority_queue<pdi>& heap);
};

void VPTree::create(vector<pddi>& items)
{
	mItems = items;
	root = buildSubtree(0, items.size());
}

vector<pddi> VPTree::kNNQuery(pddi center, int k)
{
	priority_queue<pdi> heap;
	vector<pddi> ans;
	tau = INF;
	kNNSearch(root, center, k, heap);

	while(not heap.empty())
	{
		ans.push_back(mItems[heap.top().index]);
		heap.pop();
	}
	return ans;
}
void VPTree::kNNSearch(Node* node, pddi center, int k, priority_queue<pdi>& heap)
{
	if(node == NULL)
		return;

	double dist = L2Distance(mItems[node->index], center);

	if(dist < tau)
	{
		if(heap.size()==k)
			heap.pop();
		heap.push(mp(dist, node->index));
		if(heap.size()==k)
			tau = heap.top().dist;
	}

	if(node->left == NULL and node->right == NULL)
		return;

	if(dist < node->threshold)
	{
		if(dist - tau <= node->threshold)
			kNNSearch(node->left, center, k, heap);
		if(dist + tau >= node->threshold)
			kNNSearch(node->right, center, k, heap);
	}
	else
	{
		if(dist + tau >= node->threshold)
			kNNSearch(node->right, center, k, heap);
		if(dist - tau <= node->threshold)
			kNNSearch(node->left, center, k, heap);
	}
}

vector<pddi> VPTree::rangeQuery(pddi center, double radius)
{
	priority_queue<pdi> heap;
	vector<pddi> ans;
	tau = radius;
	rangeSearch(root, center, radius, heap);

	while(not heap.empty())
	{
		ans.push_back(mItems[heap.top().index]);
		heap.pop();
	}
	return ans;
}

void VPTree::rangeSearch(Node* node, pddi center, double radius, priority_queue<pdi>& heap)
{
	if(node == NULL)
		return;

	double dist = L2Distance(mItems[node->index], center);

	if(dist <= tau)
		heap.push(mp(dist, node->index));

	if(node->left == NULL and node->right == NULL)
		return;

	if(dist < node->threshold)
	{
		if(dist - tau <= node->threshold)
			rangeSearch(node->left, center, radius, heap);
		if(dist + tau >= node->threshold)
			rangeSearch(node->right, center, radius, heap);
	}
	else
	{
		if(dist + tau >= node->threshold)
			rangeSearch(node->right, center, radius, heap);
		if(dist - tau <= node->threshold)
			rangeSearch(node->left, center, radius, heap);
	}
}

Node* VPTree::buildSubtree(int lower, int upper)
{
	if(upper==lower)
		return NULL;
	Node* node = new Node();
	node->index = lower;

	if(upper-lower>1)
	{
		int i = (int)((double)rand() / RAND_MAX * (upper - lower - 1) ) + lower;
		swap(mItems[lower], mItems[i]);
		int median = (upper+lower)/2;
		nth_element(mItems.begin()+lower+1, mItems.begin()+median, mItems.begin()+upper, DC(mItems[lower]));
		node->threshold = L2Distance(mItems[lower], mItems[median]);
		node->index = lower;
		node->left = buildSubtree(lower+1, median);
		node->right = buildSubtree(median, upper);
	}
	return node;
}

int main()
{
	pddi a;
	string objectContent;
	vector<pddi> v;
	map<int, string> contentMap;
	int qtype, k;
	double radius;
	vector<pddi> ans;
	vector<long long> statsRangeQuery;
	vector<long long> statsRangeDistComp;
	vector<long long> statsknnQuery;
	vector<long long> statsknnDistComp;
	auto start = myclock::now();
	auto end = myclock::now();

	bool printStatsAtEnd = true;
	string dataFileName = "small.in"; // "assgn5_data.txt"
	string queryFileName = "query.in"; // "assgn5_samplequery.txt"

	ifstream dataFile(dataFileName);
	while(dataFile>>a.X>>a.Y>>a.ID>>objectContent)
	{
		v.push_back(a);
		contentMap[a.ID] = objectContent;
	}

	VPTree T;
	T.create(v);

	ifstream queryFile(queryFileName);
	while(queryFile>>qtype)
	{
		ans.clear();
		switch(qtype)
		{
			case 2: // Range Query
					queryFile>>a.X>>a.Y>>radius;
					distCompCount = 0;
					start = myclock::now();
					ans = T.rangeQuery(a, radius);
					end = myclock::now();
					statsRangeQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
					statsRangeDistComp.push_back(distCompCount);
					cout<<qtype<<"\t"<<fixed<<a.X<<"\t"<<fixed<<a.Y<<"\t"<<fixed<<radius<<endl;
					break;
			
			case 3: // knn Query
					queryFile>>a.X>>a.Y>>k;
					distCompCount = 0;
					start = myclock::now();
					if(k>0)
						ans = T.kNNQuery(a, k);
					end = myclock::now();
					statsknnQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
					statsknnDistComp.push_back(distCompCount);
					cout<<qtype<<"\t"<<fixed<<a.X<<"\t"<<fixed<<a.Y<<"\t"<<k<<endl;
					break;
		}
		for (int i=0; i<ans.size(); i++)
		{
			cout<<fixed<<ans[i].X<<"\t"<<fixed<<ans[i].Y<<"\t"<<ans[i].ID<<"\t"<<contentMap[ans[i].ID]<<endl;
		}
		cout<<endl;
	}

	if(printStatsAtEnd) // make it true to print the statistics.
	{
		cout<<"\n--------------STATISTICS-------------\n";
		long long sum; 
		double mean, sq_sum, stdev;

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

			sum = accumulate(statsRangeDistComp.begin(), statsRangeDistComp.end(),0);
			mean = (double)sum / statsRangeDistComp.size();
			sq_sum = inner_product(statsRangeDistComp.begin(), statsRangeDistComp.end(), statsRangeDistComp.begin(),0.0);
			stdev = sqrt(sq_sum/statsRangeDistComp.size() - mean*mean);
			maximum = max_element(statsRangeDistComp.begin(), statsRangeDistComp.end());
			minimum = min_element(statsRangeDistComp.begin(), statsRangeDistComp.end());

			cout<<"\nRange Query Distance Computations:\n";
			cout<<"Maximum : "<<*maximum<<endl;
			cout<<"Minimum : "<<*minimum<<endl;
			cout<<"Average : "<<mean<<endl;
			cout<<"Std Dev : "<<stdev<<endl;
		}

		if(statsknnQuery.size()>0)
		{
			sum = accumulate(statsknnQuery.begin(), statsknnQuery.end(),0);
			mean = (double)sum / statsknnQuery.size();
			sq_sum = inner_product(statsknnQuery.begin(), statsknnQuery.end(), statsknnQuery.begin(),0.0);
			stdev = sqrt(sq_sum/statsknnQuery.size() - mean*mean);
			auto maximum = max_element(statsknnQuery.begin(), statsknnQuery.end());
			auto minimum = min_element(statsknnQuery.begin(), statsknnQuery.end());
			cout<<"\nknn Query:\n";
			cout<<"Maximum : "<<*maximum<<" us"<<endl;
			cout<<"Minimum : "<<*minimum<<" us"<<endl;
			cout<<"Average : "<<mean<<" us"<<endl;
			cout<<"Std Dev : "<<stdev<<" us"<<endl;

			sum = accumulate(statsknnDistComp.begin(), statsknnDistComp.end(),0);
			mean = (double)sum / statsknnDistComp.size();
			sq_sum = inner_product(statsknnDistComp.begin(), statsknnDistComp.end(), statsknnDistComp.begin(),0.0);
			stdev = sqrt(sq_sum/statsknnDistComp.size() - mean*mean);
			maximum = max_element(statsknnDistComp.begin(), statsknnDistComp.end());
			minimum = min_element(statsknnDistComp.begin(), statsknnDistComp.end());

			cout<<"\nknn Query Distance Computations:\n";
			cout<<"Maximum : "<<*maximum<<endl;
			cout<<"Minimum : "<<*minimum<<endl;
			cout<<"Average : "<<mean<<endl;
			cout<<"Std Dev : "<<stdev<<endl;
		}
	}
	return 0;
}