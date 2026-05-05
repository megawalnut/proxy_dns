#ifndef TASK_H
#define TASK_H

#include "../outQueue.h"

class Task {
public:
    virtual void execute(OutQueue& out) = 0;
    virtual ~Task() = default;
};

#endif // TASK_H
