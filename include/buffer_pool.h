#pragma once

#include "page.h"
#include "page_manager.h"
#include <unordered_map>
#include <list>
#include <cstdint>

using namespace std;

class BufferPool {
public:
    BufferPool(PageManager* page_manager, size_t max_pages = 100);
    ~BufferPool();

    // get a page
    void getPage(uint32_t page_id, Page& page);

    // save/modify a page
    void savePage(uint32_t page_id, const Page& page);

    // pin/unpin pages
    void pinPage(uint32_t page_id);
    void unpinPage(uint32_t page_id);

    void flushAll();

private:
    PageManager* page_manager_;
    unordered_map<uint32_t, Page> cache_; // page_id->page (actual cache)
    unordered_map<uint32_t, bool> pinned_;
    unordered_map<uint32_t, bool> dirty_;
    list<uint32_t> lru_list_;
    unordered_map<uint32_t, list<uint32_t>::iterator> lru_map_; // page_id->iterator in LRU list, for fast access
    size_t max_pages_;

    // evict a page when cache is full
    void evictPage(uint32_t page_id);

    // helper methods
    void moveToFront(uint32_t page_id);
    void flushPage(uint32_t page_id);

};