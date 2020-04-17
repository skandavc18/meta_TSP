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
int graph2D(__global const int *graph,int row,int col);
int rand(int* seed);
double drand(int* seed);
void print(__global int *arr);
int sum_(__global int *x,__global const int *graph);
void swap_(__global int *a,__global int *b);
void swap(int *a,int *b) ;
void rand_arr(__global int *arr,int* seed);
void randomize (__global int *arr,int* seed);
void randomize_(int *arr,int* seed) ;
void cumsum(__global member *arr,int *cum_arr);
member roulette_wheel_selection(__global  member *arr,int* seed);
void update_b(__global int *x,double I);
void normalize_(int *arr,__global const int *graph,__global int *ub);
member random_walk(__global const int *graph,__global int *ub,__global member *selected,int curr_iter,int* seed);
member random_walk_(__global const int *graph,__global int *ub,member *selected,int curr_iter,int* seed);
void correct_path(__global int *arr);
void pos_update(__global int *arr,__global const int *graph,member *RA,member *RE);

int graph2D(__global const int *graph,int row,int col)
{
    return graph[DIM * row + col];
}

void print(__global int *arr)
{
    char temp[DIM+1];
    for(int i=0;i<DIM;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}


int rand(int* seed) // 1 <= *seed < m
{
    int const a = 16807; //ie 7**5
    int const m = RAND_MAX; //ie 2**32-1

    *seed = (int)((long)(*seed * a)%m);
    return(*seed);
}

double drand(int* seed)
{
   return (double)rand(seed) / (double)RAND_MAX ;
}

int sum_(__global int *x,__global const int *graph)
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
        int j = rand(seed) % (i+1); 
  
        swap_(&arr[i], &arr[j]); 
    } 
} 

void randomize_(int *arr,int* seed) 
{ 
    for (int i = DIM-1; i > 0; i--) 
    { 
        // Pick a random index from 0 to i 
        int j = rand(seed) % (i+1); 
  
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



void cumsum(__global member *arr,int *cum_arr)
{
    int sum=0;
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        sum+=arr[i].fitness;
        cum_arr[i]=sum;
    }
}

member roulette_wheel_selection(__global  member *arr,int* seed)
{
    int cum_arr[SEARCH_AGENTS];
    cumsum(arr,cum_arr);
    double p=-(drand(seed)*cum_arr[SEARCH_AGENTS-1]);
    //printf("%d %lf\n",cum_arr[r-1],p);
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        if((-cum_arr[i])>(int)p)
            return arr[i];
    }
    return arr[0];
}


void update_b(__global int *x,double I)
{
    if(I==0 || I ==1) return ;
    for(int i=0;i<DIM;i++)
    {
        x[i]=x[i]-(int)(x[i]*I);
    }
}

void normalize_(int *arr,__global const int *graph,__global int *ub)
{
    int i=0,j=0;
    int set[DIM]={0,0};
    int pos[DIM]={0,0};
    for(i=0;i<DIM;i++)
    {
        pos[arr[i]]=i;
    }
    for(i=0;i<DIM-1;i+=2)
    {
        if(graph2D(graph,arr[i],arr[i+1])>ub[i])
        {
            for(j=0;j<DIM;j++)
            {
                if(graph2D(graph,arr[i],j)<=ub[i] && set[j]==0)
                    break;
            }
            if(j!=DIM)
            {
                swap(&arr[i+1],&arr[pos[j]]);
                swap(&pos[arr[i+1]],&pos[j]);
            }

        }
        set[arr[i]]=1;
        set[arr[i+1]]=1;
    }
}

