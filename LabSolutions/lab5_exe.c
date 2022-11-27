/**
 *   \file skeleton.c
 *   \authors Konstantinos Petridis, Dimitris Floros
 *   \brief Skeleton program for testing your implemention on elearning.auth.gr
 *
 *  The program defines the main (entry point) and the parsing routines to read
 *  a request from STDIN and call the corresponding functions. You only need to
 *  implement the requested functions. DO NOT CHANGE main and/or the parsing
 *  functions. The functions you need to implement are located at the bottom
 *  of the file.
 *
 *  DO NOT ADD ANY PRINT COMMANDS WHEN UPLOADING TO ELEARNING; THE TESTER PARSES
 *  THE OUTPUT AND WILL REPORT ERRORS OTHERWISE !
 *
 */


/*****************************************************************************/
/*                 TESTER/PARSER: DO NOT CHANGE ANYTHING HERE                */
/*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MaxBufferSize 200

/**
 * Each memory segment (block) is represented by a memorySegment structure object.
 */
typedef struct memorySegment {
    uint16_t startAddress;
    uint16_t length;
    bool occupied;
    struct memorySegment *next;
} memorySegment;

/**
 * Functions for the actual handling of the memory segments. You need to implement these!
 */
void printList(memorySegment *memList);
void insertListItemAfter(memorySegment *current);
void removeListItemAfter(memorySegment *current);
memorySegment *initializeMemory();

memorySegment *assignFirst(memorySegment *memList, uint16_t requestedMem);
memorySegment *assignBest(memorySegment *memList, uint16_t requestedMem);
memorySegment *assignNext(memorySegment *memList, uint16_t requestedMem);
void reclaim(memorySegment *memList, memorySegment *thisOne);

memorySegment *assignFirstDyn(memorySegment *memList, uint16_t requestedMem);
memorySegment *assignBestDyn(memorySegment *memList, uint16_t requestedMem);
memorySegment *assignNextDyn(memorySegment *memList, uint16_t requestedMem);
void reclaimDyn(memorySegment *memList, memorySegment *thisOne);



void parseMessage(char *buffer, size_t size);
void execute(char *token, memorySegment *(*assignMemory)(memorySegment *mem, uint16_t size),
             void (*reclaimMemory)(memorySegment *mem, memorySegment *thisOne),
             memorySegment *memList, char *savePointer1, char *savePointer2);

memorySegment *initializeStaticMemory(int memorySize, int blockSize);
memorySegment *initializeDynamicMemory(int memorySize);

int main() {
    char buff[MaxBufferSize];
    parseMessage(buff, sizeof(buff));
}

void parseMessage(char *buffer, size_t size) {
    /* read the string from the stdin and check for error */
    if (fgets(buffer, size, stdin) == NULL) {
        printf("Error reading stdint.");
        exit(1);
    }

    /* pointer to the memory, which is represented by a linked list */
    memorySegment *memList;

    /* declare the delimiter, so that the string is divided in separate tokens */
    const char delimiter[2] = " ";

    /* used to specify on which string, the strtok_r performs */
    char *savePointer1 = NULL;
    char *savePointer2 = NULL;
    char *savePointer3 = NULL;
    char *savePointer4 = NULL;

    /* reading of the string's header with the necessary information about the test */
    char *sizeOfMemory = strtok_r(buffer, delimiter, &savePointer1);
    char *typeOfMemory = strtok_r(NULL, delimiter, &savePointer1);
    char *assignMethod = strtok_r(NULL, delimiter, &savePointer1);

    /* array of pointers to the appropriate memory management methods */
    memorySegment *(*methodOfAssignement) (memorySegment *memList, uint16_t requestedMem);
    void (*methodOfReclaim) (memorySegment *memList, memorySegment* thisOne);

    if (typeOfMemory[0] == 'S') {
        if (strcmp(assignMethod, "AF") == 0) {
            methodOfAssignement = assignFirst;
        } else if (strcmp(assignMethod, "AB") == 0) {
            methodOfAssignement = assignBest;
        } else if (strcmp(assignMethod, "AN") == 0) {
            methodOfAssignement = assignNext;
        } else {
            printf("Unknown memory assignement method.");
            exit(1);
        }
        methodOfReclaim = reclaim;
        char *blockSize = strtok_r(typeOfMemory, "S", &savePointer2);
        memList = initializeStaticMemory(atoi(sizeOfMemory), atoi(blockSize));
    } else if (typeOfMemory[0] == 'D') {
        if (strcmp(assignMethod, "AF") == 0) {
            methodOfAssignement = assignFirstDyn;
        } else if (strcmp(assignMethod, "AB") == 0) {
            methodOfAssignement = assignBestDyn;
        } else if (strcmp(assignMethod, "AN") == 0) {
            methodOfAssignement = assignNextDyn;
        } else {
            printf("Unknown memory assignement method.");
            exit(1);
        }
        methodOfReclaim = reclaimDyn;
        memList = initializeDynamicMemory(atoi(sizeOfMemory));
    } else {
        printf("Invalid memory type.");
        exit(1);
    }

    char *token = strtok_r(NULL, delimiter, &savePointer1);

    while (true) {
        token = strtok_r(NULL, delimiter, &savePointer1);
        if (token == NULL) {
            break;
        }
        execute(token, (*methodOfAssignement), methodOfReclaim, memList, savePointer3, savePointer4);
    }
    printList(memList);

}

