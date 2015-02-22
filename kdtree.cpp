/*
Language : C++11
Compilation: g++ -std=c++0x -O3 kdtree.cpp -o kdtree
Running :    ./kdtree > outputfile
*/
#include <bits/stdc++.h>
#define CAPACITY 20
#define eps 0.000001
using namespace std;
typedef chrono::high_resolution_clock myclock;

struct Point
{
	double x,y;
	Point(double nx=0.0, double ny=0.0) : x(nx), y(ny) {}
};

double euclidDistance(Point a, Point b)
{
	return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}

class KDTreeNode
{
	public:
		KDTreeNode(double X, double Y, double W, double H, int D);
		~KDTreeNode();
		void split();
		KDTreeNode& findNode(Point p);
		void insert(Point& p);
		vector<Point> pointQuery(Point p);
		vector<Point> rangeQuery(Point center, double radius);
		vector<Point> knnQuery(Point center, int k);
		vector<Point> windowQuery(Point bottomLeft, Point topRight);
	// private:
		Point origin; // at left bottom corner
		double width,height;
		KDTreeNode *subNodes[2];
		// vector<Point> points;
		bool isLeaf;
		bool isOccupied;
		Point point;
		int depth; // direction to split = depth % 2
};

KDTreeNode::KDTreeNode(double X, double Y, double W, double H, int D)
{
	origin.x = X;
	origin.y = Y;
	width = W;
	height = H;
	isLeaf = true;
	isOccupied = false;
	depth = D;
}

KDTreeNode::~KDTreeNode()
{
	if(!isLeaf)
		for(int i=0; i<2; i++)
			delete subNodes[i];
}

void KDTreeNode::split()
{
	isLeaf = false;
	isOccupied = false;
	double w = width;
	double h = height;
	double x = origin.x;
	double y = origin.y;

	if(depth%2==0)
	{
		w = w/2; // split on x-axis
		subNodes[0] = new KDTreeNode(x,y,w,h,depth+1);
		subNodes[1] = new KDTreeNode(x+w,y,width-w,h,depth+1);
	}
	else
	{
		h = h/2;
		subNodes[0] = new KDTreeNode(x,y,w,h,depth+1);
		subNodes[1] = new KDTreeNode(x,y+h,w,height-h,depth+1);
	}
	findNode(point).insert(point);
	// for(int i=0; i<points.size(); i++)
	// 	findNode(points[i]).insert(points[i]);
	// points.clear();
}

KDTreeNode& KDTreeNode::findNode(Point p)
{
	double x = p.x;
	double y = p.y;
	for(int i=0; i<2; i++)
	{
		KDTreeNode& c = *subNodes[i];
		if(x>=c.origin.x and y>=c.origin.y and x <c.origin.x +c.width and y<c.origin.y +c.height)
			return c;
	}
}

void KDTreeNode::insert(Point& p)
{
	if(isLeaf and isOccupied)
		split();
	if(isLeaf)
	{
		isOccupied = true;
		point = p;
	}
	else
		findNode(p).insert(p);
}

