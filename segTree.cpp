#include <bits/stdc++.h>
using namespace std;

#define pii pair<int, int>
#define vi vector<int>
#define X first
#define Y second
#define mp make_pair
#define pb push_back
#define ll long long
#define inf 1000000000

struct node
{
	int a,c;
	void merge(node &l, node &r)
	{
		c = 0;
		a = min(l.a,r.a);
	}
	void split(node &l, node &r)
	{
		l.a += c;
		r.a += c;
		l.c += c;
		r.c += c;
	}
	void update(int val)
	{
		a = a + val;
		c = c + val;
	}
	void print()
	{
		cout<<"a= "<<a;
		cout<<", c= "<<c<<endl;
	}
};


class segTree
{
	/*
	* CREDITS: Triveni Mahatha and Utkarsh Lath
	*/
	public:
		void init(node * A, int n, node nd) {
			create(A,n,nd);
		}
		void update(int l, int r, int val) {
			range_update(1,L,R,l+L,r+L,val);
		}
		void update(int idx, int val) {
			point_update(1,L,R,idx+L,val);
		}
		node query(int l, int r) {
			return range_query(1,L,R,l+L,r+L);
		}
		node query(int idx) {
			return point_query(1,L,R,idx+L);
		}
		void print() {
			for(int i=1;i<sz;i++) { 
				cout <<" node " << i<<" - "; 
				Tree[i].print();
			}
		}
		void clear() {
			deconstruct();
		}
	 
	private:
		node * Tree;
		node identity;
		int height;		
		int L,R;
		int sz;	
		void create(node * A, int n,node nd) {
			height = ceil(log2(n));
			L = 1<<height, R = (L<<1)-1;
			sz = R+1;
			identity = nd;
			Tree = (node*)calloc(sz,sizeof(node));
			for(int i=0;i<sz;i++)
				Tree[i]=nd;
			for(int i=0;i<n;i++)
				Tree[L+i]=A[i];
			initialise(1,L,R);
			return ;
		}
		void initialise(int root, int l, int r) {
			if(l==r) return;
			int m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			initialise(lchild,l,m);
			initialise(rchild,m+1,r);
			Tree[root].merge(Tree[lchild],Tree[rchild]);
		}
		void range_update(int root, int l, int r, int i, int j, int val) {
			if(i<=l && r<=j) { Tree[root].update(val); return ;}
			int m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			Tree[root].split(Tree[lchild],Tree[rchild]);
			if(i<=m) range_update(lchild,l,m,i,j,val);
			if(j>m) range_update(rchild,m+1,r,i,j,val);
			Tree[root].merge(Tree[lchild],Tree[rchild]);
		}
		void point_update(int root, int l, int r, int idx, int val) {
			if(l==r && root==idx){ Tree[root].update(val); return ;}
			int m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			Tree[root].split(Tree[lchild],Tree[rchild]);
			if(idx<=m) point_update(lchild,l,m,idx,val);
			else point_update(rchild,m+1,r,idx,val);
			Tree[root].merge(Tree[lchild],Tree[rchild]);
		}
		node range_query(int root, int l, int r, int i, int j) {
			if(i<=l && r<=j) return Tree[root];
			int m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			node nd1=identity, nd2 = nd1;
			Tree[root].split(Tree[lchild],Tree[rchild]);
			if(i<=m) nd1 = range_query(lchild,l,m,i,j);
			if(j>m) nd2 =  range_query(rchild,m+1,r,i,j);
			Tree[root].merge(Tree[lchild],Tree[rchild]);
			node nd; nd.merge(nd1,nd2);
			return nd;
		}
		node point_query(int root, int l, int r, int idx) {
			if(l==r && root==idx) return Tree[root];
			int m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			node nd = identity;
			Tree[root].split(Tree[lchild],Tree[rchild]);
			if(idx<=m) nd = point_query(lchild,l,m,idx);
			else nd = point_query(rchild,m+1,r,idx);
			Tree[root].merge(Tree[lchild],Tree[rchild]);
			return nd;
		}
		void deconstruct() {
			free(Tree);
			identity=(node){0};
			height = sz = L = R = 0;
		}
};

int main()
{
	segTree s;
	node A[100];
	int n,q;
	int z,x,y,w;
	cin>>n>>q;
	node identity = (node){inf};
	// node identity;
	// identity.a=inf;
	for(int i=0;i<n; i++)
	{
		cin>>A[i].a; 
		A[i].c = 0;//doubt
	}
	//insert into this.
	s.init(A,n,identity);
	s.print();
	while(q--)
	{
		cin>>z;
		if(z==0)
		{
			// point update 
			cin>> x >>y;
			s.update(x,y);
			s.print();
		}
		else if(z==1)
		{
			// range update
			cin>>x>>y>>w;
			s.update(x,y,w);
			s.print();
		}
		else if(z==2)
		{
			// point query
			cin>>x;
			node nd = s.query(x);
			s.print();
			cout << " Value : "<<nd.a<<endl;
		}
		else if (z==3)
		{
			// range query
			cin>>x>>y;
			node nd = s.query(x,y);
			s.print();
			cout << " Value : "<<nd.a<<endl;


		}
	}
	return 0;
}