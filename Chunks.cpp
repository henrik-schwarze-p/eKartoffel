#include "Chunks.h"
#include "Instance.h"
#include "FatalError.h"
#include "Target.h"

//
// CHUNK MANAGEMENT
// Chunks are memory allocations in the eprom.
// Each chunk has an identifier GIVEN BY THE USER.
//
/* The persistant memory heap consiste of many chunks.

 A chunk has the following structure

 0           INSTANCE (FF==unused)
 1           HANDLE
 2,3         SIZE (the empty chunk may use 2 bytes...)
 4           BEGIN OF DATA
 4+SIZE-1    END OF DATA

 4+SIZE      NEXT CHUNK

 That is, we have 4 bytes overhead per chunk.
 */

// ALL THE METHODS WORK FOR THE ACTIVE INSTANCE

int goToFirstChunk() {
    return PERSISTANT_HEAP_ADDRESS;
}

int endOfChunks(int _chunk) {
    return _chunk >= EPROM_SIZE;
}

int nextChunk(int _chunk) {
    return _chunk + readIntFromEEPROM(_chunk + P_SIZE) + P_OVERHEAD;
}

uint8_t _chunkData(int _chunk, int i) {
    return readByteFromEEPROM(_chunk + P_OVERHEAD + i);
}

uint8_t chunkInstance(int _chunk) {
    return readByteFromEEPROM(_chunk + P_INSTANCE);
}

uint8_t chunkHandle(int _chunk) {
    return readByteFromEEPROM(_chunk + P_HANDLE);
}

int _chunkUnused(int _chunk) {
    return readByteFromEEPROM(_chunk + P_INSTANCE) == UNUSED_CHUNK;
}

int _chunkSize(int _chunk) {
    return readIntFromEEPROM(_chunk + P_SIZE);
}

int chunkForInstanceAndHandleExists(int instance, int handle) {
    int _chunk = goToFirstChunk();
    while (!endOfChunks(_chunk)) {
        if (chunkHandle(_chunk) == handle && chunkInstance(_chunk) == instance)
            return 1;
        _chunk = nextChunk(_chunk);
    }
    return 0;
}

int chunkForHandleExists(int handle) {
    return chunkForInstanceAndHandleExists(getActiveInstance(), handle);
}

int chunkForHandle(int handle) {
    int _chunk = goToFirstChunk();
    while (!endOfChunks(_chunk)) {
        if (chunkHandle(_chunk) == handle && chunkInstance(_chunk) == getActiveInstance())
            return _chunk;
        _chunk = nextChunk(_chunk);
    }
    fatalError(31, handle);
    return 0;
}

// This breaks encapsulation, use with care
// Returns the address of the data, not the beginning of the chunk
int chunkAddressForInstanceAndHandle(int instance, int handle) {
    int _chunk = goToFirstChunk();
    while (!endOfChunks(_chunk)) {
        if (chunkHandle(_chunk) == handle && chunkInstance(_chunk) == instance)
            return _chunk + P_OVERHEAD;
        _chunk = nextChunk(_chunk);
    }
    fatalError(32, handle);
    return 0;
}

// Called when instance destroyed
void freeAllChunks() {
    int _chunk = goToFirstChunk();
    while (!endOfChunks(_chunk)) {
        if (chunkInstance(_chunk) == getActiveInstance())
            writeByteToEEPROM(_chunk + P_INSTANCE, UNUSED_CHUNK);
        _chunk = nextChunk(_chunk);
    }
}

// All chunks that have an instance value > activeInstance,
// are decreased by 1
void decreaseInstanceOffset() {
    int _chunk = goToFirstChunk();
    while (!endOfChunks(_chunk)) {
        if (chunkInstance(_chunk) > getActiveInstance() && !_chunkUnused(_chunk))
            writeByteToEEPROM(_chunk + P_INSTANCE, chunkInstance(_chunk) - 1);
        _chunk = nextChunk(_chunk);
    }
}

