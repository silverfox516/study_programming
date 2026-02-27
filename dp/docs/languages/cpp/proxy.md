# Proxy Pattern - C++ Implementation

## 개요

Proxy 패턴은 다른 객체에 대한 접근을 제어하는 대리자나 자리 표시자를 제공하는 구조적 패턴입니다. 실제 객체에 대한 접근을 제어하여 지연 로딩, 액세스 제어, 캐싱, 로깅 등의 추가 기능을 제공할 수 있습니다.

## 구조

- **Subject**: 실제 객체와 프록시가 공유하는 인터페이스
- **RealSubject**: 실제 작업을 수행하는 객체
- **Proxy**: 실제 객체에 대한 참조를 가지고 접근을 제어

## C++ 구현

### 1. Virtual Proxy (지연 로딩)

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>

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

        fileSize = fileName.length() * 1024; // Fake file size
        imageData.resize(fileSize);
        std::cout << "Image loaded: " << fileName << " (" << fileSize << " bytes)" << std::endl;
    }

public:
    explicit RealImage(const std::string& fileName) : fileName(fileName), fileSize(0) {
        loadFromDisk();
    }

    void display() override {
        std::cout << "Displaying image: " << fileName << " (" << fileSize << " bytes)" << std::endl;
    }

    std::string getFileName() const override { return fileName; }
    size_t getFileSize() const override { return fileSize; }
};

// Virtual Proxy (Lazy Loading)
class ImageProxy : public Image {
private:
    std::string fileName;
    mutable std::unique_ptr<RealImage> realImage;

public:
    explicit ImageProxy(const std::string& fileName) : fileName(fileName) {}

    void display() override {
        if (!realImage) {
            std::cout << "Proxy: Creating real image on first access" << std::endl;
            realImage = std::make_unique<RealImage>(fileName);
        }
        realImage->display();
    }

    std::string getFileName() const override { return fileName; }

    size_t getFileSize() const override {
        if (!realImage) {
            return 0; // Unknown size until loaded
        }
        return realImage->getFileSize();
    }
};
```

### 2. Protection Proxy (접근 제어)

```cpp
// Subject interface for database access
class DatabaseAccess {
public:
    virtual ~DatabaseAccess() = default;
    virtual std::string query(const std::string& sql) = 0;
    virtual bool insert(const std::string& table, const std::string& data) = 0;
    virtual bool deleteRecord(const std::string& table, const std::string& condition) = 0;
};

// Real database implementation
class RealDatabase : public DatabaseAccess {
public:
    std::string query(const std::string& sql) override {
        std::cout << "Executing query: " << sql << std::endl;
        return "Query result for: " + sql;
    }

    bool insert(const std::string& table, const std::string& data) override {
        std::cout << "Inserting into " << table << ": " << data << std::endl;
        return true;
    }

    bool deleteRecord(const std::string& table, const std::string& condition) override {
        std::cout << "Deleting from " << table << " where " << condition << std::endl;
        return true;
    }
};

// Protection Proxy with access control
class DatabaseProxy : public DatabaseAccess {
private:
    std::unique_ptr<RealDatabase> realDatabase;
    std::string userRole;

    bool hasPermission(const std::string& operation) const {
        if (userRole == "admin") return true;
        if (userRole == "user" && operation == "query") return true;
        if (userRole == "user" && operation == "insert") return true;
        return false;
    }

public:
    DatabaseProxy(const std::string& userRole) : userRole(userRole) {
        realDatabase = std::make_unique<RealDatabase>();
    }

    std::string query(const std::string& sql) override {
        if (hasPermission("query")) {
            return realDatabase->query(sql);
        } else {
            std::cout << "Access denied: Insufficient permissions for query" << std::endl;
            return "Access denied";
        }
    }

    bool insert(const std::string& table, const std::string& data) override {
        if (hasPermission("insert")) {
            return realDatabase->insert(table, data);
        } else {
            std::cout << "Access denied: Insufficient permissions for insert" << std::endl;
            return false;
        }
    }

    bool deleteRecord(const std::string& table, const std::string& condition) override {
        if (hasPermission("delete")) {
            return realDatabase->deleteRecord(table, condition);
        } else {
            std::cout << "Access denied: Insufficient permissions for delete" << std::endl;
            return false;
        }
    }
};
```

### 3. Smart Proxy (참조 카운팅과 추가 기능)

```cpp
#include <unordered_map>

class NetworkService {
public:
    virtual ~NetworkService() = default;
    virtual std::string fetchData(const std::string& url) = 0;
};

