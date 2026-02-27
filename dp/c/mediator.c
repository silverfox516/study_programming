#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Forward declarations
typedef struct Mediator Mediator;
typedef struct Component Component;

// Component types
typedef enum {
    COMPONENT_BUTTON,
    COMPONENT_TEXTBOX,
    COMPONENT_CHECKBOX,
    COMPONENT_LISTBOX
} ComponentType;

// Mediator interface
typedef struct Mediator {
    void (*notify)(struct Mediator* self, Component* sender, const char* event);
    void (*destroy)(struct Mediator* self);
} Mediator;

// Base Component
typedef struct Component {
    ComponentType type;
    char* name;
    Mediator* mediator;
    bool enabled;
    char* text;

    void (*click)(struct Component* self);
    void (*set_text)(struct Component* self, const char* text);
    void (*set_enabled)(struct Component* self, bool enabled);
    void (*destroy)(struct Component* self);
} Component;

// Base component functions
void component_set_mediator(Component* component, Mediator* mediator) {
    component->mediator = mediator;
}

void component_notify_mediator(Component* component, const char* event) {
    if (component->mediator) {
        component->mediator->notify(component->mediator, component, event);
    }
}

// Button Component
typedef struct {
    Component base;
} Button;

void button_click(Component* self) {
    printf("Button '%s' clicked\n", self->name);
    component_notify_mediator(self, "click");
}

void button_set_text(Component* self, const char* text) {
    if (self->text) {
        free(self->text);
    }
    self->text = malloc(strlen(text) + 1);
    strcpy(self->text, text);
    printf("Button '%s' text set to: '%s'\n", self->name, text);
}

void button_set_enabled(Component* self, bool enabled) {
    self->enabled = enabled;
    printf("Button '%s' %s\n", self->name, enabled ? "enabled" : "disabled");
}

void button_destroy(Component* self) {
    if (self->name) free(self->name);
    if (self->text) free(self->text);
    free(self);
}

Component* button_create(const char* name) {
    Button* button = malloc(sizeof(Button));
    button->base.type = COMPONENT_BUTTON;
    button->base.name = malloc(strlen(name) + 1);
    strcpy(button->base.name, name);
    button->base.mediator = NULL;
    button->base.enabled = true;
    button->base.text = malloc(strlen("Button") + 1);
    strcpy(button->base.text, "Button");

    button->base.click = button_click;
    button->base.set_text = button_set_text;
    button->base.set_enabled = button_set_enabled;
    button->base.destroy = button_destroy;

    return (Component*)button;
}

// TextBox Component
typedef struct {
    Component base;
} TextBox;

void textbox_click(Component* self) {
    printf("TextBox '%s' focused\n", self->name);
    component_notify_mediator(self, "focus");
}

void textbox_set_text(Component* self, const char* text) {
    if (self->text) {
        free(self->text);
    }
    self->text = malloc(strlen(text) + 1);
    strcpy(self->text, text);
    printf("TextBox '%s' text changed to: '%s'\n", self->name, text);
    component_notify_mediator(self, "text_changed");
}

void textbox_set_enabled(Component* self, bool enabled) {
    self->enabled = enabled;
    printf("TextBox '%s' %s\n", self->name, enabled ? "enabled" : "disabled");
}

void textbox_destroy(Component* self) {
    if (self->name) free(self->name);
    if (self->text) free(self->text);
    free(self);
}

Component* textbox_create(const char* name) {
    TextBox* textbox = malloc(sizeof(TextBox));
    textbox->base.type = COMPONENT_TEXTBOX;
    textbox->base.name = malloc(strlen(name) + 1);
    strcpy(textbox->base.name, name);
    textbox->base.mediator = NULL;
    textbox->base.enabled = true;
    textbox->base.text = malloc(1);
    textbox->base.text[0] = '\0';

    textbox->base.click = textbox_click;
    textbox->base.set_text = textbox_set_text;
    textbox->base.set_enabled = textbox_set_enabled;
    textbox->base.destroy = textbox_destroy;

    return (Component*)textbox;
}

