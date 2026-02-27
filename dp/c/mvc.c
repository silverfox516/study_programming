#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 100
#define MAX_EMAIL_LEN 100

// Model
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char email[MAX_EMAIL_LEN];
} User;

typedef struct {
    User* users;
    int count;
    int capacity;
} UserModel;

UserModel* user_model_create() {
    UserModel* model = malloc(sizeof(UserModel));
    if (!model) return NULL;

    model->capacity = 10;
    model->users = malloc(sizeof(User) * model->capacity);
    if (!model->users) {
        free(model);
        return NULL;
    }
    model->count = 0;
    return model;
}

void user_model_destroy(UserModel* model) {
    if (model) {
        free(model->users);
        free(model);
    }
}

int user_model_add(UserModel* model, int id, const char* name, const char* email) {
    if (!model || !name || !email) return 0;
    if (model->count >= model->capacity) return 0;

    User* user = &model->users[model->count];
    user->id = id;
    strncpy(user->name, name, MAX_NAME_LEN - 1);
    user->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(user->email, email, MAX_EMAIL_LEN - 1);
    user->email[MAX_EMAIL_LEN - 1] = '\0';

    model->count++;
    return 1;
}

User* user_model_get(UserModel* model, int id) {
    if (!model) return NULL;

    for (int i = 0; i < model->count; i++) {
        if (model->users[i].id == id) {
            return &model->users[i];
        }
    }
    return NULL;
}

// View
typedef struct {
    void (*display_user)(const User* user);
    void (*display_all_users)(const User* users, int count);
    void (*display_message)(const char* message);
} UserView;

void view_display_user(const User* user) {
    if (!user) {
        printf("User not found\n");
        return;
    }
    printf("User ID: %d\n", user->id);
    printf("Name: %s\n", user->name);
    printf("Email: %s\n", user->email);
    printf("---\n");
}

void view_display_all_users(const User* users, int count) {
    printf("=== All Users ===\n");
    for (int i = 0; i < count; i++) {
        view_display_user(&users[i]);
    }
}

void view_display_message(const char* message) {
    printf("Message: %s\n", message);
}

UserView* user_view_create() {
    UserView* view = malloc(sizeof(UserView));
    if (!view) return NULL;

    view->display_user = view_display_user;
    view->display_all_users = view_display_all_users;
    view->display_message = view_display_message;

    return view;
}

void user_view_destroy(UserView* view) {
    free(view);
}

// Controller
typedef struct {
    UserModel* model;
    UserView* view;
} UserController;

UserController* user_controller_create(UserModel* model, UserView* view) {
    if (!model || !view) return NULL;

    UserController* controller = malloc(sizeof(UserController));
    if (!controller) return NULL;

    controller->model = model;
    controller->view = view;
    return controller;
}

void user_controller_destroy(UserController* controller) {
    free(controller);
}

void user_controller_add_user(UserController* controller, int id, const char* name, const char* email) {
    if (!controller) return;

    if (user_model_add(controller->model, id, name, email)) {
        controller->view->display_message("User added successfully");
    } else {
        controller->view->display_message("Failed to add user");
    }
}

void user_controller_show_user(UserController* controller, int id) {
    if (!controller) return;

    User* user = user_model_get(controller->model, id);
    controller->view->display_user(user);
}

void user_controller_show_all_users(UserController* controller) {
    if (!controller) return;

    controller->view->display_all_users(controller->model->users, controller->model->count);
}

int main() {
    printf("=== MVC Pattern Demo ===\n");

    // Create MVC components
    UserModel* model = user_model_create();
    UserView* view = user_view_create();
    UserController* controller = user_controller_create(model, view);

    if (!model || !view || !controller) {
        printf("Failed to create MVC components\n");
        return 1;
    }

    // Use controller to manage users
    user_controller_add_user(controller, 1, "Alice Johnson", "alice@example.com");
    user_controller_add_user(controller, 2, "Bob Smith", "bob@example.com");
    user_controller_add_user(controller, 3, "Charlie Brown", "charlie@example.com");

    // Display specific user
    printf("\nShowing user with ID 2:\n");
    user_controller_show_user(controller, 2);

    // Display all users
    printf("\nShowing all users:\n");
    user_controller_show_all_users(controller);

    // Try to show non-existent user
    printf("\nTrying to show user with ID 999:\n");
    user_controller_show_user(controller, 999);

    // Cleanup
    user_controller_destroy(controller);
    user_view_destroy(view);
    user_model_destroy(model);

    return 0;
}