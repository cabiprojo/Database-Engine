#pragma once

#include "file_manager.h"
#include "page.h"
#include <string>
#include <cstdint>
using namespace std;

class PageManager {
public:
    PageManager(const string& filename);
    ~PageManager();
    
    // allocate a new page (returns page ID)
    uint32_t allocatePage();
    
    // read a page from disk
    void readPage(uint32_t page_id, Page& page);
    
    // write a page to disk
    void writePage(uint32_t page_id, const Page& page);
    
    // free a page (add to free list - simple version for now)
    void freePage(uint32_t page_id);
    
private:
    FileManager file_manager_;
    uint32_t next_page_id_;  // track next page to allocate
};