// CheckBox Component
typedef struct {
    Component base;
    bool checked;
} CheckBox;

void checkbox_click(Component* self) {
    CheckBox* checkbox = (CheckBox*)self;
    checkbox->checked = !checkbox->checked;
    printf("CheckBox '%s' %s\n", self->name, checkbox->checked ? "checked" : "unchecked");
    component_notify_mediator(self, checkbox->checked ? "checked" : "unchecked");
}

void checkbox_set_text(Component* self, const char* text) {
    if (self->text) {
        free(self->text);
    }
    self->text = malloc(strlen(text) + 1);
    strcpy(self->text, text);
    printf("CheckBox '%s' label set to: '%s'\n", self->name, text);
}

void checkbox_set_enabled(Component* self, bool enabled) {
    self->enabled = enabled;
    printf("CheckBox '%s' %s\n", self->name, enabled ? "enabled" : "disabled");
}

void checkbox_destroy(Component* self) {
    if (self->name) free(self->name);
    if (self->text) free(self->text);
    free(self);
}

Component* checkbox_create(const char* name) {
    CheckBox* checkbox = malloc(sizeof(CheckBox));
    checkbox->base.type = COMPONENT_CHECKBOX;
    checkbox->base.name = malloc(strlen(name) + 1);
    strcpy(checkbox->base.name, name);
    checkbox->base.mediator = NULL;
    checkbox->base.enabled = true;
    checkbox->base.text = malloc(strlen("CheckBox") + 1);
    strcpy(checkbox->base.text, "CheckBox");
    checkbox->checked = false;

    checkbox->base.click = checkbox_click;
    checkbox->base.set_text = checkbox_set_text;
    checkbox->base.set_enabled = checkbox_set_enabled;
    checkbox->base.destroy = checkbox_destroy;

    return (Component*)checkbox;
}

// ListBox Component
typedef struct {
    Component base;
    char** items;
    int item_count;
    int selected_index;
    int capacity;
} ListBox;

void listbox_click(Component* self) {
    ListBox* listbox = (ListBox*)self;
    printf("ListBox '%s' item selected: %s\n", self->name,
           listbox->selected_index >= 0 ? listbox->items[listbox->selected_index] : "none");
    component_notify_mediator(self, "selection_changed");
}

void listbox_set_text(Component* self, const char* text) {
    // For listbox, this adds an item
    ListBox* listbox = (ListBox*)self;

    if (listbox->item_count >= listbox->capacity) {
        listbox->capacity *= 2;
        listbox->items = realloc(listbox->items, listbox->capacity * sizeof(char*));
    }

    listbox->items[listbox->item_count] = malloc(strlen(text) + 1);
    strcpy(listbox->items[listbox->item_count], text);
    listbox->item_count++;

    printf("ListBox '%s' item added: '%s'\n", self->name, text);
}

void listbox_select_item(ListBox* listbox, int index) {
    if (index >= 0 && index < listbox->item_count) {
        listbox->selected_index = index;
        printf("ListBox '%s' selected item %d: '%s'\n",
               listbox->base.name, index, listbox->items[index]);
        component_notify_mediator((Component*)listbox, "selection_changed");
    }
}

void listbox_set_enabled(Component* self, bool enabled) {
    self->enabled = enabled;
    printf("ListBox '%s' %s\n", self->name, enabled ? "enabled" : "disabled");
}

void listbox_destroy(Component* self) {
    ListBox* listbox = (ListBox*)self;
    for (int i = 0; i < listbox->item_count; i++) {
        free(listbox->items[i]);
    }
    if (listbox->items) free(listbox->items);
    if (self->name) free(self->name);
    if (self->text) free(self->text);
    free(self);
}

