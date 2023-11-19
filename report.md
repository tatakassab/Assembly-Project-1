**Project Report: RISC-V Simulator Implementation**

**Team Members:**
1. [Mohamed Abdelmagid], ID: 900223215
2. [Tarek Kassab], ID: 900213491
3. [Shaza Ali], ID: 900213391

### 1. Brief Description of Implementation

Our RISC-V simulator is designed to simulate the behavior of a RISC-V instructions, supporting a wide range of instructions. The simulator includes the basic set of instructions, arithmetic and logical operations, memory access instructions, conditional branches, and jumps. We implemented bonus features such as support for the multiplication and dicision instructions (First Bonus). We added test cases for real programs (Second Bonus). We also support providing data in binary, hexadecimal, and deciaml as well (Third Bonus).

### 2. Design Decisions and Assumptions

1. **Modularity:**
   - Decision: Separate the simulator into distinct modules or classes for better organization.
   - Reasoning: Modular design allows for easier maintenance, testing, and extension of the simulator.

2. **Instruction Execution Model:**
   - Decision: Implement an execution model that follows the RISC-V instruction set architecture. It is a single function called `execute` to call the other funcion that is related to the instruction.
   - Reasoning: Adhering to the RISC-V ISA ensures compatibility with RISC-V programs and facilitates easier validation against the official specification.

3. **Memory Representation:**
   - Decision: Use a memory data structure for instructions as a vector of strings and memory as a map to simulate load and store operations.

4. **Error Handling:**
   - Decision: Implement error-checking mechanisms for invalid instructions, memory access violations, or other exceptional conditions.
   - Reasoning: Robust error handling improves the simulator's reliability and aids in debugging.


### Assumptions:

1. **Correct Input Programs:**
   - Assumption: Input programs are correctly written RISC-V assembly code.
   - Reasoning: The simulator focuses on the execution of valid RISC-V instructions. Handling invalid or malformed input just by telling the user that there is an error in a certain line.

2. **Little-Endian Architecture:**
   - Assumption: The simulator assumes a little-endian memory architecture unless specified otherwise.
   - Reasoning: Little-endian is a common architecture, and assuming it simplifies memory access and byte-ordering operations.

3. **Single-Threaded Execution:**
   - Assumption: The simulator assumes single-threaded execution and does not handle multi-threading or parallelism.
   - Reasoning: Simplifying the simulator to a single-threaded model reduces complexity and is suitable for many educational or introductory purposes.

4. **Static Memory Allocation:**
   - Assumption: Memory allocation is static, and there is no support for dynamic memory management (e.g., malloc/free).
   - Reasoning: Dynamic memory management adds complexity, and a focus on static allocation aligns with the simplicity often desired in simulators.


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