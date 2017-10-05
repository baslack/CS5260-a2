/**
 * Benjamin A. Slack
 * CS 5260
 * Assignment #2
 *
 * Description:
 * Estimates pi using a Monte Carlo method
 * of approximation. Tosses at a virtual
 * dart board are measured and determined
 * to be within or without a unit circle.
 * The proportion of hits to the number
 * of tosses allows for the extraction of
 * pi from the forumlae for area.
 */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/** Macros **/
#define rand_comp() ((double)(((double)random())/RAND_MAX) * 2.0 - 1.0)
#define dist_sq(x, y) (pow(x, 2) + pow(y, 2))
#define toss() ((dist_sq(rand_comp(), rand_comp()) <= 1) ? 1 : 0)
#define pi(hits, max_toss) (4.0*((double)(hits)/(max_toss)))

//double rand_comp(void);
//double dist_sq(double, double);
//int toss(void);

long long int do_some_tosses(long long int);

struct timespec diff(struct timespec, struct timespec);

/** Constants **/
#define DEFUALT_NUM_TOSS 1024

int main(int argc, char **argv) {

    //common var init
    long long int num_toss = DEFUALT_NUM_TOSS;

    //MPI Init materials
    int comm_sz = 0;
    int my_rank = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0) {
        //get the number of tosses from the broadcast
        long long int number_of_tosses_per_process = 0;
        MPI_Bcast(&number_of_tosses_per_process, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

        //do some tosses
        long long int local_hits = 0;
        local_hits = do_some_tosses(number_of_tosses_per_process);

        // send out the hits
        MPI_Reduce(&local_hits, NULL, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    } else {
        //command line parsing
        if (argc > 1) {
            //identify "-t" flag
            int toss_param_index = -1;
            for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], (void *) &"-t") == 0) {
                    toss_param_index = i + 1;
                }
            }
            //set num_toss
            if (toss_param_index != -1) {
                num_toss = strtoll(argv[toss_param_index], NULL, 10);
            }
        }

        //inform the child processes what their toss numbers will be
        long long int number_of_tosses_per_process = num_toss / (long long int) comm_sz;
        long long int left_over_tosses = num_toss % (long long int) comm_sz;

        //start timing here
        struct timespec start;
        clock_gettime(CLOCK_REALTIME, &start);

        //broadcast the number of tosses out to the processes
        MPI_Bcast(&number_of_tosses_per_process, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

        //do some tosses and reduce
        long long int local_hits = 0;
        long long int total_hits = 0;
        local_hits = do_some_tosses(number_of_tosses_per_process + left_over_tosses);
        MPI_Reduce(&local_hits, &total_hits, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        //end timing here
        struct timespec end;
        clock_gettime(CLOCK_REALTIME, &end);

        struct timespec elapsed = diff(start, end);

        //output pi
        printf("no_proc: %d, num_toss: %lld, s: %ld, ns: %ld, pi: %.16f\n",
               comm_sz, num_toss, elapsed.tv_sec, elapsed.tv_nsec, pi(total_hits, num_toss));
    }

    //MPI Closure
    MPI_Finalize();
    return 0;
}

long long int do_some_tosses(long long int no_tosses) {
    long long int hits = 0;
    for (long long int i = 0; i < no_tosses; i++) {
        hits += toss();
//            printf("rand_comp: %f\n", rand_comp());
    }
    return hits;
//        printf("hits: %lld\n",  hits);
}

/** care of guyretuenberg:
 * http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 *
 * @param start
 * @param end
 * @return time difference
 */
struct timespec diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

//inline double rand_comp(void){
//    return (double)(random()/RAND_MAX) * 2.0 - 1.0;
//}

//inline double dist_sq(double x, double y){
//    return pow(x, 2) + pow(y, 2);
//}

//inline int toss(void){
//    return (dist_sq(rand_comp(), rand_comp()) <= 1) ? 1 : 0;
//}
