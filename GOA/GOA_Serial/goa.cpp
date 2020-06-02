#include <cstdlib>
#include <bits/stdc++.h> 
#include <iostream> 
using namespace std; 

#define DIM 50 
#define SEARCH_AGENTS 150
#define MAX_ITER 1000
#define CMAX 1
#define CMIN 0.00001

typedef struct grasshopper
{
    int *path;
    int fitness;
}grasshopper;

void swap(int *a, int *b) //function to swap values
{ 
    int temp = *a; 
    *a = *b; 
    *b = temp; 
} 

int sum(int *x,int **graph,int n) //fitness function used for TSP
{
    int s=0;

    for(int i=1;i<n;i++)
    {
        s+=graph[x[i-1]][x[i]];
    }
    s+=graph[x[n-1]][x[0]];
    return -s;
}

void randomize(int *arr, int n) //function to create random solutions
{ 
    for (int i = n-1; i > 0; i--) 
    { 
        int j = rand() % (i+1); 
        swap(&arr[i], &arr[j]); 
    } 
} 

double distance(int *a, int *b, int dim) //function to calculate distance
{
    double d = 0.0;
    int i = 0;
    while(i<dim)
    {
        d += pow((a[i]-b[i]),2);
        i++;
    }
    return sqrt(d);
}

grasshopper* init_grasshopper(int dim)  //function to initialise a grasshopper
{
    grasshopper *S = (grasshopper *)malloc(sizeof(grasshopper));
    S->path = (int *)malloc(dim*sizeof(int));
    for(int i=0;i<dim;i++)
        S->path[i] = i;
    randomize(S->path,dim);
    return S;
}

void correct_path(int *arr,int dim)  //function to correct the path if is not a possible path
{
    
    int *set=(int *)calloc(dim,sizeof(int));int j=0,k=0;
    for(j=0;j<dim;j++)
    {
        set[arr[j]]+=1;
    }
    for(j=0;j<dim;j++)
    {
        if(set[arr[j]]>1)
        {
            for(k=0;k<dim;k++)
            {
                if(set[k]==0)
                    break;
            }
            if(k!=dim)
            {
                set[arr[j]]-=1;
                arr[j]=k;
                set[k]=1;
            }
        }
    }
    free(set);
}

int roulette_wheel_selection(int arr1, double p1, int arr2, double p2)  //function to perform roulette wheel selection
{
    double r = ((double) rand() / (RAND_MAX));
    if(r<p1)
        return arr1;
    return arr2;
}

void cumsum(double *prob, double *cum_arr, int r) //function to calculate cumulative sum
{
    double sum=0;
    for(int i=0;i<r;i++)
    {
        sum+=prob[i];
        cum_arr[i]=sum;
    }
}

int roulette_wheel_selection(double *prob, int search_agents, int dim)
{
    double cum_arr[search_agents];
    cumsum(prob, cum_arr, search_agents);
    double p=(((double) rand() / (RAND_MAX))*cum_arr[search_agents-1]);
    //printf("%d %lf\n",cum_arr[r-1],p);
    for(int i=0;i<search_agents;i++)
    {
        if(cum_arr[i]>p)
            return i;
    }
    return 0;
}

void print(int *arr,int n)
{
    for(int i=0;i<n;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}

void update_grasshopper(int **graph, grasshopper **S, int curr, int search_agents, double c, int *ub, int *lb, int elite, int dim) //function to update grasshopper solution
{
    int curr_dist=0,elite_dist=0,temp_dist=0,dist=0;
    int *set=(int *)calloc(dim,sizeof(int));
    int *temp=(int *)calloc(dim,sizeof(int));
    double *prob=(double *)calloc(search_agents,sizeof(double));

    for(int i=0;i<dim;i++)
    {
        temp[i]=-1;
    }
    for(int i=0;i<search_agents;i++)
    {
        prob[i]=0.0;
    }
    for(int i=0;i<dim;i++)
    {
        for(int j=0;j<search_agents;j++)
        {
            if(j != curr)
            {
                prob[j]+=(ub[i]-abs(graph[S[j]->path[i-1]][S[j]->path[i]]-graph[S[curr]->path[i-1]][S[curr]->path[i]]))/ub[i];  //asssign probability to each search agent
            }
        }
        int r = roulette_wheel_selection(prob, search_agents, dim);
        temp[i] = S[r]->path[i];
    }
    for(int i=0;i<dim;i++)
    {
        temp[i] = roulette_wheel_selection(temp[i],(1-c),S[curr]->path[i],c); //select a random search agent using roulette wheel selection
    }
    for(int i=0;i<dim;i++)
    {
        S[curr]->path[i] = roulette_wheel_selection(temp[i],c,S[elite]->path[i],(1-c));  //update path based on random search agent and elite search agent
    }
    correct_path(S[curr]->path,dim);
    free(set);
}

int best_agent(grasshopper **S, int search_agents) //function to find elite search agent
{
    int max = 0;
    for(int i=1;i<search_agents;i++)
    {
        if(S[max]->fitness<S[i]->fitness)
            max = i;
    }
    return max;
}

int GOA(int search_agents, int dim, double cmax, double cmin, int Max_iterations, int **graph, int *lb, int *ub, int (*func)(int *,int **,int))
{
    grasshopper *S[search_agents];
    for(int i=0;i<search_agents;i++) 
        S[i] = init_grasshopper(dim);
    for(int i=0;i<search_agents;i++)  
    {
        S[i]->fitness=func(S[i]->path,graph,dim);
    }
    int elite = best_agent(S, search_agents);
    int l=0;
    double c = 0.0;
    while(l < Max_iterations)
    {
        c = cmax - l*(cmax - cmin)/Max_iterations;
        for(int i=0;i<search_agents;i++)
        {
            update_grasshopper(graph, S, i, search_agents, c, ub, lb, elite, dim);
            S[i]->fitness=func(S[i]->path,graph,dim);
        }
        int new_elite = best_agent(S, search_agents);
        elite = new_elite;
        l = l+1;
    }
    return -S[elite]->fitness;
}

int main()
{
    int r=DIM,c=DIM;
    int **graph = (int **)malloc(r * sizeof(int *)); 
    for (int i=0; i<r; i++) 
        graph[i] = (int *)malloc(c * sizeof(int)); 
    for(int i=0;i<DIM;i++)
    {
        for(int j=0;j<DIM;j++)
        {
            scanf("%d",&graph[i][j]);
        }
    }
    int lb[DIM]={0};
    int ub[DIM]={0};
    for(int i=0;i<DIM;i++)
        ub[i]=1000;
    int res=GOA(SEARCH_AGENTS,DIM,CMAX,CMIN,MAX_ITER,graph,lb,ub,sum);
    printf("%d",res);
    for (int i=0; i<r; i++) 
        free(graph[i]);
    free(graph); 
    return 0;
}