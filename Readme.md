# Starve-Free Readers Writers Problem

## Overview

**Starve Free Readers-Writers Problem:** All ***readers*** and ***writers*** will be granted *access to the resource in their order of arrival*. If a writer arrives while readers are accessing the resource, it will wait until those readers free the resource, and then modify it. The same goes for readers a writer has the access to the resource.

This repo contains the pseudocode of the solution.

## Documentation

#### Global variables:

- Use [semaphores](https://www.geeksforgeeks.org/semaphores-in-process-synchronization/) for [mutex](https://en.wikipedia.org/wiki/Lock_(computer_science)). All `semaphores` are initialized to `1`.

- `orderMutex`: Materialize order of arrival. Taken by the entity that requests the access to the resource and is released after it gains the access.
- `accessMutex`: Requested by a writer before modifying a resource.
- `readers`: Counter for the number of readers accessing the resource.
- `readersMutex`: Protect the counter against conflicting accesses.

````C++
  semaphore orderMutex;         // Initialized to 1
  semaphore accessMutex;        // Initialized to 1
  semaphore readersMutex;       // Initialized to 1

  unsigned int readers = 0;     // Number of readers accessing the resource
````

#### Readers Part:

- `Wait()`: Decrements the value of a semaphore by 1.
- `Signal()`: Increments the value of a semaphore by 1.

These are same as [P() or V()](https://cs.nyu.edu/~yap/classes/os/resources/origin_of_PV.html) which are generally used with semaphores.

````C++
void reader(){
  Wait(orderMutex);           // Remember our order of arrival

  Wait(readersMutex);         // We will manipulate the readers counter
  if (readers == 0)           // If there are currently no readers (we came first)
    Wait(accessMutex);        // requests exclusive access to the resource for readers
  readers++;                  // Note that there is now one more reader
  
  Signal(orderMutex);         // Release order of arrival semaphore (we have been served)
  Signal(readersMutex);       // We are done accessing the number of readers for now

  ReadResource();             // Here the reader can read the resource at will

  Wait(readersMutex);         // We will manipulate the readers counter
  readers--;                  // We are leaving, there is one less reader
  if (readers == 0)           // If there are no more readers currently reading...
    Signal(accessMutex);      // ...release exclusive access to the resource
  Signal(readersMutex);       // We are done accessing the number of readers for now
}
````

#### Writers Part:

- `Wait()`: Decrements the value of a semaphore by 1.
- `Signal()`: Increments the value of a semaphore by 1.

These are same as [P() or V()](https://cs.nyu.edu/~yap/classes/os/resources/origin_of_PV.html) which are generally used with semaphores.

````C++
void writer(){
  Wait(orderMutex);           // Remember our order of arrival
  Wait(accessMutex);          // Request exclusive access to the resource
  Signal(orderMutex);         // Release order of arrival semaphore (we have been served)

  WriteResource();            // Here the writer can modify the resource at will

  Signal(accessMutex);        // Release exclusive access to the resource
}
````