Component* listbox_create(const char* name) {
    ListBox* listbox = malloc(sizeof(ListBox));
    listbox->base.type = COMPONENT_LISTBOX;
    listbox->base.name = malloc(strlen(name) + 1);
    strcpy(listbox->base.name, name);
    listbox->base.mediator = NULL;
    listbox->base.enabled = true;
    listbox->base.text = malloc(1);
    listbox->base.text[0] = '\0';

    listbox->base.click = listbox_click;
    listbox->base.set_text = listbox_set_text;
    listbox->base.set_enabled = listbox_set_enabled;
    listbox->base.destroy = listbox_destroy;

    listbox->items = malloc(4 * sizeof(char*));
    listbox->item_count = 0;
    listbox->selected_index = -1;
    listbox->capacity = 4;

    return (Component*)listbox;
}

// Concrete Mediator - Dialog Box
typedef struct {
    Mediator base;
    Component* ok_button;
    Component* cancel_button;
    Component* username_textbox;
    Component* password_textbox;
    Component* remember_checkbox;
    Component* user_list;
    Component* clear_button;
} AuthDialog;

void auth_dialog_notify(Mediator* self, Component* sender, const char* event) {
    AuthDialog* dialog = (AuthDialog*)self;

    printf("Mediator received event '%s' from '%s'\n", event, sender->name);

    if (sender == dialog->username_textbox && strcmp(event, "text_changed") == 0) {
        // Enable OK button only if username is not empty
        bool has_username = strlen(sender->text) > 0;
        dialog->ok_button->set_enabled(dialog->ok_button, has_username);

        if (has_username) {
            dialog->password_textbox->set_enabled(dialog->password_textbox, true);
        }

    } else if (sender == dialog->remember_checkbox && strcmp(event, "checked") == 0) {
        // When remember is checked, enable user list
        dialog->user_list->set_enabled(dialog->user_list, true);
        dialog->clear_button->set_enabled(dialog->clear_button, true);

        // Add current username to list if not empty
        if (strlen(dialog->username_textbox->text) > 0) {
            dialog->user_list->set_text(dialog->user_list, dialog->username_textbox->text);
        }

    } else if (sender == dialog->remember_checkbox && strcmp(event, "unchecked") == 0) {
        // When remember is unchecked, disable user list
        dialog->user_list->set_enabled(dialog->user_list, false);
        dialog->clear_button->set_enabled(dialog->clear_button, false);

    } else if (sender == dialog->user_list && strcmp(event, "selection_changed") == 0) {
        // When user is selected from list, fill username textbox
        ListBox* listbox = (ListBox*)dialog->user_list;
        if (listbox->selected_index >= 0) {
            dialog->username_textbox->set_text(dialog->username_textbox,
                                             listbox->items[listbox->selected_index]);
        }

    } else if (sender == dialog->clear_button && strcmp(event, "click") == 0) {
        // Clear all form fields
        dialog->username_textbox->set_text(dialog->username_textbox, "");
        dialog->password_textbox->set_text(dialog->password_textbox, "");
        CheckBox* checkbox = (CheckBox*)dialog->remember_checkbox;
        if (checkbox->checked) {
            dialog->remember_checkbox->click(dialog->remember_checkbox);
        }

    } else if (sender == dialog->ok_button && strcmp(event, "click") == 0) {
        // Process login
        printf("Processing login for user: %s\n", dialog->username_textbox->text);
        printf("Password length: %zu characters\n", strlen(dialog->password_textbox->text));

    } else if (sender == dialog->cancel_button && strcmp(event, "click") == 0) {
        // Cancel dialog
        printf("Login canceled\n");
    }

    printf("---\n");
}

void auth_dialog_destroy(Mediator* self) {
    AuthDialog* dialog = (AuthDialog*)self;

    dialog->ok_button->destroy(dialog->ok_button);
    dialog->cancel_button->destroy(dialog->cancel_button);
    dialog->username_textbox->destroy(dialog->username_textbox);
    dialog->password_textbox->destroy(dialog->password_textbox);
    dialog->remember_checkbox->destroy(dialog->remember_checkbox);
    dialog->user_list->destroy(dialog->user_list);
    dialog->clear_button->destroy(dialog->clear_button);

    free(dialog);
}

