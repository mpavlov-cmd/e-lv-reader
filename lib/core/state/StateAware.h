#ifndef STATEAWARE_H
#define STATEAWARE_H

template <typename T>
class StateAware {
    virtual T getState() = 0;
};

#endif