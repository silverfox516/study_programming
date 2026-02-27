#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <array>
#include <list>
#include <algorithm>
#include <iterator>
#include <functional>
#include <numeric>

// Traditional Iterator Pattern
template<typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual bool hasNext() const = 0;
    virtual T next() = 0;
    virtual T current() const = 0;
    virtual void reset() = 0;
};

template<typename T>
class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual std::unique_ptr<Iterator<T>> createIterator() = 0;
    virtual size_t size() const = 0;
};

// Concrete Iterator for Array-based collection
template<typename T>
class ArrayIterator : public Iterator<T> {
private:
    const std::vector<T>& collection;
    size_t position;

public:
    explicit ArrayIterator(const std::vector<T>& collection)
        : collection(collection), position(0) {}

    bool hasNext() const override {
        return position < collection.size();
    }

    T next() override {
        if (!hasNext()) {
            throw std::out_of_range("No more elements");
        }
        return collection[position++];
    }

    T current() const override {
        if (position == 0 || position > collection.size()) {
            throw std::out_of_range("Invalid position");
        }
        return collection[position - 1];
    }

    void reset() override {
        position = 0;
    }
};

// Concrete Aggregate - Book Collection
class Book {
private:
    std::string title;
    std::string author;
    int year;

public:
    Book(const std::string& title, const std::string& author, int year)
        : title(title), author(author), year(year) {}

    const std::string& getTitle() const { return title; }
    const std::string& getAuthor() const { return author; }
    int getYear() const { return year; }

    std::string toString() const {
        return "\"" + title + "\" by " + author + " (" + std::to_string(year) + ")";
    }
};

class BookCollection : public Aggregate<Book> {
private:
    std::vector<Book> books;

public:
    void addBook(const Book& book) {
        books.push_back(book);
        std::cout << "📚 Added: " << book.toString() << std::endl;
    }

    void removeBook(const std::string& title) {
        auto it = std::find_if(books.begin(), books.end(),
            [&title](const Book& book) {
                return book.getTitle() == title;
            });

        if (it != books.end()) {
            std::cout << "🗑️ Removed: " << it->toString() << std::endl;
            books.erase(it);
        } else {
            std::cout << "❌ Book not found: " << title << std::endl;
        }
    }

    std::unique_ptr<Iterator<Book>> createIterator() override {
        return std::make_unique<ArrayIterator<Book>>(books);
    }

    size_t size() const override {
        return books.size();
    }

    const std::vector<Book>& getBooks() const { return books; }
};

// Example 2: Different Iterator Types
class Student {
private:
    std::string name;
    int grade;
    std::string subject;

public:
    Student(const std::string& name, int grade, const std::string& subject)
        : name(name), grade(grade), subject(subject) {}

    const std::string& getName() const { return name; }
    int getGrade() const { return grade; }
    const std::string& getSubject() const { return subject; }

    std::string toString() const {
        return name + " (Grade: " + std::to_string(grade) + ", Subject: " + subject + ")";
    }
};

class Classroom {
private:
    std::vector<Student> students;

public:
    void addStudent(const Student& student) {
        students.push_back(student);
        std::cout << "👨‍🎓 Added student: " << student.toString() << std::endl;
    }

    // Forward Iterator
    class ForwardIterator {
    private:
        const std::vector<Student>& students;
        size_t position;

    public:
        explicit ForwardIterator(const std::vector<Student>& students)
            : students(students), position(0) {}

        bool hasNext() const { return position < students.size(); }

        const Student& next() {
            if (!hasNext()) throw std::out_of_range("No more students");
            return students[position++];
        }

        void reset() { position = 0; }
    };

    // Reverse Iterator
    class ReverseIterator {
    private:
        const std::vector<Student>& students;
        int position; // int to handle negative values

    public:
        explicit ReverseIterator(const std::vector<Student>& students)
            : students(students), position(static_cast<int>(students.size()) - 1) {}

        bool hasNext() const { return position >= 0; }

        const Student& next() {
            if (!hasNext()) throw std::out_of_range("No more students");
            return students[position--];
        }

        void reset() { position = static_cast<int>(students.size()) - 1; }
    };

