#ifndef SIM_CACHE_H
#define SIM_CACHE_H

#include <vector>
#include <math.h>
#include <iomanip>
using namespace std;
#define LRU_max LRU_cache->set_vector.at(LRU_index).block_vector.at(block_index).setLRU(most_recently_used)

typedef struct cache_params{
    unsigned long int block_size;
    unsigned long int l1_size;
    unsigned long int l1_assoc;
    unsigned long int vc_num_blocks;
    unsigned long int l2_size;
    unsigned long int l2_assoc;
}cache_params;

/*this is a cache object for all the cache in the program.
 it has a #cache[#sets[#blocks]]
 */

class cache {
private:
    unsigned long int num_sets; //sets
    cache* L2_flag = NULL; // chache level 2 exists if = 1
    unsigned long int blockSize; // = associativity
    unsigned long int assoc; //associativity
public:
    int reads = 0;
    int read_miss = 0;
    int write = 0;
    int write_miss = 0;
    int write_back = 0;
    cache(unsigned long int size, unsigned long int associativity, unsigned long int block_size) {
       num_sets = size/((associativity)*(block_size));
       blockSize = block_size;
       assoc = associativity;
       for(unsigned long int i = 0; i < num_sets; i++){
         set_vector.push_back(sets(assoc)); //set number of sets
       }
    }
    class sets {
        unsigned long int num_blocks;
    public:
        class blocks {
            unsigned long int tag;
            int valid;
            int dirty;
            unsigned long int LRU_counter; // counter
            unsigned long int address;
        public:
            blocks(unsigned long int tag_bit=0,int valid_bit=0, int dirty_bit=0, unsigned long int LRU_count=0, unsigned long int addr=0) {
                tag = tag_bit;
                valid = valid_bit;
                dirty = dirty_bit ;
                LRU_counter = LRU_count;
                address = addr;
            }
            void setTag(unsigned long int input_tag){ tag = input_tag; }
            void setValid(int input){ valid = input; }
            void setDirty(int input_dirty){ dirty = input_dirty; }
            void setLRU(unsigned long int input_LRU){ LRU_counter = input_LRU; }
            void setAddr(unsigned long int addr) {address = addr;}
            void setBlock(unsigned long int tag_bit,int valid_bit, int dirty_bit,unsigned long int addr) {
                tag = tag_bit;
                valid = valid_bit;
                dirty = dirty_bit;
                address = addr;
            }
            unsigned long int getTag(){
                unsigned long int temp;
                temp = tag;
                
                return temp;
            }
            unsigned long int getLRU(){
                unsigned long int temp_LRU;
                temp_LRU = LRU_counter;
                
                return temp_LRU;
            }
            int getValid(){
                int temp;
                temp = valid;
                
                return temp;
            }
            int getDirty() {
                int temp_Dirty;
                temp_Dirty = dirty;
                
                return temp_Dirty;
            }
            unsigned long int getAddr() {
                unsigned long int addrs;
                addrs = address;
                
                return addrs;
            }
        };
        sets(unsigned long int assoc){
            num_blocks = assoc;
            //cout << "number of blocks: " << num_blocks << endl;
            for(unsigned long int j = 0; j < num_blocks; j++){
                block_vector.push_back(blocks(0,0,0,j,0)); //set the number of blocks
            }
        }
        vector<blocks> block_vector;
        unsigned long int getNum_b(){
            unsigned long int temp;
            temp = num_blocks;
            
            return temp;
        }
    };
    vector<sets> set_vector;
    void setL2_flag(cache* x){ L2_flag = x; } //if not null cache level 2 exists
    cache* getL2_flag(){
        return L2_flag;
    } //get the L2 flag value
    unsigned long int getBlockSize(){
        unsigned long int temp_blocks;
        temp_blocks = blockSize;
        
        return temp_blocks;
    }
    unsigned long int getNum_sets(){
        unsigned long int temp;
        temp = num_sets;
        
        return temp;
    }
    unsigned long int getAssoc() {
        long unsigned int temp;
        temp = assoc;
        
        return temp;
    }
    void print(){
        cout << "I am here\n";
    }
};


