#include "kv_store.h"
#include "file_manager.h"
#include <iostream>

KVStore::KVStore(const string& filename) : file_manager_(filename) {
    // rebuild the index from the file
    rebuildIndex();
}

KVStore::~KVStore() {
    // FileManager destructor will automatically close the file
    // Index will be automatically destroyed
    
}

bool KVStore::put(const string& key, const string& value) {
    // format key value pair for storage
    uint32_t key_len = key.size();
    uint32_t value_len = value.size();

    string data;
    data.reserve(key_len + value_len + 8); // reserve is used to allocate memory for the string
    data.append(reinterpret_cast<const char*>(&key_len), sizeof(key_len)); // reinterpret_cast is used to convert the pointer to a char pointer
    data.append(key);
    data.append(reinterpret_cast<const char*>(&value_len), sizeof(value_len));
    data.append(value);

    // write to file using file manager
    uint64_t position = file_manager_.write(data);

    // update index with new position
    index_[key] = position;

    return true;
}

string KVStore::get(const string& key) {
    // look up key in index. check if key exists in index, if not, return empty string
    if (index_.find(key) == index_.end()) { 
        return "";
    }

    // get position from index
    uint64_t position = index_[key];

    // read key length (4 bytes)
    string bytes = file_manager_.read(position, 4);
    uint32_t key_len = *reinterpret_cast<const uint32_t*>(bytes.c_str());

    // skip key since we already read it
    position += 4 + key_len;

    // read value length (4 bytes)
    bytes = file_manager_.read(position, 4);
    uint32_t value_len = *reinterpret_cast<const uint32_t*>(bytes.c_str());

    // skip value length (4 bytes) and read the value
    position += 4;
    string value = file_manager_.read(position, value_len);

    // return the value
    return value;
}

bool KVStore::remove(const string& key) {
    // check if key exists in index
    if (index_.find(key) == index_.end()) return false;
    
    // remove key from index
    index_.erase(key);

    // return true
    return true;
}


void KVStore::rebuildIndex() {
    // start at beginning of file
    uint64_t position = 0;
    uint64_t file_size = file_manager_.size();

    while (true) {
        uint64_t start = position; // start of record
        
        // if can't read or end of file, break
        if (position + 4 > file_size) break;

        // read key length (4 bytes)
        string bytes = file_manager_.read(position, 4);
        uint32_t key_len = *reinterpret_cast<const uint32_t*>(bytes.c_str());
        position += 4;

        // check if can read key
        if (position + key_len > file_size) break;

        // read key
        string key = file_manager_.read(position, key_len);
        position += key_len;

        // check if can read value length
        if (position + 4 > file_size) break;

        // read value length
        bytes = file_manager_.read(position, 4);
        uint32_t value_len = *reinterpret_cast<const uint32_t*>(bytes.c_str());
        position += 4;

        // check if can read value
        if (position + value_len > file_size) break;

        // add to index
        index_[key] = start;

        // move to next record
        position += value_len;
    }
}
