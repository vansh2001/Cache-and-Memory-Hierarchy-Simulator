#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "sim_cache.h"

using namespace std;
/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim_cache 32 8192 4 7 262144 8 gcc_trace.txt
    argc = 8
    argv[0] = "sim_cache"
    argv[1] = "32"
    argv[2] = "8192"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    cache_params params;    // look at sim_cache.h header file for the the definition of struct cache_params
    char rw;                // variable holds read/write type read from input file. The array size is 2 because it holds 'r' or 'w' and '\0'. Make sure to adapt in future projects
    unsigned long int addr; // Variable holds the address read from input file

    if(argc != 8)           // Checks if correct number of inputs have been given. Throw error and exit if wrong
    {
        printf("Error: Expected inputs:7 Given inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }

    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    params.block_size       = strtoul(argv[1], NULL, 10);
    params.l1_size          = strtoul(argv[2], NULL, 10);
    params.l1_assoc         = strtoul(argv[3], NULL, 10);
    params.vc_num_blocks    = strtoul(argv[4], NULL, 10);
    params.l2_size          = strtoul(argv[5], NULL, 10);
    params.l2_assoc         = strtoul(argv[6], NULL, 10);
    trace_file              = argv[7];

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    // Print params
    printf("  ===== Simulator configuration =====\n"
            "  L1_BLOCKSIZE:                     %lu\n"
            "  L1_SIZE:                          %lu\n"
            "  L1_ASSOC:                         %lu\n"
            "  VC_NUM_BLOCKS:                    %lu\n"
            "  L2_SIZE:                          %lu\n"
            "  L2_ASSOC:                         %lu\n"
            "  trace_file:                       %s\n"
            "  \n\n", params.block_size, params.l1_size, params.l1_assoc, params.vc_num_blocks, params.l2_size, params.l2_assoc, trace_file);

    char str[2];
    int L2_flag_print = 0;
    //unsigned long int tag;
    //unsigned long int index;
    
     // Create L1 cache
    cache * L1 = NULL;
    if(params.l1_size > 0) {
        L1 = new cache(params.l1_size,params.l1_assoc, params.block_size); //caculate # of sets for L1
    }
    
    //Create L2 cache
    cache *L2 = NULL;
    if(params.l2_size > 0) {
        L2 = new cache(params.l2_size,params.l2_assoc,params.block_size); //caculate # of sets for L2
        L1->setL2_flag(L2);
       // L1->setL2_flag(1);
        L2_flag_print = 1;
    }
    //int counter = 0;
    
    while(fscanf(FP, "%s %lx", str, &addr) != EOF)
    {
        //tag = tag_decoder(addr, params.block_size, *L1);
        //index = index_decoder(addr, params.block_size, *L1);
        
        //if(counter == 57910) {
            //cout << "hello";
        //}
        //printf("--------------------------------------------\n");
        //cout << counter << endl;
        rw = str[0];
        if (rw == 'r') { //check for read
             //printf("%s %lx\n", "read", addr); // Print and test if file is read correctly
            /*ones read check for cache miss read or read hit */
            //cout << "# " << dec <<counter << ": " << "read " << hex << addr << endl;
            //cout << "L1 read: " << hex << addr << "  " << "(tag "  << tag << "," << " index " << dec <<index <<")" << endl;
            read_write_process('r', addr, params.block_size, L1);
        }
        else if (rw == 'w') {
            //printf("%s %lx\n", "write", addr);          // Print and test if file is read correctly
            //cout << "# " << dec <<counter << ": " << "write " << hex << addr << endl;
            //cout << "L1 write: " << hex << addr << "  " << "(tag "  << tag << "," << " index " << dec <<index <<")" << endl;
            read_write_process('w', addr, params.block_size, L1);
            
        }
        
        /*************************************
                  Add cache code here
        **************************************/
        //check_cache(rw, addr, params.block_size, L1, L2);
        // address_decode(addr);
        //tag_decoder(addr, params.block_size);
        
        //tag_decoder(addr, params.block_size, *L1);
        //index_decoder(addr, params.block_size, *L1);
        //counter++;
        
    }
    print_cache(1,L1); //print L1 cache
    
    //If L2 cache exists print L2
    if(L2_flag_print) {
        print_cache(0,L2);
        L2_flag_print = 0;
    }
    
    print_sim_results(L1, L2);
    
    return 0;
}
