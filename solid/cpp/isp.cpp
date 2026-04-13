// Interface Segregation Principle (ISP)
// Clients should not be forced to depend on interfaces they do not use.

#include <iostream>
#include <string>
#include <memory>

// ============================================================
// BAD: Fat interface forces unnecessary implementations
// ============================================================
namespace bad {

class Machine {
public:
    virtual ~Machine() = default;
    virtual void print(const std::string& doc) = 0;
    virtual void scan(const std::string& doc) = 0;
    virtual void fax(const std::string& doc) = 0;
};

class MultiFunctionPrinter : public Machine {
public:
    void print(const std::string& doc) override {
        std::cout << "  [MFP] Printing: " << doc << "\n";
    }
    void scan(const std::string& doc) override {
        std::cout << "  [MFP] Scanning: " << doc << "\n";
    }
    void fax(const std::string& doc) override {
        std::cout << "  [MFP] Faxing: " << doc << "\n";
    }
};

// PROBLEM: SimplePrinter is forced to implement scan() and fax()
class SimplePrinter : public Machine {
public:
    void print(const std::string& doc) override {
        std::cout << "  [Simple] Printing: " << doc << "\n";
    }
    void scan(const std::string&) override {
        throw std::runtime_error("SimplePrinter cannot scan!");
    }
    void fax(const std::string&) override {
        throw std::runtime_error("SimplePrinter cannot fax!");
    }
};

} // namespace bad

// ============================================================
// GOOD: Segregated interfaces — implement only what you need
// ============================================================
namespace good {

class Printer {
public:
    virtual ~Printer() = default;
    virtual void print(const std::string& doc) = 0;
};

class Scanner {
public:
    virtual ~Scanner() = default;
    virtual void scan(const std::string& doc) = 0;
};

class Faxer {
public:
    virtual ~Faxer() = default;
    virtual void fax(const std::string& doc) = 0;
};

// Multi-function: implements all three
class MultiFunctionPrinter : public Printer, public Scanner, public Faxer {
public:
    void print(const std::string& doc) override {
        std::cout << "  [MFP] Printing: " << doc << "\n";
    }
    void scan(const std::string& doc) override {
        std::cout << "  [MFP] Scanning: " << doc << "\n";
    }
    void fax(const std::string& doc) override {
        std::cout << "  [MFP] Faxing: " << doc << "\n";
    }
};

// Simple: only implements Printer — no dead methods
class SimplePrinter : public Printer {
public:
    void print(const std::string& doc) override {
        std::cout << "  [Simple] Printing: " << doc << "\n";
    }
};

// Client depends only on what it needs
void do_print(Printer& p) {
    p.print("report.pdf");
}

void do_scan(Scanner& s) {
    s.scan("photo.jpg");
}

} // namespace good

int main() {
    std::cout << "=== BAD: Fat interface ===\n";
    {
        bad::MultiFunctionPrinter mfp;
        mfp.print("doc.pdf");
        mfp.scan("photo.jpg");

        bad::SimplePrinter sp;
        sp.print("doc.pdf");
        try {
            sp.scan("photo.jpg");  // throws!
        } catch (const std::exception& e) {
            std::cout << "  ERROR: " << e.what() << "\n";
        }
    }

    std::cout << "\n=== GOOD: Segregated interfaces ===\n";
    {
        good::MultiFunctionPrinter mfp;
        good::SimplePrinter sp;

        good::do_print(mfp);
        good::do_print(sp);
        good::do_scan(mfp);
        // good::do_scan(sp);  // Won't compile — sp is not a Scanner
    }

    return 0;
}
