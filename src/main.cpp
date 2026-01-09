#include "file_manager.h"
#include "kv_store.h"
#include "bplus_tree.h"
#include "page_manager.h"
#include <iostream>

using namespace std;

int main() {
    KVStore store("test_kv.db");
    
    // Test put
    cout << "Testing put..." << endl;
    store.put("key1", "value1");
    store.put("key2", "value2");
    store.put("key3", "hello world");
    cout << "Put 3 keys" << endl;
    
    // Test get
    cout << "\nTesting get..." << endl;
    string val1 = store.get("key1");
    string val2 = store.get("key2");
    string val3 = store.get("key3");
    
    cout << "key1 = " << val1 << endl;
    cout << "key2 = " << val2 << endl;
    cout << "key3 = " << val3 << endl;
    
    // Verify
    if (val1 == "value1" && val2 == "value2" && val3 == "hello world") {
        cout << "\nSUCCESS! KVStore with pages works!" << endl;
    } else {
        cout << "\nFAILED!" << endl;
    }
    
    // Test persistence (reopen and check)
    cout << "\nTesting persistence..." << endl;
    KVStore store2("test_kv.db");  // Reopen same file
    string val1_again = store2.get("key1");
    if (val1_again == "value1") {
        cout << "Persistence works! Got: " << val1_again << endl;
    } else {
        cout << "Persistence failed!" << endl;
    }
    
    return 0;
}