class RealNetworkService : public NetworkService {
public:
    std::string fetchData(const std::string& url) override {
        std::cout << "Fetching data from: " << url << std::endl;
        // Simulate network delay
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return "Data from " + url;
    }
};

// Smart Proxy with caching and logging
class SmartNetworkProxy : public NetworkService {
private:
    std::unique_ptr<RealNetworkService> realService;
    mutable std::unordered_map<std::string, std::string> cache;
    mutable std::unordered_map<std::string, int> accessCount;

public:
    SmartNetworkProxy() : realService(std::make_unique<RealNetworkService>()) {}

    std::string fetchData(const std::string& url) override {
        // Logging
        accessCount[url]++;
        std::cout << "Proxy: Access count for " << url << ": " << accessCount[url] << std::endl;

        // Check cache first
        auto it = cache.find(url);
        if (it != cache.end()) {
            std::cout << "Proxy: Returning cached data for " << url << std::endl;
            return it->second;
        }

        // Fetch from real service and cache
        std::string data = realService->fetchData(url);
        cache[url] = data;
        std::cout << "Proxy: Data cached for " << url << std::endl;

        return data;
    }

    void clearCache() {
        cache.clear();
        std::cout << "Proxy: Cache cleared" << std::endl;
    }

    void printStatistics() const {
        std::cout << "=== Proxy Statistics ===" << std::endl;
        std::cout << "Cached URLs: " << cache.size() << std::endl;
        std::cout << "Access counts:" << std::endl;
        for (const auto& [url, count] : accessCount) {
            std::cout << "  " << url << ": " << count << " accesses" << std::endl;
        }
    }
};
```

## 사용 예제

```cpp
int main() {
    // 1. Virtual Proxy (지연 로딩)
    std::cout << "=== Virtual Proxy Demo ===" << std::endl;
    std::vector<std::unique_ptr<Image>> images;

    // 프록시 생성 (실제 로딩은 지연됨)
    images.push_back(std::make_unique<ImageProxy>("photo1.jpg"));
    images.push_back(std::make_unique<ImageProxy>("photo2.png"));

    std::cout << "Proxy objects created (no real loading yet)" << std::endl;

    // 첫 번째 접근 시 실제 로딩 발생
    images[0]->display();
    images[1]->display();

    // 2. Protection Proxy (접근 제어)
    std::cout << "\\n=== Protection Proxy Demo ===" << std::endl;

    auto adminDB = std::make_unique<DatabaseProxy>("admin");
    auto userDB = std::make_unique<DatabaseProxy>("user");

    // Admin can do everything
    adminDB->query("SELECT * FROM users");
    adminDB->insert("users", "new_user_data");
    adminDB->deleteRecord("users", "id=123");

    // User has limited access
    userDB->query("SELECT * FROM products");
    userDB->insert("orders", "order_data");
    userDB->deleteRecord("users", "id=456"); // This will be denied

    // 3. Smart Proxy (캐싱과 로깅)
    std::cout << "\\n=== Smart Proxy Demo ===" << std::endl;

    auto networkProxy = std::make_unique<SmartNetworkProxy>();

    // 첫 번째 요청 - 실제 네트워크 호출
    std::string data1 = networkProxy->fetchData("http://api.example.com/data");

    // 두 번째 요청 - 캐시에서 반환
    std::string data2 = networkProxy->fetchData("http://api.example.com/data");

    // 다른 URL 요청
    std::string data3 = networkProxy->fetchData("http://api.example.com/other");

    networkProxy->printStatistics();

    return 0;
}
```

## C++의 장점

1. **스마트 포인터**: 자동 메모리 관리와 안전한 참조
2. **mutable 키워드**: const 메서드에서도 필요시 상태 변경 가능
3. **RAII**: 자동 리소스 관리
4. **템플릿**: 제네릭 프록시 구현 가능

## Proxy 종류

1. **Virtual Proxy**: 비용이 큰 객체의 지연 생성
2. **Protection Proxy**: 접근 권한 제어
3. **Remote Proxy**: 원격 객체에 대한 로컬 대리자
4. **Smart Reference**: 참조 카운팅, 로깅, 캐싱

## 적용 상황

1. **지연 로딩**: 큰 이미지, 문서 파일
2. **접근 제어**: 데이터베이스, 파일 시스템
3. **캐싱**: 웹 서비스, API 응답
4. **로깅**: 메서드 호출 추적
5. **참조 카운팅**: 메모리 관리

Proxy 패턴은 객체에 대한 간접적 접근을 통해 다양한 부가 기능을 제공하는 유용한 패턴입니다.