#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <fstream>

// Subject interface
class Image {
public:
    virtual ~Image() = default;
    virtual void display() = 0;
    virtual std::string getFileName() const = 0;
    virtual size_t getFileSize() const = 0;
};

// Real Subject
class RealImage : public Image {
private:
    std::string fileName;
    size_t fileSize;
    std::vector<uint8_t> imageData;

    void loadFromDisk() {
        std::cout << "Loading image from disk: " << fileName << std::endl;
        // Simulate slow disk I/O
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Simulate image data loading
        fileSize = fileName.length() * 1024; // Fake file size
        imageData.resize(fileSize);
        std::fill(imageData.begin(), imageData.end(), 0xFF); // Fake data

        std::cout << "Image loaded: " << fileName << " (" << fileSize << " bytes)" << std::endl;
    }

public:
    explicit RealImage(const std::string& fileName) : fileName(fileName), fileSize(0) {
        loadFromDisk();
    }

    void display() override {
        std::cout << "Displaying image: " << fileName << " (" << fileSize << " bytes)" << std::endl;
    }

    std::string getFileName() const override {
        return fileName;
    }

    size_t getFileSize() const override {
        return fileSize;
    }
};

// Virtual Proxy (Lazy Loading)
class ImageProxy : public Image {
private:
    std::string fileName;
    mutable std::unique_ptr<RealImage> realImage;
    mutable size_t cachedFileSize;

public:
    explicit ImageProxy(const std::string& fileName)
        : fileName(fileName), cachedFileSize(0) {}

    void display() override {
        if (!realImage) {
            std::cout << "Proxy: Creating real image on first access" << std::endl;
            realImage = std::make_unique<RealImage>(fileName);
            cachedFileSize = realImage->getFileSize();
        }
        realImage->display();
    }

    std::string getFileName() const override {
        return fileName;
    }

    size_t getFileSize() const override {
        if (!realImage) {
            // Could return cached metadata without loading full image
            return fileName.length() * 1024; // Estimated size
        }
        return realImage->getFileSize();
    }
};

// Example 2: Protection Proxy
class BankAccount {
public:
    virtual ~BankAccount() = default;
    virtual void deposit(double amount) = 0;
    virtual bool withdraw(double amount) = 0;
    virtual double getBalance() const = 0;
    virtual std::string getAccountNumber() const = 0;
};

class RealBankAccount : public BankAccount {
private:
    std::string accountNumber;
    double balance;

public:
    RealBankAccount(const std::string& accountNumber, double initialBalance)
        : accountNumber(accountNumber), balance(initialBalance) {}

    void deposit(double amount) override {
        balance += amount;
        std::cout << "Deposited $" << amount << ". New balance: $" << balance << std::endl;
    }

    bool withdraw(double amount) override {
        if (balance >= amount) {
            balance -= amount;
            std::cout << "Withdrew $" << amount << ". New balance: $" << balance << std::endl;
            return true;
        } else {
            std::cout << "Insufficient funds. Current balance: $" << balance << std::endl;
            return false;
        }
    }

    double getBalance() const override {
        return balance;
    }

    std::string getAccountNumber() const override {
        return accountNumber;
    }
};

enum class UserRole {
    CUSTOMER,
    EMPLOYEE,
    ADMINISTRATOR
};

class ProtectionProxy : public BankAccount {
private:
    std::unique_ptr<RealBankAccount> realAccount;
    UserRole userRole;
    std::string userId;

    bool hasPermission(const std::string& operation) const {
        std::cout << "[Security] Checking permission for " << userId << " ("
                  << roleToString(userRole) << ") to perform: " << operation << std::endl;

        if (operation == "getBalance") {
            return true; // All users can check balance
        } else if (operation == "deposit") {
            return userRole == UserRole::CUSTOMER || userRole == UserRole::EMPLOYEE || userRole == UserRole::ADMINISTRATOR;
        } else if (operation == "withdraw") {
            return userRole == UserRole::CUSTOMER || userRole == UserRole::ADMINISTRATOR;
        } else if (operation == "getAccountNumber") {
            return userRole == UserRole::EMPLOYEE || userRole == UserRole::ADMINISTRATOR;
        }
        return false;
    }

