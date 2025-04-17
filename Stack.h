#ifndef Stack_h
#define Stack_h

#include <Arduino.h>

class Stack {
    private:
        int _top;
        const int capacity = 20;
        float _stack[20];

    public:
        Stack();
        void push(float operand);
        float pop();
        float peek();
        bool isEmpty();
        bool isFull();
};

#endif /* Stack_h */