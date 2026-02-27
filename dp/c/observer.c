#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Observer Observer;
typedef struct Subject Subject;

// Observer structure
struct Observer {
    void (*update)(Observer* self, Subject* subject);
    char* name;
    void* context; // User data for more flexibility
};

// Node for Linked List
typedef struct ObserverNode {
    Observer* observer;
    struct ObserverNode* next;
} ObserverNode;

// Subject structure with Linked List
struct Subject {
    ObserverNode* observer_list; // Head of the list
    int state;
};

// Function prototypes
void subject_attach(Subject* subject, Observer* observer);
void subject_detach(Subject* subject, Observer* observer);
void subject_notify(Subject* subject);
void subject_set_state(Subject* subject, int new_state);

// --- Subject Implementation ---

Subject* create_subject() {
    Subject* subject = malloc(sizeof(Subject));
    if (!subject) return NULL;
    subject->observer_list = NULL;
    subject->state = 0; // Default state
    return subject;
}

void destroy_subject(Subject* subject) {
    if (!subject) return;
    
    // Free the linked list nodes (but not the observers themselves, as they might be shared)
    ObserverNode* current = subject->observer_list;
    while (current != NULL) {
        ObserverNode* next = current->next;
        free(current);
        current = next;
    }
    free(subject);
}

void subject_attach(Subject* subject, Observer* observer) {
    if (!subject || !observer) return;

    // Create new node
    ObserverNode* new_node = malloc(sizeof(ObserverNode));
    if (!new_node) return;
    
    new_node->observer = observer;
    new_node->next = subject->observer_list; // Prepend to list (O(1))
    subject->observer_list = new_node;
    
    printf("Observer %s attached\n", observer->name);
}

void subject_detach(Subject* subject, Observer* observer) {
    if (!subject || !observer) return;

    ObserverNode* current = subject->observer_list;
    ObserverNode* prev = NULL;

    while (current != NULL) {
        if (current->observer == observer) {
            if (prev == NULL) {
                // Removing head
                subject->observer_list = current->next;
            } else {
                // Removing from middle/end
                prev->next = current->next;
            }
            free(current);
            printf("Observer %s detached\n", observer->name);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void subject_notify(Subject* subject) {
    printf("Notifying observers about state change to %d\n", subject->state);
    
    ObserverNode* current = subject->observer_list;
    while (current != NULL) {
        if (current->observer && current->observer->update) {
            current->observer->update(current->observer, subject);
        }
        current = current->next;
    }
}

void subject_set_state(Subject* subject, int new_state) {
    if (subject->state != new_state) {
        subject->state = new_state;
        subject_notify(subject);
    }
}

// --- Observer Implementation ---

// Standard update callbacks
void display_observer_update(Observer* self, Subject* subject) {
    printf("Display %s: State updated to %d\n", self->name, subject->state);
}

void alert_observer_update(Observer* self, Subject* subject) {
    if (subject->state > 30) {
        printf("Alert %s: HIGH VALUE WARNING! (%d)\n", self->name, subject->state);
    }
}

Observer* create_observer(const char* name, void (*update_func)(Observer*, Subject*)) {
    Observer* obs = malloc(sizeof(Observer));
    if (!obs) return NULL;
    
    obs->update = update_func;
    obs->name = strdup(name); // Requires POSIX or custom implementation. Using malloc/strcpy for portability below.
    if (!obs->name) { // Fallback if strdup is not available or fails
         obs->name = malloc(strlen(name) + 1);
         if (obs->name) strcpy(obs->name, name);
    }
    obs->context = NULL;
    
    return obs;
}

void destroy_observer(Observer* obs) {
    if (obs) {
        free(obs->name);
        free(obs);
    }
}

// --- Main Example ---

int main() {
    printf("=== C Observer Pattern with Linked List ===\n");

    Subject* subject = create_subject();

    Observer* display1 = create_observer("Display1", display_observer_update);
    Observer* display2 = create_observer("Display2", display_observer_update);
    Observer* alertSystem = create_observer("AlertSystem", alert_observer_update);

    printf("\n--- Attaching Observers ---\n");
    subject_attach(subject, display1);
    subject_attach(subject, display2);
    subject_attach(subject, alertSystem);

    printf("\n--- State Change 1 (Value: 25) ---\n");
    subject_set_state(subject, 25);

    printf("\n--- State Change 2 (Value: 35) ---\n");
    subject_set_state(subject, 35); // Should trigger alert

    printf("\n--- Detaching Display2 ---\n");
    subject_detach(subject, display2);

    printf("\n--- State Change 3 (Value: 10) ---\n");
    subject_set_state(subject, 10);

    // Cleanup
    destroy_observer(display1);
    destroy_observer(display2);
    destroy_observer(alertSystem);
    destroy_subject(subject);

    return 0;
}