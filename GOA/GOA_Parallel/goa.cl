
#include "defs.h"
int graph2D(__global  int *graph,int row,int col);
int rand_(int* seed);
void print(__global int *arr);
int sum_(__global int *x,__global  int *graph);
void swap_(__global int *a,__global int *b);
void swap(int *a,int *b) ;
void rand_arr(__global int *arr,int* seed);
void randomize (__global int *arr,int* seed);
void randomize_(int *arr,int* seed) ;
void cumsum(double *prob,int *cum_arr);
int roulette_wheel_selection(double *prob,int* seed);
int roulette_wheel_selection_(int arr1, double p1, int arr2, double p2,int* seed);
void normalize_(int *arr,__global  int *graph,__global int *ub);
void correct_path(__global int *arr);
void update_grasshopper(__global  int *graph, __global grasshopper *S, int curr, double c,__global int *ub, __global grasshopper* elite,int* seed);

int graph2D(__global  int *graph,int row,int col)
{
    return graph[DIM * row + col];
}

void print(__global int *arr)
{
    //char temp[DIM+1];
    for(int i=0;i<DIM;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}


int rand_(int* seed) // 1 <= *seed < m
{
    int  a = 16807; //ie 7**5
    int  m = RAND_MAX; //ie 2**32-1

    *seed = (int)(abs((long)(*seed * a)%m));
    return(*seed);
}

int sum_(__global int *x,__global  int *graph)
{
    int s=0;

    for(int i=1;i<DIM;i++)
    {
        s+=graph2D(graph,x[i-1],x[i]);
        //printf("%d %d %d %d\n",i,x[i-1],x[i],graph2D(graph,x[i-1],x[i]));
    }
    s+=graph2D(graph,x[DIM-1],x[0]);
    //printf("%d\n",-s);
    return -s;
}

void swap_(__global int *a,__global int *b) 
{ 
    int temp = *a; 
    *a = *b; 
    *b = temp; 
} 

void swap(int *a,int *b) 
{ 
    int temp = *a; 
    *a = *b; 
    *b = temp; 
} 

void randomize (__global int *arr,int* seed) 
{ 
    for (int i = DIM-1; i >= 0; i--) 
    { 
        // Pick a random index from 0 to i 
        int j = rand_(seed) % (i+1); 
  
        swap_(&arr[i], &arr[j]); 
    } 
} 

void randomize_(int *arr,int* seed) 
{ 
    for (int i = DIM-1; i >= 0; i--) 
    { 
        // Pick a random index from 0 to i 
        int j = rand_(seed) % (i+1); 
  
        swap(&arr[i], &arr[j]); 
    } 
} 

void rand_arr(__global int *arr,int* seed)
{
        for(int j=0;j<DIM;j++)
        {
            arr[j]=j;
        }
        randomize(arr,seed);
}

void cumsum(double *prob,int *cum_arr)
{
    double sum=0;
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        sum+=prob[i];
        cum_arr[i]=sum;
        //printf("%d %f %d\n",i,prob[i],cum_arr[i]);
    }
}

int roulette_wheel_selection_(int arr1, double p1, int arr2, double p2,int* seed)
{
    double r = ((double) rand_(seed) / (RAND_MAX));
    if(r<p1)
        return arr1;
    return arr2;
}

int roulette_wheel_selection(double *prob,int* seed)
{
    int cum_arr[SEARCH_AGENTS];
    cumsum(prob,cum_arr);
    double p=(((double) rand_(seed) / (RAND_MAX))*cum_arr[SEARCH_AGENTS-1]);
    //printf("%d %lf\n",cum_arr[r-1],p);
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        if(cum_arr[i]>p)
            return i;
    }
    return 0;
}

void correct_path(__global int *arr)
{
    int set[DIM];int j=0,k=0;
    for(j=0;j<DIM;j++)
    {
        set[j]=0;
    }    
    for(j=0;j<DIM;j++)
    {
        set[arr[j]]+=1;
    }
    for(j=0;j<DIM;j++)
    {
        if(set[arr[j]]>1)
        {
            for(k=0;k<DIM;k++)
            {
                if(set[k]==0)
                    break;
            }
            if(k!=DIM)
            {
                set[arr[j]]-=1;
                arr[j]=k;
                set[k]=1;
            }
        }
    }
}

