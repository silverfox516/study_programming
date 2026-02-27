#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>

// Domain Entity
class Product {
private:
    int id_;
    std::string name_;
    double price_;
    int stock_;

public:
    Product() : id_(0), price_(0.0), stock_(0) {}

    Product(int id, std::string name, double price, int stock)
        : id_(id), name_(std::move(name)), price_(price), stock_(stock) {
        if (price < 0) throw std::invalid_argument("Price cannot be negative");
        if (stock < 0) throw std::invalid_argument("Stock cannot be negative");
    }

    // Getters
    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    double getPrice() const { return price_; }
    int getStock() const { return stock_; }

    // Setters
    void setName(const std::string& name) { name_ = name; }
    void setPrice(double price) {
        if (price < 0) throw std::invalid_argument("Price cannot be negative");
        price_ = price;
    }
    void setStock(int stock) {
        if (stock < 0) throw std::invalid_argument("Stock cannot be negative");
        stock_ = stock;
    }

    // For debugging
    std::string toString() const {
        return "Product{id=" + std::to_string(id_) + ", name='" + name_ +
               "', price=" + std::to_string(price_) + ", stock=" + std::to_string(stock_) + "}";
    }
};

// Repository Interface
template<typename T, typename IdType>
class Repository {
public:
    virtual ~Repository() = default;

    virtual bool save(const T& entity) = 0;
    virtual std::optional<T> findById(const IdType& id) const = 0;
    virtual std::vector<T> findAll() const = 0;
    virtual bool update(const T& entity) = 0;
    virtual bool deleteById(const IdType& id) = 0;
    virtual size_t count() const = 0;
};

// Product-specific repository interface
class ProductRepository : public Repository<Product, int> {
public:
    virtual std::vector<Product> findByName(const std::string& name) const = 0;
    virtual std::vector<Product> findByPriceRange(double minPrice, double maxPrice) const = 0;
    virtual std::vector<Product> findByStock(int minStock) const = 0;
};

// In-Memory Implementation
class InMemoryProductRepository : public ProductRepository {
private:
    std::unordered_map<int, Product> products_;

public:
    bool save(const Product& product) override {
        products_[product.getId()] = product;
        return true;
    }