    // Filtered Iterator
    class FilteredIterator {
    private:
        const std::vector<Student>& students;
        std::function<bool(const Student&)> predicate;
        size_t position;

        void findNext() {
            while (position < students.size() && !predicate(students[position])) {
                position++;
            }
        }

    public:
        FilteredIterator(const std::vector<Student>& students,
                        std::function<bool(const Student&)> pred)
            : students(students), predicate(pred), position(0) {
            findNext();
        }

        bool hasNext() const { return position < students.size(); }

        const Student& next() {
            if (!hasNext()) throw std::out_of_range("No more students");
            const Student& student = students[position++];
            findNext();
            return student;
        }

        void reset() {
            position = 0;
            findNext();
        }
    };

    ForwardIterator getForwardIterator() const {
        return ForwardIterator(students);
    }

    ReverseIterator getReverseIterator() const {
        return ReverseIterator(students);
    }

    FilteredIterator getHighGradeIterator() const {
        return FilteredIterator(students, [](const Student& s) {
            return s.getGrade() >= 90;
        });
    }

    FilteredIterator getSubjectIterator(const std::string& subject) const {
        return FilteredIterator(students, [subject](const Student& s) {
            return s.getSubject() == subject;
        });
    }

    size_t size() const { return students.size(); }
};

// Example 3: Tree Iterator (Depth-First and Breadth-First)
template<typename T>
class TreeNode {
public:
    T data;
    std::vector<std::shared_ptr<TreeNode<T>>> children;

    explicit TreeNode(const T& data) : data(data) {}

    void addChild(std::shared_ptr<TreeNode<T>> child) {
        children.push_back(child);
    }

    std::string toString() const {
        return std::to_string(data);
    }
};

template<typename T>
class Tree {
private:
    std::shared_ptr<TreeNode<T>> root;

public:
    explicit Tree(const T& rootData) : root(std::make_shared<TreeNode<T>>(rootData)) {}

    std::shared_ptr<TreeNode<T>> getRoot() { return root; }

    // Depth-First Iterator
    class DepthFirstIterator {
    private:
        std::vector<std::shared_ptr<TreeNode<T>>> stack;

    public:
        explicit DepthFirstIterator(std::shared_ptr<TreeNode<T>> root) {
            if (root) {
                stack.push_back(root);
            }
        }

        bool hasNext() const { return !stack.empty(); }

        const T& next() {
            if (!hasNext()) throw std::out_of_range("No more nodes");

            auto current = stack.back();
            stack.pop_back();

            // Add children in reverse order for correct DFS order
            for (auto it = current->children.rbegin(); it != current->children.rend(); ++it) {
                stack.push_back(*it);
            }

            return current->data;
        }
    };

    // Breadth-First Iterator
    class BreadthFirstIterator {
    private:
        std::vector<std::shared_ptr<TreeNode<T>>> queue;
        size_t front;

    public:
        explicit BreadthFirstIterator(std::shared_ptr<TreeNode<T>> root) : front(0) {
            if (root) {
                queue.push_back(root);
            }
        }

        bool hasNext() const { return front < queue.size(); }

        const T& next() {
            if (!hasNext()) throw std::out_of_range("No more nodes");

            auto current = queue[front++];

            // Add children to queue
            for (auto child : current->children) {
                queue.push_back(child);
            }

            return current->data;
        }
    };

    DepthFirstIterator getDepthFirstIterator() {
        return DepthFirstIterator(root);
    }

    BreadthFirstIterator getBreadthFirstIterator() {
        return BreadthFirstIterator(root);
    }
};

// Example 4: Modern C++ Range-based Iterator
class NumberSequence {
private:
    std::vector<int> numbers;

public:
    void addNumber(int num) { numbers.push_back(num); }

    // STL-compatible iterator
    class iterator {
    private:
        std::vector<int>::const_iterator it;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = ptrdiff_t;
        using pointer = const int*;
        using reference = const int&;

        explicit iterator(std::vector<int>::const_iterator it) : it(it) {}

        reference operator*() const { return *it; }
        pointer operator->() const { return &(*it); }

        iterator& operator++() { ++it; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++it; return tmp; }

        bool operator==(const iterator& other) const { return it == other.it; }
        bool operator!=(const iterator& other) const { return it != other.it; }
    };

