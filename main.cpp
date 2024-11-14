#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#endif
#include <memory>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <numeric>
#include <iostream>
#include <sstream>

template <class T>
class RBTree
{

public:
    enum Color
    {
        R,
        B
    };

private:
    struct Node
    {
        Node(Color c,
             std::shared_ptr<const Node> const &lft,
             T val,
             std::shared_ptr<const Node> const &rgt)
            : _c(c), _lft(lft), _val(val), _rgt(rgt)
        {
        }
        Color _c;
        std::shared_ptr<const Node> _lft;
        T _val;
        std::shared_ptr<const Node> _rgt;
    };

public:
    explicit RBTree(std::shared_ptr<const Node> const &node) : _root(node) {}
    Color rootColor() const
    {
        return _root->_c;
    }
    RBTree() {}
    RBTree(Color c, RBTree const &lft, T val, RBTree const &rgt)
        : _root(std::make_shared<const Node>(c, lft._root, val, rgt._root))
    {
    }
    RBTree(std::initializer_list<T> init)
    {
        RBTree t;
        for (T v : init)
        {
            t = t.inserted(v);
        }
        _root = t._root;
    }
    template <class I>
    RBTree(I b, I e)
    {
        RBTree t;
        for_each(b, e, [&t](T const &v)
                 { t = t.inserted(v); });
        _root = t._root;
    }

    bool isEmpty() const { return !_root; }
    T root() const
    {
        return _root->_val;
    }

    RBTree left() const
    {
        return RBTree(_root->_lft);
    }
    RBTree right() const
    {
        return RBTree(_root->_rgt);
    }

    RBTree inserted(T x) const
    {
        RBTree t = ins(x);
        return RBTree(B, t.left(), t.root(), t.right());
    }

private:
    RBTree ins(T x) const
    {
        if (isEmpty())
            return RBTree(R, RBTree(), x, RBTree());
        T y = root();
        Color c = rootColor();
        if (rootColor() == B)
        {
            if (x < y)
                return balance(left().ins(x), y, right());
            else if (y < x)
                return balance(left(), y, right().ins(x));
            else
                return *this;
        }
        else
        {
            if (x < y)
                return RBTree(c, left().ins(x), y, right());
            else if (y < x)
                return RBTree(c, left(), y, right().ins(x));
            else
                return *this;
        }
    }

    static RBTree balance(RBTree const &lft, T x, RBTree const &rgt)
    {
        if (lft.doubledLeft())
            return RBTree(R, lft.left().paint(B), lft.root(), RBTree(B, lft.right(), x, rgt));
        else if (lft.doubledRight())
            return RBTree(R, RBTree(B, lft.left(), lft.root(), lft.right().left()), lft.right().root(), RBTree(B, lft.right().right(), x, rgt));
        else if (rgt.doubledLeft())
            return RBTree(R, RBTree(B, lft, x, rgt.left().left()), rgt.left().root(), RBTree(B, rgt.left().right(), rgt.root(), rgt.right()));
        else if (rgt.doubledRight())
            return RBTree(R, RBTree(B, lft, x, rgt.left()), rgt.root(), rgt.right().paint(B));
        else
            return RBTree(B, lft, x, rgt);
    }
    bool doubledLeft() const
    {
        return !isEmpty() && rootColor() == R && !left().isEmpty() && left().rootColor() == R;
    }
    bool doubledRight() const
    {
        return !isEmpty() && rootColor() == R && !right().isEmpty() && right().rootColor() == R;
    }
    RBTree paint(Color c) const
    {
        return RBTree(c, left(), root(), right());
    }

private:
    std::shared_ptr<const Node> _root;
};

template <class T>
std::vector<T> traverseInOrder(RBTree<T> const &t)
{
    if (t.isEmpty())
    {
        return {};
    }

    // Recursively get values from left subtree
    std::vector<T> leftValues = traverseInOrder(t.left());

    // Get current root value
    std::vector<T> result = std::move(leftValues);
    result.push_back(t.root());

    // Get values from right subtree and combine
    std::vector<T> rightValues = traverseInOrder(t.right());
    result.insert(result.end(), rightValues.begin(), rightValues.end());

    return result;
}

