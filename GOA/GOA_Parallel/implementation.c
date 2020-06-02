#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
cl_int err;
void print(int *arr)
{
    for(int i=0;i<DIM;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}

cl_kernel best_agent(cl_program program,cl_mem buffer,cl_mem elite_buffer)
{
    cl_kernel kernel = clCreateKernel(program,"best_agent", &err);
    if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
    };
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    err |= clSetKernelArg(kernel,1, sizeof(cl_mem), &elite_buffer);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
    }
    return kernel;
}

cl_kernel fitness(cl_program program,cl_mem buffer,cl_mem graph)
{
    cl_kernel kernel = clCreateKernel(program,"fitness", &err);
    if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
    };

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    err |= clSetKernelArg(kernel,1, sizeof(cl_mem), &graph);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
    }
    return kernel;
}

cl_kernel grasshopper_swarm(cl_program program,cl_mem S,cl_mem elite,int c,cl_mem graph,cl_mem ub,int rand_num)
{
    cl_kernel kernel = clCreateKernel(program,"grasshopper_swarm", &err);
    if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
    };

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &S);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &elite);
    err |= clSetKernelArg(kernel,2, sizeof(int), &c);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &graph);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &ub);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &rand_num);

    if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
    }
    return kernel;
}

cl_kernel init_grasshopper(cl_program program,cl_mem buffer,int rand_num)
{
    cl_kernel kernel = clCreateKernel(program,"init_grasshopper", &err);
    if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
    };

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(int), &rand_num);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
    }
    return kernel;

}

cl_kernel mem_copy(cl_program program,cl_mem destination,cl_mem src_buffer ,cl_mem source)
{
    cl_kernel kernel = clCreateKernel(program,"mem_cpy", &err);
    if(err < 0) {
      //printf("%d\n",err);
      perror("Couldn't create a kernel");
      exit(1);
    };

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &destination);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &src_buffer);
    err |= clSetKernelArg(kernel,2, sizeof(cl_mem), &source);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
    }
    return kernel;
}

void enqueue(cl_command_queue queue,cl_kernel kernel[6],const size_t *global_sizes,const size_t *local_sizes,int a,int b)
{
    
    for(int i=a;i<b;i++)
    {
        
        err = clEnqueueNDRangeKernel(queue, kernel[i], 1, NULL,&global_sizes[i],&local_sizes[i], 0, NULL, NULL); 
        if(err < 0) {
            printf(" %d %d\n",i,err);
            perror("Couldn't enqueue the kernel");
            exit(1);
        }
        
        
        clFlush(queue);
        clFinish(queue);
    }
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}
cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 


   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}


int GOA(int *graph,int *ub)
{
    srand(time(0)); 
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel[6];
    cl_command_queue queue;
    cl_int err;
    cl_mem S_buffer,graph_buffer,elite_buffer,temp_elite_buffer,ub_buffer;
    
    //int graph1[DIM*DIM];
    //memmove(graph1,graph,sizeof(int)*DIM*DIM);
    grasshopper S[SEARCH_AGENTS];
    grasshopper elite; 

    int current_iter=0;
    int temp_elite=0;
    int local_size = 1;
    int global_size = SEARCH_AGENTS;

    device = create_device();

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
    }

    program = build_program(context, device, PROGRAM_FILE);

    S_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, SEARCH_AGENTS * sizeof(grasshopper),&S, &err); 
    ub_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, DIM * sizeof(int),&ub, &err);
    graph_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, DIM* DIM * sizeof(int),graph, &err); 
    if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
    };

    elite_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, sizeof(grasshopper),&elite, &err); 
    temp_elite_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, sizeof(int),&elite, &err);  
    if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
    };

    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
    };

    const size_t local_sizes[]={local_size,local_size,1,local_size,1,1};
    const size_t global_sizes[]={global_size,global_size,1,global_size,1,DIM+1};

    kernel[0]=init_grasshopper(program,S_buffer,rand());
    kernel[1]=fitness(program,S_buffer,graph_buffer); 
    kernel[2]=best_agent(program,S_buffer,temp_elite_buffer);
    kernel[4]=best_agent(program,S_buffer,temp_elite_buffer);
    kernel[5]=mem_copy(program,elite_buffer,S_buffer,temp_elite_buffer);

    enqueue(queue,kernel,global_sizes,local_sizes,0,3);
    //printf("here");
    while(current_iter<MAX_ITER)
    {
      kernel[3]=grasshopper_swarm(program,S_buffer,elite_buffer,current_iter,graph_buffer,ub_buffer,rand());
      enqueue(queue,kernel,global_sizes,local_sizes,3,6);
      current_iter++;
      clReleaseKernel(kernel[3]);
      //printf("hi");
    }

    clEnqueueReadBuffer(queue,elite_buffer, CL_TRUE, 0,sizeof(grasshopper),&elite, 0, NULL, NULL );

    
    for(int i=0;i<6;i++)
    {
        if(i==3) //because already released
          continue; 
        clReleaseKernel(kernel[i]);
    }
    clReleaseMemObject(S_buffer);
    clReleaseMemObject(graph_buffer);
    clReleaseMemObject(ub_buffer);
    clReleaseMemObject(elite_buffer);
    clReleaseMemObject(temp_elite_buffer);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    //print(elite.path);

    return -elite.fitness;

}