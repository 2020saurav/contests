#include <bits/stdc++.h>
using namespace std;
/*
* Suffix Array Function
* Input: string s (of length n)
* Return Value: a pointer to the suffix array of s (0 based indexing)
* Call: int * suffixArray = getSuffixArray(s);
* Time: O(n lgn lgn)
* Update the size of P as per requirement. P[n][m]. m must be atleast ceil(log2(n))+2 
* Similarly update L[n]
* use std as namespace, else add std to below keywords
* Acknowledgement: http://www.stanford.edu/class/cs97si/suffix-array.pdf
* Author: Saurav Kumar <2020saurav@gmail.com>
*/
int P[65536][18];
pair< pair<int,int>,int > L [65536];
int* getSuffixArray(string s)
{
	int n = s.size();
	int m = ceil(log2(n))+2;
	int step, count;
	int * SA = (int*) calloc(n, sizeof(int));
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
	return SA;
}
int main()
{
	string s = "banana";
	int * a = getSuffixArray(s);
	for(int i=0; i<s.size(); i++)
		cout<<a[i]<<" ";
	cout<<endl;
	return 0;
}