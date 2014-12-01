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
	ll sum, carry, range;
	void merge(node &l, node &r) {
		sum = l.sum + r.sum;
		range = l.range + r.range;
	}
	void split(node &l, node &r) {
		l.carry += carry;
		r.carry += carry;
		l.sum += (l.range*carry);
		r.sum += (r.range*carry);
		carry = 0;
	}
	void update(ll val) {
		sum += (range*val);
		carry += val;
	}
	void print() {
		cout<<sum<<" "<<carry<<" "<<range<<endl;
	}
};

class segTree
{
	/*
	* CREDITS: Triveni Mahatha and Utkarsh Lath
	*/
	public:
		void init(node * A, ll n, node nd) {
			create(A,n,nd);
		}
		void update(ll l, ll r, ll val) {
			range_update(1,L,R,l+L,r+L,val);
		}
		void update(ll idx, ll val) {
			poll_update(1,L,R,idx+L,val);
		}
		node query(ll l, ll r) {
			return range_query(1,L,R,l+L,r+L);
		}
		node query(ll idx) {
			return poll_query(1,L,R,idx+L);
		}
		void print() {
			for(ll i=1;i<sz;i++) { 
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
		ll height;		
		ll L,R;
		ll sz;	
		void create(node * A, ll n,node nd) {
			height = ceil(log2(n));
			L = 1<<height, R = (L<<1)-1;
			sz = R+1;
			identity = nd;
			Tree = (node*)calloc(sz,sizeof(node));
			for(ll i=0;i<sz;i++)
				Tree[i]=nd;
			for(ll i=0;i<n;i++)
				Tree[L+i]=A[i];
			initialise(1,L,R);
			return ;
		}
		void initialise(ll root, ll l, ll r) {
			if(l==r) return;
			ll m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			initialise(lchild,l,m);
			initialise(rchild,m+1,r);
			Tree[root].merge(Tree[lchild],Tree[rchild]);
		}
		void range_update(ll root, ll l, ll r, ll i, ll j, ll val) {
			if(i<=l && r<=j) { Tree[root].update(val); return ;}
			ll m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			Tree[root].split(Tree[lchild],Tree[rchild]);
			if(i<=m) range_update(lchild,l,m,i,j,val);
			if(j>m) range_update(rchild,m+1,r,i,j,val);
			Tree[root].merge(Tree[lchild],Tree[rchild]);
		}
		void poll_update(ll root, ll l, ll r, ll idx, ll val) {
			if(l==r && root==idx){ Tree[root].update(val); return ;}
			ll m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			Tree[root].split(Tree[lchild],Tree[rchild]);
			if(idx<=m) poll_update(lchild,l,m,idx,val);
			else poll_update(rchild,m+1,r,idx,val);
			Tree[root].merge(Tree[lchild],Tree[rchild]);
		}
		node range_query(ll root, ll l, ll r, ll i, ll j) {
			if(i<=l && r<=j) return Tree[root];
			ll m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			node nd1=identity, nd2 = nd1;
			Tree[root].split(Tree[lchild],Tree[rchild]);
			if(i<=m) nd1 = range_query(lchild,l,m,i,j);
			if(j>m) nd2 =  range_query(rchild,m+1,r,i,j);
			Tree[root].merge(Tree[lchild],Tree[rchild]);	
		node nd; nd.merge(nd1,nd2);
			return nd;
		}
		node poll_query(ll root, ll l, ll r, ll idx) {
			if(l==r && root==idx) return Tree[root];
			ll m = (l+r)>>1, lchild = root<<1, rchild = lchild|1;
			node nd = identity;
			Tree[root].split(Tree[lchild],Tree[rchild]);
			if(idx<=m) nd = poll_query(lchild,l,m,idx);
			else nd = poll_query(rchild,m+1,r,idx);
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
	node identity = (node){0,0,0};
	node A[100005];

	ll t,n,c,p,q,v,z;
	scanf("%lld",&t);
	while(t--)
	{
		scanf("%lld %lld",&n,&c);
		for(int i=0; i<n; i++)
		{
			A[i] = (node){0,0,1};
		}
		segTree s;
		s.init(A,n,identity);
		// s.print()
;		while(c--)
		{
			scanf("%lld",&z);
			if(z==0)
			{
				// update
				scanf("%lld %lld %lld",&p,&q,&v);
				s.update(p-1,q-1,v);
				// s.print();
			}
			else
			{
				// query
				scanf("%lld %lld",&p,&q);
				node nd = s.query(p-1,q-1);
				printf("%lld\n",nd.sum);
				// s.print();
			}
		}
	}
	
	return 0;
}