#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <queue>
#include <iostream>
#include "Types.h"

class BinaryTree
{
public:
    struct Node
    {
        void *data;
        Node *left;
        Node *right;
        Node(void *d) : data(d), left(nullptr), right(nullptr) {}
    };

    explicit BinaryTree(Type *t);
    ~BinaryTree();

    void clear();
    bool insertRaw(void *d);
    bool searchRaw(void *key) const;
    bool removeRaw(void *key);

    void balance();

    BinaryTree *subtree(void *key) const;
    bool containsSubtree(const BinaryTree &sub) const;

    std::string toStringInorder() const;
    std::string toStringPreorder() const;
    std::string toStringPostorder() const;
    std::string toStringFormatted() const;

    bool fromStringTraversal(const std::string &str, const std::string &order);
    bool fromFormattedString(const std::string &str);

    std::vector<std::pair<void *, void *>> toPairList() const;
    bool fromPairList(const std::vector<std::pair<void *, void *>> &list);

    void *searchByPathRaw(const std::string &path) const;
    void merge(const BinaryTree &other);
    void printTree(std::ostream &os = std::cout) const;

private:
    Type *type;
    Node *root;
    size_t count;

    bool insertRec(Node *&nd, void *d);
    Node *removeRec(Node *nd, void *key, bool &rem);
    Node *findMin(Node *nd) const;
    void inorderRec(Node *nd, std::ostringstream &os) const;
    void destroyRec(Node *nd);
    int height(Node *nd) const;
};
