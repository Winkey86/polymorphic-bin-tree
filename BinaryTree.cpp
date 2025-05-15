#include "BinaryTree.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>
int BinaryTree::height(Node *nd) const
{
    if (!nd)
        return 0;
    return 1 + std::max(height(nd->left), height(nd->right));
}

BinaryTree::BinaryTree(Type *t) : type(t), root(nullptr), count(0) {}
BinaryTree::~BinaryTree() { clear(); }

void BinaryTree::clear()
{
    destroyRec(root);
    root = nullptr;
    count = 0;
}
void BinaryTree::destroyRec(Node *nd)
{
    if (!nd)
        return;
    destroyRec(nd->left);
    destroyRec(nd->right);
    type->destroy(nd->data);
    delete nd;
}

bool BinaryTree::insertRaw(void *d)
{
    void *v = type->clone(d);
    bool ok = insertRec(root, v);
    if (!ok)
        type->destroy(v);
    return ok;
}
bool BinaryTree::insertRec(Node *&nd, void *d)
{
    if (!nd)
    {
        nd = new Node(d);
        count++;
        return true;
    }
    int cmp = type->compare(d, nd->data);
    if (cmp == 0)
        return false;
    return cmp < 0 ? insertRec(nd->left, d) : insertRec(nd->right, d);
}

bool BinaryTree::searchRaw(void *key) const
{
    Node *cur = root;
    while (cur)
    {
        int cmp = type->compare(key, cur->data);
        if (cmp == 0)
            return true;
        cur = (cmp < 0 ? cur->left : cur->right);
    }
    return false;
}

bool BinaryTree::removeRaw(void *key)
{
    bool rem = false;
    root = removeRec(root, key, rem);
    if (rem)
        --count;
    return rem;
}
BinaryTree::Node *BinaryTree::removeRec(Node *nd, void *key, bool &rem)
{
    if (!nd)
        return nullptr;
    int cmp = type->compare(key, nd->data);
    if (cmp < 0)
        nd->left = removeRec(nd->left, key, rem);
    else if (cmp > 0)
        nd->right = removeRec(nd->right, key, rem);
    else
    {
        rem = true;
        if (!nd->left && !nd->right)
        {
            type->destroy(nd->data);
            delete nd;
            return nullptr;
        }
        if (!nd->left)
        {
            Node *tmp = nd->right;
            type->destroy(nd->data);
            delete nd;
            return tmp;
        }
        if (!nd->right)
        {
            Node *tmp = nd->left;
            type->destroy(nd->data);
            delete nd;
            return tmp;
        }
        Node *m = findMin(nd->right);
        void *copy = type->clone(m->data);
        type->destroy(nd->data);
        nd->data = copy;
        nd->right = removeRec(nd->right, m->data, rem);
    }
    return nd;
}
BinaryTree::Node *BinaryTree::findMin(Node *nd) const
{
    while (nd && nd->left)
        nd = nd->left;
    return nd;
}

std::string BinaryTree::toStringInorder() const
{
    std::ostringstream os;
    inorderRec(root, os);
    std::string s = os.str();
    if (!s.empty())
        s.pop_back();
    return s;
}
void BinaryTree::inorderRec(Node *nd, std::ostringstream &os) const
{
    if (!nd)
        return;
    inorderRec(nd->left, os);
    type->print(nd->data, os);
    os << ' ';
    inorderRec(nd->right, os);
}

std::string BinaryTree::toStringPreorder() const
{
    std::ostringstream os;
    std::function<void(Node *)> pre = [&](Node *n)
    {
        if (!n)
            return;
        type->print(n->data, os);
        os << ' ';
        pre(n->left);
        pre(n->right);
    };
    pre(root);
    std::string s = os.str();
    if (!s.empty())
        s.pop_back();
    return s;
}

std::string BinaryTree::toStringPostorder() const
{
    std::ostringstream os;
    std::function<void(Node *)> post = [&](Node *n)
    {
        if (!n)
            return;
        post(n->left);
        post(n->right);
        type->print(n->data, os);
        os << ' ';
    };
    post(root);
    std::string s = os.str();
    if (!s.empty())
        s.pop_back();
    return s;
}

std::string BinaryTree::toStringFormatted() const
{
    std::ostringstream os;
    std::function<void(Node *)> fmt = [&](Node *n)
    {
        if (!n)
            return;
        os << '{';
        type->print(n->data, os);
        os << '}';
        os << '(';
        fmt(n->left);
        os << ')';
        os << '[';
        fmt(n->right);
        os << ']';
    };
    fmt(root);
    return os.str();
}

bool BinaryTree::fromStringTraversal(const std::string &str, const std::string &order)
{
    clear();
    std::istringstream iss(str);
    std::string tok;
    while (iss >> tok)
    {
        void *e = type->createFromString(tok);
        insertRaw(e);
    }
    return true;
}

bool BinaryTree::fromFormattedString(const std::string &str)
{
    clear();
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '{')
        {
            size_t j = str.find('}', i);
            if (j == std::string::npos)
                break;
            std::string tok = str.substr(i + 1, j - (i + 1));
            void *e = type->createFromString(tok);
            insertRaw(e);
            i = j;
        }
    }
    return true;
}

