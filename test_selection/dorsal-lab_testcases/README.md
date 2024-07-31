# Examples of OTF2 and converted CTF traces   
This repository contains examples of OTF2 traces and the resulting CTF traces after conversion with the OTF2 to CTF converter available [here](https://github.com/dorsal-lab/OTF2-to-CTF-converter)  
The codes for the examples have been found on [CodinGame](https://www.codingame.com) and [RookieHPC](https://www.rookiehpc.com/) and are available in the ```src``` directory.  
The resulting traces are in the ```traces``` directory, there are 5 differents programs and corresponding traces: 
- One program with 2 ranks and a Mpi_Bsend/Recv routine
- One program with 2 ranks and a Mpi_Ssend/Irecv routine
- One program with 4 ranks and a Mpi_Gather routine
- One program with 8 ranks : 2 communicators are created and an allreduce operation is called in both of them
- One program with 4 ranks on 2 remote nodes. Each rank create 2 additional threads calling a "Hello world" function.

All the files generated with Score-P are present in each trace, but only the ```traces.otf2```, ```traces.def``` files and ```traces``` directory are required to do the conversion to CTF. The resulting CTF traces are in the ```converted_otf2_<identifier>``` directory.

# How to regenerate the traces
To regenerate the traces you need [Score-P](https://perftools.pages.jsc.fz-juelich.de/cicd/scorep/tags/scorep-7.0/html/index.html) and [OTF2](http://perftools.pages.jsc.fz-juelich.de/cicd/otf2/tags/otf2-2.3/otf2-2.3.tar.gz).  
You need to compile the code with Score-P as you would with mpicc : ```scorep mpicc my_program.c -o my_program```  
Then you set the following environment variable : ```export SCOREP_ENABLE_TRACING=1```  
You run the program :  ```mpirun -n <required number of nodes> my_program```  
It will generate a trace directory containing a ```traces.otf2``` file.  
If you want the converted CTF trace you have to set ```OTF2_CONVERTER=<path to the converter>``` and convert the trace ```<path to the converter>/otf2_converter traces.otf2```  
For the program running with pthread you need to add -lpthread at build step.

