#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include<CL/cl.h>
cl_int err;
void print(int *arr) // function to print the path
{
    for(int i=0;i<DIM;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}

cl_kernel max_fitness(cl_program program,cl_mem buffer,cl_mem elite_buffer) // function to create kernel for calculating max fitness
{
    cl_kernel kernel = clCreateKernel(program,"max_fitness", &err);
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


cl_kernel min_fitness(cl_program program,cl_mem buffer,cl_mem least_elite_buffer) // function to create kernel to calcuate minimum fittness
{
    cl_kernel kernel = clCreateKernel(program,"min_fitness", &err);
    if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
    };
    cl_mem temp_buffer;
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    err |= clSetKernelArg(kernel,1, sizeof(cl_mem), &least_elite_buffer);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
    }
    return kernel;
}

cl_kernel rand_init(cl_program program,cl_mem buffer,int rand_num) // function to create kernel for initialization of the population
{
    cl_kernel kernel = clCreateKernel(program,"rand_init", &err);
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

cl_kernel fitness(cl_program program,cl_mem buffer,cl_mem graph) // function to create kernel for calculating fitness
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

cl_kernel random_walk_of_all_ants(cl_program program,cl_mem ants, cl_mem antlions,cl_mem elite_buffer,cl_mem graph,cl_mem ub,int curr_iter,int rand_num)
{
    cl_kernel kernel = clCreateKernel(program,"random_walk_of_all_ants", &err); // kernel for random walk of ants
    if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
    };

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &ants);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &antlions);
    err |= clSetKernelArg(kernel,2, sizeof(cl_mem), &elite_buffer);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &graph);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &ub);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &curr_iter);
    err |= clSetKernelArg(kernel, 6, sizeof(int), &rand_num);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
    }
    return kernel;
}

cl_kernel mem_copy(cl_program program,cl_mem destination,cl_mem src_buffer ,cl_mem source)
{
    cl_kernel kernel = clCreateKernel(program,"mem_cpy", &err); // kernel to copy the elite antlion
    if(err < 0) {
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

cl_kernel mem_replace(cl_program program,cl_mem dest_buffer,cl_mem destination,cl_mem src_buffer,cl_mem source)
{
    cl_kernel kernel = clCreateKernel(program,"mem_replace", &err); // kernel to replace the elite antlion
    if(err < 0) { 
      perror("Couldn't create a kernel");
      exit(1);
    };

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &dest_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &destination);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &src_buffer);
    err |= clSetKernelArg(kernel,3, sizeof(cl_mem), &source);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
    }
    return kernel;
}

void enqueue(cl_command_queue queue,cl_kernel kernel[11],const size_t *global_sizes,const size_t *local_sizes,int a,int b) 
{
    // function to equeue kernel into queue
    for(int i=a;i<b;i++)
    {
        
        //err = clEnqueueNDRangeKernel(queue, kernel[i], 1, NULL,&global_sizes[i],&local_sizes[i], 0, NULL, NULL); 
        err = clEnqueueNDRangeKernel(queue, kernel[i], 1, NULL,&global_sizes[i],&local_sizes[i], 0, NULL, NULL); 
        if(err < 0) {
            perror("Couldn't enqueue the kernel");
            exit(1);
        }
        //printf("hi a=%d b=%d i=%d\n",a,b,i);
        clFlush(queue);
        clFinish(queue);
    }
    
}


void replace_if(cl_command_queue queue,cl_kernel kernel[],cl_mem ant_buffer,member *ants,cl_mem elite_buffer,member *elite,cl_mem temp_elite_buffer,int *temp_elite,const size_t* global_sizes,const size_t *local_sizes)
{ 
    //replacement logic for elite antlion
    clEnqueueReadBuffer(queue,elite_buffer, CL_TRUE, 0,sizeof(member),elite, 0, NULL, NULL );
    clEnqueueReadBuffer(queue,temp_elite_buffer, CL_TRUE, 0,sizeof(int),temp_elite, 0, NULL, NULL );
    clEnqueueReadBuffer(queue,ant_buffer, CL_TRUE, 0,sizeof(member)*SEARCH_AGENTS,ants, 0, NULL, NULL );
    //printf("%d\n",elite->fitness);
    if(ants[*temp_elite].fitness>elite->fitness) // replace only if fitness of temp elite greater than the global elite antlion
    {
        enqueue(queue,kernel,global_sizes,local_sizes,8,11);
    }

}


cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {
  // function which has the logic to build the opencl program
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
  // function which has the logic to get the opencl device
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


int ALO(int *graph,int *ub)
{
    // main function which has the logic for the overall algorithm 
    srand(time(0)); 
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel[11];
    cl_command_queue queue;
    cl_int err;
    cl_mem antlion_buffer, ant_buffer,graph_buffer,elite_buffer,temp_elite_buffer,least_elite_buffer,ub_buffer; // opencl  buffers
    
    member antlions[SEARCH_AGENTS];
    member ants[SEARCH_AGENTS];
    member elite; 

    int current_iter=0;
    int least_elite=0; 
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

    antlion_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, SEARCH_AGENTS * sizeof(member),&antlions, &err); //creation of buffers

    ant_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, SEARCH_AGENTS * sizeof(member),&ants, &err);
    ub_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, DIM * sizeof(int),&ub, &err);
    graph_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, DIM* DIM * sizeof(int),graph, &err); 

    if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
    };

    elite_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, sizeof(member),&elite, &err); 
    temp_elite_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, sizeof(int),&elite, &err); 
    least_elite_buffer =  clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, sizeof(int),&least_elite, &err); 
    if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
    };

    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
    };

    const size_t local_sizes[]={local_size,local_size,local_size,1,1,local_size,local_size,1,1,1,1};
    const size_t global_sizes[]={global_size,global_size,global_size,1,DIM+1,global_size,global_size,1,DIM+1,1,DIM+1};
    kernel[0]=rand_init(program,antlion_buffer,rand());
    kernel[1]=rand_init(program,ant_buffer,rand());
    kernel[2]=fitness(program,antlion_buffer,graph_buffer); 
    kernel[3]=max_fitness(program,antlion_buffer,temp_elite_buffer);
    kernel[4]=mem_copy(program,elite_buffer,antlion_buffer,temp_elite_buffer); // creation of kernel

    enqueue(queue,kernel,global_sizes,local_sizes,0,5);

    kernel[6]=fitness(program,ant_buffer,graph_buffer); 
    kernel[7]=max_fitness(program,ant_buffer,temp_elite_buffer);
    kernel[8]=mem_copy(program,elite_buffer,ant_buffer,temp_elite_buffer);
    kernel[9]=min_fitness(program,antlion_buffer,least_elite_buffer);
    kernel[10]=mem_replace(program,antlion_buffer,least_elite_buffer,ant_buffer,temp_elite_buffer);
    while(current_iter<MAX_ITER)
    {
      kernel[5]=random_walk_of_all_ants(program,ant_buffer,antlion_buffer,elite_buffer,graph_buffer,ub_buffer,current_iter,rand());
      enqueue(queue,kernel,global_sizes,local_sizes,5,8);
      replace_if(queue,kernel,ant_buffer,ants,elite_buffer,&elite,temp_elite_buffer,&temp_elite,global_sizes,local_sizes);
      current_iter++;
      clReleaseKernel(kernel[5]);
    }

    clEnqueueReadBuffer(queue,elite_buffer, CL_TRUE, 0,sizeof(member),&elite, 0, NULL, NULL );
    
    for(int i=0;i<11;i++)
    {
        if(i==5) //because already released
          continue; 
        clReleaseKernel(kernel[i]);
    }
    clReleaseMemObject(antlion_buffer);
    clReleaseMemObject(ant_buffer);
    clReleaseMemObject(graph_buffer);
    clReleaseMemObject(ub_buffer);
    clReleaseMemObject(elite_buffer);
    clReleaseMemObject(temp_elite_buffer);
    clReleaseMemObject(least_elite_buffer);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    return -elite.fitness;

}