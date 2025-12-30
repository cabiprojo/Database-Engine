/*
#pragma once

class BPlusTree {
public:
    // constructor, order is max number of keys in each node
    BPlusTree(int order = 3);

    // destructor
    ~BPlusTree();

    // insert a key-value pair into the tree
    bool insert(const string& key, const string& value);

    // search for key, return file position (0 if not found)
    string search(const string& key);


private:

}

struct Node {
    bool is_leaf;
    vector<string> keys;
    vector<uint64_t> values; // for leaf nodes, values are file positions
    vector<Node*> children; // internal nodes
    Node* next;

    Node(bool leaf) : is_leaf(leaf), next(nullptr) {}

    ~Node() {
        if (!is_leaf) {
            for (Node* child : children {
                delete child;
            })
        }
    }
}
*/