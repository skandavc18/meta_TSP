#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#define DIM 50
#define POPULATION 150
#define MAX_ITER 2000
typedef struct memb
{
    int *arr;
    int fitness;
}member;


void print(int *arr,int n) // to print the path
{
    for(int i=0;i<n;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}

unsigned int rand_()
{
   /*static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
   unsigned int b;
   b  = ((z1 << 6) ^ z1) >> 13;
   z1 = ((z1 & 4294967294U) << 18) ^ b;
   b  = ((z2 << 2) ^ z2) >> 27; 
   z2 = ((z2 & 4294967288U) << 2) ^ b;
   b  = ((z3 << 13) ^ z3) >> 21;
   z3 = ((z3 & 4294967280U) << 7) ^ b;
   b  = ((z4 << 3) ^ z4) >> 12;
   z4 = ((z4 & 4294967168U) << 13) ^ b;
   return (z1 ^ z2 ^ z3 ^ z4);*/
   
   return rand();
}

double drand_()// to generate random numbers
{
   return (double)rand_() / (double)DIM ; 
}

int sum(int *x,int **graph,int n)
{
    int s=0;

    for(int i=1;i<n;i++)
    {
        s+=graph[x[i-1]][x[i]];
    }
    s+=graph[x[n-1]][x[0]];
    return -s;
}

member *new_(int r,int c)
{
    member *temp=(member*)malloc(sizeof(member)*r);
    for(int i=0;i<r;i++)
    {
        temp[i].arr=(int *)calloc(c,sizeof(int));
        temp[i].fitness=0;
    }
    return temp;
}

void fitness(member *arr,int r,int c,int **graph,int (*func)(int *,int **,int))
{
    for(int i=0;i<r;i++)
    {
        arr[i].fitness=func(arr[i].arr,graph,c);
        //printf("%d\n",arr[i].fitness);
    }
}
void swap (int *a, int *b) 
{ 
    int temp = *a; 
    *a = *b; 
    *b = temp; 
} 

void randomize ( int *arr, int n ) 
{ 
    // Use a different seed value so that we don't get same 
    // result each time we run this program 
    //srand ( time(NULL) ); 
  
    // Start from the last element and swap one by one. We don't 
    // need to run for the first element that's why i > 0 
    for (int i = n-1; i > 0; i--) 
    { 
        // Pick a random index from 0 to i 
        int j = rand_() % (i+1); 
  
        // Swap arr[i] with the element at random index 
        swap(&arr[i], &arr[j]); 
    } 
} 

void rand_arr(int *arr,int c)
{
        for(int j=0;j<c;j++)
        {
            arr[j]=j;
        }
        randomize(arr,c);
}

void rand_init(member *arr,int r,int c) // initialization of the population
{
    for(int i=0;i<r;i++)// parallelize this loop
    {
        rand_arr(arr[i].arr,c);
        //print(arr[i].arr,c);
    }
}

int max_fitness(member *arr,int r,int c) // find min fittness
{
    int max=arr[0].fitness;int pos=0;
    for(int i=0;i<r;i++)
    {
        if(arr[i].fitness>max)
        {
            max=arr[i].fitness;
            pos=i;
        }
    }
    return pos;
}

int min_fitness(member *arr,int r,int c)
{
    int min=0;int pos=0;
    for(int i=0;i<r;i++)
    {
        if(arr[i].fitness<min)
        {
            min=arr[i].fitness;
            pos=i;
        }
    }
    return pos;
}

void cumsum(member *arr,int *cum_arr,int r,int c) // to find cumulative sum
{
    int sum=0;
    for(int i=0;i<r;i++)
    {
        sum+=arr[i].fitness;
        cum_arr[i]=sum;
    }
}

member *roulette_wheel_selection(member *arr,int r,int c) // implements roulette wheel selection
{
    int cum_arr[r];
    cumsum(arr,cum_arr,r,c);
    double p=-(drand_()*cum_arr[r-1]);
    //printf("%d %lf\n",cum_arr[r-1],p);
    for(int i=0;i<r;i++)
    {
        if((-cum_arr[i])>(int)p)
            return &arr[i];
    }
    return &arr[0];
}


void update_b(int *x,int dim,double I)
{
    if(I==0 || I ==1) return ;
    for(int i=0;i<dim;i++)
    {
        x[i]=x[i]-(int)(x[i]*I);
    }
}

void normalize(int *arr,int **graph,int *lb,int *ub,int dim) // function to normalize the paths.
{ 
    int i=0,j=0;
    int *set=(int *)calloc(dim,sizeof(int));
    int *pos=(int *)calloc(dim,sizeof(int));
    for(i=0;i<dim;i++)
    {
        pos[arr[i]]=i;
    }
    for(i=0;i<dim-1;i+=2)
    {
        if(graph[arr[i]][arr[i+1]]>ub[i])
        {
            for(j=0;j<dim;j++)
            {
                if(graph[arr[i]][j]<=ub[i] && set[j]==0)
                    break;
            }
            if(j!=dim)
            {
                swap(&arr[i+1],&arr[pos[j]]);
                swap(&pos[arr[i+1]],&pos[j]);
            }

        }
        set[arr[i]]=1;
        set[arr[i+1]]=1;
    }
    free(set);
    free(pos);
}

member* random_walk(int dim,int max_iter,int **graph,int *lb,int *ub,member *selected,int curr_iter)
{
    // this function implements random walk of ants presented in the paper
    double I=1;
    if(curr_iter>max_iter/10)
        I=0.1;

    if(curr_iter>max_iter/2)
        I=0.2;

    if(curr_iter>max_iter*(3/4))
        I=0.2;

    if(curr_iter>max_iter*(0.9))
        I=0.3;

    if(curr_iter>max_iter*(0.95))
        I=0.5;
    
    //update_b(lb,dim,I); // this will be parallelized
    update_b(ub,dim,I);
    member *temp=(member*)malloc(sizeof(member));
    temp->arr=(int*)malloc(sizeof(int)*dim);
    memmove(temp->arr,selected->arr,sizeof(int)*dim);
    temp->fitness=selected->fitness;
    randomize(temp->arr,dim);
    normalize(temp->arr,graph,lb,ub,dim);
    //print(ub,dim);
    return temp;

}

void correct_path(int *arr,int dim) // function to correct the path. If there are repated nodes in the path, it replaces them with nodes not repeated
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

void pos_update(int *arr,int **graph,member *RA,member *RE,int dim) // ant position update. 
{
    int old_dist=0,ra_dist=0,re_dist=0,j=0;
    int *set=(int *)calloc(dim,sizeof(int));
    
    for(int i=1;i<dim;i+=2)
    {
        old_dist=graph[arr[i-1]][arr[i]];
        ra_dist=graph[RA->arr[i-1]][RA->arr[i]];
        re_dist=graph[RE->arr[i-1]][RE->arr[i]];
        if(old_dist<ra_dist && old_dist<re_dist && set[arr[i]]==0 && set[arr[i-1]]==0)
        {
            set[arr[i]]+=1;
            set[arr[i-1]]+=1;
        }
        else
        {
            if(ra_dist<re_dist  && set[RA->arr[i]]==0 && set[RA->arr[i-1]]==0)
            {
                if(ra_dist<old_dist)
                {
                arr[i]=RA->arr[i];
                arr[i-1]=RA->arr[i-1];
                set[RA->arr[i]]+=1;
                set[RA->arr[i-1]]+=1;
                }
                else
                {
                    set[arr[i]]+=1;
                    set[arr[i-1]]+=1;
                }
                
            }
            else
            {
                if(re_dist<old_dist && set[RE->arr[i]]==0 && set[RE->arr[i-1]]==0)
                {
                arr[i]=RE->arr[i];
                arr[i-1]=RE->arr[i-1];
                set[RE->arr[i]]+=1;
                set[RE->arr[i-1]]+=1;
                }
                else
                {
                    set[arr[i]]+=1;
                    set[arr[i-1]]+=1;
                }
                
                
            }
            
        }

    }
    correct_path(arr,dim);
    free(set);

}



int ALO(int search_agents,int dim,int max_iter,int **graph,int *lb,int *ub,int (*func)(int *,int **,int))
{

    member *antlions=new_(search_agents,dim);
    member *ants=new_(search_agents,dim);

    rand_init(antlions,search_agents,dim);// a loop inside this will be parallelized
    rand_init(ants,search_agents,dim);

    fitness(antlions,search_agents,dim,graph,func); // this will be parallelized
    member *elite=&antlions[max_fitness(antlions,search_agents,dim)];
    
    int temp_elite=0;int pos=0;
    int current_iter=0;

    while(current_iter<max_iter)
    {
        
        for(int i=0;i<search_agents;i++) // this loop will be parallelized
        {
            member *antlion_select=roulette_wheel_selection(antlions,search_agents,dim);
            member *RA=random_walk(dim,max_iter,graph,lb, ub,antlion_select,current_iter);
            member *RE=random_walk(dim,max_iter,graph,lb, ub,elite,current_iter);
            pos_update(ants[i].arr,graph,RA,RE,dim);
            free(RA);
            free(RE);
        }
        
        fitness(ants,search_agents,dim,graph,func); 
        temp_elite=max_fitness(ants,search_agents,dim);
        if(ants[temp_elite].fitness>elite->fitness)
        {
            pos=min_fitness(antlions,search_agents,dim);
            memmove(antlions[pos].arr,ants[temp_elite].arr,sizeof(int)*dim);
            antlions[pos].fitness=ants[temp_elite].fitness;
            elite=&antlions[pos];
            //print(elite->arr,dim);
        }
        
        current_iter++;
    }
    //print(elite->arr,dim);
    int temp=-elite->fitness;
    free(antlions);
    free(ants);
    return temp;

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
            //scanf_s("%d",&graph[i][j]);
            scanf("%d",&graph[i][j]);
        }
    }
    int lb[DIM]={0};
    int ub[DIM]={0};
    for(int i=0;i<DIM;i++)
        ub[i]=1000;
    printf("%d\n",ALO(POPULATION,DIM,MAX_ITER,graph,lb,ub,sum));
    //ALO(POPULATION,DIM,MAX_ITER,graph,lb,ub,sum);
    for (int i=0; i<r; i++) 
        free(graph[i]);
    free(graph); 
    return 0;
}