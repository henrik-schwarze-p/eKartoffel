#ifndef KernelHeap_h
#define KernelHeap_h

#define MAIN_HEAP_HANDLE 1
#define UNUSED_HEAP 255

#define HEAP_SIZE 4242  // any number that sounds cool

#define H_INSTANCE 0
#define H_HANDLE 1
#define H_LEN 2
#define H_DATA 3
#define H_OVERHEAD 3

#define MAIN_HEAP_HANDLE 1

void freeAllHeaps();

void          initHeap(void);
void          freeHeap(int heapHandle);
unsigned char getVar(int owner, int heapHandle, int address);
float         getFloatVar(int owner, int heapHandle, int address);
void          mset(int address, unsigned char value);

void         msetFloat(int heapHandle, int address, float value);
unsigned int availableHeap(void);
void         setHeapData(int address, unsigned char value);
void         msetFloat(int address, float value);
int          heapSize(int heapHandle);

void          dumpHeaps();
void          dumpHeapsTxt();
int           allocHeap(int handle, int size);
void          mset(int heapHandle, int address, unsigned char value);
unsigned char mget(int heapHandle, int address);
unsigned char mget(int address);
void          deleteHeapFragment(int handle, int address, int delta);
int           heapForHandleExists(int heapHandle);

#endif
