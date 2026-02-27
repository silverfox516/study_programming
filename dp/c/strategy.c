#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Strategy interface
typedef struct PaymentStrategy {
    void (*pay)(struct PaymentStrategy* self, double amount);
    void (*destroy)(struct PaymentStrategy* self);
} PaymentStrategy;

// Credit Card Strategy
typedef struct CreditCardPayment {
    PaymentStrategy base;
    char* card_number;
    char* name;
    char* cvv;
    char* expiry_date;
} CreditCardPayment;

void credit_card_pay(PaymentStrategy* strategy, double amount) {
    CreditCardPayment* cc = (CreditCardPayment*)strategy;
    printf("Paid $%.2f using Credit Card\n", amount);
    printf("Card: ****-****-****-%s\n", &cc->card_number[strlen(cc->card_number) - 4]);
    printf("Cardholder: %s\n", cc->name);
}

void credit_card_destroy(PaymentStrategy* strategy) {
    CreditCardPayment* cc = (CreditCardPayment*)strategy;
    free(cc->card_number);
    free(cc->name);
    free(cc->cvv);
    free(cc->expiry_date);
    free(cc);
}

PaymentStrategy* create_credit_card_payment(const char* card_number,
                                          const char* name,
                                          const char* cvv,
                                          const char* expiry_date) {
    CreditCardPayment* cc = malloc(sizeof(CreditCardPayment));
    cc->base.pay = credit_card_pay;
    cc->base.destroy = credit_card_destroy;

    cc->card_number = malloc(strlen(card_number) + 1);
    strcpy(cc->card_number, card_number);

    cc->name = malloc(strlen(name) + 1);
    strcpy(cc->name, name);

    cc->cvv = malloc(strlen(cvv) + 1);
    strcpy(cc->cvv, cvv);

    cc->expiry_date = malloc(strlen(expiry_date) + 1);
    strcpy(cc->expiry_date, expiry_date);

    return (PaymentStrategy*)cc;
}

// PayPal Strategy
typedef struct PayPalPayment {
    PaymentStrategy base;
    char* email;
    char* password;
} PayPalPayment;

void paypal_pay(PaymentStrategy* strategy, double amount) {
    PayPalPayment* pp = (PayPalPayment*)strategy;
    printf("Paid $%.2f using PayPal\n", amount);
    printf("Account: %s\n", pp->email);
}

void paypal_destroy(PaymentStrategy* strategy) {
    PayPalPayment* pp = (PayPalPayment*)strategy;
    free(pp->email);
    free(pp->password);
    free(pp);
}

PaymentStrategy* create_paypal_payment(const char* email, const char* password) {
    PayPalPayment* pp = malloc(sizeof(PayPalPayment));
    pp->base.pay = paypal_pay;
    pp->base.destroy = paypal_destroy;

    pp->email = malloc(strlen(email) + 1);
    strcpy(pp->email, email);

    pp->password = malloc(strlen(password) + 1);
    strcpy(pp->password, password);

    return (PaymentStrategy*)pp;
}

// Crypto Strategy
typedef struct CryptoPayment {
    PaymentStrategy base;
    char* wallet_address;
    char* crypto_type;
} CryptoPayment;

void crypto_pay(PaymentStrategy* strategy, double amount) {
    CryptoPayment* crypto = (CryptoPayment*)strategy;
    printf("Paid $%.2f using %s\n", amount, crypto->crypto_type);
    printf("Wallet: %s...%s\n",
           crypto->wallet_address,
           &crypto->wallet_address[strlen(crypto->wallet_address) - 6]);
}

void crypto_destroy(PaymentStrategy* strategy) {
    CryptoPayment* crypto = (CryptoPayment*)strategy;
    free(crypto->wallet_address);
    free(crypto->crypto_type);
    free(crypto);
}

PaymentStrategy* create_crypto_payment(const char* wallet_address, const char* crypto_type) {
    CryptoPayment* crypto = malloc(sizeof(CryptoPayment));
    crypto->base.pay = crypto_pay;
    crypto->base.destroy = crypto_destroy;

    crypto->wallet_address = malloc(strlen(wallet_address) + 1);
    strcpy(crypto->wallet_address, wallet_address);

    crypto->crypto_type = malloc(strlen(crypto_type) + 1);
    strcpy(crypto->crypto_type, crypto_type);

    return (PaymentStrategy*)crypto;
}

// Shopping Cart Context
typedef struct ShoppingCart {
    PaymentStrategy* payment_strategy;
    double total_amount;
} ShoppingCart;

ShoppingCart* create_shopping_cart() {
    ShoppingCart* cart = malloc(sizeof(ShoppingCart));
    cart->payment_strategy = NULL;
    cart->total_amount = 0.0;
    return cart;
}

void add_item(ShoppingCart* cart, const char* item, double price) {
    cart->total_amount += price;
    printf("Added %s: $%.2f (Total: $%.2f)\n", item, price, cart->total_amount);
}

void set_payment_strategy(ShoppingCart* cart, PaymentStrategy* strategy) {
    cart->payment_strategy = strategy;
}

void checkout(ShoppingCart* cart) {
    if (cart->payment_strategy) {
        printf("\n--- Checkout ---\n");
        printf("Total Amount: $%.2f\n", cart->total_amount);
        cart->payment_strategy->pay(cart->payment_strategy, cart->total_amount);
        printf("Payment completed successfully!\n");
    } else {
        printf("No payment method selected!\n");
    }
}

void destroy_shopping_cart(ShoppingCart* cart) {
    if (cart) {
        if (cart->payment_strategy) {
            cart->payment_strategy->destroy(cart->payment_strategy);
        }
        free(cart);
    }
}

int main() {
    printf("--- E-Commerce Payment System ---\n");

    ShoppingCart* cart = create_shopping_cart();

    // Add items to cart
    add_item(cart, "Laptop", 999.99);
    add_item(cart, "Mouse", 29.99);
    add_item(cart, "Keyboard", 79.99);

    // Test different payment strategies
    printf("\n=== Payment with Credit Card ===\n");
    PaymentStrategy* credit_card = create_credit_card_payment(
        "1234-5678-9012-3456", "John Doe", "123", "12/25");
    set_payment_strategy(cart, credit_card);
    checkout(cart);

    // Reset cart for next payment
    cart->payment_strategy = NULL;

    printf("\n=== Payment with PayPal ===\n");
    PaymentStrategy* paypal = create_paypal_payment("john.doe@email.com", "password123");
    set_payment_strategy(cart, paypal);
    checkout(cart);

    // Reset cart for next payment
    cart->payment_strategy = NULL;

    printf("\n=== Payment with Cryptocurrency ===\n");
    PaymentStrategy* crypto = create_crypto_payment(
        "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa", "Bitcoin");
    set_payment_strategy(cart, crypto);
    checkout(cart);

    destroy_shopping_cart(cart);

    return 0;
}