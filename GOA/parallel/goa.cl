/*
Code that contains kernels to run on accelerator in parallel. A kernel 
represents the basic unit of executable code. Each kernel will be 
executed on one work item ("pixel") of your parallel task:
1 work item = 1 "pixel" in your image 
A practical application may generate thousands or even millions of 
work-items, but for the simple task of adding 64 numbers, 
eight work-items will suffice. The program file add_numbers.cl 
contains a function called add_numbers that performs this operation. 
Like all kernel functions, it returns void and its name is preceded by 
the __kernel identifier.
The kernel has 64 values to add together and eight work-items with 
which to add them. After each work-item computes its sum of eight 
values, these partial results will be added together to form a sum for 
the entire group. In the end, the kernel will return two sums — one for 
each work-group executing the kernel.
data <===== input_buffer
group_result <===== sum_buffer
*/
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
    }
    s+=graph2D(graph,x[DIM-1],x[0]);
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
    for (int i = DIM-1; i > 0; i--) 
    { 
        // Pick a random index from 0 to i 
        int j = rand_(seed) % (i+1); 
  
        swap_(&arr[i], &arr[j]); 
    } 
} 

void randomize_(int *arr,int* seed) 
{ 
    for (int i = DIM-1; i > 0; i--) 
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
    double p=-(((double) rand_(seed) / (RAND_MAX))*cum_arr[SEARCH_AGENTS-1]);
    //printf("%d %lf\n",cum_arr[r-1],p);
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        if((-cum_arr[i])>p)
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

    for(int i=0;i<DIM;i++)
    {
        temp[i]=-1;
    }
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        prob[i]=0.0;
    }
    for(int i=0;i<DIM;i++)
    {
        for(int j=0;j<SEARCH_AGENTS;j++)
        {
            if(j != curr)
            {
                prob[j]+=(ub[i]-abs(graph2D(graph,S[j].path[i-1],S[j].path[i])-graph2D(graph,S[curr].path[i-1],S[curr].path[i])))/ub[i];
                //printf("%f",prob[j]);
            }
        }
        int r = roulette_wheel_selection(prob,seed);
        //printf("%d %d",i,temp[i]);
        temp[i] = S[r].path[i];
    }

    for(int i=0;i<DIM;i++)
    {
        temp[i] = roulette_wheel_selection_(temp[i],(1-c),S[curr].path[i],c,seed);
    }

    for(int i=0;i<DIM;i++)
    {
        S[curr].path[i] = roulette_wheel_selection_(temp[i],c,elite->path[i],(1-c),seed);
    }

    correct_path(S[curr].path);
    //printf("%d",S[curr].fitness);
}

__kernel void grasshopper_swarm(__global grasshopper *S, __global grasshopper* elite, int current_iter, __global  int *graph,__global int *ub,int rand_num) 
{
    int i = get_global_id(0);
    int seed=i+rand_num;
    int c = cmax - current_iter*(cmax - cmin)/MAX_ITER;
    update_grasshopper(graph, S, i, c, ub, elite,&seed);
    S[i].fitness=sum_(S[i].path,graph);
}

__kernel void init_grasshopper(__global grasshopper* arr,int rand_num) 
{
    int i = get_global_id(0);
    int seed=i+rand_num;
    rand_arr(arr[i].path,&seed);
}

__kernel void best_agent(__global grasshopper *arr,__global int *dest)
{
    
    int max = 0;
    for(int i=1;i<SEARCH_AGENTS;i++)
    {
        
        if(arr[max].fitness<arr[i].fitness)
            max = i;
    }
    //printf("hi");
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
    //printf("%d\n",arr[i].fitness);
}