#include <vector>
#include <unordered_map>

//using namespace std;

struct process {
	uint64_t pid : 20;
	uint64_t bst : 7;
	uint64_t arr : 14;
	uint64_t pri : 7;
	uint64_t ddLine : 14;
	uint64_t io : 4;
	uint64_t ticks : 22;
	uint64_t pTime : 22;
    uint64_t age : 20;
};

typedef std::vector<process> processes;
typedef std::unordered_map<int, processes> process_map;
