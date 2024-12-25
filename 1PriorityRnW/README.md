# Reader-Writer Problem with Reader Priority

## Description

This program demonstrates a multi-threaded solution to the **Reader-Writer Problem** with **Reader Priority**. The objective is to ensure that reader threads have priority over writer threads when accessing a shared variable. Writers can only write when there are no readers actively reading.

The solution uses **pthreads**, **mutexes**, and **condition variables** for synchronization to meet the following constraints:

- **Readers have priority**: If any readers are waiting, they are served before writers.
- Multiple readers and writers are supported (default: 5 of each).
- Readers and writers perform their respective operations multiple times (default: `X` reads/writes).

## Features

### Reader Threads
- Read the value of the shared variable.
- Print:
  - The value read.
  - The number of active readers at the time of reading.

### Writer Threads
- Write a new value to the shared variable.
- Print:
  - The value written.
  - The number of active readers when writing (always `0` for writer priority enforcement).

### Random Delays
- Before accessing the shared variable, both readers and writers wait for a random duration. This ensures that reads and writes do not occur simultaneously.

## Requirements

- **Programming Language**: C
- **Libraries Used**:
  - `pthread.h` for threading.
  - `stdlib.h` and `time.h` for random delays.
  - `stdio.h` for input/output.
  - `unistd.h` for thread sleep.

## How It Works

The program employs:
- A **shared variable** to be read and written by threads.
- A **reader count** to keep track of the number of active readers.
- A **mutex** to ensure mutual exclusion when modifying shared resources.
- **Condition variables** to coordinate between readers and writers:
  - Writers wait until there are no active readers.
  - Readers are prioritized over writers.

## Execution Flow

1. **Readers**:
   - Wait for access to the shared variable.
   - Read the value.
   - Print the value read and the number of active readers.
   - Release access after reading.

2. **Writers**:
   - Wait for all readers to finish.
   - Write a new value to the shared variable.
   - Print the value written and confirm that no readers were present.
   - Release access after writing.

3. **Synchronization**:
   - **Readers** increment/decrement the reader count while ensuring mutual exclusion.
   - **Writers** wait until the reader count is `0` to proceed with writing.

## How to Run

### Compilation
Use the following command to compile the program:
```bash
gcc -o reader_writer reader_writer.c -lpthread
