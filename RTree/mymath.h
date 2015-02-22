bool rectangleContainsPoint(double* leftBottom, double* topRight, double* key)
{
	for(int i=0; i<d; i++)
	{
		if(leftBottom[i]>key[i] or topRight[i]<key[i])
			return false;
	}
	return true;
}

bool rectangleIntersectsRectangle(double* lb1, double* tr1, double* lb2, double* tr2)
{
	return (	rectangleContainsPoint(lb1, tr1, lb2) 
			or 	rectangleContainsPoint(lb1, tr1, tr2)
			or	rectangleContainsPoint(lb2, tr2, lb1)
			or 	rectangleContainsPoint(lb2, tr2, tr2)
			);
}	

double euclidDistanceSq(double* p, double* q)
{
	double ans = 0.0;
	for(int i=0; i<d; i++)
	{
		ans = ans + (p[i]-q[i])*(p[i]-q[i]);
	}
	return ans;
}

double findVolume(double* leftBottom, double* topRight)
{
	double ans = 1.0;
	for(int i=0; i<d; i++)
	{
		ans = ans * (topRight[i]-leftBottom[i]);
	}
	return fabs(ans);
}

double* arrayMin(double* key1, double* key2)
{
	double* key = new double[d];
	for(int i=0; i<d; i++)
		key[i] = min(key1[i], key2[i]);
	return key;
}

double* arrayMax(double* key1, double* key2)
{
	double* key = new double[d];
	for(int i=0; i<d; i++)
		key[i] = max(key1[i], key2[i]);
	return key;
}