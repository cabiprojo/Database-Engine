#include "file_manager.h"
#include "kv_store.h"
#include "bplus_tree.h"
#include "page_manager.h"
#include <iostream>

using namespace std;

int main() {
     // Test PageManager
     PageManager pm("test_pages.db");
    
     // Allocate a page
     uint32_t page_id = pm.allocatePage();
     std::cout << "Allocated page ID: " << page_id << std::endl;
     
     // Write data to page
     Page page(true);  // zero-initialized
     page.writeUint32(0, 12345);
     page.writeString(100, "hello", 10);
     
     pm.writePage(page_id, page);
     std::cout << "Wrote page " << page_id << std::endl;
     
     // Read it back
     Page page2;
     pm.readPage(page_id, page2);
     
     // Verify
     uint32_t val = page2.readUint32(0);
     std::string str = page2.readString(100, 5);
     
     std::cout << "Read value: " << val << std::endl;
     std::cout << "Read string: " << str << std::endl;
     
     if (val == 12345 && str == "hello") {
         std::cout << "SUCCESS! Phase 0 works!" << std::endl;
     } else {
         std::cout << "FAILED!" << std::endl;
     }
     
     return 0;
}