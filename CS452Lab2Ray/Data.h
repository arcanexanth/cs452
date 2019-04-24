#ifndef DATA_H
#define DATA_H

#include <cstdlib>
#include <bitset>
#include <climits>

using namespace std;

const size_t size = sizeof(long long) * CHAR_BIT;
typedef bitset<size> our_bs;
class Data{

    private:
        our_bs bs;

    public:
    //Funkytions//
    Data(long long i);
    void setBitAtSpotToZero(int i);
    bool getBitAtSpot(int i);
    int bitSize();
};

#endif