    iterator begin() const { return iterator(numbers.begin()); }
    iterator end() const { return iterator(numbers.end()); }

    size_t size() const { return numbers.size(); }
};

// Example 5: Custom Range with Filters
template<typename Container, typename Predicate>
class FilteredRange {
private:
    const Container& container;
    Predicate predicate;

public:
    FilteredRange(const Container& container, Predicate pred)
        : container(container), predicate(pred) {}

    class iterator {
    private:
        typename Container::const_iterator current;
        typename Container::const_iterator end;
        Predicate predicate;

        void findNext() {
            while (current != end && !predicate(*current)) {
                ++current;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Container::value_type;
        using difference_type = ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        iterator(typename Container::const_iterator current,
                typename Container::const_iterator end,
                Predicate pred)
            : current(current), end(end), predicate(pred) {
            findNext();
        }

        reference operator*() const { return *current; }
        pointer operator->() const { return &(*current); }

        iterator& operator++() {
            ++current;
            findNext();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    iterator begin() const {
        return iterator(container.begin(), container.end(), predicate);
    }

    iterator end() const {
        return iterator(container.end(), container.end(), predicate);
    }
};

template<typename Container, typename Predicate>
FilteredRange<Container, Predicate> filter(const Container& container, Predicate pred) {
    return FilteredRange<Container, Predicate>(container, pred);
}

int main() {
    std::cout << "=== Iterator Pattern Demo ===" << std::endl;

    // Example 1: Traditional Iterator Pattern
    std::cout << "\n1. Traditional Iterator Pattern - Book Collection:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    BookCollection library;
    library.addBook(Book("1984", "George Orwell", 1949));
    library.addBook(Book("To Kill a Mockingbird", "Harper Lee", 1960));
    library.addBook(Book("The Great Gatsby", "F. Scott Fitzgerald", 1925));
    library.addBook(Book("Pride and Prejudice", "Jane Austen", 1813));

    std::cout << "\nIterating through books:" << std::endl;
    auto iterator = library.createIterator();
    while (iterator->hasNext()) {
        Book book = iterator->next();
        std::cout << "📖 " << book.toString() << std::endl;
    }

    std::cout << "\nTotal books: " << library.size() << std::endl;

    // Example 2: Different Iterator Types
    std::cout << "\n\n2. Different Iterator Types - Classroom:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    Classroom classroom;
    classroom.addStudent(Student("Alice", 95, "Math"));
    classroom.addStudent(Student("Bob", 87, "Science"));
    classroom.addStudent(Student("Charlie", 92, "Math"));
    classroom.addStudent(Student("Diana", 78, "English"));
    classroom.addStudent(Student("Eve", 96, "Science"));

    std::cout << "\nForward iteration:" << std::endl;
    auto forward = classroom.getForwardIterator();
    while (forward.hasNext()) {
        std::cout << "👨‍🎓 " << forward.next().toString() << std::endl;
    }

    std::cout << "\nReverse iteration:" << std::endl;
    auto reverse = classroom.getReverseIterator();
    while (reverse.hasNext()) {
        std::cout << "👩‍🎓 " << reverse.next().toString() << std::endl;
    }

    std::cout << "\nHigh grade students (>=90):" << std::endl;
    auto highGrade = classroom.getHighGradeIterator();
    while (highGrade.hasNext()) {
        std::cout << "🏆 " << highGrade.next().toString() << std::endl;
    }

    std::cout << "\nMath students only:" << std::endl;
    auto mathStudents = classroom.getSubjectIterator("Math");
    while (mathStudents.hasNext()) {
        std::cout << "📐 " << mathStudents.next().toString() << std::endl;
    }

    // Example 3: Tree Traversal Iterators
    std::cout << "\n\n3. Tree Traversal Iterators:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    Tree<int> tree(1);
    auto root = tree.getRoot();

    auto child2 = std::make_shared<TreeNode<int>>(2);
    auto child3 = std::make_shared<TreeNode<int>>(3);
    auto child4 = std::make_shared<TreeNode<int>>(4);

    root->addChild(child2);
    root->addChild(child3);
    child2->addChild(std::make_shared<TreeNode<int>>(5));
    child2->addChild(std::make_shared<TreeNode<int>>(6));
    child3->addChild(child4);
    child4->addChild(std::make_shared<TreeNode<int>>(7));

    std::cout << "Tree structure:\n";
    std::cout << "    1\n";
    std::cout << "   / \\\n";
    std::cout << "  2   3\n";
    std::cout << " / \\   \\\n";
    std::cout << "5   6   4\n";
    std::cout << "         \\\n";
    std::cout << "          7\n";

    std::cout << "\nDepth-First Traversal: ";
    auto dfs = tree.getDepthFirstIterator();
    while (dfs.hasNext()) {
        std::cout << dfs.next() << " ";
    }
    std::cout << std::endl;

    std::cout << "Breadth-First Traversal: ";
    auto bfs = tree.getBreadthFirstIterator();
    while (bfs.hasNext()) {
        std::cout << bfs.next() << " ";
    }
    std::cout << std::endl;

    // Example 4: STL-Compatible Iterator with Range-Based For
    std::cout << "\n\n4. STL-Compatible Iterator with Range-Based For:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    NumberSequence sequence;
    for (int i = 1; i <= 10; ++i) {
        sequence.addNumber(i * i); // Square numbers
    }

    std::cout << "Numbers using range-based for loop: ";
    for (const auto& num : sequence) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::cout << "Numbers using STL algorithms:" << std::endl;
    std::cout << "Sum: " << std::accumulate(sequence.begin(), sequence.end(), 0) << std::endl;
    std::cout << "Count > 25: " << std::count_if(sequence.begin(), sequence.end(),
                                                [](int n) { return n > 25; }) << std::endl;

    // Example 5: Custom Filtered Range
    std::cout << "\n\n5. Custom Filtered Range:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::cout << "Original numbers: ";
    for (const auto& num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::cout << "Even numbers only: ";
    for (const auto& num : filter(numbers, [](int n) { return n % 2 == 0; })) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::cout << "Numbers > 5: ";
    for (const auto& num : filter(numbers, [](int n) { return n > 5; })) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // Example 6: Iterator with STL Containers
    std::cout << "\n\n6. Working with STL Containers:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    std::list<std::string> words = {"apple", "banana", "cherry", "date", "elderberry"};

    std::cout << "Using iterators manually:" << std::endl;
    for (auto it = words.begin(); it != words.end(); ++it) {
        std::cout << "🍎 " << *it << std::endl;
    }

    std::cout << "\nUsing reverse iterators:" << std::endl;
    for (auto it = words.rbegin(); it != words.rend(); ++it) {
        std::cout << "🔄 " << *it << std::endl;
    }

    std::cout << "\nUsing STL algorithms with iterators:" << std::endl;
    auto longWords = std::count_if(words.begin(), words.end(),
                                  [](const std::string& word) {
                                      return word.length() > 5;
                                  });
    std::cout << "Words longer than 5 characters: " << longWords << std::endl;

    auto foundIt = std::find(words.begin(), words.end(), "cherry");
    if (foundIt != words.end()) {
        std::cout << "Found 'cherry' at position: "
                  << std::distance(words.begin(), foundIt) << std::endl;
    }

    // Benefits and Usage
    std::cout << "\n\n7. Iterator Pattern Benefits:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "✓ Provides uniform interface for traversing collections" << std::endl;
    std::cout << "✓ Supports multiple simultaneous traversals" << std::endl;
    std::cout << "✓ Decouples algorithms from data structures" << std::endl;
    std::cout << "✓ Enables polymorphic iteration" << std::endl;
    std::cout << "✓ Supports different traversal strategies" << std::endl;
    std::cout << "✓ Integrates well with STL algorithms" << std::endl;
    std::cout << "✓ Enables range-based for loops in modern C++" << std::endl;

    std::cout << "\n8. Modern C++ Iterator Features:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "✓ Range-based for loops (C++11)" << std::endl;
    std::cout << "✓ Iterator categories and concepts" << std::endl;
    std::cout << "✓ STL algorithm compatibility" << std::endl;
    std::cout << "✓ Lazy evaluation with custom ranges" << std::endl;
    std::cout << "✓ Lambda-based filtering and transformation" << std::endl;

    return 0;
}