void execute(char *token, memorySegment *(*assignMemory)(memorySegment *mem, uint16_t size),
             void (*reclaimMemory)(memorySegment *mem, memorySegment *thisOne),
             memorySegment *memList, char *savePointer1, char *savePointer2) {
    if (token[0] == 'A') {
            char *requestedMemory = strtok_r(token, "A", &savePointer1);
            (*assignMemory)(memList, atoi(requestedMemory));
    } else if (token[0] == 'R') {
        int indexOfBlockToReclaim = atoi(strtok_r(token, "R", &savePointer2));
        memorySegment *blockToReclaim = memList;
        if (indexOfBlockToReclaim <= 0) {   // 1-based, first block is block-1
            exit(1);
        }
        if (indexOfBlockToReclaim > 1) {
            while (indexOfBlockToReclaim > 1) {
                blockToReclaim = blockToReclaim->next;
                indexOfBlockToReclaim--;
            }
        }
        (*reclaimMemory)(memList, blockToReclaim);
    }
}


memorySegment *initializeStaticMemory(int memorySize, int blockSize) {
    int numberOfBlocks = memorySize / blockSize;
    int remainderSize = memorySize % blockSize;

    memorySegment *firstBlock = (memorySegment *)malloc(sizeof(memorySegment));
    firstBlock->occupied = false;
    firstBlock->length = blockSize;
    firstBlock->startAddress = 0;

    memorySegment *previousSegment = firstBlock;

    for (int i = 0; i < numberOfBlocks - 1; i++) {
        memorySegment *nextMemorySegment = (memorySegment *)malloc(sizeof(memorySegment));
        nextMemorySegment->occupied = false;
        nextMemorySegment->startAddress = previousSegment->startAddress + blockSize;
        nextMemorySegment->length = blockSize;
        previousSegment->next = nextMemorySegment;
        previousSegment = nextMemorySegment;
    }
    if (remainderSize > 0) {
        memorySegment *lastMemorySegment = (memorySegment *)malloc(sizeof(memorySegment));
        lastMemorySegment->length = remainderSize;
        lastMemorySegment->occupied = false;
        lastMemorySegment->startAddress = previousSegment->startAddress + blockSize;
        previousSegment->next = lastMemorySegment;
    }
    return firstBlock;
}

memorySegment *initializeDynamicMemory(int memorySize) {
    memorySegment *memory = (memorySegment *)malloc(sizeof(memorySegment));
    memory->startAddress = 0;
    memory->length = memorySize;
    memory->occupied = false;
    return memory;
}
//////panw to skeleton tou synadelfou
////////////////////////////////////////////////////kanw ta parakatw basei ta hints kai tis metablhtes tou skeleton kai tou elearning
/* ==================== (1) LIST FUNCTIONS */
static uint16_t lengthOfNewBlock = 0;
static uint16_t startAddressOfNewBlock = 0;

void printList(memorySegment *memList) {
    /* TODO: Implement this function */
    memorySegment *current;
    current = memList;

    while (true) {
        printf("%d %d %s\n", current->startAddress, current->length, current->occupied ? "Occupied!" : "Free");
        if (current->next == NULL) {
            break;
        }
        current = current->next;
    }
}

