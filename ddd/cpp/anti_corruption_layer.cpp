// Anti-Corruption Layer (ACL)
// Translates between your domain model and an external system's model.
// Prevents foreign concepts from leaking into your domain.

#include <iostream>
#include <string>
#include <optional>

// ============================================================
// External system (legacy/third-party) — messy, different naming
// ============================================================
namespace legacy_crm {

struct CRM_Contact {
    int CONTACT_ID;
    char FIRST_NM[32];
    char LAST_NM[32];
    char EMAIL_ADDR[64];
    int STATUS_CD;       // 1=active, 2=inactive, 3=deleted
    char ACCT_TYPE[4];   // "PRM" = premium, "STD" = standard
};

// Simulated legacy API
CRM_Contact fetch_contact(int id) {
    CRM_Contact c{};
    c.CONTACT_ID = id;
    snprintf(c.FIRST_NM, 32, "John");
    snprintf(c.LAST_NM, 32, "Doe");
    snprintf(c.EMAIL_ADDR, 64, "john.doe@legacy.com");
    c.STATUS_CD = 1;
    snprintf(c.ACCT_TYPE, 4, "PRM");
    return c;
}

} // namespace legacy_crm

// ============================================================
// Our domain model — clean, typed, meaningful
// ============================================================
namespace domain {

enum class CustomerStatus { Active, Inactive };
enum class Tier { Standard, Premium };

struct Customer {
    int id;
    std::string full_name;
    std::string email;
    CustomerStatus status;
    Tier tier;
};

std::ostream& operator<<(std::ostream& os, const Customer& c) {
    os << "Customer{id=" << c.id << ", name=\"" << c.full_name
       << "\", email=\"" << c.email << "\", status="
       << (c.status == CustomerStatus::Active ? "Active" : "Inactive")
       << ", tier=" << (c.tier == Tier::Premium ? "Premium" : "Standard")
       << "}";
    return os;
}

} // namespace domain

// ============================================================
// Anti-Corruption Layer — the translator
// ============================================================
namespace acl {

class CrmAdapter {
public:
    // Translate legacy CRM contact to our domain Customer
    std::optional<domain::Customer> find_customer(int id) {
        auto contact = legacy_crm::fetch_contact(id);

        // Filter out deleted contacts
        if (contact.STATUS_CD == 3)
            return std::nullopt;

        domain::Customer customer;
        customer.id = contact.CONTACT_ID;
        customer.full_name = std::string(contact.FIRST_NM) + " " + contact.LAST_NM;
        customer.email = contact.EMAIL_ADDR;

        // Translate status codes to domain enums
        customer.status = (contact.STATUS_CD == 1)
            ? domain::CustomerStatus::Active
            : domain::CustomerStatus::Inactive;

        // Translate account type to tier
        customer.tier = (std::string(contact.ACCT_TYPE) == "PRM")
            ? domain::Tier::Premium
            : domain::Tier::Standard;

        return customer;
    }
};

} // namespace acl

// ============================================================
// Our service — only sees domain types, never legacy types
// ============================================================
namespace service {

class CustomerService {
    acl::CrmAdapter& adapter_;
public:
    explicit CustomerService(acl::CrmAdapter& adapter) : adapter_(adapter) {}

    void greet_customer(int id) {
        auto customer = adapter_.find_customer(id);
        if (!customer) {
            std::cout << "[Service] Customer #" << id << " not found\n";
            return;
        }

        std::cout << "[Service] Found: " << *customer << "\n";

        if (customer->tier == domain::Tier::Premium)
            std::cout << "[Service] Welcome back, valued Premium member!\n";
        else
            std::cout << "[Service] Hello, " << customer->full_name << "!\n";
    }
};

} // namespace service

int main() {
    std::cout << "=== Anti-Corruption Layer ===\n\n";

    std::cout << "Legacy CRM returns:\n";
    auto raw = legacy_crm::fetch_contact(42);
    std::cout << "  CONTACT_ID=" << raw.CONTACT_ID
              << " FIRST_NM=" << raw.FIRST_NM
              << " STATUS_CD=" << raw.STATUS_CD
              << " ACCT_TYPE=" << raw.ACCT_TYPE << "\n\n";

    std::cout << "After ACL translation:\n";
    acl::CrmAdapter adapter;
    service::CustomerService svc(adapter);
    svc.greet_customer(42);

    // Key insight:
    // - Our domain code never sees CRM_Contact, STATUS_CD, ACCT_TYPE
    // - If the legacy CRM changes its field names, only ACL changes
    // - Our domain model remains clean and testable

    return 0;
}