Mediator* auth_dialog_create() {
    AuthDialog* dialog = malloc(sizeof(AuthDialog));
    dialog->base.notify = auth_dialog_notify;
    dialog->base.destroy = auth_dialog_destroy;

    // Create components
    dialog->ok_button = button_create("OK");
    dialog->cancel_button = button_create("Cancel");
    dialog->username_textbox = textbox_create("Username");
    dialog->password_textbox = textbox_create("Password");
    dialog->remember_checkbox = checkbox_create("RememberMe");
    dialog->user_list = listbox_create("UserList");
    dialog->clear_button = button_create("Clear");

    // Set initial states
    dialog->ok_button->set_enabled(dialog->ok_button, false);
    dialog->password_textbox->set_enabled(dialog->password_textbox, false);
    dialog->user_list->set_enabled(dialog->user_list, false);
    dialog->clear_button->set_enabled(dialog->clear_button, false);

    // Set button texts
    dialog->ok_button->set_text(dialog->ok_button, "Login");
    dialog->cancel_button->set_text(dialog->cancel_button, "Cancel");
    dialog->remember_checkbox->set_text(dialog->remember_checkbox, "Remember Me");
    dialog->clear_button->set_text(dialog->clear_button, "Clear Form");

    // Set mediator for all components
    component_set_mediator(dialog->ok_button, (Mediator*)dialog);
    component_set_mediator(dialog->cancel_button, (Mediator*)dialog);
    component_set_mediator(dialog->username_textbox, (Mediator*)dialog);
    component_set_mediator(dialog->password_textbox, (Mediator*)dialog);
    component_set_mediator(dialog->remember_checkbox, (Mediator*)dialog);
    component_set_mediator(dialog->user_list, (Mediator*)dialog);
    component_set_mediator(dialog->clear_button, (Mediator*)dialog);

    return (Mediator*)dialog;
}

// Helper function to get dialog components
AuthDialog* get_auth_dialog(Mediator* mediator) {
    return (AuthDialog*)mediator;
}

// Client code
int main() {
    printf("=== Mediator Pattern Demo - Authentication Dialog ===\n\n");

    // Create the dialog (mediator)
    Mediator* mediator = auth_dialog_create();
    AuthDialog* dialog = get_auth_dialog(mediator);

    printf("=== Initial State ===\n");
    printf("OK button is disabled, password field is disabled\n");
    printf("User list and clear button are disabled\n\n");

    printf("=== User Interaction Simulation ===\n");

    printf("1. User types username:\n");
    dialog->username_textbox->set_text(dialog->username_textbox, "john_doe");

    printf("\n2. User types password:\n");
    dialog->password_textbox->set_text(dialog->password_textbox, "secret123");

    printf("\n3. User checks 'Remember Me':\n");
    dialog->remember_checkbox->click(dialog->remember_checkbox);

    printf("\n4. User types another username:\n");
    dialog->username_textbox->set_text(dialog->username_textbox, "jane_smith");

    printf("\n5. User selects from user list:\n");
    ListBox* listbox = (ListBox*)dialog->user_list;
    listbox_select_item(listbox, 0); // Select first user

    printf("\n6. User clicks Login:\n");
    dialog->ok_button->click(dialog->ok_button);

    printf("\n7. User clicks Clear:\n");
    dialog->clear_button->click(dialog->clear_button);

    printf("\n8. User clicks Cancel:\n");
    dialog->cancel_button->click(dialog->cancel_button);

    // Cleanup
    mediator->destroy(mediator);

    printf("\n=== Mediator Pattern Benefits ===\n");
    printf("1. Loose coupling: Components don't reference each other directly\n");
    printf("2. Centralized control: All interaction logic is in the mediator\n");
    printf("3. Reusable components: Components can be used in different contexts\n");
    printf("4. Easy to maintain: Changes to interaction logic only affect mediator\n");

    return 0;
}