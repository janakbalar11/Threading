# Description

This is an encoding program that can take files containing large amounts of input and encode them in binary fromat. It uses multithreading to do so in order to be able to encode data more efficiently.  
## Features:
It separates the file into fixed chunks of characters and inserts them into a task queue. Whenever a chunk is inserted into the task queue, the threads are alerted and begin encoding the chunk. Tasks are contiuously added to the queue while the threads are encoding and returning chunks. Once all chunks have been encoded and returned, the program terminates.