member random_walk(__global const int *graph,__global int *ub,__global member *selected,int curr_iter,int* seed)
{
    double I=1;
    if(curr_iter>MAX_ITER/10)
        I=0.1;

    if(curr_iter>MAX_ITER/2)
        I=0.2;

    if(curr_iter>MAX_ITER*(3/4))
        I=0.2;

    if(curr_iter>MAX_ITER*(0.9))
        I=0.3;

    if(curr_iter>MAX_ITER*(0.95))
        I=0.3;
    
    //update_b(lb,DIM,I); // this will be parallelized
    update_b(ub,I);
    member temp;
    for(int i=0;i<DIM;i++)
        temp.arr[i]=selected->arr[i];
    temp.fitness=selected->fitness;
    randomize_(temp.arr,seed);
    normalize_(temp.arr,graph,ub);
    //print(ub,DIM);
    return temp;

}
member random_walk_(__global const int *graph,__global int *ub,member *selected,int curr_iter,int* seed)
{
    double I=1;
    if(curr_iter>MAX_ITER/10)
        I=0.1;

    if(curr_iter>MAX_ITER/2)
        I=0.2;

    if(curr_iter>MAX_ITER*(3/4))
        I=0.2;

    if(curr_iter>MAX_ITER*(0.9))
        I=0.3;

    if(curr_iter>MAX_ITER*(0.95))
        I=0.3;
    
    //update_b(lb,DIM,I); // this will be parallelized
    update_b(ub,I);
    member temp;
    for(int i=0;i<DIM;i++)
        temp.arr[i]=selected->arr[i];
    temp.fitness=selected->fitness;
    randomize_(temp.arr,seed);
    normalize_(temp.arr,graph,ub);
    //print(ub,DIM);
    return temp;
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

void pos_update(__global int *arr,__global const int *graph,member *RA,member *RE) //may be parallelized
{
    int old_dist=0,ra_dist=0,re_dist=0;
    int set[DIM];
    for(int i=0;i<DIM;i++)
        set[i]=0;
    for(int i=1;i<DIM;i+=2)
    {
        old_dist=graph2D(graph,arr[i-1],arr[i]);
        ra_dist=graph2D(graph,RA->arr[i-1],RA->arr[i]);
        re_dist=graph2D(graph,RE->arr[i-1],RE->arr[i]);
        if(old_dist<ra_dist && old_dist<re_dist && set[arr[i]]==0 && set[arr[i-1]]==0)
        {
            set[arr[i]]+=1;
            set[arr[i-1]]+=1;
            continue;
        }
        else
        {
            if(ra_dist<re_dist  && set[RA->arr[i]]==0 && set[RA->arr[i-1]]==0)
            {
                arr[i]=RA->arr[i];
                arr[i-1]=RA->arr[i-1];
                set[RA->arr[i]]+=1;
                set[RA->arr[i-1]]+=1;
            }
            else
            {
                if( set[RE->arr[i]]==0 && set[RE->arr[i-1]]==0)
                {
                arr[i]=RE->arr[i];
                arr[i-1]=RE->arr[i-1];
                set[RE->arr[i]]+=1;
                set[RE->arr[i-1]]+=1;
                }
                
            }
            
        }

    }
    correct_path(arr);

}


__kernel void random_walk_of_all_ants(__global member *ants, __global member *antlions, __global member* elite, __global const int *graph,__global int *ub,int curr_iter,int rand_num) {

   int i = get_global_id(0);int seed=i+rand_num;
   member antlion_select=roulette_wheel_selection(antlions,&seed);
   member RA=random_walk_(graph,ub,&antlion_select,curr_iter,&seed);
   member RE=random_walk(graph,ub,elite,curr_iter,&seed);
   pos_update(ants[i].arr,graph,&RA,&RE);
   //printf("%d\n",ants[i].fitness);
}


__kernel void rand_init(__global member* arr,int rand_num) {

     int i = get_global_id(0);int seed=i+rand_num;
     rand_arr(arr[i].arr,&seed);
}

__kernel void  max_fitness(__global member *arr,__global int *dest)
{
    int max=arr[0].fitness;int pos=0;
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        if(arr[i].fitness>max)
        {
            max=arr[i].fitness;
            pos=i;
        }
    }

    *dest=pos;
}

__kernel void  min_fitness(__global member *arr,__global int *dest)
{
    int min=0;int pos=0;
    for(int i=0;i<SEARCH_AGENTS;i++)
    {
        if(arr[i].fitness<min)
        {
            min=arr[i].fitness;
            pos=i;
        }
    }
    *dest=pos;
}

__kernel void  mem_cpy(__global member *dst,__global member *src_buffer,__global int *src)
{
    int i = get_global_id(0);
    if(i==DIM)
    {
        //printf("hi\n");
        dst->fitness=src_buffer[*src].fitness;
    }
    else
    {
        dst->arr[i]=src_buffer[*src].arr[i];
    }
}

__kernel void  mem_replace(__global member *dst_buffer,__global int *dst,__global member *src_buffer,__global int *src)
{
    int i = get_global_id(0);
    if(i==DIM)
    {
        dst_buffer[*dst].fitness=src_buffer[*src].fitness;
    }
    else
    {
        dst_buffer[*dst].arr[i]=src_buffer[*src].arr[i];
    }
}

__kernel void fitness(__global member *arr,__global const int *graph)
{
    int i = get_global_id(0);
    arr[i].fitness=sum_(arr[i].arr,graph);
}