#include "defs.h"
#include <stdio.h>
int main() {
   
   int graph[DIM*DIM]= {0,0};
   int ub[DIM]={0,0};
   for(int i=0;i<DIM;i++)
        ub[i]=1000;
   for(int i=0;i<DIM*DIM;i++)
   {
      scanf_s("%d",&graph[i]);
   }
   printf("%d",GOA(graph,ub));
   return 0;
}