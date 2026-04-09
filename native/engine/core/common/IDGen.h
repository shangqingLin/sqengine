#ifndef _ID_GEN_H_
#define _ID_GEN_H_
#include <vector>

class IDGen{
 private:
    int id = 0;
    std::vector<int> pool;   
 public:
    int get();
    void recvoery(int id);
};

#endif