    std::string roleToString(UserRole role) const {
        switch (role) {
            case UserRole::CUSTOMER: return "Customer";
            case UserRole::EMPLOYEE: return "Employee";
            case UserRole::ADMINISTRATOR: return "Administrator";
            default: return "Unknown";
        }
    }

public:
    ProtectionProxy(std::unique_ptr<RealBankAccount> account, UserRole role, const std::string& userId)
        : realAccount(std::move(account)), userRole(role), userId(userId) {}

    void deposit(double amount) override {
        if (hasPermission("deposit")) {
            realAccount->deposit(amount);
        } else {
            std::cout << "Access denied: Insufficient permissions for deposit" << std::endl;
        }
    }

    bool withdraw(double amount) override {
        if (hasPermission("withdraw")) {
            return realAccount->withdraw(amount);
        } else {
            std::cout << "Access denied: Insufficient permissions for withdrawal" << std::endl;
            return false;
        }
    }

    double getBalance() const override {
        if (hasPermission("getBalance")) {
            return realAccount->getBalance();
        } else {
            std::cout << "Access denied: Insufficient permissions to view balance" << std::endl;
            return -1;
        }
    }

    std::string getAccountNumber() const override {
        if (hasPermission("getAccountNumber")) {
            return realAccount->getAccountNumber();
        } else {
            std::cout << "Access denied: Insufficient permissions to view account number" << std::endl;
            return "***HIDDEN***";
        }
    }
};

// Example 3: Caching Proxy
class WebService {
public:
    virtual ~WebService() = default;
    virtual std::string fetchData(const std::string& url) = 0;
};

class RealWebService : public WebService {
public:
    std::string fetchData(const std::string& url) override {
        std::cout << "Fetching data from: " << url << std::endl;
        // Simulate network delay
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        // Simulate fetched data
        return "Data from " + url + " - Content: Lorem ipsum dolor sit amet...";
    }
};

class CachingProxy : public WebService {
private:
    std::unique_ptr<RealWebService> realService;
    mutable std::unordered_map<std::string, std::string> cache;
    mutable std::unordered_map<std::string, std::chrono::steady_clock::time_point> cacheTimestamps;
    std::chrono::seconds cacheExpiryTime;

    bool isCacheValid(const std::string& url) const {
        auto it = cacheTimestamps.find(url);
        if (it == cacheTimestamps.end()) {
            return false;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second);
        return elapsed < cacheExpiryTime;
    }

public:
    CachingProxy(std::chrono::seconds expiryTime = std::chrono::seconds(30))
        : realService(std::make_unique<RealWebService>()), cacheExpiryTime(expiryTime) {}

    std::string fetchData(const std::string& url) override {
        if (cache.find(url) != cache.end() && isCacheValid(url)) {
            std::cout << "Cache hit! Returning cached data for: " << url << std::endl;
            return cache[url];
        }

        std::cout << "Cache miss. Fetching fresh data..." << std::endl;
        std::string data = realService->fetchData(url);

        // Store in cache
        cache[url] = data;
        cacheTimestamps[url] = std::chrono::steady_clock::now();

        return data;
    }

    void clearCache() {
        cache.clear();
        cacheTimestamps.clear();
        std::cout << "Cache cleared" << std::endl;
    }

    size_t getCacheSize() const {
        return cache.size();
    }
};

// Example 4: Remote Proxy (Simplified)
class DatabaseService {
public:
    virtual ~DatabaseService() = default;
    virtual std::string query(const std::string& sql) = 0;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
};

