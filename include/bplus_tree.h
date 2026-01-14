#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "page.h"
#include "buffer_pool.h"
#include "page_manager.h"

using namespace std;

struct Node;

class BPlusTree {
public:
    // constructor, order is max number of keys in each node
    BPlusTree(BufferPool* buffer_pool, PageManager* page_manager, int order = 3);

    // insert a key-value pair into the tree
    bool insert(const string& key, uint64_t value);

    // search for key, return file position (0 if not found)
    uint64_t search(const string& key);

private:
    BufferPool* buffer_pool_;
    PageManager* page_manager_;  // for allocating pages
    uint32_t root_page_id_;  // page ID instead of pointer, 0 means empty tree
    int order_;

    // insert helper, returns page id of the new right child if split, otherwise 0
    uint32_t insertHelper(uint32_t page_id, const string& key, uint64_t value, string& promotedKey, uint32_t& newRightChildPage);
    // search helpers
    uint32_t findLeaf(const string& key);
    
    // page-based node operations
    void loadNode(uint32_t page_id, Node& node);
    void saveNode(uint32_t page_id, const Node& node);
    uint32_t allocateNode(bool is_leaf);
    
    // persistence for root_page_id_
    void loadRootPageId();
    void saveRootPageId();

};

struct Node {
    bool is_leaf;
    vector<string> keys;
    vector<uint64_t> values; // for leaf nodes, values are file positions
    vector<uint32_t> children_page_ids; // internal nodes
    uint32_t next_page_id; // for leaf nodes, points to next leaf node in linked list
    uint32_t page_id;

    Node(bool leaf = false) : is_leaf(leaf), next_page_id(0), page_id(0) {}

    void serializeToPage(Page& page) const; // write node data to page
    void deserializeFromPage(const Page& page); // read node data from page

};
