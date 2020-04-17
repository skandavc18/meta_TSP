#define PROGRAM_FILE "alo.cl"
#define DIM 400
#define SEARCH_AGENTS 150
#define MAX_ITER 1000
#define RAND_MAX 0x7fffffff


typedef struct __attribute__ ((packed)) memb 
{
    int arr[DIM];
    int fitness;
}member;



int ALO(int *graph,int *ub);