void insertListItemAfter(memorySegment * current) {
  /* TODO: Implement this function */
    memorySegment *newItem;
    newItem = (memorySegment *)malloc(sizeof(memorySegment));
    newItem->length = lengthOfNewBlock;
    newItem->startAddress = startAddressOfNewBlock;
    newItem->occupied = false;

    if (current != NULL) { 
        if (current->next) {
            newItem->next = current->next;
            current->next = newItem;
        } else {
            current->next = newItem;
        }
        current = newItem->next;
    } else {
        current = newItem;
    }
}

void removeListItemAfter(memorySegment *current) {
    /* TODO: Implement this function */
    if (current) {
        if (current->next->next) {
            uint16_t offsetToSubtract = current->next->length;
            current->next = current->next->next;
            current = current->next;
            while (current != NULL) {
                current->startAddress -= offsetToSubtract;
                current = current->next;
            }
        } else {
            current->next = NULL;
        }
    }
}

/* ==================== (2) FIXED MEMORY ALLOCATIONS */
/**
 * Accesses the memory in a linear fashion, iterating over one block at a time. It assigns the first memory block, 
 * that fits the requested memory.
 * 
 * @param memList the memory as a linked list, with each node representing a memory block.
 * @param requestedMem the memory requested by a process.
 * @return memorySegment* the memory block that was allocated by the memory management service.
 */

//memorySegment * assignFirst(memorySegment * memList, uint requestedMem);
memorySegment *assignFirst(memorySegment *memList, uint16_t requestedMem) {
    /* TODO: Implement this function */
    memorySegment *currentSegment;
    currentSegment = memList;

    while(currentSegment != NULL) {
        if (currentSegment->occupied) {
            currentSegment = currentSegment->next;
            continue;
        } 
        if (currentSegment->length >= requestedMem) {
            currentSegment->occupied = true;
            return currentSegment;
        }
        currentSegment = currentSegment->next;
    }

    return (NULL);
}
/**
 * Accesses the memory in a linear fashion, iterating over one block at a time. It locates the memory block that both 
 * fits the requested memory and is closest to it, in order to minimize memory gaps after memory assignement.
 * 
 * @param memList the memory as a linked list, with each node representing a memory block.
 * @param requestedMem the memory requested by a process.
 * @return memorySegment* the memory block that was allocated by the memory management service.
 */
//memorySegment * assignBest(memorySegment * memList, uint requestedMem);
memorySegment *assignBest(memorySegment *memList, uint16_t requestedMem) {
    /* TODO: Implement this function */
    memorySegment *currentSegment;
    currentSegment = memList;
    memorySegment *bestBlock = NULL;
    uint16_t bestFit = UINT16_MAX;

    while(currentSegment != NULL) {
        if (currentSegment->occupied) {
            currentSegment = currentSegment->next;
            continue;
        } 
        if (currentSegment->length >= requestedMem) {
            uint16_t currentFit = currentSegment->length - requestedMem;
            if (currentFit <= bestFit) {
                bestFit = currentFit;
                bestBlock = currentSegment;
            }
        }
        currentSegment = currentSegment->next; 
    }

    if (bestBlock != NULL) {
        bestBlock->occupied = true;
        return bestBlock;
    }

    return (NULL);
}
/**
 * Static memory allocation techniques, for a requested memory size, and memory blocks. The memory remains intact, no 
 * segments are added, removed or changed.
 */

/**
 * Pointer to the last allocated block in the memory, that works as an indicator for the starting point of the Next Fit search.
 */
memorySegment *lastAllocatedBlock;
/**
 * Accesses the memory in a linear fashion, iterating over one block at a time. It has the same functionality as the 
 * Firs Fit, but the searching starts from the block that was allocated during the last memory assignement. It tends to 
 * allocate memory segments at the end of the memory list, leaving gaps which need to be concatenated in order to boost
 * the efficiency of the method.
 * 
 * @param memList the memory as a linked list, with each node representing a memory block.
 * @param requestedMem the memory requested by a process.
 * @return memorySegment* the memory block that was allocated by the memory management service.
 */
