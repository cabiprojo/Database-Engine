#pragma once

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

struct Node;

class BPlusTree {
public:
    // constructor, order is max number of keys in each node
    BPlusTree(int order = 3);

    // destructor
    ~BPlusTree();

    // insert a key-value pair into the tree
    bool insert(const string& key, uint64_t value);

    // search for key, return file position (0 if not found)
    uint64_t search(const string& key);

private:
    Node* root_;
    int order_;

    // insert helpers
    Node* insertHelper(Node* node, const string& key, uint64_t value, string& promotedKey, Node*& newRightChild);

    // search helpers
    Node* findLeaf(const string& key);

    // utility
    void deleteTree(Node* node);
    void printNode(Node* node, int level);

};

struct Node {
    bool is_leaf;
    vector<string> keys;
    vector<uint64_t> values; // for leaf nodes, values are file positions
    vector<Node*> children; // internal nodes
    Node* next; // for leaf nodes, points to next leaf node in linked list

    Node(bool leaf) : is_leaf(leaf), next(nullptr) {}

    ~Node() {
        if (!is_leaf) {
            for (Node* child : children) {
                delete child;
            }
        }
    }
};
