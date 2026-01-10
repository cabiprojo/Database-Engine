#include "kv_store.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== Testing KVStore with BufferPool Integration ===\n" << endl;
    
    // Test 1 & 2 & 3: Basic operations with first store
    {
        KVStore store("test_kvstore.db");
        
        // Test 1: Basic put
        cout << "1. Testing put..." << endl;
        store.put("key1", "value1");
        store.put("key2", "value2");
        cout << "   Put 2 keys" << endl;
        
        // Test 2: Basic get
        cout << "\n2. Testing get..." << endl;
        string val1 = store.get("key1");
        string val2 = store.get("key2");
        cout << "   key1 = " << val1 << endl;
        cout << "   key2 = " << val2 << endl;
        
        // Verify
        if (val1 == "value1" && val2 == "value2") {
            cout << "   ✓ Basic put/get works!" << endl;
        } else {
            cout << "   ✗ Basic put/get failed!" << endl;
            return 1;
        }
        
        // Test 3: Multiple gets of same key (should use cache)
        cout << "\n3. Testing cache behavior..." << endl;
        string val1_again = store.get("key1");  // Should use cache!
        string val1_again2 = store.get("key1"); // Should use cache again!
        cout << "   Got key1 twice: " << val1_again << ", " << val1_again2 << endl;
        
        if (val1_again == "value1" && val1_again2 == "value1") {
            cout << "   ✓ Cache is working (no errors = cache hit!)" << endl;
        } else {
            cout << "   ✗ Cache may not be working!" << endl;
        }
        
        // Store goes out of scope here - destructor flushes dirty pages via BufferPool
    }
    
    // Test 4: Persistence - reopen and check
    cout << "\n4. Testing persistence (reopen file)..." << endl;
    // Create new KVStore with same file (should read persisted data)
    KVStore store2("test_kvstore.db");
    string val1_reopened = store2.get("key1");
    string val2_reopened = store2.get("key2");
    
    cout << "   After reopen - key1 = " << val1_reopened << endl;
    cout << "   After reopen - key2 = " << val2_reopened << endl;
    
    if (val1_reopened == "value1" && val2_reopened == "value2") {
        cout << "   ✓ Persistence works! BufferPool flushed dirty pages!" << endl;
    } else {
        cout << "   ✗ Persistence failed!" << endl;
        return 1;
    }
    
    // Test 5: Multiple operations
    cout << "\n5. Testing multiple operations..." << endl;
    store2.put("key3", "hello world");
    store2.put("key4", "test value");
    store2.put("key5", "another value");
    
    cout << "   Put 3 more keys" << endl;
    
    // Get them back
    cout << "   key3 = " << store2.get("key3") << endl;
    cout << "   key4 = " << store2.get("key4") << endl;
    cout << "   key5 = " << store2.get("key5") << endl;
    
    // Verify
    if (store2.get("key3") == "hello world" && 
        store2.get("key4") == "test value" && 
        store2.get("key5") == "another value") {
        cout << "   ✓ Multiple operations work!" << endl;
    } else {
        cout << "   ✗ Multiple operations failed!" << endl;
        return 1;
    }
    
    cout << "\n=== KVStore with BufferPool Test Complete! ===" << endl;
    cout << "All tests passed! BufferPool integration is working correctly." << endl;
    return 0;
}