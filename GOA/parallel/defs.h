#define PROGRAM_FILE "goa.cl"
#define DIM 400
#define SEARCH_AGENTS 150
#define MAX_ITER 1000
#define cmax 1
#define cmin 0.00001
#define RAND_MAX 0x7fffffff


typedef struct __attribute__ ((packed)) grasshopper
{
    int path[DIM];
    int fitness;
}grasshopper;


int GOA(int *graph,int *ub);
