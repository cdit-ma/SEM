//$Id: intop.cpp 3380 2014-11-16 19:58:59Z marianne.rieckmann $

#include <math.h>       /* for sin, exp etc.           */
#include <stdio.h>      /* standard I/O                */ 
#include <string.h>     /* for strcpy - 3 occurrences  */
#include <stdlib.h>     /* for exit   - 1 occurrence   */

class integerop 
{

public:

	void loop(unsigned long long num, bool bSilent=true)
	{
		/* given number of operations, divide by number or basic operations in set */
		unsigned long long numLoops = (num / 15);
		/* always execute minimum basic operations */  
		if (numLoops <= 0) numLoops = 1;

		/* if requesting a very large number of operations divide into manageable chunks */
		long x1000000000 = 1000000000;
		long x1000000 = 1000000;
		long x1000 = 1000;
		long x1 = 1;
		long xG = 0, xM = 0, xK = 0;

		/* process number of operations in chunks */
		if (numLoops > x1000000000)
		{
			xG = (long)(numLoops / x1000000000);
			integerops( xG, x1000000000, bSilent);
			numLoops = numLoops % x1000000000;
		}
		if (numLoops > x1000000)
		{
			xM = (long)(numLoops / x1000000);
			integerops( xM, x1000000, bSilent);
			numLoops = numLoops % x1000000;
		}
		if (numLoops > x1000)
		{
			xK = (long)(numLoops / x1000);
			integerops( xK, x1000, bSilent);
			numLoops = numLoops % x1000;
		}
		if (numLoops >= x1)
		{
			integerops( (long) numLoops, x1, bSilent);
		}

		if(!bSilent)
		{    
			printf("\nIntop num   xG   xM   xK   x  ");
			printf("\n%lld %ld %ld %ld %lld\n",num,xG,xM,xK,numLoops);
		}// end bSilent

	}

	/* Basic Operations for arithmetic [+ - * / %] and assignment [=]
	   Code snipt taken from Whetstone Benchmark in C/C++
	*/
	void integerops( long xtra, long x100, bool bSilent)
	{

		long i,ix;
		int x;              
		long j,k,l;
		int e1[4];

		/* Integer arithmetic */
		j = 1;
		k = 2;
		l = 3;
		e1[0] = 0;
		e1[1] = 0;
		{
			for (ix=0; ix<xtra; ix++)
			{
				for(i=0; i<x100; i++)
				{
					j = j *(k-j)*(l-k);
					k = l * k - (l-j) * k;
					l = (l-k) * (k+j);
					e1[l-2] = e1[l-2] + j + k + l;
					e1[k-2] = e1[k-2] + j * k * l;

				}
			}
		}
		x = (e1[0]+e1[1])/(long)x100/(long)xtra;   
		if(!bSilent)
		{
			printf("\nIntOp  x100  loops  result    ");
			printf("\n%ld %ld %d\n",x100,xtra,x);
		}
		return;
	}

}; // end of class