unsigned int availablePersistantHeap() {
    int r = 0;
    int _chunk = goToFirstChunk();
    while (!endOfChunks(_chunk)) {
        if (_chunkUnused(_chunk))
            r += _chunkSize(_chunk);
        _chunk = nextChunk(_chunk);
    }
    return r;
}

unsigned int chunkSize(int handle) {
    int p = chunkForHandle(handle);
    if (!p)
        return 0;
    return readIntFromEEPROM(p + P_SIZE);
}

// Returns 1 for success, 0 for no mem
unsigned int _allocChunk(int handle, int size) {
    // find an unused chunk bigger than size
    int _chunk = goToFirstChunk();
    int psize = 0;
    while (!endOfChunks(_chunk)) {
        psize = _chunkSize(_chunk);
        if (_chunkUnused(_chunk) && size + 2 * P_OVERHEAD <= psize)  // overhead + overhead for empty chunk
            break;
        _chunk = nextChunk(_chunk);
    }
    if (endOfChunks(_chunk)) {
        addStatusForInstance(STATUS_NO_MEMORY);
        fatalError(33, handle);
        return 0;
    }
    // we break the unused chunk into a used an unused one
    writeByteToEEPROM(_chunk, getActiveInstance());
    writeByteToEEPROM(_chunk + P_HANDLE, handle);
    writeIntToEEPROM(_chunk + P_SIZE, size);
    int u = _chunk + P_OVERHEAD + size;
    writeByteToEEPROM(u, UNUSED_CHUNK);
    writeByteToEEPROM(u + P_HANDLE, (unsigned int)0);
    writeIntToEEPROM(u + P_SIZE, (unsigned int)(psize - size - P_OVERHEAD));
    return handle;
}

// Merges contigual chunks without owners...
// Not very efficient from the point of view of the algorithm, but it is more
// simple AND the part that spends the time is anyway modifying the EEPROM, not
// reading it.
void _mergeChunks() {
    int atLeastOneMerged = 1;
    while (atLeastOneMerged) {
        atLeastOneMerged = 0;
        int lastChunk = 0;
        int _chunk = goToFirstChunk();
        while (!endOfChunks(_chunk)) {
            if (lastChunk && readByteFromEEPROM(lastChunk + P_INSTANCE) == UNUSED_CHUNK && _chunkUnused(_chunk)) {
                writeIntToEEPROM(lastChunk + P_SIZE,
                                 readIntFromEEPROM(lastChunk + P_SIZE) + _chunkSize(_chunk) + P_OVERHEAD);
                dumpChunksTxt();
                atLeastOneMerged = 1;
                break;
            }
            lastChunk = _chunk;
            _chunk = nextChunk(_chunk);
        }
    }
}

// The public alloc chunk, takes care of fragmenting
unsigned int allocChunk(int handle, int size) {
    int r = _allocChunk(handle, size);
    if (size)
        pset(handle, 0, 0);
    _mergeChunks();
    return r;
}

void deallocChunk(int handle) {
    int chunk = chunkForHandle(handle);
    writeByteToEEPROM(chunk + P_INSTANCE, UNUSED_CHUNK);
    writeByteToEEPROM(chunk + P_HANDLE, (unsigned int)0);
}

int resizeChunk(unsigned int handle, int newSize) {
    int chunk = chunkForHandle(handle);
    allocChunk(TMP_HANDLE, newSize);
    int chunk2 = chunkForHandle(TMP_HANDLE);
    for (int i = 0; i < newSize; i++)
        writeByteToEEPROM(chunk2 + P_OVERHEAD + i, readByteFromEEPROM(chunk + P_OVERHEAD + i));
    deallocChunk(handle);
    _mergeChunks();

    writeByteToEEPROM(chunk2 + P_HANDLE, handle);
    return 1;
}

