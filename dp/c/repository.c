#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 100
#define MAX_PRODUCTS 100

// Domain Entity
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    double price;
    int stock;
} Product;

// Repository interface (using function pointers)
typedef struct Repository {
    void* impl; // Implementation-specific data

    // CRUD operations
    int (*save)(struct Repository* repo, const Product* product);
    Product* (*find_by_id)(struct Repository* repo, int id);
    Product** (*find_all)(struct Repository* repo, int* count);
    Product** (*find_by_name)(struct Repository* repo, const char* name, int* count);
    int (*update)(struct Repository* repo, const Product* product);
    int (*delete_by_id)(struct Repository* repo, int id);
    void (*destroy)(struct Repository* repo);
} Repository;

// In-Memory Repository Implementation
typedef struct {
    Product products[MAX_PRODUCTS];
    int count;
} InMemoryData;

int memory_save(Repository* repo, const Product* product) {
    InMemoryData* data = (InMemoryData*)repo->impl;

    if (!product || data->count >= MAX_PRODUCTS) {
        return 0; // Failed
    }

    // Check if product already exists (update case)
    for (int i = 0; i < data->count; i++) {
        if (data->products[i].id == product->id) {
            data->products[i] = *product;
            return 1; // Updated
        }
    }

    // Add new product
    data->products[data->count] = *product;
    data->count++;
    return 1; // Added
}

Product* memory_find_by_id(Repository* repo, int id) {
    InMemoryData* data = (InMemoryData*)repo->impl;

    for (int i = 0; i < data->count; i++) {
        if (data->products[i].id == id) {
            return &data->products[i];
        }
    }
    return NULL;
}

Product** memory_find_all(Repository* repo, int* count) {
    InMemoryData* data = (InMemoryData*)repo->impl;

    if (data->count == 0) {
        *count = 0;
        return NULL;
    }

    Product** result = malloc(sizeof(Product*) * data->count);
    if (!result) {
        *count = 0;
        return NULL;
    }

    for (int i = 0; i < data->count; i++) {
        result[i] = &data->products[i];
    }

    *count = data->count;
    return result;
}

Product** memory_find_by_name(Repository* repo, const char* name, int* count) {
    InMemoryData* data = (InMemoryData*)repo->impl;
    Product** result = malloc(sizeof(Product*) * data->count);
    int found = 0;

    if (!result || !name) {
        *count = 0;
        return NULL;
    }

    for (int i = 0; i < data->count; i++) {
        if (strstr(data->products[i].name, name) != NULL) {
            result[found++] = &data->products[i];
        }
    }

    if (found == 0) {
        free(result);
        result = NULL;
    }

    *count = found;
    return result;
}

int memory_update(Repository* repo, const Product* product) {
    return memory_save(repo, product); // Same as save for in-memory
}

int memory_delete_by_id(Repository* repo, int id) {
    InMemoryData* data = (InMemoryData*)repo->impl;

    for (int i = 0; i < data->count; i++) {
        if (data->products[i].id == id) {
            // Shift remaining products
            for (int j = i; j < data->count - 1; j++) {
                data->products[j] = data->products[j + 1];
            }
            data->count--;
            return 1; // Success
        }
    }
    return 0; // Not found
}

void memory_destroy(Repository* repo) {
    if (repo && repo->impl) {
        free(repo->impl);
        free(repo);
    }
}

Repository* create_memory_repository() {
    Repository* repo = malloc(sizeof(Repository));
    if (!repo) return NULL;

    InMemoryData* data = malloc(sizeof(InMemoryData));
    if (!data) {
        free(repo);
        return NULL;
    }

    data->count = 0;
    repo->impl = data;

    // Set function pointers
    repo->save = memory_save;
    repo->find_by_id = memory_find_by_id;
    repo->find_all = memory_find_all;
    repo->find_by_name = memory_find_by_name;
    repo->update = memory_update;
    repo->delete_by_id = memory_delete_by_id;
    repo->destroy = memory_destroy;

    return repo;
}

// File Repository Implementation (simulated)
typedef struct {
    char filename[256];
} FileData;

int file_save(Repository* repo, const Product* product) {
    FileData* data = (FileData*)repo->impl;
    FILE* file = fopen(data->filename, "a");

    if (!file || !product) return 0;

    fprintf(file, "%d,%s,%.2f,%d\n",
            product->id, product->name, product->price, product->stock);
    fclose(file);
    return 1;
}

Product* file_find_by_id(Repository* repo, int id) {
    // Simplified implementation - in reality would parse file
    printf("File repository: searching for product ID %d\n", id);
    return NULL; // Not implemented for demo
}

Product** file_find_all(Repository* repo, int* count) {
    printf("File repository: finding all products\n");
    *count = 0;
    return NULL; // Not implemented for demo
}

Product** file_find_by_name(Repository* repo, const char* name, int* count) {
    printf("File repository: searching for products with name '%s'\n", name);
    *count = 0;
    return NULL; // Not implemented for demo
}

