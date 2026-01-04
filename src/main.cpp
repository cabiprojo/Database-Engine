#include "file_manager.h"
#include "kv_store.h"
#include "bplus_tree.h"
#include <iostream>

using namespace std;

int main() {
    // Test B+ Tree with order 3
    BPlusTree tree(3);
    
    cout << "=== Testing B+ Tree Insertion and Search ===" << endl << endl;
    
    // Insert some keys
    cout << "Inserting keys..." << endl;
    tree.insert("apple", 100);
    tree.insert("banana", 200);
    tree.insert("cherry", 300);
    tree.insert("date", 400);
    tree.insert("elderberry", 500);
    tree.insert("fig", 600);
    tree.insert("grape", 700);
    cout << "Done inserting!" << endl << endl;
    
    // Test searches
    cout << "Searching for keys..." << endl;
    uint64_t val;
    
    val = tree.search("apple");
    cout << "apple: " << (val == 100 ? "✓ Found (100)" : "✗ Wrong value") << endl;
    
    val = tree.search("banana");
    cout << "banana: " << (val == 200 ? "✓ Found (200)" : "✗ Wrong value") << endl;
    
    val = tree.search("cherry");
    cout << "cherry: " << (val == 300 ? "✓ Found (300)" : "✗ Wrong value") << endl;
    
    val = tree.search("date");
    cout << "date: " << (val == 400 ? "✓ Found (400)" : "✗ Wrong value") << endl;
    
    val = tree.search("elderberry");
    cout << "elderberry: " << (val == 500 ? "✓ Found (500)" : "✗ Wrong value") << endl;
    
    val = tree.search("fig");
    cout << "fig: " << (val == 600 ? "✓ Found (600)" : "✗ Wrong value") << endl;
    
    val = tree.search("grape");
    cout << "grape: " << (val == 700 ? "✓ Found (700)" : "✗ Wrong value") << endl;
    
    // Test non-existent key
    val = tree.search("zebra");
    cout << "zebra: " << (val == 0 ? "✓ Not found (correct)" : "✗ Should return 0") << endl;
    
    cout << endl << "=== Test Complete ===" << endl;
    
    return 0;
}