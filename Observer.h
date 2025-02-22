#ifndef OBSERVER_H
#define OBSERVER_H
#include <vector>
#include "Buffer.h"
using namespace std;


class Observer {
  public:
    virtual ~Observer() {};
    virtual void updateView (vector<Buffer*>* lines, int l) = 0;

};

#endif
