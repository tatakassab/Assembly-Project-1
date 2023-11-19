**Project Report: RISC-V Simulator Implementation**

**Team Members:**
1. [Mohamed Abdelmagid], ID: 900223215
2. [Tarek Kassab], ID: 
3. [Shaza Ali], ID: 

### 1. Brief Description of Implementation

Our RISC-V simulator is designed to emulate the behavior of a RISC-V instructions, supporting a wide range of instructions. The simulator includes the basic set of instructions, arithmetic and logical operations, memory access instructions, conditional branches, jumps, and multiplication/division instructions. We implemented bonus features such as support for the multiplication and dicision instructions (First Bonus). We added test cases for real programs (Second Bonus). We also support providing data in binary, hexadecimal, and deciaml as well (Third Bonus).

### 2. Design Decisions and Assumptions

- **Modular Design:** We adopted a modular design to enhance maintainability. Different components handle specific instruction types, allowing for easy extension and modification.
  
- **Memory Management:** Our simulator assumes a simple memory model with separate data and instruction memory. Memory is initialized with default values, and the stack grows downward.
  
- **Error Handling:** We implemented error checks for boundary violations, undefined instructions, and memory access issues. In case of an error, the simulator provides meaningful error messages.

### 3. Known Bugs or Issues

#### System Integration Bugs

- Bug:
Issues related to integrating the simulator into a larger system or debugging environment.

- Solution:
To address system integration bugs, it is crucial to ensure proper integration by adhering to any provided guidelines or specifications. Thoroughly test the simulator in the intended environment, identifying and resolving any compatibility issues that may arise during integration. This process may involve collaboration with the larger system development team to ensure seamless compatibility and interoperability.

#### Performance Bugs

- Bug:
Suboptimal performance or excessive resource usage.

- Solution:
To tackle performance bugs, it is essential to optimize critical sections of the code and implement efficient algorithms. Profiling the simulator can help identify and prioritize performance bottlenecks. By employing optimization techniques, such as algorithmic improvements or code restructuring, the simulator can achieve better overall performance. Regular profiling and performance testing should be conducted to maintain optimal system efficiency.

#### Instruction Execution Bugs

- Bug:
Incorrect implementation of one or more RISC-V instructions.

- Solution:
To mitigate instruction execution bugs, a thorough review of the execution logic for each RISC-V instruction is necessary. This review should consider both the RISC-V specification and potential edge cases that may affect correct execution. Implementing a comprehensive set of test cases, covering various scenarios and corner cases, is essential to validate the correctness of instruction execution. Continuous testing and validation efforts should be employed to catch and rectify any discrepancies in the implementation.

### 4. User Guide: Compile and Run

#### Compilation:
1. Clone the repository from [GitHub URL].
2. Navigate to the project directory.
3. Add your instructions.txt and data.txt files in the main directory
4. Run the file simulator.cpp
5. Open the terminal
   ```


### 5. Simulated Programs

1. **Program 1: Basic Arithmetic and Logic**
   - A program that covers basic arithmetic (addition, subtraction), logical (AND, OR, XOR), and immediate instructions.

2. **Program 2: Memory Access and Branching**
   - This program involves memory access (load and store instructions) and conditional branching.

3. **Program 3: Looping Structure**
   - A program that includes a loop, demonstrating the capability of the simulator to handle iterative structures.

### Conclusion:

Our RISC-V simulator provides a robust and versatile platform for simulating RISC-V programs. The modular design, error handling, and bonus features contribute to a comprehensive tool for educational and research purposes. Continuous testing and feedback will be crucial for further enhancements and bug fixes.