//memorySegment * assignNext(memorySegment * memList, uint requestedMem);
memorySegment *assignNext(memorySegment *memList, uint16_t requestedMem) {
    /* TODO: Implement this function */
    memorySegment *currentSegment;
    if (lastAllocatedBlock == NULL) {
        currentSegment = memList;
    } else {
        currentSegment = lastAllocatedBlock;
    }

    while(currentSegment != NULL) {
        if (currentSegment->occupied) {
            currentSegment = currentSegment->next;
            continue;
        } 
        if (currentSegment->length >= requestedMem) {
            currentSegment->occupied = true;
            lastAllocatedBlock = currentSegment;
            return currentSegment;
        }
        currentSegment = currentSegment->next;
    }


    return (NULL);
}
/**
 * Statically frees the requested memory block.
 * 
 * @param memList the memory as a linked list, with each node representing a memory block.
 * @param thisOne the memory block to reclaim.
 */
//void reclaim(memorySegment * memList, memorySegment * thisOne);
void reclaim(memorySegment *memList, memorySegment* thisOne) {
    /* TODO: Implement this function */
    memorySegment *currentSegment;
    currentSegment = memList;

    while (currentSegment != NULL) {
        if (currentSegment->startAddress == thisOne->startAddress) {
            currentSegment->occupied = false;
            break;
        }
        currentSegment = currentSegment->next;
    }
}


/* ==================== (3) DYNAMIC MEMORY ALLOCATIONS */

/**
 * Dynamic memory allocation techniques, for a requested memory size, and memory blocks. Each time a block is allocated, 
 * the remaining unallocated space of the block, is concatenated to the next, if it exists and is free. Otherwise, we
 * insert a new, independent block of the corresponding size, after the allocated space.
 */

/**
 * Accesses the memory in a linear fashion, iterating over one block at a time. It assigns the first memory block, 
 * that fits the requested memory. 
 * 
 * @param memList the memory as a linked list, with each node representing a memory block.
 * @param requestedMem the memory requested by a process.
 * @return memorySegment* the memory block that was allocated by the memory management service.
 */
//memorySegment * assignFirstDyn(memorySegment * memList, uint requestedMem);
memorySegment *assignFirstDyn(memorySegment *memList, uint16_t requestedMem) {
    /* TODO: Implement this function */
    memorySegment *currentSegment;
    currentSegment = memList;

    while(currentSegment != NULL) {
        if (currentSegment->occupied) {
            currentSegment = currentSegment->next;
            continue;
        } 
        if (currentSegment->length == requestedMem) {
            currentSegment->occupied = true;
            return currentSegment;
        }
        if (currentSegment->length > requestedMem) {
            uint16_t freeMemory = currentSegment->length - requestedMem;
            currentSegment->occupied = true;
            currentSegment->length = requestedMem;
            if (currentSegment->next) {
                if (currentSegment->next->occupied == false) {
                    currentSegment->next->startAddress = currentSegment->startAddress + requestedMem;
                    currentSegment->next->length += freeMemory;
                    return currentSegment;
                } 
            }
            lengthOfNewBlock = freeMemory;
            startAddressOfNewBlock = currentSegment->startAddress + requestedMem;
            insertListItemAfter(currentSegment);
            return currentSegment;
        } 
        currentSegment = currentSegment->next;
    }

    return (NULL);
}

/**
 * Accesses the memory in a linear fashion, iterating over one block at a time. It locates the memory block that both 
 * fits the requested memory and is closest to it, in order to minimize memory gaps after memory assignement.
 * 
 * @param memList the memory as a linked list, with each node representing a memory block.
 * @param requestedMem the memory requested by a process.
 * @return memorySegment* the memory block that was allocated by the memory management service.
 */
