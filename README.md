# Description

This is an encoding program written in C that is designed to efficiently encode files containing large amounts of input into binary format. The program utilizes multithreading to improve the encoding process and achieve higher efficiency.
## Features:
The encoding program offers the following features:

1. Chunk Encoding: The program divides the input file into fixed-size chunks of characters. Each chunk is then encoded into binary format. This chunk-based approach allows for efficient processing of large files by breaking them into manageable portions.

2. Task Queue: The encoded chunks are inserted into a task queue. The program continuously adds chunks to the task queue while the encoding threads are actively encoding and returning previously processed chunks. This ensures a continuous encoding process and optimal utilization of computing resources.

3. Multithreading: The program utilizes multithreading to encode the chunks concurrently. Multiple encoding threads are created to handle the encoding tasks in parallel. This parallel processing significantly improves the encoding speed and overall efficiency of the program.

4. Termination: Once all the chunks from the input file have been encoded and returned, the program terminates gracefully. It ensures that all encoding tasks are completed before exiting, allowing for a complete and accurate encoding process.

## Usage
To use the encoding program, follow these steps:

1. Clone or download the project repository to your local machine.

2. Open the terminal or command prompt and navigate to the project directory.

3. Run the make command to compile the source code and generate the executable binary file.

4. It will create an executable file called nyuenc

5. You will be able to pass files to ./nyuenc and encode them in binary 
