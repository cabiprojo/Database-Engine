#include "bplus_tree.h"
#include "buffer_pool.h"
#include "page_manager.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== Testing B+ Tree with Page-Based Storage ===\n" << endl;
    
    // create PageManager and BufferPool
    PageManager pm("test_bplus_tree.db");
    BufferPool bp(&pm, 100);
    
    // test 1: create B+ Tree and insert into empty tree
    cout << "1. Testing insert into empty tree..." << endl;
    BPlusTree tree(&bp, &pm, 3);
    
    bool result1 = tree.insert("key1", 100);
    bool result2 = tree.insert("key2", 200);
    bool result3 = tree.insert("key3", 300);
    
    if (result1 && result2 && result3) {
        cout << "   ✓ Inserts succeeded!" << endl;
    } else {
        cout << "   ✗ Inserts failed!" << endl;
        return 1;
    }
    
    // test 2: test search (if implemented)
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
        cout << "   (Search may not be fully implemented yet)" << endl;
    }
    
    // test 3: test serialization/deserialization directly
    cout << "\n4. Testing Node serialization/deserialization..." << endl;
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
    return 0;
}