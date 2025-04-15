#include "Stack2.h"
#include <iostream>

int main() {
    Stack myStack;
    
    std::cout << "Comprehensive Stack Test Program" << std::endl;
    std::cout << "------------------------" << std::endl;
    
    // Test 1: Initial state
    std::cout << "\nTest 1: Initial State" << std::endl;
    std::cout << "Stack size: " << myStack.size() << std::endl;
    std::cout << "Is stack empty? " << (myStack.isEmpty() ? "Yes" : "No") << std::endl;
    std::cout << "Is stack full? " << (myStack.isFull() ? "Yes" : "No") << std::endl;
    
    // Test 2: Basic push and pop operations
    std::cout << "\nTest 2: Basic Operations" << std::endl;
    std::cout << "Pushing numbers 10.5, 20.7, 30.9..." << std::endl;
    myStack.push(10.5f);
    myStack.push(20.7f);
    myStack.push(30.9f);
    
    std::cout << "Stack size: " << myStack.size() << std::endl;
    std::cout << "Top element: " << myStack.peek() << std::endl;
    
    // Test 3: Multiple pops
    std::cout << "\nTest 3: Multiple Pops" << std::endl;
    std::cout << "Popping elements:" << std::endl;
    while (!myStack.isEmpty()) {
        std::cout << "Popped: " << myStack.pop() << std::endl;
        std::cout << "Current size: " << myStack.size() << std::endl;
    }
    
    // Test 4: Edge Cases
    std::cout << "\nTest 4: Edge Cases" << std::endl;
    std::cout << "Trying to pop from empty stack: " << myStack.pop() << std::endl;
    std::cout << "Trying to peek empty stack: " << myStack.peek() << std::endl;
    
    // Test 5: Stack Full Test
    std::cout << "\nTest 5: Stack Full Test" << std::endl;
    std::cout << "Filling stack to capacity..." << std::endl;
    for (int i = 0; i < Stack::MAX_SIZE; i++) {
        myStack.push(i * 1.1f);
        std::cout << "Pushed: " << (i * 1.1f) << ", Size: " << myStack.size() << std::endl;
    }
    std::cout << "Is stack full? " << (myStack.isFull() ? "Yes" : "No") << std::endl;
    
    // Test 6: Clear Operation
    std::cout << "\nTest 6: Clear Operation" << std::endl;
    myStack.clear();
    std::cout << "After clear - Size: " << myStack.size() << std::endl;
    std::cout << "Is stack empty? " << (myStack.isEmpty() ? "Yes" : "No") << std::endl;
    
    // Test 7: Mixed Operations
    std::cout << "\nTest 7: Mixed Operations" << std::endl;
    std::cout << "Pushing 5 elements..." << std::endl;
    for (int i = 0; i < 5; i++) {
        myStack.push(i * 2.5f);
    }
    std::cout << "Current size: " << myStack.size() << std::endl;
    std::cout << "Top element: " << myStack.peek() << std::endl;
    
    std::cout << "Popping 2 elements..." << std::endl;
    for (int i = 0; i < 2; i++) {
        std::cout << "Popped: " << myStack.pop() << std::endl;
    }
    std::cout << "Current size: " << myStack.size() << std::endl;
    std::cout << "Top element: " << myStack.peek() << std::endl;
    
    std::cout << "Pushing 3 more elements..." << std::endl;
    for (int i = 0; i < 3; i++) {
        myStack.push(i * 3.3f);
    }
    std::cout << "Final size: " << myStack.size() << std::endl;
    std::cout << "Final top element: " << myStack.peek() << std::endl;
    
    return 0;
} 