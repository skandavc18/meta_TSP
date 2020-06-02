This repo contains implementations of 2 meta heuristic algorithms

1. ALO
2. GOA
<br>
It contains both the serial and parallel versions of the 2 algorithms

To run, the following dependencies are necessary :

1. gcc
2. g++
3. OpenCL
4. make
<br>
To compile and run ALO, follow the steps (similar steps for GOA as well)

1. `cd ALO`
2. `cd ALO_Parallel` or `cd ALO_Serial`
3. set dimensions of the graph
4. `make`
5. `./alo< ip<dim>.txt`

<br>
To compare accuracy of ALO, GOA, GA and ACO, follow the steps

1. `cd 'Accuracy Measure'`
2. `./script`

<br>
To check the performance of serial ALO and parallel ALO, follow the steps (similar steps for GOA as well)

1. `cd 'Performance Measure'`
2. `cd ALO`
3. `./script`
