#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Subject interface
typedef struct {
    void (*display)(void* self, const char* filename);
    void (*destroy)(void* self);
} ImageInterface;

// Real subject - Image
typedef struct {
    ImageInterface interface;
    char* filename;
    bool is_loaded;
} Image;

void image_display(void* self, const char* filename) {
    Image* img = (Image*)self;
    if (!img->is_loaded) {
        printf("Loading image from disk: %s\n", filename);
        // Simulate expensive loading operation
        for (int i = 0; i < 1000000; i++); // Simulate delay
        img->is_loaded = true;
    }
    printf("Displaying image: %s\n", filename);
}

void image_destroy(void* self) {
    Image* img = (Image*)self;
    if (img->filename) {
        free(img->filename);
    }
    free(img);
}

Image* image_create(const char* filename) {
    Image* img = malloc(sizeof(Image));
    img->interface.display = image_display;
    img->interface.destroy = image_destroy;
    img->filename = malloc(strlen(filename) + 1);
    strcpy(img->filename, filename);
    img->is_loaded = false;
    return img;
}

// Proxy - Image Proxy
typedef struct {
    ImageInterface interface;
    char* filename;
    Image* real_image;
    bool access_granted;
    char* user_role;
} ImageProxy;

void proxy_display(void* self, const char* filename) {
    ImageProxy* proxy = (ImageProxy*)self;

    // Access control
    if (strcmp(proxy->user_role, "admin") != 0 && strcmp(proxy->user_role, "user") != 0) {
        printf("Access denied: Invalid user role '%s'\n", proxy->user_role);
        return;
    }

    // Logging
    printf("Proxy: Logging access attempt by %s for %s\n", proxy->user_role, filename);

    // Lazy initialization
    if (!proxy->real_image) {
        printf("Proxy: Creating real image object\n");
        proxy->real_image = image_create(filename);
    }

    // Caching check
    if (proxy->real_image->is_loaded) {
        printf("Proxy: Image already cached, serving from cache\n");
    }

    // Delegate to real object
    proxy->real_image->interface.display(proxy->real_image, filename);
}

void proxy_destroy(void* self) {
    ImageProxy* proxy = (ImageProxy*)self;
    if (proxy->filename) {
        free(proxy->filename);
    }
    if (proxy->user_role) {
        free(proxy->user_role);
    }
    if (proxy->real_image) {
        proxy->real_image->interface.destroy(proxy->real_image);
    }
    free(proxy);
}

ImageProxy* image_proxy_create(const char* filename, const char* user_role) {
    ImageProxy* proxy = malloc(sizeof(ImageProxy));
    proxy->interface.display = proxy_display;
    proxy->interface.destroy = proxy_destroy;

    proxy->filename = malloc(strlen(filename) + 1);
    strcpy(proxy->filename, filename);

    proxy->user_role = malloc(strlen(user_role) + 1);
    strcpy(proxy->user_role, user_role);

    proxy->real_image = NULL; // Lazy initialization
    proxy->access_granted = true;

    return proxy;
}

// Client code
int main() {
    printf("=== Proxy Pattern Demo ===\n\n");

    // Create proxies for different users
    ImageProxy* admin_proxy = image_proxy_create("vacation.jpg", "admin");
    ImageProxy* user_proxy = image_proxy_create("document.pdf", "user");
    ImageProxy* guest_proxy = image_proxy_create("secret.jpg", "guest");

    printf("1. Admin accessing image:\n");
    admin_proxy->interface.display(admin_proxy, "vacation.jpg");
    printf("\n");

    printf("2. Admin accessing same image again (cached):\n");
    admin_proxy->interface.display(admin_proxy, "vacation.jpg");
    printf("\n");

    printf("3. Regular user accessing image:\n");
    user_proxy->interface.display(user_proxy, "document.pdf");
    printf("\n");

    printf("4. Guest trying to access image (access denied):\n");
    guest_proxy->interface.display(guest_proxy, "secret.jpg");
    printf("\n");

    // Cleanup
    admin_proxy->interface.destroy(admin_proxy);
    user_proxy->interface.destroy(user_proxy);
    guest_proxy->interface.destroy(guest_proxy);

    return 0;
}