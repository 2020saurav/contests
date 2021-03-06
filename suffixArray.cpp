#include <bits/stdc++.h>
using namespace std;

class SuffixArray
{
	private:
		string s; int n,m,step,count;
		int P[18][65536];
		pair< pair<int,int>,int > L [65536];
	public:
		int * SA; // pointer to actual suffix array
		int size()
		{
			return n;
		}
		SuffixArray(string s)
		{
			/*
			* Suffix Array
			* Input: string s (of length n)
			* Time: O(n lgn lgn)
			* Update the size of P as per requirement. P[m][n]. m must be atleast ceil(log2(n))+2 
			* Similarly update L[n]
			* Acknowledgement: http://www.stanford.edu/class/cs97si/suffix-array.pdf
			* Author: Saurav Kumar <2020saurav@gmail.com>
			*/
			s = s;
			n = s.size();
			m = ceil(log2(n))+2;
			SA = (int*) calloc(n, sizeof(int));
			vector<pair<int,int> > keyValue(n);
			for(int i=0; i<n; i++) {
				P[0][i] = s[i] - 'a';
			}
			for(step = 1, count = 1; count>>1 < n; step++, count<<=1) {
				for(int i=0; i<n; i++) {
					L[i].first.first 	= P[step-1][i];
					L[i].first.second 	= (i+count < n) ? (P[step-1][i+count]) : (-1);
					L[i].second 		= i;
				}
				sort(L,L+n);
				for(int i=0; i<n; i++) {
					P[step][L[i].second] = ((i>0) && (L[i].first.first == L[i-1].first.first) && (L[i].first.second == L[i-1].first.second)) ? P[step][L[i-1].second] : i;
				}
			}
			for(int i=0; i<n; i++)
				keyValue[i] = make_pair(P[step-1][i],i);
			sort(keyValue.begin(),keyValue.end());
			for(int i=0; i<n; i++)
				SA[i]=keyValue[i].second;
		}
		int lcp(int x, int y)
		{
			int k, ret = 0; 
			if (x == y) 
				return n - x;
 			for (k = step - 1; k >= 0 && x < n && y < n; k --)
 				if (P[k][x] == P[k][y])
 					x += 1 << k, y += 1 << k, ret += 1 << k; 
 			return ret; 
		}


};

int main()
{
	SuffixArray s1("banana");
	SuffixArray *s2 = new SuffixArray("banana");
	for(int i=0; i<s1.size(); i++)
		cout<<s1.SA[i]<<" ";
	cout<<endl;
	for(int i=0;i<s1.size(); i++)
	{
		for(int j=0; j<s1.size(); j++)
			cout<<"LCP("<<i<<","<<j<<"): "<<s1.lcp(i,j)<<endl;
		cout<<endl;
	}
	
	return 0;
}