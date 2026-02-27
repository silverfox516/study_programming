#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <optional>

// Model
class User {
private:
    int id_;
    std::string name_;
    std::string email_;

public:
    User(int id, std::string name, std::string email)
        : id_(id), name_(std::move(name)), email_(std::move(email)) {}

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getEmail() const { return email_; }

    void setName(const std::string& name) { name_ = name; }
    void setEmail(const std::string& email) { email_ = email; }
};

class UserModel {
private:
    std::vector<std::unique_ptr<User>> users_;

public:
    void addUser(int id, const std::string& name, const std::string& email) {
        users_.push_back(std::make_unique<User>(id, name, email));
    }

    std::optional<std::reference_wrapper<const User>> getUser(int id) const {
        auto it = std::find_if(users_.begin(), users_.end(),
            [id](const auto& user) { return user->getId() == id; });

        if (it != users_.end()) {
            return std::cref(**it);
        }
        return std::nullopt;
    }

    const std::vector<std::unique_ptr<User>>& getAllUsers() const {
        return users_;
    }

    bool removeUser(int id) {
        auto it = std::remove_if(users_.begin(), users_.end(),
            [id](const auto& user) { return user->getId() == id; });

        if (it != users_.end()) {
            users_.erase(it, users_.end());
            return true;
        }
        return false;
    }

    size_t getUserCount() const {
        return users_.size();
    }
};

// View
class UserView {
public:
    virtual ~UserView() = default;

    virtual void displayUser(const User& user) const {
        std::cout << "User ID: " << user.getId() << "\n";
        std::cout << "Name: " << user.getName() << "\n";
        std::cout << "Email: " << user.getEmail() << "\n";
        std::cout << "---\n";
    }

    virtual void displayAllUsers(const std::vector<std::unique_ptr<User>>& users) const {
        std::cout << "=== All Users ===\n";
        for (const auto& user : users) {
            displayUser(*user);
        }
    }

    virtual void displayMessage(const std::string& message) const {
        std::cout << "Message: " << message << "\n";
    }

    virtual void displayError(const std::string& error) const {
        std::cout << "Error: " << error << "\n";
    }

    virtual void displayUserNotFound(int id) const {
        std::cout << "User with ID " << id << " not found\n";
    }
};

// Specialized views
class ConsoleUserView : public UserView {
public:
    void displayUser(const User& user) const override {
        std::cout << "┌─────────────────────────\n";
        std::cout << "│ User ID: " << user.getId() << "\n";
        std::cout << "│ Name: " << user.getName() << "\n";
        std::cout << "│ Email: " << user.getEmail() << "\n";
        std::cout << "└─────────────────────────\n";
    }
};

class JsonUserView : public UserView {
public:
    void displayUser(const User& user) const override {
        std::cout << "{\n";
        std::cout << "  \"id\": " << user.getId() << ",\n";
        std::cout << "  \"name\": \"" << user.getName() << "\",\n";
        std::cout << "  \"email\": \"" << user.getEmail() << "\"\n";
        std::cout << "}\n";
    }
};

// Controller
class UserController {
private:
    std::unique_ptr<UserModel> model_;
    std::unique_ptr<UserView> view_;

public:
    UserController(std::unique_ptr<UserModel> model, std::unique_ptr<UserView> view)
        : model_(std::move(model)), view_(std::move(view)) {}

    void addUser(int id, const std::string& name, const std::string& email) {
        if (name.empty() || email.empty()) {
            view_->displayError("Name and email cannot be empty");
            return;
        }

        // Check if user already exists
        if (model_->getUser(id).has_value()) {
            view_->displayError("User with ID " + std::to_string(id) + " already exists");
            return;
        }

        model_->addUser(id, name, email);
        view_->displayMessage("User added successfully");
    }

    void showUser(int id) {
        auto user = model_->getUser(id);
        if (user.has_value()) {
            view_->displayUser(user->get());
        } else {
            view_->displayUserNotFound(id);
        }
    }

    void showAllUsers() {
        const auto& users = model_->getAllUsers();
        if (users.empty()) {
            view_->displayMessage("No users found");
        } else {
            view_->displayAllUsers(users);
        }
    }

    void removeUser(int id) {
        if (model_->removeUser(id)) {
            view_->displayMessage("User removed successfully");
        } else {
            view_->displayUserNotFound(id);
        }
    }

    void showUserCount() {
        view_->displayMessage("Total users: " + std::to_string(model_->getUserCount()));
    }
};

int main() {
    std::cout << "=== MVC Pattern Demo ===\n\n";

    // Create MVC components
    auto model = std::make_unique<UserModel>();
    auto view = std::make_unique<ConsoleUserView>();
    auto controller = std::make_unique<UserController>(std::move(model), std::move(view));

    // Use controller to manage users
    controller->addUser(1, "Alice Johnson", "alice@example.com");
    controller->addUser(2, "Bob Smith", "bob@example.com");
    controller->addUser(3, "Charlie Brown", "charlie@example.com");

    std::cout << "\n";
    controller->showUserCount();

    // Display specific user
    std::cout << "\nShowing user with ID 2:\n";
    controller->showUser(2);

    // Display all users
    std::cout << "\nShowing all users:\n";
    controller->showAllUsers();

    // Try to show non-existent user
    std::cout << "\nTrying to show user with ID 999:\n";
    controller->showUser(999);

    // Test error handling
    std::cout << "\nTesting error handling:\n";
    controller->addUser(4, "", "invalid@example.com"); // Empty name
    controller->addUser(1, "Duplicate", "duplicate@example.com"); // Duplicate ID

    // Remove user
    std::cout << "\nRemoving user with ID 2:\n";
    controller->removeUser(2);
    controller->showUserCount();

    // Demonstrate different view
    std::cout << "\n=== Using JSON View ===\n";
    auto jsonModel = std::make_unique<UserModel>();
    auto jsonView = std::make_unique<JsonUserView>();
    auto jsonController = std::make_unique<UserController>(std::move(jsonModel), std::move(jsonView));

    jsonController->addUser(1, "John Doe", "john@example.com");
    jsonController->showUser(1);

    return 0;
}