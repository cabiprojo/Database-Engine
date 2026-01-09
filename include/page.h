#pragma once

#include <string>
#include <cstdint> 
#include <cstring>  // for memset, memcpy

constexpr uint32_t PAGE_SIZE = 4096; // 4KB, constexpr for compile time constant

using namespace std;

class Page {
public:
    char data[PAGE_SIZE];

    Page(bool clear = false);
    ~Page();

    // low-level read/write data to page
    void writeBytes(uint32_t offset, const char* data, uint32_t size);
    void readBytes(uint32_t offset, char* data, uint32_t size) const;

    // type-safe helpers
    void writeUint32(uint32_t offset, uint32_t value);
    uint32_t readUint32(uint32_t offset) const;
    
    void writeUint64(uint32_t offset, uint64_t value);
    uint64_t readUint64(uint32_t offset) const;
    
    void writeString(uint32_t offset, const string& str, uint32_t max_size);
    string readString(uint32_t offset, uint32_t length) const;    

    // utility functions
    void clear();
    char* getData(uint32_t offset);
    const char* getData(uint32_t offset) const; 


private:

};