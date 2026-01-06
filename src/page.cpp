#include "page.h"

using namespace std;

Page::Page(bool clear) {
    if (clear) {
        memset(data, 0, PAGE_SIZE);
    }
}

Page::~Page() {}

void Page::writeBytes(uint32_t offset, const char* data, uint32_t size) {
    if (offset + size <= PAGE_SIZE) {
        memcpy(this->data + offset, data, size);
    }
}

void Page::readBytes(uint32_t offset, char* data, uint32_t size) const{
    if (offset + size <= PAGE_SIZE) {
        memcpy(data, this->data + offset, size);
    }
}

void Page::writeUint32(uint32_t offset, uint32_t value) {
    if (offset + sizeof(uint32_t) <= PAGE_SIZE) {
        memcpy(data + offset, &value, sizeof(uint32_t));
    }
}

uint32_t Page::readUint32(uint32_t offset) const {
    uint32_t value = 0;
    if (offset + sizeof(uint32_t) <= PAGE_SIZE) {
        memcpy(&value, data + offset, sizeof(uint32_t));
    }
    return value;
}

void Page::writeUint64(uint32_t offset, uint64_t value) {
    if (offset + sizeof(uint64_t) <= PAGE_SIZE) {
        memcpy(data + offset, &value, sizeof(uint64_t));
    }
}

uint64_t Page::readUint64(uint32_t offset) const {
    uint64_t value = 0;
    if (offset + sizeof(uint64_t) <= PAGE_SIZE) {
        memcpy(&value, data + offset, sizeof(uint64_t));
    }
    return value;
}

void Page::writeString(uint32_t offset, const string& str, uint32_t max_size) {
    uint32_t copy_size = (str.size() < max_size) ? str.size() : max_size;
    if (offset + copy_size <= PAGE_SIZE) {
        memcpy(data + offset, str.c_str(), copy_size);
    }
}

string Page::readString(uint32_t offset, uint32_t length) const {
    string result;
    if (offset + length <= PAGE_SIZE) {
        result.assign(data + offset, length);
    }
    return result;
}

void Page::clear() {
    memset(data, 0, PAGE_SIZE);
}

char* Page::getData(uint32_t offset) {
    return (offset < PAGE_SIZE) ? data + offset : nullptr;
}

const char* Page::getData(uint32_t offset) const {
    return (offset < PAGE_SIZE) ? data + offset : nullptr;
}
