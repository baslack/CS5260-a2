Benjamin Slack
benjamin.a.slack@wmich.edu
CS5260
Assignment #2
Monte Carlo Estimation of PI using MPI

Compilation:

An accompanying Makefile will allow for complication of main.c into
the a2 executable. I used the most recent MPI module on Thor for 
the complication environment. To compile the program, simple enter:

make

at the command prompt.

Running the batches:

I setup the a2.pbs file to run a single job. It expects a single parameter
that must be specified in the qsub statement, the number of tosses to be 
executed. The node configuration is not supplied in the a2.pbs torque 
script. This configuration is supplied by the a2_batch.sh script, as a qsub
parameter. Please be mindful of this fact and check the syntax that script 
uses before trying to execute a2.pbs directly.

The batches of jobs themselves are queued by running the a2_batch.sh 
shell script. This script will queue up batches of jobs for varying core 
configurations and numbers of tosses. I had some issues with overloading
the Thor job submission queue, so the script will throttle itself and wait for 
groups of jobs to complete before starting the next batch. The longest 
batches are of course those only using a single processor, with wait times 
of 480 seconds. This value was determined experimentally in early testing. 
The maximum value is then halved to a minimum of 60 seconds between 
batches of more expansive resources requests.

Finally, once all requests in a given batch are complete, the shell script 
aggregates the output of the batch into a single CSV file for that batch. 
It then cleans up the working directory, eliminating all the outfiles and 
begins the next batch. The number of batches is controlled via a command 
line parameter of the a2_batch script of the following form:

a2_batch.sh <number of batches>

Each batch takes approximately 15 minutes, with all the waits, etc. So 
be mindful before queuing up a large number of them.

CMake:

The cmake files are for the use of my IDE (Jetbrains CLion). They are not 
required in the compliation outside of that IDE. The Makefile supplied 
should allow complication, so long as the OpenMPI module is loaded in 
the Thor session.