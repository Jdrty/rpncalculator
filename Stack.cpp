#include "Stack.h"
#include <Arduino.h>

Stack::Stack() {
    _top = -1;
}

void Stack::push(float operand) {
    if (!isFull()) {
        _stack[++_top] = operand;
    }
}

float Stack::pop() {
    if (!isEmpty()) {
        return _stack[_top--];
    }
    return 0;
}

float Stack::peek() {
    if (!isEmpty()) {
        return _stack[_top];
    }
    return 0;
}

bool Stack::isEmpty() {
    return _top == -1;
}

bool Stack::isFull() {
    return _top == capacity - 1;
}