//memorySegment * assignBestDyn(memorySegment * memList, uint requestedMem);
memorySegment *assignBestDyn(memorySegment *memList, uint16_t requestedMem) {
    /* TODO: Implement this function */
    memorySegment *currentSegment;
    currentSegment = memList;
    memorySegment *bestBlock = NULL;
    uint16_t bestFit = UINT16_MAX;
    bool exactFit = false;

    while(currentSegment != NULL) {
        if (currentSegment->occupied) {
            currentSegment = currentSegment->next;
            continue;
        } 
        if (currentSegment->length == requestedMem) {
            bestBlock = currentSegment;
            exactFit = true;
            break; 
        }
        if (currentSegment->length > requestedMem) {
            uint16_t currentFit = currentSegment->length - requestedMem;
            if (currentFit <= bestFit) {
                bestFit = currentFit;
                bestBlock = currentSegment;
            }
        }
        currentSegment = currentSegment->next; 
    }

    if (bestBlock == NULL) {
        return (NULL);
    }
    currentSegment = memList;

    while (currentSegment != NULL) {
        if (currentSegment->occupied) {
            currentSegment = currentSegment->next;
            continue;
        }
        if (currentSegment->startAddress == bestBlock->startAddress) {
            uint16_t freeMemory = currentSegment->length - requestedMem;
            currentSegment->occupied = true;
            if (exactFit) {
                return currentSegment;
            }
            currentSegment->length = requestedMem;
            if (currentSegment->next) {
                if (currentSegment->next->occupied == false) {
                    currentSegment->next->startAddress = currentSegment->startAddress + requestedMem;
                    currentSegment->next->length += freeMemory;
                    return currentSegment;
                } 
            }
            lengthOfNewBlock = freeMemory;
            startAddressOfNewBlock = currentSegment->startAddress + requestedMem;
            insertListItemAfter(currentSegment);
            return currentSegment;
        }
        currentSegment = currentSegment->next; 
    }

    return (NULL);
}
/**
 * Accesses the memory in a linear fashion, iterating over one block at a time. It has the same functionality as the 
 * Firs Fit, but the searching starts from the block that was allocated during the last memory assignement. It tends to 
 * allocate memory segments at the end of the memory list, leaving gaps which need to be concatenated in order to boost
 * the efficiency of the method.
 * 
 * @param memList the memory as a linked list, with each node representing a memory block.
 * @param requestedMem the memory requested by a process.
 * @return memorySegment* the memory block that was allocated by the memory management service.
 */
//memorySegment * assignNextDyn(memorySegment * memList, uint requestedMem);
memorySegment *assignNextDyn(memorySegment *memList, uint16_t requestedMem) {
    /* TODO: Implement this function */
    memorySegment *currentSegment;
    if (lastAllocatedBlock == NULL) {
        currentSegment = memList;
    } else {
        currentSegment = lastAllocatedBlock;
    }

    while(currentSegment != NULL) {
        if (currentSegment->occupied) {
            currentSegment = currentSegment->next;
            continue;
        } 
        if (currentSegment->length == requestedMem) {
            currentSegment->occupied = true;
            return currentSegment;
        }
        if (currentSegment->length > requestedMem) {
            uint16_t freeMemory = currentSegment->length - requestedMem;
            currentSegment->occupied = true;
            currentSegment->length = requestedMem;
            lastAllocatedBlock = currentSegment;
            if (currentSegment->next) {
                if (currentSegment->next->occupied == false) {
                    currentSegment->next->startAddress = currentSegment->startAddress + requestedMem;
                    currentSegment->next->length += freeMemory;
                    return currentSegment;
                } 
            }
            lengthOfNewBlock = freeMemory;
            startAddressOfNewBlock = currentSegment->startAddress + requestedMem;
            insertListItemAfter(currentSegment);
            return currentSegment;
        }
        currentSegment = currentSegment->next;
    }



    return (NULL);
}
/**
 * Dynamically frees the requested memory block. If the next memory block is free as well, it concatenates the two.
 * 
 * @param memList the memory as a linked list, with each node representing a memory block.
 * @param thisOne the memory block to reclaim.
 */
//void reclaimDyn(memorySegment * memList, memorySegment * thisOne);
void reclaimDyn(memorySegment *memList, memorySegment *thisOne) {
    /* TODO: Implement this function */
    memorySegment *currentSegment;
    currentSegment = memList;

    while (currentSegment != NULL) {
        if (currentSegment->startAddress == thisOne->startAddress) {
            currentSegment->occupied = false;
            if (currentSegment->next) {
                if (currentSegment->next->occupied == false) {
                    currentSegment->length += currentSegment->next->length;
                    currentSegment->next = currentSegment->next->next;
                }
            }
            break;
        }
        currentSegment = currentSegment->next;
    }
}
