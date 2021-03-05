#ifndef KernelChunks_h
#define KernelChunks_h

#include <stdint.h>
#include <unistd.h>

#define MAIN_CHUNK_HANDLE 1
#define EPROM_SIZE 4096
#define P_INSTANCE 0
#define P_HANDLE 1
#define P_SIZE 2
#define P_OVERHEAD 4

#define PERSISTANT_HEAP_ADDRESS 130

#define UNUSED_CHUNK 255
#define TMP_HANDLE 254

int goToFirstChunk();
int endOfChunks(int);
int nextChunk(int);

void    freeAllChunks();
void    decreaseInstanceOffset();
uint8_t _chunkData(int chunk, int i);
uint8_t chunkInstance(int);
uint8_t chunkHandle(int);
int     _chunkUnused(int);
int     _chunkSize(int);

unsigned int chunkSize(int handle);
void         _mergeChunks();

int  chunkForHandleExists(int handle);
int  chunkForInstanceAndHandleExists(int instance, int handle);
int  chunkAddressForInstanceAndHandle(int instance, int handle);
int  resizeChunk(unsigned int handle, int newSize);
void deallocChunk(int handle);

void          pset(int address, unsigned char value);
unsigned char pget(int handle, int address);
unsigned char pget(int address);
void          pset(int handle, int address, unsigned char value);
void          dumpChunks();
void          dumpChunksTxt();
unsigned int  allocChunk(int handle, int size);
int           insertChunkFragment(int handle, int address, int delta);
int           deleteChunkFragment(int handle, int address, int delta);
unsigned int  availablePersistantHeap();
int           pgetInt(int address);
void          psetInt(int address, unsigned int value);
void          psetFloat(int address, float value);
float         pgetFloat(int address);

#endif
