#include "Menu.h"
#include "BinaryTree.h"
#include "Types.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <memory>

void MenuTree::run()
{
    std::unordered_map<std::string, std::unique_ptr<BinaryTree>> trees;
    std::unordered_map<std::string, std::unique_ptr<Type>> types;
    std::string current;

    auto parseValue = [&](const std::string &s)
    {
        Type *t = types.at(current).get();
        return t->createFromString(s);
    };

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "CREATE")
        {
            std::string name, tp;
            iss >> name >> tp;
            Type *t = nullptr;
            if (tp == "INT")
                t = new IntType();
            else if (tp == "DOUBLE")
                t = new DoubleType();
            else if (tp == "COMPLEX")
                t = new ComplexType();
            else if (tp == "STRING")
                t = new StringType();
            else if (tp == "FUNCTION")
                t = new FunctionType();
            else if (tp == "PERSON")
                t = new PersonType();
            else
            {
                std::cout << "Unknown type\n";
                continue;
            }

            types[name].reset(t);
            trees[name].reset(new BinaryTree(t));
            current = name;
            std::cout << "Created " << name << "\n";
        }
        else if (cmd == "SELECT")
        {
            std::string name;
            iss >> name;
            if (!trees.count(name))
            {
                std::cout << "No such tree\n";
                continue;
            }
            current = name;
            std::cout << "Selected " << name << "\n";
        }
        else if (cmd == "INSERT")
        {
            std::string v;
            iss >> v;
            void *e = parseValue(v);
            bool ok = trees[current]->insertRaw(e);
            std::cout << (ok ? "Inserted " : "Exists ") << v << "\n";
        }
        else if (cmd == "SEARCH")
        {
            std::string v;
            iss >> v;
            void *e = parseValue(v);
            bool ok = trees[current]->searchRaw(e);
            std::cout << (ok ? "Found " : "Not found ") << v << "\n";
            types[current]->destroy(e);
        }
        else if (cmd == "REMOVE")
        {
            std::string v;
            iss >> v;
            void *e = parseValue(v);
            bool ok = trees[current]->removeRaw(e);
            std::cout << (ok ? "Removed " : "No such ") << v << "\n";
        }
        else if (cmd == "PRINT")
        {
            std::string ord;
            iss >> ord;
            if (ord == "IN")
                std::cout << trees[current]->toStringInorder() << "\n";
            else if (ord == "PRE")
                std::cout << trees[current]->toStringPreorder() << "\n";
            else if (ord == "POST")
                std::cout << trees[current]->toStringPostorder() << "\n";
            else if (ord == "FORM")
                std::cout << trees[current]->toStringFormatted() << "\n";
            else if (ord == "TREE")
                trees[current]->printTree(std::cout);
            else
                std::cout << "Unknown order\n";
        }
        else if (cmd == "PAIRS")
        {
            auto vec = trees[current]->toPairList();
            for (auto &pr : vec)
            {
                types[current]->print(pr.first, std::cout);
                std::cout << " - ";
                if (pr.second)
                    types[current]->print(pr.second, std::cout);
                else
                    std::cout << "NULL";
                std::cout << "\n";
            }
        }
        else if (cmd == "BALANCE")
        {
            trees[current]->balance();
            std::cout << "Balanced\n";
        }
        else if (cmd == "LOAD")
        {
            std::string sub;
            iss >> sub;
            if (sub == "STR")
            {
                std::string ord;
                iss >> ord;
                std::string rest;
                std::getline(iss, rest);
                trees[current]->fromStringTraversal(rest, ord);
                std::cout << "Loaded from str\n";
            }
            else if (sub == "FORM")
            {
                std::string rest;
                std::getline(iss, rest);
                trees[current]->fromFormattedString(rest);
                std::cout << "Loaded formatted\n";
            }
            else if (sub == "PAIRS")
            {
                int n;
                iss >> n;
                std::vector<std::pair<void *, void *>> pairs;
                for (int i = 0; i < n; ++i)
                {
                    std::string a, b;
                    std::cin >> a >> b;
                    void *va = types[current]->createFromString(a);
                    void *vb = (b == "NULL" ? nullptr : types[current]->createFromString(b));
                    pairs.emplace_back(va, vb);
                }
                trees[current]->fromPairList(pairs);
                std::cout << "Loaded pairs\n";
            }
        }
        else if (cmd == "MERGE")
        {
            std::string other;
            iss >> other;
            trees[current]->merge(*trees[other]);
            std::cout << "Merged " << other << "\n";
        }
        else if (cmd == "SUBTREE")
        {
            std::string v;
            iss >> v;
            void *e = parseValue(v);
            BinaryTree *sub = trees[current]->subtree(e);
            std::string name2 = current + "_sub";
            trees[name2].reset(sub);
            types[name2].reset(types[current]->cloneType());
            std::cout << "Subtree " << name2 << "\n";
        }
        else if (cmd == "CONTAINS")
        {
            std::string other;
            iss >> other;
            bool ok = trees[current]->containsSubtree(*trees[other]);
            std::cout << (ok ? "Yes" : "No") << "\n";
        }
        else if (cmd == "PATH")
        {
            std::string path;
            iss >> path;
            void *r = trees[current]->searchByPathRaw(path);
            if (!r)
                std::cout << "No node\n";
            else
            {
                types[current]->print(r, std::cout);
                std::cout << "\n";
            }
        }
        else
        {
            std::cout << "Unknown command\n";
        }
    }
}