unsigned long int tag_decoder(unsigned long int addr, unsigned long int block_s, cache s){
    unsigned long int tag_temp;
    unsigned long int index_temp;
    unsigned long int offSet;

    offSet = log2(block_s);
    index_temp = log2(s.getNum_sets());
    tag_temp = addr >> (offSet + index_temp);   //Offset + index + tag  = address. shift right

    return tag_temp;
}


unsigned long int index_decoder(unsigned long int addr, unsigned long int block_s, cache s){
    unsigned long int index_temp;
    unsigned long int offSet;
    unsigned long int index_value; //final index value
    int twos_power;

    offSet = log2(block_s); //Number of offset bits
    index_temp = log2(s.getNum_sets()); //Number for index bits
    index_value =  addr >> offSet; //shift right
    twos_power = pow(2, index_temp) - 1;  //2^index - 1
    index_value = index_value & twos_power;
    
    return index_value;
}

//updates LRU for Most recently used and Least recently used
void LRU_tracker(unsigned long int LRU_index, unsigned long int LRU_tag , unsigned long int block_index ,cache *LRU_cache){
    
    //printf("LRU-tracker used");
    unsigned long int most_recently_used = LRU_cache->set_vector.at(LRU_index).getNum_b()-1; //total #blocks - 1
    unsigned long int get_LRU = LRU_cache->set_vector.at(LRU_index).block_vector.at(block_index).getLRU(); //value at the block of hit LRU
    unsigned long int current_get_LRU; //holds current interations of LRU
    
    for(unsigned long int k = 0; k <= most_recently_used; k++){
        current_get_LRU = LRU_cache->set_vector.at(LRU_index).block_vector.at(k).getLRU();  //get current lru of each block in that set
        
        if(current_get_LRU > get_LRU) {  //if current LRU value is greater than the hit lru value decrement the LRU counter
            LRU_cache->set_vector.at(LRU_index).block_vector.at(k).setLRU(current_get_LRU - 1);
        }
    }
    LRU_max; //sets the blocks that it hits to max counter or MRU
}

/*check if it exists in cache if it does exist then calls LRU tracking for read then returns true
else returns false */
bool check_exists( unsigned long int tag, unsigned long int index, cache *s_1) {
    for(unsigned long int i = 0; i < s_1->set_vector.at(index).getNum_b(); i++) {  //Iterate for loop to match the assoc/#of blocks
        if(s_1->set_vector.at(index).block_vector.at(i).getTag() == tag) { //if the tag bits match then return true
            //LRU_tracker(index, tag, i ,s_1); //call LRU_tracker to update LRU
            return true; //does exist
        }
    }
    return false; //does not exist
}

//returns current block index
unsigned long int LRU_index(unsigned long int tag, unsigned long int index, cache *s_1) {
    for(unsigned long int i = 0; i < s_1->set_vector.at(index).getNum_b(); i++) {  //Iterate for loop to match the assoc/#of blocks
        if(s_1->set_vector.at(index).block_vector.at(i).getTag() == tag) { //if the tag bits match then return true
            return i; //returns block index
        }
    }
    return 0; //return null
}

//returns the LRU block index
long unsigned int least_LRU_block(unsigned long int store_tag_L2, unsigned long int index, cache LRU_cache){
    unsigned long int size_blocks = LRU_cache.set_vector.at(index).getNum_b(); //number blocks total
    
    for(unsigned long int h = 0; h < size_blocks; h++) { //increment till the number of blocks
        if(LRU_cache.set_vector.at(index).block_vector.at(h).getLRU() == 0) {
            return h; //return the least LRU block
        }
    }
    return 0;
}

bool check_eviction(long unsigned int evic_tag, long unsigned int evic_index, cache *s1) {
    unsigned long int least_LRU_b;
    int block_getDirty;
    
    least_LRU_b = least_LRU_block(evic_tag, evic_index, *s1); //get the index of block with LRU
    block_getDirty = s1->set_vector.at(evic_index).block_vector.at(least_LRU_b).getDirty(); //get dirty block bit

    if(block_getDirty) {
        return true;
    }
    //cout << "victim: "<< hex << eviction_block_addr << " (tag " << tag << ", index" << dec << index << ", clean)" << endl;
    return false;
}