class RemoteDatabaseService : public DatabaseService {
private:
    std::string serverAddress;
    bool connected;

public:
    explicit RemoteDatabaseService(const std::string& address)
        : serverAddress(address), connected(false) {}

    bool connect() override {
        std::cout << "Connecting to database server: " << serverAddress << std::endl;
        // Simulate connection time
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        connected = true;
        std::cout << "Connected successfully!" << std::endl;
        return true;
    }

    void disconnect() override {
        std::cout << "Disconnecting from database server" << std::endl;
        connected = false;
    }

    std::string query(const std::string& sql) override {
        if (!connected) {
            throw std::runtime_error("Not connected to database");
        }

        std::cout << "Executing query: " << sql << std::endl;
        // Simulate query execution time
        std::this_thread::sleep_for(std::chrono::milliseconds(800));

        return "Query result for: " + sql;
    }
};

class DatabaseProxy : public DatabaseService {
private:
    std::unique_ptr<RemoteDatabaseService> remoteService;
    std::string serverAddress;
    bool autoConnect;

public:
    DatabaseProxy(const std::string& address, bool autoConnect = true)
        : serverAddress(address), autoConnect(autoConnect) {}

    bool connect() override {
        if (!remoteService) {
            remoteService = std::make_unique<RemoteDatabaseService>(serverAddress);
        }
        return remoteService->connect();
    }

    void disconnect() override {
        if (remoteService) {
            remoteService->disconnect();
        }
    }

    std::string query(const std::string& sql) override {
        if (!remoteService) {
            if (autoConnect) {
                std::cout << "Proxy: Auto-connecting to database..." << std::endl;
                connect();
            } else {
                throw std::runtime_error("Not connected to database");
            }
        }

        try {
            return remoteService->query(sql);
        } catch (const std::exception& e) {
            std::cout << "Proxy: Query failed, attempting to reconnect..." << std::endl;
            connect();
            return remoteService->query(sql);
        }
    }
};

// Smart Proxy with reference counting
template<typename T>
class SmartProxy {
private:
    T* object;
    mutable int* refCount;

public:
    explicit SmartProxy(T* obj) : object(obj), refCount(new int(1)) {
        std::cout << "SmartProxy created, ref count: " << *refCount << std::endl;
    }

    SmartProxy(const SmartProxy& other) : object(other.object), refCount(other.refCount) {
        ++(*refCount);
        std::cout << "SmartProxy copied, ref count: " << *refCount << std::endl;
    }

    SmartProxy& operator=(const SmartProxy& other) {
        if (this != &other) {
            release();
            object = other.object;
            refCount = other.refCount;
            ++(*refCount);
            std::cout << "SmartProxy assigned, ref count: " << *refCount << std::endl;
        }
        return *this;
    }

    ~SmartProxy() {
        release();
    }

    T* operator->() const {
        return object;
    }

    T& operator*() const {
        return *object;
    }

