// Data Mapper — Enterprise Pattern (PoEAA)
//
// Intent: a layer of mappers that moves data between domain objects and a
//         database while keeping them independent of each other.
//         The domain model does not know how it is persisted.
//
// When to use:
//   - Rich domain model where business logic != table structure
//   - You want to swap or test persistence independently of domain logic
//   - The ORM doesn't fit your schema (multiple tables -> one entity, etc.)
//
// When NOT to use:
//   - Simple CRUD — ActiveRecord or Repository with trivial mapping is cleaner
//   - The domain and schema are 1:1 — the mapper adds pointless indirection

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

// ============================================================================
// Domain model — knows nothing about persistence
// ============================================================================
class User {
public:
    User(int id, std::string name, std::string email)
        : id_(id), name_(std::move(name)), email_(std::move(email)) {}
    int                id()    const { return id_; }
    const std::string& name()  const { return name_; }
    const std::string& email() const { return email_; }
    void update_email(const std::string& e) { email_ = e; }
private:
    int         id_;
    std::string name_;
    std::string email_;
};

// ============================================================================
// "Database" — flat row store (simulates SQL rows)
// ============================================================================
struct UserRow {
    int         id;
    std::string first_name;
    std::string last_name;
    std::string email;
};

class UserTable {
public:
    void insert(UserRow row)  { rows_[row.id] = std::move(row); }
    void update(UserRow row)  { rows_[row.id] = std::move(row); }
    void erase(int id)       { rows_.erase(id); }
    std::optional<UserRow> find(int id) const {
        auto it = rows_.find(id);
        if (it == rows_.end()) return std::nullopt;
        return it->second;
    }
    std::vector<UserRow> all() const {
        std::vector<UserRow> out;
        for (const auto& [k, v] : rows_) out.push_back(v);
        return out;
    }
private:
    std::unordered_map<int, UserRow> rows_;
};

// ============================================================================
// The mapper — translates between domain and database
// ============================================================================
class UserMapper {
public:
    explicit UserMapper(UserTable& table) : table_(table) {}

    User find(int id) const {
        auto row = table_.find(id);
        if (!row) throw std::runtime_error("not found");
        return to_domain(*row);
    }

    std::vector<User> find_all() const {
        std::vector<User> result;
        for (const auto& row : table_.all()) {
            result.push_back(to_domain(row));
        }
        return result;
    }

    void insert(const User& user) {
        table_.insert(to_row(user));
    }

    void update(const User& user) {
        table_.update(to_row(user));
    }

private:
    // Mapping: domain name = "first last", db has separate first/last columns
    static User to_domain(const UserRow& row) {
        std::string full_name = row.first_name + " " + row.last_name;
        return User(row.id, full_name, row.email);
    }

    static UserRow to_row(const User& u) {
        // Split "first last" back into separate columns
        auto sp = u.name().find(' ');
        std::string first = (sp != std::string::npos) ? u.name().substr(0, sp) : u.name();
        std::string last  = (sp != std::string::npos) ? u.name().substr(sp + 1) : "";
        return UserRow{u.id(), first, last, u.email()};
    }

    UserTable& table_;
};

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Data Mapper Pattern ===\n";
    UserTable table;
    UserMapper mapper(table);

    mapper.insert(User(1, "Alice Smith",    "alice@example.com"));
    mapper.insert(User(2, "Bob Johnson",    "bob@example.com"));

    User alice = mapper.find(1);
    std::cout << "found: " << alice.name() << " <" << alice.email() << ">\n";

    alice.update_email("alice@newmail.com");
    mapper.update(alice);

    for (const auto& u : mapper.find_all()) {
        std::cout << "  #" << u.id() << " " << u.name() << " <" << u.email() << ">\n";
    }

    std::cout << "\nKey points:\n";
    std::cout << " * Domain model and database schema are decoupled\n";
    std::cout << " * Mapper handles transformations (e.g., name splitting)\n";
    std::cout << " * Domain code stays clean — no SQL or persistence concerns\n";
    std::cout << " * Compare with Repository (dp/cpp/repository.cpp)\n";
    return 0;
}
