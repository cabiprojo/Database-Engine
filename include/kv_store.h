#pragma once

#include <string>
#include <unordered_map> // hash map for key-value pairs
#include <cstdint> // gives fixed width integers
#include "page_manager.h"
#include <utility> // for pair

using namespace std;

class KVStore {
public:

    // constructor
    KVStore(const string& filename); 

    // destructor
    ~KVStore();

    // put 
    bool put(const string& key, const string& value);

    // get
    string get(const string& key);

    // remove
    bool remove(const string& key);

private:
    PageManager page_manager_;
    unordered_map<string, pair<uint32_t, uint32_t>> index_; // key -> (page_id, offset)
    void rebuildIndex();
    uint32_t current_page_id_;
    uint32_t current_offset_;
};

