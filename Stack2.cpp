#include "Stack2.h"

Stack::Stack() {
    top = -1;  // Initialize empty stack
}

void Stack::push(float data) {
    if (!isFull()) {
        stackArray[++top] = data;
    }
    // If stack is full, do nothing
}

float Stack::pop() {
    if (!isEmpty()) {
        return stackArray[top--];
    }
    return -1.0f;  // Return -1.0 if stack is empty
}

float Stack::peek() {
    if (!isEmpty()) {
        return stackArray[top];
    }
    return -1.0f;  // Return -1.0 if stack is empty
}

bool Stack::isEmpty() {
    return (top == -1);
}

bool Stack::isFull() {
    return (top == MAX_SIZE - 1);
}

int Stack::size() {
    return top + 1;
}

void Stack::clear() {
    top = -1;
}