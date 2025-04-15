#ifndef Stack2_h
#define Stack2_h

class Stack {
    public:
        static const int MAX_SIZE = 20;  // Fixed maximum size
    private:
        float stackArray[MAX_SIZE];      // Fixed-size array
        int top;                         // Index of top element
    public:
        Stack();                         // Constructor
        void push(float data);           // Add element to top
        float pop();                     // Remove and return top element
        float peek();                    // Return top element without removing
        bool isEmpty();                  // Check if stack is empty
        bool isFull();                   // Check if stack is full
        int size();                      // Return number of elements
        void clear();                    // Clear the stack
};

#endif /* Stack2_h */