template <class T>
void print(RBTree<T> const &t, std::string outputPath)
{
    std::ofstream outFile;
    outFile.open(outputPath);

    for (T element : traverseInOrder(t))
    {
        outFile << element << "\n";
    }
    outFile.close();
}

std::string readTextFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> tokenize(const std::string &text)
{
    // Use accumulate to build a vector of words by accumulating each character
    return std::accumulate(text.begin(), text.end(), std::vector<std::string>{},
                           [](std::vector<std::string> words, char ch) mutable
                           {
                               static std::string word; // word buffer that persists across function calls

                               if (std::isalnum(ch))
                               {
                                   word += std::tolower(ch);
                               }
                               else if (!word.empty())
                               {
                                   words.push_back(word); // push the word into the vector
                                   word.clear();          // clear word for next potential word
                               }

                               return words;
                           });
}

template <class T>
RBTree<T> insertVector(std::vector<T> vec)
{
    RBTree<T> t;
    for (const auto &word : vec)
    {
        t = t.inserted(word);
    }
    return t;
}

#ifndef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    print(
        insertVector(
                tokenize(
                    readTextFile(argv[1]))),
            "output.txt");
}
#endif
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
void checkColorProperties(const RBTree<int> &node)
{
    if (node.isEmpty())
        return;

    // Left and right child must be black if node is red
    if (node.rootColor() == RBTree<int>::R)
    {
        if (!node.left().isEmpty())
            CHECK(node.left().rootColor() == RBTree<int>::Color::B);
        if (!node.right().isEmpty())
            CHECK(node.right().rootColor() == RBTree<int>::Color::B);
    }

    // Recursively check left and right subtrees
    checkColorProperties(node.left());
    checkColorProperties(node.right());
}

TEST_CASE("RBTree color property test")
{
    // Creating a Red-Black Tree with some values
    RBTree<int> t;
    t = t.inserted(10);
    t = t.inserted(20);
    t = t.inserted(30);
    t = t.inserted(75);
    t = t.inserted(55);
    t = t.inserted(15);
    t = t.inserted(23);
    t = t.inserted(15);
    t = t.inserted(45);
    t = t.inserted(65);

    // Checking if the root color is black
    REQUIRE(!t.isEmpty());
    CHECK(t.rootColor() == RBTree<int>::Color::B);

    // Use the helper function to check color properties for the entire tree
    checkColorProperties(t);
}
TEST_CASE("RBTree in order traversal")
{
    // Creating a Red-Black Tree with some values
    RBTree<int> t;
    t = t.inserted(10);
    t = t.inserted(20);
    t = t.inserted(30);
    t = t.inserted(75);
    t = t.inserted(55);
    t = t.inserted(15);
    t = t.inserted(23);
    t = t.inserted(45);
    t = t.inserted(65);

    std::vector<int> expected = {10, 15, 20, 23, 30, 45, 55, 65, 75};
    std::vector<int> notExpected = {10, 15, 20, 23, 36, 45, 55, 65, 75};
    REQUIRE(!t.isEmpty());
    CHECK_EQ(expected, traverseInOrder(t));
    CHECK(notExpected != traverseInOrder(t));
}
TEST_CASE("Tokenize Test")
{
    std::string input = "Hello World! What's up?";
    std::vector<std::string> expected = {"hello", "world", "what", "s", "up"};
    std::vector<std::string> notExpected = {"hello", "world", "!", "what", "s", "s", "up", "?"};
    CHECK_EQ(tokenize(input), expected);
    CHECK(tokenize(input) != notExpected);
}
TEST_CASE("No duplicates"){
    
    RBTree<int> t;
    t = t.inserted(10);
    t = t.inserted(10);

    std::vector<int> expected = {10};
    
    CHECK_EQ(expected, traverseInOrder(t));
}
#endif