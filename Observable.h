#ifndef OBSERVABLE_H
#define OBSERVABLE_H
#include "Observer.h"

class Observable {

  public:
    virtual ~Observable() {};
    virtual void addObserver (Observer* o) = 0;
    virtual void removeObserver (Observer* o) = 0;
    virtual void stateChanged (int current_line) = 0;
};

#endif
