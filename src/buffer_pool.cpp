#include "buffer_pool.h"

BufferPool::BufferPool(PageManager* page_manager, size_t max_pages) : page_manager_(page_manager), max_pages_(max_pages) {

}

BufferPool::~BufferPool() {
    flushAll();
}

void BufferPool::getPage(uint32_t page_id, Page& page) {
    // check if page is in cache (cache hit)
    // -copy from cache to page
    // -update LRU and return
    if (cache_.find(page_id) != cache_.end()) {
        page = cache_[page_id];
        moveToFront(page_id);
        return;
    }

    // if not (cache miss) and cache is full, evict a page to make space
    if (cache_.size() >= max_pages_) {
        if (!lru_list_.empty()) {
            evictPage(lru_list_.back());
        }
    }

    // load page from disk
    page_manager_->readPage(page_id, page);

    // add to cache
    cache_[page_id] = page;
    dirty_[page_id] = false;  // just loaded, not modified yet
    
    // add to LRU tracking
    moveToFront(page_id);
    return;
}

void BufferPool::savePage(uint32_t page_id, const Page& page) {
    // update cache
    cache_[page_id] = page;

    // mark page as dirty
    dirty_[page_id] = true;

    // update LRU
    moveToFront(page_id);
}

void BufferPool::pinPage(uint32_t page_id) {
    pinned_[page_id] = true;
}

void BufferPool::unpinPage(uint32_t page_id) {
    pinned_[page_id] = false;
}

void BufferPool::flushAll() {
    // iterate through all dirty pages and flush them
    for (auto& pair : dirty_) {
        if (pair.second) {
            flushPage(pair.first);
        }
    }

    // clear dirty pages
    dirty_.clear();
}

void BufferPool::moveToFront(uint32_t page_id) {
    if (lru_map_.find(page_id) != lru_map_.end()) {
        // already tracked - move to front
        auto iterator = lru_map_[page_id];
        lru_list_.splice(lru_list_.begin(), lru_list_, iterator);
        lru_map_[page_id] = lru_list_.begin();
    } else {
        // not tracked yet - add to front
        lru_list_.push_front(page_id);
        lru_map_[page_id] = lru_list_.begin();
    }
}

void BufferPool::flushPage(uint32_t page_id) {
    // check if page exists in cache
    if (cache_.find(page_id) == cache_.end()) return;

    // check if page is dirty (and exists in dirty map)
    if (dirty_.find(page_id) == dirty_.end() || !dirty_[page_id]) return;

    // write page to disk
    page_manager_->writePage(page_id, cache_[page_id]);

    // mark page as clean
    dirty_[page_id] = false;
}

void BufferPool::evictPage(uint32_t page_id) {
    // check if page is pinned because can't evict in use pages
    if (pinned_.find(page_id) != pinned_.end() && pinned_[page_id]) return;

    // flush if dirty
    if (dirty_.find(page_id) != dirty_.end() && dirty_[page_id]) {
        flushPage(page_id);
    }

    // remove from cache
    cache_.erase(page_id);

    // remove from LRU tracking if it exists
    if (lru_map_.find(page_id) != lru_map_.end()) {
        lru_list_.erase(lru_map_[page_id]);
        lru_map_.erase(page_id);
    }

    // clean up tracking maps
    pinned_.erase(page_id);
    dirty_.erase(page_id);
}