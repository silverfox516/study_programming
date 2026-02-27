#include <iostream>
#include <memory>
#include <vector>

struct Node {
    int value;
    std::vector<std::shared_ptr<Node>> children;
    std::weak_ptr<Node> parent; // Weak pointer to avoid circular reference

    Node(int v) : value(v) { std::cout << "Node " << value << " constructed" << std::endl; }
    ~Node() { std::cout << "Node " << value << " destructed" << std::endl; }
};

int main() {
    auto root = std::make_shared<Node>(1);
    auto child = std::make_shared<Node>(2);

    root->children.push_back(child);
    child->parent = root;

    std::cout << "Root use count: " << root.use_count() << std::endl;
    std::cout << "Child use count: " << child.use_count() << std::endl;

    if (auto p = child->parent.lock()) {
        std::cout << "Child's parent is alive. Value: " << p->value << std::endl;
    } else {
        std::cout << "Child's parent is gone." << std::endl;
    }

    return 0;
} // root and child go out of scope, both are destructed properly
