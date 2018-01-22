//$Id: floatop.cpp 3380 2014-11-26 19:58:59Z marianne.rieckmann $

#include <math.h>       /* for sin, exp etc.           */
#include <stdio.h>      /* standard I/O                */ 
#include <string.h>     /* for strcpy - 3 occurrences  */
#include <stdlib.h>     /* for exit   - 1 occurrence   */

class floatop 
{

public:

	void loop(unsigned long long num, bool bSilent=true)
	{
		/* given number of operations, divide by number or basic operations in set */
		unsigned long long numLoops = (num / 16);
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
			floatops( xG, x1000000000, bSilent);
			numLoops = numLoops % x1000000000;
		}
		if (numLoops > x1000000)
		{
			xM = (long)(numLoops / x1000000);
			floatops( xM, x1000000, bSilent);
			numLoops = numLoops % x1000000;
		}
		if (numLoops > x1000)
		{
			xK = (long)(numLoops / x1000);
			floatops( xK, x1000, bSilent);
			numLoops = numLoops % x1000;
		}
		if (numLoops >= x1)
		{
			floatops( (long) numLoops, x1, bSilent);
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
	void floatops( long xtra, long x100, bool bSilent)
	{

		long i,ix;
		float e1[4];

        float t =  (float)0.49999975;
        float t0 = t;        
        float t1 = (float)0.50000025;
        float t2 = 2.0;

        /* Float Array elements */

        e1[0] = 1.0;
        e1[1] = -1.0;
        e1[2] = -1.0;
        e1[3] = -1.0;
         {
            for (ix=0; ix<xtra; ix++)
              {
                for(i=0; i<x100; i++)
                  {
                      e1[0] = (e1[0] + e1[1] + e1[2] - e1[3]) * t;
                      e1[1] = (e1[0] + e1[1] - e1[2] + e1[3]) * t;
                      e1[2] = (e1[0] - e1[1] + e1[2] + e1[3]) * t;
                      e1[3] = (-e1[0] + e1[1] + e1[2] + e1[3]) * t;
                  }
                t = (float)1.0 - t;
              }
            t =  t0;                    
         }
		if(!bSilent)
		{
			printf("\nFloatOp  x100  loops  result    ");
			printf("\n%ld %ld %9.3f\n",x100,xtra,e1[3]);
		}
		return;
	}

}; // end of class



