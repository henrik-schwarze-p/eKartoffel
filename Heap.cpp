#include "Instance.h"
#include "FatalError.h"
#include "Target.h"
#include "Heap.h"
#include "Namespaces.h"

// RAM HEAP, EACH INSTANCE CAN ASKS FOR HANDLES, AS MANY AS THEY WANT, AS LONG
// AS THERE IS MEMORY. SIZES UP TO 255 bytes.
// * Handles range: 0->255
// * Instance 0..<250
// * Instance 250 = Kernel
// * Instance 255 = No instance
// We don't allow fragmentation by moving bytes...
//
// Data stucture
//   00: 00   instance
//   01: 01   handle
//   02: 02   length
//   03: b0   data0
//   04: b1   data1
//   05: 03   instance     <---- "Head"
//   06: 01   handle
//   07: 01   length
//   08: b2   data0
//   09: FF   end of heap
//
// All functions are owned by the active instance of the kernel.

// The data

unsigned char heapData[HEAP_SIZE];

// EASY NAVIGATION

int _goToFirstHeap() {
    return 0;
}

int _atEndOfHeap(int _head) {
    return heapData[_head] == UNUSED_HEAP;
}

int _heapHandle(int _head) {
    return heapData[_head + H_HANDLE];
}

int _heapInstance(int _head) {
    return heapData[_head + H_INSTANCE];
}

int _nextHeap(int _head) {
    return _head + heapData[_head + H_LEN] + H_OVERHEAD;
}

// INIT

void initHeap() {
    setHeapData(0, UNUSED_HEAP);
}

// WRITING
//
// Always use this fn to write, as it calls writeHeap, that can be defined in a target to do something
// else (for example, to interface with emcc)
void setHeapData(int address, unsigned char value) {
    heapData[address] = value;
    writeToHeap(address, value);
}

// AVAILABLE HEAP

unsigned int availableHeap() {
    int _head = _goToFirstHeap();
    while (!_atEndOfHeap(_head))
        _head = _nextHeap(_head);
    return HEAP_SIZE - _head;
}

unsigned int heapEnd() {
    int _head = _goToFirstHeap();
    while (!_atEndOfHeap(_head))
        _head = _nextHeap(_head);
    return _head;
}

// ALLOCATING
//
// Returns 1 if successful, 0 if not.
// If an allocation fails, the instance is marked as having no_memory and will be deallocated
// (chunks and heaps), and will be forced to be terminated next time it is "opened".
int allocHeap(int handle, int size) {
    if (size < 0)
        return 0;
    int _head = _goToFirstHeap();
    while (_head + H_OVERHEAD + size < HEAP_SIZE && !_atEndOfHeap(_head))
        _head = _nextHeap(_head);
    if (_head + H_OVERHEAD + size >= HEAP_SIZE) {
        addStatusForInstance(STATUS_NO_MEMORY);
        fatalError(27, handle);
    }
    setHeapData(_head + H_INSTANCE, getActiveInstance());
    setHeapData(_head + H_HANDLE, handle);
    setHeapData(_head + H_LEN, size);
    setHeapData(_head + H_OVERHEAD + size, UNUSED_HEAP);
    if (size)
        setHeapData(_head + H_DATA, 0);
    return handle;
}

int heapHead(int heapHandle) {
    int _head = _goToFirstHeap();
    while ((_heapHandle(_head) != heapHandle || _heapInstance(_head) != getActiveInstance()) && !_atEndOfHeap(_head))
        _head = _nextHeap(_head);
    if (_atEndOfHeap(_head))
        fatalError(28, 0);
    return _head;
}

int heapForHandleExists(int heapHandle) {
    int _head = _goToFirstHeap();
    while ((_heapHandle(_head) != heapHandle || _heapInstance(_head) != getActiveInstance()) && !_atEndOfHeap(_head)) {
        _head = _nextHeap(_head);
    }
    return !_atEndOfHeap(_head);
}

int heapSize(int heapHandle) {
    return heapData[heapHead(heapHandle) + H_LEN];
}

void freeHeap(int heapHandle) {
    int head = heapHead(heapHandle);
    int size = heapData[head + H_LEN];
    for (int i = head; i < HEAP_SIZE - size - H_OVERHEAD; i++)
        setHeapData(i, heapData[i + size + H_OVERHEAD]);
}

// for current instance
void freeAllHeaps() {
    int fred = 1;
    while (fred) {
        fred = 0;
        int _head = _goToFirstHeap();
        while (!_atEndOfHeap(_head)) {
            if (_heapInstance(_head) == getActiveInstance()) {
                freeHeap(_heapHandle(_head));
                fred = 1;
                break;
            }
            _head = _nextHeap(_head);
        }
    }
    int _head = _goToFirstHeap();
    while (!_atEndOfHeap(_head)) {
        if (_heapInstance(_head) > getActiveInstance())
            setHeapData(_head + H_INSTANCE, heapData[_head + H_INSTANCE] - 1);
        _head = _nextHeap(_head);
    }
}

unsigned char mget(int heapHandle, int address) {
    int head = heapHead(heapHandle);
    if (address < 0 || address >= heapData[head + H_LEN]) {
        fatalError(29, address);
    }
    return heapData[head + H_OVERHEAD + address];
}

unsigned char mget(int address) {
    return mget(MAIN_HEAP_HANDLE, address);
}

float mgetFloat(int heapHandle, int address) {
    float          result = 0.0;
    unsigned char* bytes = (unsigned char*)&result;
    bytes[0] = mget(heapHandle, address);
    bytes[1] = mget(heapHandle, address + 1);
    bytes[2] = mget(heapHandle, address + 2);
    bytes[3] = mget(heapHandle, address + 3);
    return result;
}

void mset(int heapHandle, int address, unsigned char value) {
    int head = heapHead(heapHandle);
    if (address < 0 || address >= heapData[head + H_LEN])
        fatalError(30, address);
    setHeapData(head + H_OVERHEAD + address, value);
}

void mset(int address, unsigned char value) {
    mset(MAIN_HEAP_HANDLE, address, value);
}

void msetFloat(int heapHandle, int address, float value) {
    unsigned char* bytes = (unsigned char*)&value;
    mset(heapHandle, address, bytes[0]);
    mset(heapHandle, address + 1, bytes[1]);
    mset(heapHandle, address + 2, bytes[2]);
    mset(heapHandle, address + 3, bytes[3]);
}

void msetFloat(int address, float value) {
    msetFloat(MAIN_HEAP_HANDLE, address, value);
}

void deleteHeapFragment(int handle, int address, int delta) {
    int head = heapHead(handle);
    int begin = head + H_OVERHEAD + address;
    int end = heapEnd();
    for (int i = begin; i < end - delta + 1; i++)
        heapData[i] = heapData[i + delta];
    heapData[head + H_LEN] -= delta;
}
