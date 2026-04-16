#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
using namespace std;

// set-associative cache using lru replacement
class Cache {
 public:
  // sets up the cache with the given number of entries and associativity
  Cache(int num_entries, int associativity)
      : associativity_(associativity) {
    num_sets_ = num_entries / associativity;
    index_bits_ = 0;
    int n = num_sets_;
    // count how many bits are needed for the index
    while (n > 1) {
      n = n / 2;
      index_bits_++;
    }
    sets_.resize(num_sets_);
  }

  // returns true on hit, false on miss
  bool Access(int address) {
    int index = address % num_sets_;
    int tag   = address / num_sets_;

    list<int>& set = sets_[index];

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
    if (set_size >= associativity_) {
      set.pop_back();
    }
    // load new tag as most recently used
    set.push_front(tag);
    return false;
  }

 private:
  int num_sets_;
  int index_bits_;
  int associativity_;
  vector<list<int>> sets_;
};

// reads memory addresses from a file and simulates cache behavior
int main(int argc, char* argv[]) {
  if (argc != 4) {
    cerr << "Usage: " << argv[0]
         << " num_entries associativity memory_reference_file\n";
    return 1;
  }

  int num_entries   = stoi(argv[1]);
  int associativity = stoi(argv[2]);
  string ref_file = argv[3];

  Cache cache(num_entries, associativity);

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

  // read each address and write result to file and terminal
  int address;
  while (infile >> address) {
    bool hit = cache.Access(address);
    string status;
    if (hit) {
      status = "HIT";
    } else {
      status = "MISS";
    }
    string result = to_string(address) + " : " + status;
    outfile << result << "\n";
    cout << result << "\n";
  }

  return 0;
}