    std::optional<Product> findById(const int& id) const override {
        auto it = products_.find(id);
        if (it != products_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<Product> findAll() const override {
        std::vector<Product> result;
        result.reserve(products_.size());

        for (const auto& [id, product] : products_) {
            result.push_back(product);
        }

        // Sort by ID for consistent output
        std::sort(result.begin(), result.end(),
                 [](const Product& a, const Product& b) {
                     return a.getId() < b.getId();
                 });

        return result;
    }

    bool update(const Product& product) override {
        auto it = products_.find(product.getId());
        if (it != products_.end()) {
            it->second = product;
            return true;
        }
        return false;
    }

    bool deleteById(const int& id) override {
        return products_.erase(id) > 0;
    }

    size_t count() const override {
        return products_.size();
    }

    std::vector<Product> findByName(const std::string& name) const override {
        std::vector<Product> result;
        for (const auto& [id, product] : products_) {
            if (product.getName().find(name) != std::string::npos) {
                result.push_back(product);
            }
        }
        return result;
    }

    std::vector<Product> findByPriceRange(double minPrice, double maxPrice) const override {
        std::vector<Product> result;
        for (const auto& [id, product] : products_) {
            if (product.getPrice() >= minPrice && product.getPrice() <= maxPrice) {
                result.push_back(product);
            }
        }
        return result;
    }

    std::vector<Product> findByStock(int minStock) const override {
        std::vector<Product> result;
        for (const auto& [id, product] : products_) {
            if (product.getStock() >= minStock) {
                result.push_back(product);
            }
        }
        return result;
    }
};

// File-based Implementation
class FileProductRepository : public ProductRepository {
private:
    std::string filename_;
    mutable std::unordered_map<int, Product> cache_;
    mutable bool cacheValid_ = false;

    void loadCache() const {
        if (cacheValid_) return;

        cache_.clear();
        std::ifstream file(filename_);
        if (!file.is_open()) {
            cacheValid_ = true;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(iss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() == 4) {
                try {
                    int id = std::stoi(tokens[0]);
                    std::string name = tokens[1];
                    double price = std::stod(tokens[2]);
                    int stock = std::stoi(tokens[3]);

                    cache_[id] = Product(id, name, price, stock);
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing line: " << line << " - " << e.what() << std::endl;
                }
            }
        }
        cacheValid_ = true;
    }

    void saveCache() const {
        std::ofstream file(filename_, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename_);
        }

        for (const auto& [id, product] : cache_) {
            file << product.getId() << ","
                 << product.getName() << ","
                 << product.getPrice() << ","
                 << product.getStock() << std::endl;
        }
    }

    void invalidateCache() const {
        cacheValid_ = false;
    }

public:
    explicit FileProductRepository(const std::string& filename)
        : filename_(filename) {}

    bool save(const Product& product) override {
        loadCache();
        cache_[product.getId()] = product;
        saveCache();
        return true;
    }

    std::optional<Product> findById(const int& id) const override {
        loadCache();
        auto it = cache_.find(id);
        if (it != cache_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<Product> findAll() const override {
        loadCache();
        std::vector<Product> result;
        result.reserve(cache_.size());

        for (const auto& [id, product] : cache_) {
            result.push_back(product);
        }

        std::sort(result.begin(), result.end(),
                 [](const Product& a, const Product& b) {
                     return a.getId() < b.getId();
                 });

        return result;
    }

    bool update(const Product& product) override {
        loadCache();
        auto it = cache_.find(product.getId());
        if (it != cache_.end()) {
            it->second = product;
            saveCache();
            return true;
        }
        return false;
    }

    bool deleteById(const int& id) override {
        loadCache();
        if (cache_.erase(id) > 0) {
            saveCache();
            return true;
        }
        return false;
    }

    size_t count() const override {
        loadCache();
        return cache_.size();
    }

    std::vector<Product> findByName(const std::string& name) const override {
        loadCache();
        std::vector<Product> result;
        for (const auto& [id, product] : cache_) {
            if (product.getName().find(name) != std::string::npos) {
                result.push_back(product);
            }
        }
        return result;
    }

    std::vector<Product> findByPriceRange(double minPrice, double maxPrice) const override {
        loadCache();
        std::vector<Product> result;
        for (const auto& [id, product] : cache_) {
            if (product.getPrice() >= minPrice && product.getPrice() <= maxPrice) {
                result.push_back(product);
            }
        }
        return result;
    }

    std::vector<Product> findByStock(int minStock) const override {
        loadCache();
        std::vector<Product> result;
        for (const auto& [id, product] : cache_) {
            if (product.getStock() >= minStock) {
                result.push_back(product);
            }
        }
        return result;
    }
};

// Service Layer
class ProductService {
private:
    std::unique_ptr<ProductRepository> repository_;

public:
    explicit ProductService(std::unique_ptr<ProductRepository> repository)
        : repository_(std::move(repository)) {}

    bool addProduct(int id, const std::string& name, double price, int stock) {
        try {
            Product product(id, name, price, stock);
            return repository_->save(product);
        } catch (const std::exception& e) {
            std::cerr << "Error adding product: " << e.what() << std::endl;
            return false;
        }
    }

    std::optional<Product> getProduct(int id) {
        return repository_->findById(id);
    }

    std::vector<Product> getAllProducts() {
        return repository_->findAll();
    }

    bool updateProduct(int id, const std::string& name, double price, int stock) {
        try {
            Product product(id, name, price, stock);
            return repository_->update(product);
        } catch (const std::exception& e) {
            std::cerr << "Error updating product: " << e.what() << std::endl;
            return false;
        }
    }

    bool removeProduct(int id) {
        return repository_->deleteById(id);
    }

    std::vector<Product> searchByName(const std::string& name) {
        return repository_->findByName(name);
    }

    std::vector<Product> getProductsInPriceRange(double minPrice, double maxPrice) {
        return repository_->findByPriceRange(minPrice, maxPrice);
    }

    std::vector<Product> getAvailableProducts(int minStock = 1) {
        return repository_->findByStock(minStock);
    }

    size_t getProductCount() {
        return repository_->count();
    }

    void printAllProducts() {
        auto products = getAllProducts();
        if (products.empty()) {
            std::cout << "No products found.\n";
            return;
        }

        std::cout << "All Products (" << products.size() << "):\n";
        std::cout << "ID\tName\t\tPrice\tStock\n";
        std::cout << "----------------------------------------\n";

        for (const auto& product : products) {
            std::cout << product.getId() << "\t"
                      << product.getName() << "\t\t$"
                      << product.getPrice() << "\t"
                      << product.getStock() << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Repository Pattern Demo ===\n\n";

    try {
        // Create in-memory repository
        std::cout << "1. In-Memory Repository Demo:\n";
        auto memoryRepo = std::make_unique<InMemoryProductRepository>();
        auto service = std::make_unique<ProductService>(std::move(memoryRepo));

        // Add products
        service->addProduct(1, "Laptop", 999.99, 10);
        service->addProduct(2, "Mouse", 25.50, 50);
        service->addProduct(3, "Keyboard", 75.00, 25);
        service->addProduct(4, "Monitor", 299.99, 15);

        std::cout << "\nProducts added. Current inventory:\n";
        service->printAllProducts();

        // Find specific product
        std::cout << "\nFinding product with ID 2:\n";
        auto product = service->getProduct(2);
        if (product) {
            std::cout << "Found: " << product->toString() << std::endl;
        } else {
            std::cout << "Product not found\n";
        }

        // Search by name
        std::cout << "\nSearching for products containing 'Key':\n";
        auto keyboardProducts = service->searchByName("Key");
        for (const auto& p : keyboardProducts) {
            std::cout << "Found: " << p.toString() << std::endl;
        }

        // Find products in price range
        std::cout << "\nProducts between $20 and $100:\n";
        auto affordableProducts = service->getProductsInPriceRange(20.0, 100.0);
        for (const auto& p : affordableProducts) {
            std::cout << p.toString() << std::endl;
        }

        // Update product
        std::cout << "\nUpdating product ID 1:\n";
        if (service->updateProduct(1, "Gaming Laptop", 1299.99, 8)) {
            std::cout << "Product updated successfully\n";
            auto updatedProduct = service->getProduct(1);
            if (updatedProduct) {
                std::cout << "Updated: " << updatedProduct->toString() << std::endl;
            }
        }

        // Delete product
        std::cout << "\nDeleting product ID 2:\n";
        if (service->removeProduct(2)) {
            std::cout << "Product deleted successfully\n";
            std::cout << "Remaining products:\n";
            service->printAllProducts();
        }

        std::cout << "\n2. File Repository Demo:\n";
        // Create file repository
        auto fileRepo = std::make_unique<FileProductRepository>("products.txt");
        auto fileService = std::make_unique<ProductService>(std::move(fileRepo));

        // Add products to file repository
        fileService->addProduct(100, "File Product 1", 49.99, 20);
        fileService->addProduct(101, "File Product 2", 89.99, 15);

        std::cout << "\nProducts saved to file:\n";
        fileService->printAllProducts();

        // Test persistence by creating new instance
        auto fileRepo2 = std::make_unique<FileProductRepository>("products.txt");
        auto fileService2 = std::make_unique<ProductService>(std::move(fileRepo2));

        std::cout << "\nLoading from file (new repository instance):\n";
        fileService2->printAllProducts();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}