/*
Language : C++11
Compilation: g++ -std=c++0x -O3 quadtree.cpp -o quadtree
Running :    ./quadtree > outputfile
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
class QuadTreeNode
{
	public:
		QuadTreeNode(double X, double Y, double W, double H);
		~QuadTreeNode();
		void split();
		QuadTreeNode& findNode(Point p);
		void insert(Point& p);
		vector<Point> pointQuery(Point p);
		vector<Point> rangeQuery(Point center, double radius);
		vector<Point> knnQuery(Point center, int k);
		vector<Point> windowQuery(Point bottomLeft, Point topRight);
	private:
		Point origin; // at left bottom corner
		double width,height;
		QuadTreeNode *subNodes[4];
		// vector<Point> points;
		Point point;	// actual point. By keeping it separate, and not deriving from origin itself, 
						// because it can be easily extended to having more than one point at a node, 
						// by uncommenting relevant parts in the code. 
		bool isLeaf;
		bool isOccupied;
};
QuadTreeNode::QuadTreeNode(double X, double Y, double W, double H)
{
	origin.x = X;
	origin.y = Y;
	width = W;
	height = H;
	isLeaf = true;
	isOccupied = false;
}
QuadTreeNode::~QuadTreeNode()
{
	if(!isLeaf)
		for(int i=0; i<4; i++)
			delete subNodes[i];
}
void QuadTreeNode::split()
{
	isLeaf = false;
	double w = width/2;
	double h = height/2;
	double x = origin.x;
	double y = origin.y;
	subNodes[0] = new QuadTreeNode(x,y,w,h);
	subNodes[1] = new QuadTreeNode(x+w,y,width-w,h);
	subNodes[2] = new QuadTreeNode(x+w,y+h,width-w, height-h);
	subNodes[3] = new QuadTreeNode(x,y+h,w,height-h);
	findNode(point).insert(point);
	// for(int i=0; i<points.size(); i++)
	// 	findNode(points[i]).insert(points[i]);
	// points.clear();
}
QuadTreeNode& QuadTreeNode::findNode(Point p)
{
	double x = p.x;
	double y = p.y;
	for(int i=0; i<4; i++)
	{
		QuadTreeNode& c = *subNodes[i];
		if(x>=c.origin.x and y>=c.origin.y and x <c.origin.x +c.width and y<c.origin.y +c.height)
			return c;
	}
}
void QuadTreeNode::insert(Point& p)
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
	// if(isLeaf and points.size()==CAPACITY)
	// 	split();
	// if(isLeaf)
	// 	points.push_back(p);
	// else
	// 	findNode(p).insert(p);
}
vector<Point> QuadTreeNode::windowQuery(Point bottomLeft, Point topRight)
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
		Point center;
		center.x = origin.x + (width/2);
		center.y = origin.y + (height/2);
		if(bottomLeft.x >= center.x)
		{
			if(bottomLeft.y >= center.y)
			{
				vector<Point> v = subNodes[2]->windowQuery(bottomLeft, topRight);
				ans.insert(ans.end(), v.begin(), v.end());
			}
			else if(topRight.y <= center.y)
			{
				vector<Point> v = subNodes[1]->windowQuery(bottomLeft, topRight);
				ans.insert(ans.end(), v.begin(), v.end());
			}
			else
			{
				Point p1,p2;
				p1.x = bottomLeft.x;
				p1.y = center.y;
				p2.x = topRight.x;
				p2.y = center.y;

				vector<Point> v1 = subNodes[2]->windowQuery(p1, topRight);
				vector<Point> v2 = subNodes[1]->windowQuery(bottomLeft, p2);
				ans.insert(ans.end(), v1.begin(), v1.end());
				ans.insert(ans.end(), v2.begin(), v2.end());
			}			
		}
		else
		{
			if(bottomLeft.y >= center.y)
			{
				if(topRight.x <= center.x)
				{
					vector<Point> v = subNodes[3]->windowQuery(bottomLeft, topRight);
					ans.insert(ans.end(), v.begin(), v.end());
				}
				else
				{
					Point p1,p2;
					p1.x = center.x;
					p1.y = bottomLeft.y;
					p2.x = center.x;
					p2.y = topRight.y;
					vector<Point> v1 = subNodes[3]->windowQuery(bottomLeft, p2);
					vector<Point> v2 = subNodes[2]->windowQuery(p1, topRight);
					ans.insert(ans.end(), v1.begin(), v1.end());
					ans.insert(ans.end(), v2.begin(), v2.end());
				}
			}
			else
			{
				if(topRight.x <= center.x)
				{
					if(topRight.y <= center.y)
					{
						vector<Point> v = subNodes[0]->windowQuery(bottomLeft, topRight);
						ans.insert(ans.end(), v.begin(), v.end());
					}
					else
					{
						Point p1,p2;
						p1.x = bottomLeft.x;
						p1.y = center.y;
						p2.x = topRight.x;
						p2.y = center.y;
						vector<Point> v1 = subNodes[0]->windowQuery(bottomLeft, p2);
						vector<Point> v2 = subNodes[3]->windowQuery(p1, topRight);
						ans.insert(ans.end(), v1.begin(), v1.end());
						ans.insert(ans.end(), v2.begin(), v2.end());
					}
				}
				else
				{
					if(topRight.y <= center.y)
					{
						Point p1,p2;
						p1.x = center.x;
						p1.y = bottomLeft.y;
						p2.x = center.x;
						p2.y = topRight.y;
						vector<Point> v1 = subNodes[0]->windowQuery(bottomLeft, p2);
						vector<Point> v2 = subNodes[1]->windowQuery(p1, topRight);
						ans.insert(ans.end(), v1.begin(), v1.end());
						ans.insert(ans.end(), v2.begin(), v2.end());
					}
					else
					{
						Point p1,p2,p3,p4;
						p1.x = bottomLeft.x;
						p1.y = center.y;
						p2.x = center.x;
						p2.y = topRight.y;
						p3.x = center.x;
						p3.y = bottomLeft.y;
						p4.x = topRight.x;
						p4.y = center.y;
						vector<Point> v1 = subNodes[0]->windowQuery(bottomLeft, center);
						vector<Point> v2 = subNodes[1]->windowQuery(p3, p4);
						vector<Point> v3 = subNodes[2]->windowQuery(center,topRight);
						vector<Point> v4 = subNodes[3]->windowQuery(p1,p2);
						ans.insert(ans.end(), v1.begin(), v1.end());
						ans.insert(ans.end(), v2.begin(), v2.end());
						ans.insert(ans.end(), v3.begin(), v3.end());
						ans.insert(ans.end(), v4.begin(), v4.end());
					}
				}

			}
		}
	}
	return ans;
}
vector<Point> QuadTreeNode::pointQuery(Point p)
{
	Point q;
	q.x = p.x + eps;
	q.y = p.y + eps;
	return windowQuery(p,q);
}
vector<Point> QuadTreeNode::rangeQuery(Point center, double radius)
{
	Point bottomLeft, topRight;
	bottomLeft.x = center.x - radius;
	bottomLeft.y = center.y - radius;
	topRight.x = center.x + radius + eps;
	topRight.y = center.y + radius + eps;

	vector<Point> boundingSquare = windowQuery(bottomLeft, topRight);
	// can be optimized more, by removing maximum size squares from four corners, and repeatedly removing such regions.
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
vector<Point> QuadTreeNode::knnQuery(Point center, int k)
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
	QuadTreeNode Q(0.0,0.0,1.0+eps,1.0+eps);
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
	ifstream file("assgn2data.txt");		// INPUT FILE

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
	// cout <<"Quad Tree constructed in "<< chrono::duration_cast<chrono::nanoseconds>(end-start).count()<<" ns.\n";

	ifstream queryfile("assgn2querysample.txt");			// QUERY FILE
	int qno=0;
	while(getline(queryfile,line)) // query processing
	{
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