vector<Point> KDTreeNode::windowQuery(Point bottomLeft, Point topRight)
{
	double x1 = bottomLeft.x;
	double y1 = bottomLeft.y; 
	double x2 = topRight.x;
	double y2 = topRight.y;
	vector<Point> ans;
	if(isLeaf)
	{
		// for (int i = 0; i < points.size(); i++)
		// {
		// 	Point& p = points[i];
		// 	if(x1<=p.x and p.x<x2 and y1<=p.y and p.y<y2)
		// 		ans.push_back(p);
		// }
		if(isOccupied and x1<=point.x and point.x<x2 and y1<=point.y and point.y<y2 )
			ans.push_back(point);
	}
	else
	{
		if(depth%2==0) 
		{
			// was split using x-axis
			Point p1, p2;
			p1.x = origin.x + (width/2);
			p1.y = origin.y;
			p2.x = p1.x;
			p2.y = origin.y + height;
			if(topRight.x < p2.x)
			{
				vector<Point> v = subNodes[0]->windowQuery(bottomLeft, topRight);
				ans.insert(ans.end(), v.begin(), v.end());
			}
			else if(bottomLeft.x >= p1.x)
			{
				vector<Point> v = subNodes[1]->windowQuery(bottomLeft, topRight);
				ans.insert(ans.end(), v.begin(), v.end());
			}
			else
			{
				Point p3, p4;
				p3.x = p1.x;
				p3.y = bottomLeft.y;
				p4.x = p1.x;
				p4.y = topRight.y;
				vector<Point> v1 = subNodes[0]->windowQuery(bottomLeft, p4);
				vector<Point> v2 = subNodes[1]->windowQuery(p3, topRight);
				ans.insert(ans.end(), v1.begin(), v1.end());
				ans.insert(ans.end(), v2.begin(), v2.end());
			}
		}
		else
		{
			// was split on y-axis
			Point p1, p2;
			p1.x = origin.x;
			p1.y = origin.y + (height/2);
			p2.x = origin.x + width;
			p2.y = p1.y;
			if(topRight.y < p2.y)
			{
				vector<Point> v = subNodes[0]->windowQuery(bottomLeft, topRight);
				ans.insert(ans.end(), v.begin(), v.end());
			}
			else if(bottomLeft.y >= p1.y)
			{
				vector<Point> v = subNodes[1]->windowQuery(bottomLeft, topRight);
				ans.insert(ans.end(), v.begin(), v.end());
			}
			else
			{
				Point p3, p4;
				p3.x = bottomLeft.x;
				p3.y = p1.y;
				p4.x = topRight.x;
				p4.y = p1.y;
				vector<Point> v1 = subNodes[0]->windowQuery(bottomLeft, p4);
				vector<Point> v2 = subNodes[1]->windowQuery(p3, topRight);
				ans.insert(ans.end(), v1.begin(), v1.end());
				ans.insert(ans.end(), v2.begin(), v2.end());
			}
		}
	}
	return ans;
}

vector<Point> KDTreeNode::pointQuery(Point p)
{
	Point q;
	q.x = p.x + eps;
	q.y = p.y + eps;
	return windowQuery(p,q);
}

vector<Point> KDTreeNode::rangeQuery(Point center, double radius)
{
	Point bottomLeft, topRight;
	bottomLeft.x = center.x - radius;
	bottomLeft.y = center.y - radius;
	topRight.x = center.x + radius + eps;
	topRight.y = center.y + radius + eps;

	vector<Point> boundingSquare = windowQuery(bottomLeft, topRight);
	// can be optimized more, by removing maxium size squares from four corners, and repeatedly removing such regions.
	// on average, this approach will have 21% false points [(4-pi)/4].
	vector<Point> ans;
	for(int i=0; i<boundingSquare.size(); i++)
	{
		if(euclidDistance(center,boundingSquare[i])<=radius)
		{
			ans.push_back(boundingSquare[i]);
		}
	}
	return ans;
}
vector<Point> KDTreeNode::knnQuery(Point center, int k)
{
	// binary searching on k.
	double Rmax = 1.415; // diagonal max length
	double Rmin = 0;
	bool containsSelf = false;

	vector<Point> v = pointQuery(center);
	if(v.size()>0)
	{
		k++; // find one extra point and remove self point at last.
		containsSelf = true;
	}

	while(Rmax-Rmin>eps)
	{
		double Rmid = (Rmax+Rmin)/2;
		v = rangeQuery(center, Rmid);
		if(v.size()<k)
			Rmin = Rmid;
		if(v.size()>k)
			Rmax = Rmid;
		if(v.size()==k)
			break;
	}
	if(containsSelf)
	{
		for(int i=0; i<v.size(); i++)
		{
			if(fabs(v[i].x-center.x)<eps and fabs(v[i].y-center.y)<eps)
			{
				v.erase(v.begin()+i);
				break;
			}
		}
	}
	return v;
}