void read_write_process(char rw, unsigned long int addr, unsigned long int block_s , cache *s1) {
    unsigned long int store_tag;
    unsigned long int store_index;
    unsigned long int least_LRU_b;
    unsigned long int eviction_block_addr;
    //unsigned long int tag;
    //unsigned long int index;
    unsigned long int current_index;
    unsigned long int update_LRU_index;
    char read = 'r';
    char write ='w';
    int dirty_L = 0;
    int dirty_H = 1;
    int valid = 1;
  
    
    store_tag   = tag_decoder(addr, block_s, *s1);  //decode tag
    store_index = index_decoder(addr, block_s, *s1); //decode index
    least_LRU_b = least_LRU_block(store_tag, store_index, *s1);
    if(rw == 'r') { s1->reads++; } else { s1->write++; } //check if read or write then increment counter
    
    
    if(check_exists(store_tag, store_index, s1)) {
        //cout << "L1 hit" << endl;
        update_LRU_index = LRU_index(store_tag, store_index, s1);
        LRU_tracker(store_index, store_tag ,update_LRU_index,s1);
        current_index = LRU_index(store_tag,store_index, s1);
        if(rw == 'w') { s1->set_vector.at(store_index).block_vector.at(current_index).setDirty(dirty_H);
           //cout << "L1 set dirty" << endl;
        } //set dirty bit high
        //cout << "L1 update LRU" << endl;
    }else { //tag does not exist
        //cout << "L1 miss" <<endl;
        if(check_eviction(store_tag, store_index, s1)) { //check for dirty
            eviction_block_addr = s1->set_vector.at(store_index).block_vector.at(least_LRU_b).getAddr(); //evicted block address
            //tag = tag_decoder(eviction_block_addr, block_s, *s1);
            //index = index_decoder(eviction_block_addr, block_s, *s1);
            s1->write_back++; //increment write back;
            //cout << "victim: "<< hex <<eviction_block_addr << " (tag " << tag << ", index" << dec << index << ", dirty)"<< endl;
            if(s1->getL2_flag()) { //call write to next level
                read_write_process(write, eviction_block_addr, block_s, s1->getL2_flag());
            }
        } //else { cout << "victim: none" << endl; }
        
        if(s1->getL2_flag()) { read_write_process(read, addr, block_s, s1->getL2_flag()); } //if next level exists
        if(rw == 'r') {
            s1->read_miss++;
            //cout << "L1 miss" <<endl;
            s1->set_vector.at(store_index).block_vector.at(least_LRU_b).setBlock(store_tag,valid,dirty_L,addr); //bit set to clean
        }
        
        if(rw == 'w') {
            s1->write_miss++;
            //cout << "L1 miss" <<endl;
            s1->set_vector.at(store_index).block_vector.at(least_LRU_b).setBlock(store_tag,valid,dirty_H,addr); //bit set to dirty
            //cout << "L1 set dirty" << endl;
        }
        update_LRU_index = LRU_index(store_tag, store_index, s1);
        LRU_tracker(store_index, store_tag, update_LRU_index,s1);
        //cout << "L1 update LRU" << endl;
    }
}