int file_update(Repository* repo, const Product* product) {
    printf("File repository: updating product ID %d\n", product ? product->id : -1);
    return 0; // Not implemented for demo
}

int file_delete_by_id(Repository* repo, int id) {
    printf("File repository: deleting product ID %d\n", id);
    return 0; // Not implemented for demo
}

void file_destroy(Repository* repo) {
    if (repo && repo->impl) {
        free(repo->impl);
        free(repo);
    }
}

Repository* create_file_repository(const char* filename) {
    Repository* repo = malloc(sizeof(Repository));
    if (!repo) return NULL;

    FileData* data = malloc(sizeof(FileData));
    if (!data) {
        free(repo);
        return NULL;
    }

    strncpy(data->filename, filename, sizeof(data->filename) - 1);
    data->filename[sizeof(data->filename) - 1] = '\0';
    repo->impl = data;

    // Set function pointers
    repo->save = file_save;
    repo->find_by_id = file_find_by_id;
    repo->find_all = file_find_all;
    repo->find_by_name = file_find_by_name;
    repo->update = file_update;
    repo->delete_by_id = file_delete_by_id;
    repo->destroy = file_destroy;

    return repo;
}

// Service layer using repository
typedef struct {
    Repository* repository;
} ProductService;

ProductService* product_service_create(Repository* repository) {
    if (!repository) return NULL;

    ProductService* service = malloc(sizeof(ProductService));
    if (!service) return NULL;

    service->repository = repository;
    return service;
}

void product_service_destroy(ProductService* service) {
    free(service);
}

int product_service_add_product(ProductService* service, int id, const char* name, double price, int stock) {
    if (!service || !name || price < 0 || stock < 0) {
        return 0;
    }

    Product product = { id, "", price, stock };
    strncpy(product.name, name, MAX_NAME_LEN - 1);
    product.name[MAX_NAME_LEN - 1] = '\0';

    return service->repository->save(service->repository, &product);
}

void product_service_list_all_products(ProductService* service) {
    if (!service) return;

    int count;
    Product** products = service->repository->find_all(service->repository, &count);

    if (count == 0) {
        printf("No products found\n");
        return;
    }

    printf("All Products (%d):\n", count);
    printf("ID\tName\t\tPrice\tStock\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%d\t%-15s\t$%.2f\t%d\n",
               products[i]->id, products[i]->name,
               products[i]->price, products[i]->stock);
    }

    free(products);
}

int main() {
    printf("=== Repository Pattern Demo ===\n");

    // Create in-memory repository
    Repository* memRepo = create_memory_repository();
    if (!memRepo) {
        printf("Failed to create memory repository\n");
        return 1;
    }

    // Create product service
    ProductService* service = product_service_create(memRepo);
    if (!service) {
        printf("Failed to create product service\n");
        memRepo->destroy(memRepo);
        return 1;
    }

    // Add products
    printf("\nAdding products to memory repository:\n");
    product_service_add_product(service, 1, "Laptop", 999.99, 10);
    product_service_add_product(service, 2, "Mouse", 25.50, 50);
    product_service_add_product(service, 3, "Keyboard", 75.00, 25);

    // List all products
    printf("\n");
    product_service_list_all_products(service);

    // Find specific product
    printf("\nFinding product with ID 2:\n");
    Product* product = memRepo->find_by_id(memRepo, 2);
    if (product) {
        printf("Found: %s - $%.2f (Stock: %d)\n",
               product->name, product->price, product->stock);
    } else {
        printf("Product not found\n");
    }

    // Search by name
    printf("\nSearching for products containing 'Key':\n");
    int searchCount;
    Product** searchResults = memRepo->find_by_name(memRepo, "Key", &searchCount);
    if (searchCount > 0) {
        for (int i = 0; i < searchCount; i++) {
            printf("Found: %s - $%.2f\n",
                   searchResults[i]->name, searchResults[i]->price);
        }
        free(searchResults);
    } else {
        printf("No products found\n");
    }

    // Update product
    printf("\nUpdating product ID 1:\n");
    Product updatedProduct = { 1, "Gaming Laptop", 1299.99, 8 };
    if (memRepo->update(memRepo, &updatedProduct)) {
        printf("Product updated successfully\n");
        product_service_list_all_products(service);
    }

    // Delete product
    printf("\nDeleting product ID 2:\n");
    if (memRepo->delete_by_id(memRepo, 2)) {
        printf("Product deleted successfully\n");
        product_service_list_all_products(service);
    }

    // Demonstrate file repository (simplified)
    printf("\n=== File Repository Demo ===\n");
    Repository* fileRepo = create_file_repository("products.txt");
    if (fileRepo) {
        Product testProduct = { 100, "Test Product", 19.99, 5 };
        fileRepo->save(fileRepo, &testProduct);
        fileRepo->find_by_id(fileRepo, 100);
        fileRepo->destroy(fileRepo);
    }

    // Cleanup
    product_service_destroy(service);
    memRepo->destroy(memRepo);

    return 0;
}