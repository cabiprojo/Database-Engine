#include "file_manager.h"
#include "kv_store.h"
#include <iostream>

using namespace std;

int main() {
    KVStore db("test.db");
    
    // ONLY get data, don't put it
    cout << "Name: " << db.get("name") << endl;
    cout << "Age: " << db.get("age") << endl;
    
    return 0;
}