int main()
{
	KDTreeNode Q(0.0,0.0,1.0+eps,1.0+eps,0);
	int qtype, k; 
	Point p, bottomLeft, topRight, center;
	double radius;
	vector<Point> ans;
	vector<long long> statsInsert;
	vector<long long> statsPointQuery;
	vector<long long> statsRangeQuery;
	vector<long long> statsknnQuery;
	vector<long long> statsWindowQuery;

	auto allstart = myclock::now();
	ifstream file("assgn2data.txt");

	auto start = myclock::now();
	string line;
	int pts = 0;
	while(getline(file, line))
	{
		stringstream linestream(line);
		linestream >> p.x>>p.y;
		ans = Q.pointQuery(p);
		if(ans.size()==0) // only unique points to be added
			Q.insert(p);
	}
	auto end = myclock::now();
	// cout <<"KD Tree constructed in "<< chrono::duration_cast<chrono::nanoseconds>(end-start).count()<<" ns.\n";
	ifstream queryfile("assgn2querysample.txt");
	int qno=0;
	while(getline(queryfile,line)) // query processing
	{
		// cout<<qno<<endl;
		qno++;
		ans.clear();
		stringstream linestream(line);
		linestream>>qtype;
		switch(qtype)
		{
			case 0:	// Insert
					linestream >> p.x >> p.y;
					cout<<qtype<<"\t"<<fixed<<p.x<<"\t"<<fixed<<p.y<<endl<<endl; // no output for insert queries
					ans = Q.pointQuery(p); // check for duplicacy! ignore duplicate points
					if(ans.size()>0)
						break;
					start = myclock::now();
					Q.insert(p);
					end = myclock::now();
					statsInsert.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
					break;
			case 1:	// Point Query
					linestream >> p.x >> p.y;
					cout<<qtype<<"\t"<<fixed<<p.x<<"\t"<<fixed<<p.y<<endl;
					start = myclock::now();
					ans = Q.pointQuery(p);
					end = myclock::now();
					statsPointQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
					for(int i=0; i<ans.size(); i++)
						printf("%.6lf\t%.6lf\n",ans[i].x,ans[i].y);
					cout<<endl;
					break;
			case 2: // Range Query
					linestream >> center.x >> center.y >> radius;
					cout<<qtype<<"\t"<<fixed<<center.x<<"\t"<<fixed<<center.y<<"\t"<<fixed<<radius<<endl;
					start = myclock::now();
					ans = Q.rangeQuery(center,radius);
					end = myclock::now();
					statsRangeQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
					for(int i=0; i<ans.size(); i++)
						printf("%.6lf\t%.6lf\n",ans[i].x,ans[i].y);
					cout<<endl;
					break;
			case 3: // kNN Query
					linestream >> center.x >> center.y >> k;
					cout<<qtype<<"\t"<<fixed<<center.x<<"\t"<<fixed<<center.y<<"\t"<<k<<endl;
					start = myclock::now();
					ans = Q.knnQuery(center,k);
					end = myclock::now();
					statsknnQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
					for(int i=0; i<ans.size(); i++)
						printf("%.6lf\t%.6lf\n",ans[i].x,ans[i].y);
					cout<<endl;
					break;
			case 4: // Window Query
					linestream >> bottomLeft.x >> bottomLeft.y >> topRight.x >> topRight.y;
					cout<<qtype<<"\t"<<fixed<<bottomLeft.x<<"\t"<<fixed<<bottomLeft.y<<"\t"<<fixed<<topRight.x<<"\t"<<fixed<<topRight.y<<endl;
					if(bottomLeft.x > topRight.x)
					{
						p.x = bottomLeft.x; 			// correction of points which donot make bottomLeft and topRight
						bottomLeft.x = topRight.x;
						topRight.x = p.x;
					}
					if(bottomLeft.y > topRight.y)
					{
						p.y = bottomLeft.y;
						bottomLeft.y = topRight.y;
						topRight.y = p.y;
					}
					topRight.x+=eps;
					topRight.y+=eps;
					start = myclock::now();
					ans = Q.windowQuery(bottomLeft,topRight);
					end = myclock::now();
					statsWindowQuery.push_back(chrono::duration_cast<chrono::nanoseconds>(end-start).count()/1000);
					for(int i=0; i<ans.size(); i++)
						printf("%.6lf\t%.6lf\n",ans[i].x,ans[i].y);
					cout<<endl;
					break;
			default:
					break;
		}
	}

	if(false) // make it true to print the statistics.
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
		auto allend = myclock::now();
		cout<<"\n Complete execution time: "<<chrono::duration_cast<chrono::nanoseconds>(allend-allstart).count()/1000<<" us.\n";
	}
	return 0;
}