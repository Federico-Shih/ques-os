# Ques-OS
Ques-OS (or CheeseOS, due to Quesos being Cheese in spanish) is a bare metal operating system created to learn about multiple implementations of common Operating Systems components.
The focus of this class is to build step by step different parts of a concurrent monocore operating system, looking to solve common concurrency problems such as the chinese philosophers problem.

This operating system can succesfully run multiple processes using the daemon operator (&) with context switching, and is able to run IO heavy processes such as the infinite fibonacci sequence that rely on sleep to print infinitely.

Kernel implements:
- Memory manager (K&R and Buddy memory manager)
- Scheduler/process management, context switching, child process spawning.
- Synchronization (semaphores)
- Inter-Process Communication

Userland implements:
- Shell
- Pipe operator
- The chinese philosophers problem

Environment setup:
1- Install the following packages before building the Toolchain and Kernel:

nasm qemu gcc make

2- Build the Toolchain

Execute the following commands on the x64BareBones project directory:

  user@linux:$ cd Toolchain
  user@linux:$ make all

3- Build the Kernel

From the x64BareBones project directory run:

  user@linux:$ make all

4- Run the kernel

From the x64BareBones project directory run:

  user@linux:$ ./run.sh


Author: Rodrigo Rearden (RowDaBoat)
Collaborator: Augusto Nizzo McIntosh