std::vector<std::pair<void *, void *>> BinaryTree::toPairList() const
{
    std::vector<std::pair<void *, void *>> out;
    if (!root)
        return out;
    std::queue<std::pair<Node *, void *>> q;
    q.push({root, nullptr});
    while (!q.empty())
    {
        auto [n, parent] = q.front();
        q.pop();
        out.emplace_back(n->data, parent);
        if (n->left)
            q.push({n->left, n->data});
        if (n->right)
            q.push({n->right, n->data});
    }
    return out;
}
bool BinaryTree::fromPairList(const std::vector<std::pair<void *, void *>> &list)
{
    clear();
    if (list.empty())
        return true;
    void *rootVal = nullptr;
    for (auto &pr : list)
        if (pr.second == nullptr)
        {
            rootVal = pr.first;
            break;
        }
    if (!rootVal)
        return false;
    insertRaw(rootVal);
    for (auto &pr : list)
        if (pr.second)
            insertRaw(pr.first);
    return true;
}

void BinaryTree::balance()
{
    std::vector<void *> vals;
    std::function<void(Node *)> collect = [&](Node *nd)
    {
        if (!nd)
            return;
        collect(nd->left);
        vals.push_back(type->clone(nd->data));
        collect(nd->right);
    };
    collect(root);
    clear();
    std::function<void(int, int)> build = [&](int l, int r)
    {
        if (l > r)
            return;
        int m = (l + r) / 2;
        insertRaw(vals[m]);
        build(l, m - 1);
        build(m + 1, r);
    };
    build(0, (int)vals.size() - 1);
    for (void *v : vals)
        type->destroy(v);
}

BinaryTree *BinaryTree::subtree(void *key) const
{
    Node *cur = root;
    while (cur)
    {
        int c = type->compare(key, cur->data);
        if (c == 0)
            break;
        cur = (c < 0 ? cur->left : cur->right);
    }
    if (!cur)
        return nullptr;
    BinaryTree *out = new BinaryTree(type);
    std::function<void(Node *)> rec = [&](Node *n)
    {
        if (!n)
            return;
        out->insertRaw(n->data);
        rec(n->left);
        rec(n->right);
    };
    rec(cur);
    return out;
}

bool BinaryTree::containsSubtree(const BinaryTree &sub) const
{
    if (!sub.root)
        return true;
    std::queue<Node *> q;
    if (root)
        q.push(root);
    while (!q.empty())
    {
        Node *n = q.front();
        q.pop();
        if (type->compare(n->data, sub.root->data) == 0)
        {
            std::function<bool(Node *, Node *)> eq = [&](Node *a, Node *b)
            {
                if (!a && !b)
                    return true;
                if (!a || !b)
                    return false;
                if (type->compare(a->data, b->data) != 0)
                    return false;
                return eq(a->left, b->left) && eq(a->right, b->right);
            };
            if (eq(n, sub.root))
                return true;
        }
        if (n->left)
            q.push(n->left);
        if (n->right)
            q.push(n->right);
    }
    return false;
}

void *BinaryTree::searchByPathRaw(const std::string &path) const
{
    Node *cur = root;
    for (char c : path)
    {
        if (!cur)
            return nullptr;
        if (c == 'L' || c == 'l')
            cur = cur->left;
        else if (c == 'R' || c == 'r' || c == 'P' || c == 'p')
            cur = cur->right;
    }
    return cur ? cur->data : nullptr;
}
void BinaryTree::merge(const BinaryTree &other)
{
    std::queue<Node *> q;
    if (other.root)
        q.push(other.root);
    while (!q.empty())
    {
        Node *n = q.front();
        q.pop();
        insertRaw(n->data);
        if (n->left)
            q.push(n->left);
        if (n->right)
            q.push(n->right);
    }
}

void BinaryTree::printTree(std::ostream &os) const
{
    if (!root)
    {
        os << "(empty)\n";
        return;
    }
    os << "\n";

    std::vector<Node *> curr{root}, next;
    int height = this->height(root);
    int maxWidth = (1 << height) - 1;

    int level = 0;
    while (!curr.empty() && level < height)
    {
        int nodes = curr.size();
        int spacing = maxWidth / nodes;

        for (int i = 0; i < nodes; ++i)
        {
            os << std::string(spacing / 2, ' ');
            if (curr[i])
            {
                type->print(curr[i]->data, os);
                next.push_back(curr[i]->left);
                next.push_back(curr[i]->right);
            }
            else
            {
                os << " ";
                next.push_back(nullptr);
                next.push_back(nullptr);
            }
            os << std::string(spacing - spacing / 2, ' ');
        }
        os << "\n";

        bool anyChild = false;
        for (auto n : next)
            if (n)
            {
                anyChild = true;
                break;
            }
        if (!anyChild)
            break;

        int conSpacing = spacing;
        for (size_t i = 0; i + 1 < next.size(); i += 2)
        {
            os << std::string(conSpacing / 2 - 1, ' ');
            os << (next[i] ? '/' : ' ');
            os << std::string(2, ' ');
            os << (next[i + 1] ? '\\' : ' ');
            os << std::string(conSpacing - conSpacing / 2 - 1, ' ');
        }
        os << "\n";

        curr.swap(next);
        next.clear();
        ++level;
    }
    os << "\n";
}
