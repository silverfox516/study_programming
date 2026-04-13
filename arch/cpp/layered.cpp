// Layered Architecture
// Separate concerns into layers: Presentation → Business → Data
// Each layer only depends on the layer directly below it.

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>

// ============================================================
// Layer 3 (bottom): Data Access Layer
// ============================================================
namespace data {

struct UserRecord {
    int id;
    std::string name;
    std::string email;
    int age;
};

class UserDao {
    std::vector<UserRecord> store_;
    int next_id_ = 1;
public:
    int insert(const std::string& name, const std::string& email, int age) {
        int id = next_id_++;
        store_.push_back({id, name, email, age});
        return id;
    }

    std::optional<UserRecord> find_by_id(int id) const {
        for (const auto& r : store_)
            if (r.id == id) return r;
        return std::nullopt;
    }

    std::vector<UserRecord> find_all() const { return store_; }
};

} // namespace data

// ============================================================
// Layer 2 (middle): Business Logic Layer
// ============================================================
namespace business {

struct User {
    int id;
    std::string name;
    std::string email;
    int age;
};

class UserService {
    data::UserDao& dao_;
public:
    explicit UserService(data::UserDao& dao) : dao_(dao) {}

    int register_user(const std::string& name, const std::string& email, int age) {
        // Business rule: age must be >= 13
        if (age < 13)
            throw std::runtime_error("User must be at least 13 years old");
        // Business rule: email must contain @
        if (email.find('@') == std::string::npos)
            throw std::runtime_error("Invalid email");

        int id = dao_.insert(name, email, age);
        std::cout << "  [Business] Registered user #" << id << ": " << name << "\n";
        return id;
    }

    User get_user(int id) {
        auto record = dao_.find_by_id(id);
        if (!record) throw std::runtime_error("User not found");
        return {record->id, record->name, record->email, record->age};
    }

    std::vector<User> list_adults() {
        std::vector<User> result;
        for (const auto& r : dao_.find_all())
            if (r.age >= 18)
                result.push_back({r.id, r.name, r.email, r.age});
        return result;
    }
};

} // namespace business

// ============================================================
// Layer 1 (top): Presentation Layer
// ============================================================
namespace presentation {

class UserController {
    business::UserService& service_;
public:
    explicit UserController(business::UserService& svc) : service_(svc) {}

    void handle_register(const std::string& name, const std::string& email, int age) {
        std::cout << "[Presentation] POST /users\n";
        try {
            int id = service_.register_user(name, email, age);
            std::cout << "  Response: 201 Created (id=" << id << ")\n";
        } catch (const std::exception& e) {
            std::cout << "  Response: 400 Bad Request (" << e.what() << ")\n";
        }
    }

    void handle_get_user(int id) {
        std::cout << "[Presentation] GET /users/" << id << "\n";
        try {
            auto user = service_.get_user(id);
            std::cout << "  Response: 200 {name: " << user.name
                      << ", email: " << user.email << "}\n";
        } catch (const std::exception& e) {
            std::cout << "  Response: 404 (" << e.what() << ")\n";
        }
    }

    void handle_list_adults() {
        std::cout << "[Presentation] GET /users?age_gte=18\n";
        auto adults = service_.list_adults();
        std::cout << "  Response: 200 [" << adults.size() << " users]\n";
        for (const auto& u : adults)
            std::cout << "    " << u.name << " (age " << u.age << ")\n";
    }
};

} // namespace presentation

int main() {
    // Wire layers bottom-up
    data::UserDao dao;
    business::UserService service(dao);
    presentation::UserController controller(service);

    controller.handle_register("Alice", "alice@example.com", 25);
    controller.handle_register("Bob", "bob@example.com", 12);  // rejected
    controller.handle_register("Charlie", "charlie@example.com", 17);

    std::cout << "\n";
    controller.handle_get_user(1);
    controller.handle_get_user(99);

    std::cout << "\n";
    controller.handle_list_adults();

    return 0;
}