int deleteChunkFragment(int handle, int address, int delta) {
    int chunk = chunkForHandle(handle);
    int size = chunkSize(handle);
    int newSize = size - delta;
    allocChunk(TMP_HANDLE, size - delta);
    int tmpChunk = chunkForHandle(TMP_HANDLE);
    for (int i = 0; i < newSize; i++) {
        if (i < address)
            writeByteToEEPROM(tmpChunk + P_OVERHEAD + i, readByteFromEEPROM(chunk + P_OVERHEAD + i));
        else
            writeByteToEEPROM(tmpChunk + P_OVERHEAD + i, readByteFromEEPROM(chunk + P_OVERHEAD + i + delta));
    }
    deallocChunk(handle);
    _mergeChunks();
    writeByteToEEPROM(tmpChunk + P_HANDLE, handle);
    return 1;
}

int insertChunkFragment(int handle, int address, int delta) {
    int p = chunkForHandle(handle);
    int size = chunkSize(handle);
    allocChunk(TMP_HANDLE, size + delta);
    // address=5, delta=3
    // 0123456
    // 01234---56
    int p2 = chunkForHandle(TMP_HANDLE);
    for (int i = 0; i < size; i++) {
        if (i < address)
            writeByteToEEPROM(p2 + P_OVERHEAD + i, readByteFromEEPROM(p + P_OVERHEAD + i));
        else
            writeByteToEEPROM(p2 + P_OVERHEAD + i + delta, readByteFromEEPROM(p + P_OVERHEAD + i));
    }
    deallocChunk(handle);
    _mergeChunks();

    writeByteToEEPROM(p2 + P_HANDLE, handle);
    return 1;
}

//
// HEAP ACCESSORS
//

unsigned char pget(int handle, int address) {
    int chunk = chunkForHandle(handle);
    if (address < 0 || address >= readIntFromEEPROM(chunk + P_SIZE))
        fatalError(34, address);
    return readByteFromEEPROM(chunk + P_OVERHEAD + address);
}

unsigned char pget(int address) {
    return pget(MAIN_CHUNK_HANDLE, address);
}

int pgetInt(int address) {
    return pget(MAIN_CHUNK_HANDLE, address) * 256 + pget(MAIN_CHUNK_HANDLE, address + 1);
}

uint32_t pgetLong(int address) {
    uint32_t       result;
    unsigned char* bytes = (unsigned char*)&result;
    bytes[0] = pget(address);
    bytes[1] = pget(address + 1);
    bytes[2] = pget(address + 2);
    bytes[3] = pget(address + 3);
    return result;
}

float pgetFloat(int handle, int address) {
    float          result = 0.0;
    unsigned char* bytes = (unsigned char*)&result;
    bytes[0] = pget(address);
    bytes[1] = pget(address + 1);
    bytes[2] = pget(address + 2);
    bytes[3] = pget(address + 3);
    return result;
}

float pgetFloat(int address) {
    return pgetFloat(MAIN_CHUNK_HANDLE, address);
}

void pset(int handle, int address, unsigned char value) {
    int chunkAddress = chunkForHandle(handle);
    if (address < 0 || address >= readIntFromEEPROM(chunkAddress + P_SIZE))
        fatalError(35, address);
    if (readByteFromEEPROM(chunkAddress + P_OVERHEAD + address) != value) {
        writeByteToEEPROM(chunkAddress + P_OVERHEAD + address, value);
    }
}

void pset(int address, unsigned char value) {
    pset(MAIN_CHUNK_HANDLE, address, value);
}

void psetInt(int address, unsigned int value) {
    pset(MAIN_CHUNK_HANDLE, address, value / 256);
    pset(MAIN_CHUNK_HANDLE, address + 1, value % 256);
}

void psetFloat(int handle, int address, float value) {
    unsigned char* bytes = (unsigned char*)&value;
    pset(handle, address, bytes[0]);
    pset(handle, address + 1, bytes[1]);
    pset(handle, address + 2, bytes[2]);
    pset(handle, address + 3, bytes[3]);
}

void psetFloat(int address, float value) {
    psetFloat(MAIN_CHUNK_HANDLE, address, value);
}