void update_grasshopper(__global  int *graph, __global grasshopper *S, int curr, double c,__global int *ub, __global grasshopper* elite,int* seed) 
{

    int set[DIM];
    int temp[DIM];
    double prob[SEARCH_AGENTS];
    double cal,tot;
    int g1,g2;

    for(int i=0;i<DIM;i++)
    {
        temp[i]=-1;
    }
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        prob[i]=0.0;
    }
    //printf("hi");
    for(int i=1;i<DIM;i++)
    {
        for(int j=0;j<SEARCH_AGENTS;j++)
        {
            if(j != curr)
            {
                //printf("1");
                g1 = graph2D(graph,S[j].path[i-1],S[j].path[i]);
                //printf("%d %d %d %d %d %d\n",i,j,g1,g2,curr,S[curr].path[i-1]);
                g2 = graph2D(graph,S[curr].path[i-1],S[curr].path[i]);
                //printf("%d %d\n",g1,g2);
                //printf("3");
                cal = (double)g1-g2;
                //printf("4");
                if(cal > 0)
                    tot = (ub[i]-cal)/ub[i];
                else
                    tot = (ub[i]+cal)/ub[i];
                prob[j]+=tot;
                //printf("%d %d %d %d\n",g1,g2,S[j].path[i-1],S[j].path[i]);
                //printf("3");
            }
        }
    }
    //printf("hi");

    double sum=0.0;

    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        prob[i]=prob[i]/DIM;
        sum += prob[i];
        //printf("%f\n",prob[i]);
    }

    for(int i=0;i<DIM;i++)
    {
        int r = roulette_wheel_selection(prob,seed);
        temp[i] = S[r].path[i];
        //printf("%d %d %d\n",i,temp[i],r);
        temp[i] = roulette_wheel_selection_(temp[i],(1-c),S[curr].path[i],c,seed);
    }

    for(int i=0;i<DIM;i++)
    {
        S[curr].path[i] = roulette_wheel_selection_(temp[i],c,elite->path[i],(1-c),seed);
    }

    correct_path(S[curr].path);
    printf("");
    //printf("%d %d\n",curr,S[curr].fitness);
    //print(S[curr].path);
}

__kernel void grasshopper_swarm(__global grasshopper *S, __global grasshopper* elite, int current_iter, __global  int *graph,__global int *ub,int rand_num) 
{
    int i = get_global_id(0);
    ub[i] = 1000;
    //printf("hi");
    int seed=i+rand_num;
    int c = cmax - current_iter*(cmax - cmin)/MAX_ITER;
    //printf("hi");
    update_grasshopper(graph, S, i, c, ub, elite,&seed);
    //printf("hi");
    S[i].fitness=sum_(S[i].path,graph);
    //printf("%d %d %d\n",i,current_iter,S[i].fitness);
}

__kernel void init_grasshopper(__global grasshopper* arr,int rand_num) 
{
    int i = get_global_id(0);
    int seed=i+rand_num;
    rand_arr(arr[i].path,&seed);
    for(int j=0;j<DIM;j++)
        if(arr[i].path[j]>=DIM)
            arr[i].path[j]=0;
}

__kernel void best_agent(__global grasshopper *arr,__global int *dest)
{
    
    int max = 0;
    for(int i=1;i<SEARCH_AGENTS;i++)
    {
        if(arr[max].fitness<arr[i].fitness)
            max = i;
    }
    printf("");
    //printf("%d %d\n",max,arr[max].fitness);
    //print(arr[max].path);
    *dest=max;
}

__kernel void mem_cpy(__global grasshopper *dst,__global grasshopper *src_buffer,__global int *src)
{
    int i = get_global_id(0);
    if(i==DIM)
    {
        //printf("hi\n");
        dst->fitness=src_buffer[*src].fitness;
    }
    else
    {
        dst->path[i]=src_buffer[*src].path[i];
    }
}

__kernel void mem_replace(__global grasshopper *dst_buffer,__global int *dst,__global grasshopper *src_buffer,__global int *src)
{
    int i = get_global_id(0);
    if(i==DIM)
    {
        dst_buffer[*dst].fitness=src_buffer[*src].fitness;
    }
    else
    {
        dst_buffer[*dst].path[i]=src_buffer[*src].path[i];
    }
}

__kernel void fitness(__global grasshopper *arr,__global  int *graph)
{
    int i = get_global_id(0);
    arr[i].fitness=sum_(arr[i].path,graph);
    printf("");
    //printf("%d %d\n",i,arr[i].fitness);
    //print(arr[i].path);
}
