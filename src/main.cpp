#include "bplus_tree.h"
#include "buffer_pool.h"
#include "page_manager.h"
#include <iostream>
#include <cstdio>

using namespace std;

int main() {
    // delete old database file for clean start (testing only)
    //remove("test_bplus_tree.db");
    
    cout << "=== Testing B+ Tree with Page-Based Storage ===\n" << endl;
    
    // create PageManager and BufferPool
    PageManager pm("test_bplus_tree.db");
    BufferPool bp(&pm, 100);
    
    // create B+ Tree
    BPlusTree tree(&bp, &pm, 3);
    
    // test persistence: check if data from previous run exists
    cout << "1. Testing persistence (loading existing data)..." << endl;
    uint64_t existing1 = tree.search("key1");
    uint64_t existing2 = tree.search("key2");
    uint64_t existing3 = tree.search("key3");
    
    if (existing1 == 100 && existing2 == 200 && existing3 == 300) {
        cout << "   ✓ Persistence works! Data from previous run found:" << endl;
        cout << "     key1 -> " << existing1 << endl;
        cout << "     key2 -> " << existing2 << endl;
        cout << "     key3 -> " << existing3 << endl;
    } else {
        cout << "   (No existing data found - first run or data was cleared)" << endl;
        cout << "   Inserting new data..." << endl;
        
        // test 2: insert into empty tree
        bool result1 = tree.insert("key1", 100);
        bool result2 = tree.insert("key2", 200);
        bool result3 = tree.insert("key3", 300);
        
        if (result1 && result2 && result3) {
            cout << "   ✓ Inserts succeeded!" << endl;
        } else {
            cout << "   ✗ Inserts failed!" << endl;
            return 1;
        }
        
        // test 3: verify search works
        cout << "\n2. Testing search..." << endl;
        uint64_t val1 = tree.search("key1");
        uint64_t val2 = tree.search("key2");
        uint64_t val3 = tree.search("key3");
        
        cout << "   key1 -> " << val1 << endl;
        cout << "   key2 -> " << val2 << endl;
        cout << "   key3 -> " << val3 << endl;
        
        if (val1 == 100 && val2 == 200 && val3 == 300) {
            cout << "   ✓ Search works!" << endl;
        } else {
            cout << "   ⚠ Search returned: " << val1 << ", " << val2 << ", " << val3 << endl;
        }
    }
    
    // test 4: test serialization/deserialization directly
    cout << "\n3. Testing Node serialization/deserialization..." << endl;
    {
        // create a node
        Node node(true);
        node.page_id = 10;
        node.keys.push_back("test_key");
        node.values.push_back(12345);
        node.next_page_id = 5;
        
        // serialize to page
        Page page;
        node.serializeToPage(page);
        
        // deserialize back
        Node node2(true);
        node2.deserializeFromPage(page);
        
        // verify
        if (node2.keys.size() == 1 && 
            node2.keys[0] == "test_key" && 
            node2.values[0] == 12345 &&
            node2.next_page_id == 5) {
            cout << "   ✓ Serialization/deserialization works!" << endl;
        } else {
            cout << "   ✗ Serialization/deserialization failed!" << endl;
            return 1;
        }
    }
    
    cout << "\n=== B+ Tree Test Complete! ===" << endl;
    cout << "Basic functionality tested. Full tree operations may need more implementation." << endl;
    
    // flush all pages to disk before exiting
    bp.flushAll();
    
    return 0;
}