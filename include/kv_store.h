#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include "file_manager.h"

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
    FileManager file_manager_;
    unordered_map<string, uint64_t> index_;
    void rebuildIndex();
};

