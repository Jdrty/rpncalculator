// PROJECT  :NPN Calculator
// PURPOSE  :To have a visually appealing, relatively complex RPN Calculator using a self made stack library
// COURSE   :ICD3O
// AUTHOR   :Julian Darou-Santos
// DATE     :Mon Apr 21
// MCU      :Arduino Nano
// STATUS   :Working
// REFERENCE:DER.Mock http://darcy.rsgc.on.ca/ACES/TEL3M/2324/images/BicolorLED23.png

#include <LiquidCrystal.h>
#include "Stack.h"

// LCD Pin Assignments (Backplane Compatible)
uint8_t RS = 9, EN = 8, D4 = 7, D5 = 6, D6 = 5, D7 = 4;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

#define LCD_COLUMNS 16      
#define LCD_ROWS 2          
#define KEYPAD A5           // Keypad Analog Pin
#define DURATION 300        // Keypad Debounce Delay (ms)
#define DEBUG 1             // Debugging flag
#define HISTORY_SIZE 10     // Size of operation history
#define LONG_PRESS 1000     // Long press duration in ms

// Custom LCD characters
byte stackTop[] = {
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte stackMid[] = {
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte stackBottom[] = {
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte modeIndicator[] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000
};

byte operationFlash[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

// Keypad Mapping
uint16_t thresholds[] = {55,58,62,66,75,81,88,97,116,131,152,178,254,341,512,1024};
char keys[] =           { '+', 'E', '.', '0', '-', '3', '2', '1', '*', '6', '5', '4', '/', '9', '8', '7' };

// Calculator state variables
Stack stack;              // RPN stack
char numberBuffer[16];    // Buffer for number entry
int bufferIndex = 0;     // Current position in number buffer
bool newNumber = true;    // Flag for starting new number entry
bool historyMode = false; // Toggle for history view
bool secondaryMode = false; // Toggle for secondary keyboard mode

// History tracking
struct OperationHistory {
  float operand1;
  float operand2;
  char operation;
  float result;
} history[HISTORY_SIZE];
int historyIndex = 0;
int historyCount = 0;  // Track actual number of operations in history

// Timing for long press detection
unsigned long keyPressStart = 0;
bool keyPressed = false;  // Track if a key is currently being pressed

void setup() {
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  
  // Create custom characters
  lcd.createChar(0, stackTop);
  lcd.createChar(1, stackMid);
  lcd.createChar(2, stackBottom);
  lcd.createChar(3, modeIndicator);
  lcd.createChar(4, operationFlash);
  
  LCDSplash();
  Serial.begin(9600);
  while (!Serial);
}

void LCDSplash() {
  lcd.clear();
  lcd.print(" RPN Calculator ");
  lcd.setCursor(0, 1);
  lcd.print("Advanced Edition");
  delay(2000);
  lcd.clear();
  displayStack();
}

void displayStack() {
  if (historyMode) {
    displayHistory();
    return;
  }
  
  lcd.clear();
  int stackSize = stack.size();
  
  if (stackSize > 0) {
    // Show top two stack elements with visual separator
    if (stackSize >= 2) {
      lcd.write(byte(0));  // Stack top symbol
      lcd.print(" ");
      lcd.print(stack.peek());
    } else {
      lcd.write(byte(2));  // Stack bottom symbol
      lcd.print(" ");
      lcd.print(stack.peek());
    }
  }
  
  // Display current input on bottom line
  lcd.setCursor(0, 1);
  if (secondaryMode) {
    lcd.print("2> ");  // Show secondary mode indicator
  } else {
    lcd.print("> ");
  }
  if (bufferIndex > 0) {
    lcd.print(numberBuffer);
    lcd.print("_");  // Cursor
  } else {
    lcd.print("_");
  }
}

void displayHistory() {
  lcd.clear();
  // Show the most recent operation
  int idx = (historyIndex - 1 + HISTORY_SIZE) % HISTORY_SIZE;
  if (history[idx].operation != '\0') {
    lcd.print(history[idx].operand1);
    lcd.print(history[idx].operation);
    lcd.print(history[idx].operand2);
    lcd.setCursor(0, 1);
    lcd.print("= ");
    lcd.print(history[idx].result);
  }
}

void addToHistory(float a, float b, char op, float result) {
  history[historyIndex].operand1 = a;
  history[historyIndex].operand2 = b;
  history[historyIndex].operation = op;
  history[historyIndex].result = result;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;
  historyCount = min(historyCount + 1, HISTORY_SIZE);
}

void undoLastOperation() {
  if (historyCount > 0) {
    // Move back in history
    historyIndex = (historyIndex - 1 + HISTORY_SIZE) % HISTORY_SIZE;
    historyCount--;
    
    // Get the last operation
    OperationHistory lastOp = history[historyIndex];
    
    // Remove the result from stack
    if (!stack.isEmpty()) {
      stack.pop();
    }
    
    // Restore the original operands
    stack.push(lastOp.operand1);
    stack.push(lastOp.operand2);
  }
}

void processDigit(char key) {
  if (bufferIndex < 15) {  // Prevent buffer overflow
    numberBuffer[bufferIndex++] = key;
    numberBuffer[bufferIndex] = '\0';  // Null terminate
  }
}

void processDecimal(char key) {
  // Only add decimal if we don't already have one
  if (strchr(numberBuffer, '.') == NULL && bufferIndex < 15) {
    numberBuffer[bufferIndex++] = key;
    numberBuffer[bufferIndex] = '\0';
  }
}

void processEnter() {
  if (bufferIndex > 0) {
    float value = atof(numberBuffer);
    stack.push(value);
    bufferIndex = 0;
    numberBuffer[0] = '\0';
  }
}

void processOperator(char op) {
  if (stack.size() < 2) return;  // Need at least 2 numbers
  
  float b = stack.pop();
  float a = stack.pop();
  float result;
  
  switch(op) {
    case '+': result = a + b; break;
    case '-': result = a - b; break;
    case '*': result = a * b; break;
    case '/': result = b != 0 ? a / b : 0; break;
    default: return;
  }
  
  addToHistory(a, b, op, result);
  stack.push(result);
}

void showModeChange() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(3));  // Mode indicator
  lcd.print(" ");
  if (secondaryMode) {
    lcd.print("Secondary Mode");
  } else {
    lcd.print("Normal Mode");
  }
  delay(500);
  displayStack();
}

void showOperationFeedback() {
  // Flash the display briefly
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(4));  // Full block character
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.setCursor(0, 1);
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  lcd.write(byte(4));
  delay(50);
  displayStack();
}