    int getRefCount() const {
        return *refCount;
    }

private:
    void release() {
        --(*refCount);
        std::cout << "SmartProxy released, ref count: " << *refCount << std::endl;
        if (*refCount == 0) {
            delete object;
            delete refCount;
            std::cout << "Object deleted" << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Proxy Pattern Demo ===" << std::endl;

    // Example 1: Virtual Proxy (Lazy Loading)
    std::cout << "\n1. Virtual Proxy - Lazy Loading Images:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::vector<std::unique_ptr<Image>> images;
    images.push_back(std::make_unique<ImageProxy>("photo1.jpg"));
    images.push_back(std::make_unique<ImageProxy>("photo2.jpg"));
    images.push_back(std::make_unique<ImageProxy>("photo3.jpg"));

    std::cout << "\nImages created (but not loaded yet)" << std::endl;

    std::cout << "\nDisplaying first image:" << std::endl;
    images[0]->display();

    std::cout << "\nDisplaying first image again (already loaded):" << std::endl;
    images[0]->display();

    std::cout << "\nChecking file sizes:" << std::endl;
    for (const auto& image : images) {
        std::cout << image->getFileName() << ": " << image->getFileSize() << " bytes" << std::endl;
    }

    // Example 2: Protection Proxy
    std::cout << "\n\n2. Protection Proxy - Bank Account Security:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    auto realAccount = std::make_unique<RealBankAccount>("ACC-123456", 1000.0);

    // Customer access
    std::cout << "\nCustomer Access:" << std::endl;
    ProtectionProxy customerProxy(std::make_unique<RealBankAccount>("ACC-123456", 1000.0),
                                 UserRole::CUSTOMER, "john_doe");

    std::cout << "Balance: $" << customerProxy.getBalance() << std::endl;
    customerProxy.deposit(100.0);
    customerProxy.withdraw(50.0);
    std::cout << "Account Number: " << customerProxy.getAccountNumber() << std::endl;

    // Employee access
    std::cout << "\nEmployee Access:" << std::endl;
    ProtectionProxy employeeProxy(std::make_unique<RealBankAccount>("ACC-789012", 2000.0),
                                 UserRole::EMPLOYEE, "jane_smith");

    std::cout << "Balance: $" << employeeProxy.getBalance() << std::endl;
    employeeProxy.deposit(200.0);
    employeeProxy.withdraw(100.0); // Should be denied
    std::cout << "Account Number: " << employeeProxy.getAccountNumber() << std::endl;

    // Example 3: Caching Proxy
    std::cout << "\n\n3. Caching Proxy - Web Service:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    CachingProxy webProxy(std::chrono::seconds(5)); // 5-second cache

    std::cout << "\nFirst request:" << std::endl;
    std::string data1 = webProxy.fetchData("https://api.example.com/users");

    std::cout << "\nSecond request (should hit cache):" << std::endl;
    std::string data2 = webProxy.fetchData("https://api.example.com/users");

    std::cout << "\nDifferent URL:" << std::endl;
    std::string data3 = webProxy.fetchData("https://api.example.com/posts");

    std::cout << "\nCache size: " << webProxy.getCacheSize() << std::endl;

    // Example 4: Remote Proxy
    std::cout << "\n\n4. Remote Proxy - Database Connection:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    DatabaseProxy dbProxy("db.example.com:5432", true);

    try {
        std::cout << "\nExecuting queries through proxy:" << std::endl;
        std::string result1 = dbProxy.query("SELECT * FROM users");
        std::cout << "Result: " << result1 << std::endl;

        std::string result2 = dbProxy.query("SELECT * FROM orders WHERE user_id = 1");
        std::cout << "Result: " << result2 << std::endl;

        dbProxy.disconnect();
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    // Example 5: Smart Proxy (Reference Counting)
    std::cout << "\n\n5. Smart Proxy - Reference Counting:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    {
        SmartProxy<std::string> proxy1(new std::string("Hello, World!"));
        std::cout << "proxy1 content: " << *proxy1 << std::endl;

        {
            SmartProxy<std::string> proxy2 = proxy1;
            std::cout << "proxy2 content: " << *proxy2 << std::endl;
            std::cout << "Reference count: " << proxy2.getRefCount() << std::endl;
        } // proxy2 goes out of scope

        std::cout << "After proxy2 destruction, ref count: " << proxy1.getRefCount() << std::endl;
    } // proxy1 goes out of scope, object should be deleted

    std::cout << "\n\n6. Proxy Pattern Benefits:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "✓ Virtual Proxy: Lazy loading saves memory and startup time" << std::endl;
    std::cout << "✓ Protection Proxy: Controls access based on user permissions" << std::endl;
    std::cout << "✓ Caching Proxy: Improves performance by avoiding redundant operations" << std::endl;
    std::cout << "✓ Remote Proxy: Hides complexity of remote communication" << std::endl;
    std::cout << "✓ Smart Proxy: Adds automatic memory management" << std::endl;

    return 0;
}