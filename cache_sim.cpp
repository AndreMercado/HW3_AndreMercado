#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
using namespace std;

// set-associative cache using lru replacement with multi-word block support
class Cache {
 public:
  // sets up the cache with given entries, associativity, and block size
  Cache(int num_entries, int ways, int words_per_block) {
    assoc      = ways;
    block_size = words_per_block;
    num_sets   = num_entries / ways;
    sets.resize(num_sets);
  }

  // returns true on hit, false on miss
  bool Access(int address) {
    int block_address = address / block_size;
    int index = block_address % num_sets;
    int tag   = block_address / num_sets;

    list<int>& set = sets[index];

    // search for tag in the set
    for (int cached_tag : set) {
      if (cached_tag == tag) {
        // hit: move tag to front as most recently used
        set.remove(tag);
        set.push_front(tag);
        return true;
      }
    }

    // miss: evict least recently used if set is full
    int set_size = set.size();
    if (set_size >= assoc) {
      set.pop_back();
    }
    // load new tag as most recently used
    set.push_front(tag);
    return false;
  }

 private:
  int num_sets;
  int assoc;
  int block_size;
  vector<list<int>> sets;
};

// classifies a miss as compulsory, capacity, or conflict
string ClassifyMiss(int address, int block_size, bool fa_hit, vector<int>& seen_blocks) {
  int block_address = address / block_size;
  bool is_new_block = true;
  for (int b : seen_blocks) {
    if (b == block_address) {
      is_new_block = false;
      break;
    }
  }

  if (is_new_block) {
    return "COMPULSORY";
  }
  if (!fa_hit) {
    return "CAPACITY";
  }
  return "CONFLICT";
}

// builds the output string for a miss
string MissResult(int address, string miss_type, bool use_l2, Cache* l2_cache) {
  if (use_l2) {
    bool l2_hit = l2_cache->Access(address);
    string l2_status;
    if (l2_hit) {
      l2_status = "L2 HIT";
    } else {
      l2_status = "L2 MISS";
    }
    return to_string(address) + " : L1 MISS (" + miss_type + ") " + l2_status;
  }
  return to_string(address) + " : MISS (" + miss_type + ")";
}

// reads memory addresses from a file and simulates cache behavior
int main(int argc, char* argv[]) {
  bool basic = (argc == 4);
  bool with_block_size = (argc == 5);
  bool with_l2 = (argc == 7);
  bool valid_argc = basic || with_block_size || with_l2;
  if (!valid_argc) {
    cerr << "Usage: " << argv[0]
         << " num_entries associativity memory_reference_file"
         << " [block_size [l2_entries l2_assoc]]\n";
    return 1;
  }

  int num_entries   = stoi(argv[1]);
  int associativity = stoi(argv[2]);
  string ref_file   = argv[3];
  bool has_block_size = (argc >= 5);
  int block_size = 1;
  if (has_block_size) {
    block_size = stoi(argv[4]);
  }

  bool use_l2 = (argc == 7);
  int l2_entries = 0;
  int l2_assoc = 0;
  if (use_l2) {
    l2_entries = stoi(argv[5]);
    l2_assoc   = stoi(argv[6]);
  }

  Cache l1_cache(num_entries, associativity, block_size);
  Cache fa_cache(num_entries, num_entries, block_size);

  Cache* l2_cache = nullptr;
  if (use_l2) {
    l2_cache = new Cache(l2_entries, l2_assoc, block_size);
  }

  vector<int> seen_blocks;

  // open input file
  ifstream infile(ref_file);
  if (!infile) {
    cerr << "Error: cannot open input file: " << ref_file << "\n";
    return 1;
  }

  // open output file
  ofstream outfile("cache_sim_output");
  if (!outfile) {
    cerr << "Error: cannot create cache_sim_output\n";
    return 1;
  }

  // read each address and simulate cache behavior
  int address;
  infile >> address;
  while (!infile.fail()) {
    bool l1_hit = l1_cache.Access(address);
    bool fa_hit = fa_cache.Access(address);
    int block_address = address / block_size;

    string result;
    if (l1_hit) {
      result = to_string(address) + " : HIT";
    } else {
      string miss_type = ClassifyMiss(address, block_size, fa_hit, seen_blocks);
      result = MissResult(address, miss_type, use_l2, l2_cache);
    }

    seen_blocks.push_back(block_address);
    outfile << result << "\n";
    cout << result << "\n";
    infile >> address;
  }

  delete l2_cache;
  return 0;
}