void checkLongPress(char key) {
  unsigned long currentTime = millis();
  
  if (key == 'E') {  // Use Enter key for mode toggle
    if (currentTime - keyPressStart > LONG_PRESS) {
      if (historyMode) {
        historyMode = false;
      } else {
        undoLastOperation();
      }
      showOperationFeedback();
      delay(200);  // Debounce
      keyPressed = false;  // Reset key press state
    }
  } else if (key == '.') {  // Decimal point for secondary mode toggle
    if (currentTime - keyPressStart > LONG_PRESS) {
      secondaryMode = !secondaryMode;
      showModeChange();
      delay(200);  // Debounce
      keyPressed = false;  // Reset key press state
    }
  }
}

void processKey(char key) {
  if (!keyPressed) {
    keyPressed = true;
    keyPressStart = millis();
  }
  
  // Check for long press first
  checkLongPress(key);
  
  // If we just entered secondary mode, don't add the decimal point
  if (key == '.' && secondaryMode) {
    return;
  }
  
  if (historyMode && key != 'E') {
    historyMode = false;
    displayStack();
    return;
  }
  
  bool operationPerformed = false;
  
  if (secondaryMode) {
    // Secondary mode key handling
    if (key == '7') {
      // Delete last character
      if (bufferIndex > 0) {
        bufferIndex--;
        numberBuffer[bufferIndex] = '\0';
        operationPerformed = true;
      }
    } else if (key == '8') {
      // Clear the buffer
      bufferIndex = 0;
      numberBuffer[0] = '\0';
      operationPerformed = true;
    } else if (key == 'E') {
      // Enter still works in secondary mode
      processEnter();
      operationPerformed = true;
    } else if (key == '+') {
      // Swap top 2 values
      if (stack.size() >= 2) {
        float a = stack.pop();
        float b = stack.pop();
        stack.push(a);
        stack.push(b);
        operationPerformed = true;
      }
    } else if (key == '-') {
      // Drop top value
      if (!stack.isEmpty()) {
        stack.pop();
        operationPerformed = true;
      }
    } else if (key == '*') {
      // Duplicate top value
      if (!stack.isEmpty()) {
        float a = stack.peek();
        stack.push(a);
        operationPerformed = true;
      }
    } else if (key == '/') {
      // Rotate top 3 values
      if (stack.size() >= 3) {
        float a = stack.pop();
        float b = stack.pop();
        float c = stack.pop();
        stack.push(b);
        stack.push(a);
        stack.push(c);
        operationPerformed = true;
      }
    } else if (key == '0') {
      // Square root
      if (!stack.isEmpty()) {
        float x = stack.pop();
        if (x >= 0) {
          stack.push(sqrt(x));
        } else {
          stack.push(0);  // Handle negative input
        }
        operationPerformed = true;
      }
    } else if (key == '1') {
      // Square
      if (!stack.isEmpty()) {
        float x = stack.pop();
        stack.push(x * x);
        operationPerformed = true;
      }
    } else if (key == '2') {
      // Log base 10
      if (!stack.isEmpty()) {
        float x = stack.pop();
        if (x > 0) {
          stack.push(log10(x));
        } else {
          stack.push(0);  // Handle non-positive input
        }
        operationPerformed = true;
      }
    } else if (key == '3') {
      // Exponential (e^x)
      if (!stack.isEmpty()) {
        float x = stack.pop();
        stack.push(exp(x));
        operationPerformed = true;
      }
    } else if (key == '4') {
      // Sine
      if (!stack.isEmpty()) {
        float x = stack.pop();
        stack.push(sin(x));
        operationPerformed = true;
      }
    } else if (key == '5') {
      // Cosine
      if (!stack.isEmpty()) {
        float x = stack.pop();
        stack.push(cos(x));
        operationPerformed = true;
      }
    } else if (key == '6') {
      // Tangent
      if (!stack.isEmpty()) {
        float x = stack.pop();
        stack.push(tan(x));
        operationPerformed = true;
      }
    } else if (key == '9') {
      // Modulo
      if (stack.size() >= 2) {
        float b = stack.pop();
        float a = stack.pop();
        if (b != 0) {
          stack.push(fmod(a, b));
        } else {
          stack.push(0);  // Handle division by zero
        }
        operationPerformed = true;
      }
    }
  } else {
    // Normal mode key handling
    if (isdigit(key)) {
      processDigit(key);
    } else if (key == '.') {
      processDecimal(key);
    } else if (key == 'E') {
      processEnter();
      operationPerformed = true;
    } else if (key == '+' || key == '-' || key == '*' || key == '/') {
      if (bufferIndex > 0) {
        processEnter();
      }
      processOperator(key);
      operationPerformed = true;
    }
  }
  
  if (operationPerformed) {
    showOperationFeedback();
  } else {
    displayStack();
  }
}

char getKey() {
  uint16_t value = 0;
  
  while ((value = analogRead(KEYPAD)) == 0) {
    if (keyPressed) {
      keyPressed = false;  // Reset key press state when key is released
    }
  }
  
  delay(DURATION);  // Debounce
  
  if (DEBUG) {
    Serial.print("ADC: ");
    Serial.print(value);
    Serial.print("\tVoltage: ");
    Serial.println(value * 5.0 / 1023);
  }

  // Determine key index
  uint8_t index = 0;
  while (value > thresholds[index] && index < sizeof(thresholds) / sizeof(thresholds[0]) - 1) {
    index++;
  }

  return keys[index];
}

void loop() {
  char key = getKey();
  processKey(key);
  delay(50);  // Small delay to prevent bouncing
}