void print_sim_results(cache *s1, cache *s2) {
    int swap_request = 0;
    double swap_rate = 0.00;
    int read_miss = s1->read_miss;
    int write_miss = s1->write_miss;
    int reads = s1->reads;
    int write = s1->write;
    int write_back = s1->write_back;
    double L1_miss_rate = (double)(read_miss + write_miss)/(reads + write);
    int L2_reads = 0;
    int L2_writes = 0;
    int L2_read_miss = 0;
    int L2_write_miss = 0;
    int L2_write_back = 0;
    double L2_miss_rate = 0.00;
    int total_memory_traffic = read_miss + write_miss + write_back;
    
    if(s1->getL2_flag()) {
        L2_reads = s2->reads;
        L2_writes = s2->write;
        L2_read_miss = s2->read_miss;
        L2_write_miss = s2->write_miss;
        L2_write_back = s2->write_back;
        L2_miss_rate = (double) (L2_read_miss)/(L2_reads);
        total_memory_traffic = L2_read_miss + L2_write_miss + L2_write_back;
    }
    
    printf("===== Simulation results =====\n");
    cout << "a. number of L1 reads:            "<< "\t\t\t\t" << dec   << s1->reads            <<endl;
    cout << "b. number of L1 read misses:      "<< "\t\t\t\t" << dec   << s1->read_miss        <<endl;
    cout << "c. number of L1 writes:           "<< "\t\t\t\t" << dec   << s1->write            <<endl;
    cout << "d. number of L1 write misses:     "<< "\t\t\t\t" << dec   << s1->write_miss       <<endl;
    cout << "e. number of swap requests:       "<< "\t\t\t\t" << dec   << swap_request         <<endl;
    cout << "f. swap request rate:             "<< "\t\t\t\t" << fixed << setprecision(4) << swap_rate            <<endl;
    cout << "g. number of swaps:               "<< "\t\t\t\t" << dec   << swap_request         <<endl;
    cout << "h. combined L1+VC miss rate:      "<< "\t\t\t\t" << fixed << setprecision(4) << L1_miss_rate         <<endl;
    cout << "i. number writebacks from L1/VC:  "<< "\t\t\t\t" << dec   << s1->write_back       <<endl;
    cout << "j. number of L2 reads:            "<< "\t\t\t\t" << dec   << L2_reads             <<endl;
    cout << "k. number of L2 read misses:      "<< "\t\t\t\t" << dec   << L2_read_miss         <<endl;
    cout << "l. number of L2 writes:           "<< "\t\t\t\t" << dec   << L2_writes            <<endl;
    cout << "m. number of L2 write misses:     "<< "\t\t\t\t" << dec   << L2_write_miss        <<endl;
    cout << "n. L2 miss rate:                  "<< "\t\t\t\t" << fixed << setprecision(4) << L2_miss_rate         <<endl;
    cout << "o. number of writebacks from L2:  "<< "\t\t\t\t" << dec   << L2_write_back        <<endl;
    cout << "p. total memory traffic:          "<< "\t\t\t\t" << dec   << total_memory_traffic <<endl;
}

void print_cache(int cache_ind, cache *s1){
    //printf("print cache\n");
    unsigned long int num_L1_sets = s1->getNum_sets();
    unsigned long int num_blocks_L1 = s1->getAssoc();
    unsigned long int max_block_size = num_blocks_L1-1; //max_associativity
    unsigned long int LRU_value = 0;
    unsigned long int block_tag = 0;
    int dirty_bit = 0;
    unsigned long int max_increment = max_block_size;
    unsigned long int j = 0;
    
    if(cache_ind == 1) {
        printf("===== L1 contents =====\n");
    } else {
        printf("===== L2 contents =====\n");
    }
    
    
    for(unsigned long int i = 0; i < num_L1_sets; i++) { //goes through the sets
        max_block_size = max_increment;
        cout << "  set\t" << dec << i << ": " << "\t";
        for(unsigned long int k = 0; k < num_blocks_L1; k++) { //goes through blocks
            for(j = 0; j < num_blocks_L1; j++) {
                LRU_value = s1->set_vector.at(i).block_vector.at(j).getLRU();
                block_tag = s1->set_vector.at(i).block_vector.at(j).getTag();
                dirty_bit = s1->set_vector.at(i).block_vector.at(j).getDirty();
                    if(LRU_value == max_block_size) {
                        cout << hex << block_tag;
                        if(dirty_bit) { cout << " D"; }  else cout << "  ";
                        cout << "\t" ;
                        max_block_size--;
                        break;
                        }
                  }
        }
         printf("\n");
    }
    printf("\n");
}


#endif
