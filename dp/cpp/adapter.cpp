#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Target interface that client expects
class MediaPlayer {
public:
    virtual ~MediaPlayer() = default;
    virtual void play(const std::string& audioType, const std::string& fileName) = 0;
};

// Adaptee - External MP3 library
class Mp3Player {
public:
    void playMp3(const std::string& fileName) {
        std::cout << "Playing MP3 file: " << fileName << std::endl;
    }
};

// Adaptee - External MP4 library
class Mp4Player {
public:
    void playMp4(const std::string& fileName) {
        std::cout << "Playing MP4 file: " << fileName << std::endl;
    }
};

// Adaptee - External FLAC library
class FlacPlayer {
public:
    void playFlac(const std::string& fileName) {
        std::cout << "Playing FLAC file: " << fileName << std::endl;
    }
};

// Adapter for advanced audio formats
class AdvancedMediaAdapter : public MediaPlayer {
private:
    std::unique_ptr<Mp4Player> mp4Player;
    std::unique_ptr<FlacPlayer> flacPlayer;

public:
    AdvancedMediaAdapter()
        : mp4Player(std::make_unique<Mp4Player>()),
          flacPlayer(std::make_unique<FlacPlayer>()) {}

    void play(const std::string& audioType, const std::string& fileName) override {
        if (audioType == "mp4") {
            mp4Player->playMp4(fileName);
        } else if (audioType == "flac") {
            flacPlayer->playFlac(fileName);
        } else {
            std::cout << "Format " << audioType << " not supported by advanced adapter" << std::endl;
        }
    }
};

// Concrete implementation of MediaPlayer
class AudioPlayer : public MediaPlayer {
private:
    std::unique_ptr<Mp3Player> mp3Player;
    std::unique_ptr<AdvancedMediaAdapter> adapter;

public:
    AudioPlayer()
        : mp3Player(std::make_unique<Mp3Player>()),
          adapter(std::make_unique<AdvancedMediaAdapter>()) {}

    void play(const std::string& audioType, const std::string& fileName) override {
        if (audioType == "mp3") {
            mp3Player->playMp3(fileName);
        } else {
            adapter->play(audioType, fileName);
        }
    }
};

// Example 2: Object Adapter for legacy payment system
class LegacyPaymentSystem {
public:
    void makePayment(double amount) {
        std::cout << "Legacy payment: $" << std::fixed << std::setprecision(2)
                  << amount << " processed" << std::endl;
    }
};

// Modern payment interface
class PaymentProcessor {
public:
    virtual ~PaymentProcessor() = default;
    virtual bool processPayment(const std::string& currency, double amount, const std::string& method) = 0;
    virtual std::string getTransactionId() const = 0;
};

// Adapter for legacy payment system
class LegacyPaymentAdapter : public PaymentProcessor {
private:
    std::unique_ptr<LegacyPaymentSystem> legacySystem;
    std::string lastTransactionId;
    static int transactionCounter;

    double convertToUSD(const std::string& currency, double amount) {
        // Simple currency conversion (in real world, would use actual rates)
        if (currency == "EUR") return amount * 1.1;
        if (currency == "GBP") return amount * 1.3;
        if (currency == "JPY") return amount * 0.009;
        return amount; // USD
    }

public:
    LegacyPaymentAdapter()
        : legacySystem(std::make_unique<LegacyPaymentSystem>()) {}

    bool processPayment(const std::string& currency, double amount, const std::string& method) override {
        try {
            double usdAmount = convertToUSD(currency, amount);

            std::cout << "Adapting modern payment request:" << std::endl;
            std::cout << "  Original: " << amount << " " << currency << " via " << method << std::endl;
            std::cout << "  Converted: $" << std::fixed << std::setprecision(2) << usdAmount << " USD" << std::endl;

            legacySystem->makePayment(usdAmount);

            // Generate transaction ID
            std::ostringstream oss;
            oss << "TXN_" << ++transactionCounter << "_" << currency;
            lastTransactionId = oss.str();

            return true;
        } catch (...) {
            return false;
        }
    }

    std::string getTransactionId() const override {
        return lastTransactionId;
    }
};

int LegacyPaymentAdapter::transactionCounter = 0;

// Example 3: Class Adapter using multiple inheritance (less common in modern C++)
class Rectangle {
public:
    virtual ~Rectangle() = default;
    virtual void draw(int x, int y, int width, int height) = 0;
};

class LegacyRectangle {
public:
    void drawRectangle(int x1, int y1, int x2, int y2) {
        std::cout << "Legacy Rectangle drawn from (" << x1 << "," << y1
                  << ") to (" << x2 << "," << y2 << ")" << std::endl;
    }
};

// Class Adapter using inheritance
class RectangleAdapter : public Rectangle, private LegacyRectangle {
public:
    void draw(int x, int y, int width, int height) override {
        // Convert modern interface to legacy interface
        int x2 = x + width;
        int y2 = y + height;
        drawRectangle(x, y, x2, y2);
    }
};

// Example 4: STL Container Adapter example
template<typename Container>
class Stack {
private:
    Container container;

public:
    void push(const typename Container::value_type& value) {
        container.push_back(value);
    }

    void pop() {
        if (!container.empty()) {
            container.pop_back();
        }
    }

    typename Container::value_type& top() {
        return container.back();
    }

    const typename Container::value_type& top() const {
        return container.back();
    }

    bool empty() const {
        return container.empty();
    }

    size_t size() const {
        return container.size();
    }
};

// Client code for payment system
class PaymentService {
private:
    std::vector<std::unique_ptr<PaymentProcessor>> processors;

public:
    void addProcessor(std::unique_ptr<PaymentProcessor> processor) {
        processors.push_back(std::move(processor));
    }

    void processAllPayments(const std::string& currency, double amount, const std::string& method) {
        for (auto& processor : processors) {
            std::cout << "\nProcessing payment..." << std::endl;
            if (processor->processPayment(currency, amount, method)) {
                std::cout << "Payment successful! Transaction ID: "
                          << processor->getTransactionId() << std::endl;
            } else {
                std::cout << "Payment failed!" << std::endl;
            }
        }
    }
};

int main() {
    std::cout << "=== Adapter Pattern Demo ===" << std::endl;

    // Example 1: Media Player Adapter
    std::cout << "\n1. Media Player Adapter:" << std::endl;
    std::cout << std::string(30, '-') << std::endl;

    auto audioPlayer = std::make_unique<AudioPlayer>();

    audioPlayer->play("mp3", "song.mp3");
    audioPlayer->play("mp4", "video.mp4");
    audioPlayer->play("flac", "highquality.flac");
    audioPlayer->play("wav", "unsupported.wav");

    // Example 2: Payment System Adapter
    std::cout << "\n2. Payment System Adapter:" << std::endl;
    std::cout << std::string(30, '-') << std::endl;

    PaymentService paymentService;
    paymentService.addProcessor(std::make_unique<LegacyPaymentAdapter>());

    paymentService.processAllPayments("USD", 100.50, "Credit Card");
    paymentService.processAllPayments("EUR", 85.75, "PayPal");
    paymentService.processAllPayments("GBP", 75.25, "Bank Transfer");

    // Example 3: Rectangle Adapter
    std::cout << "\n3. Rectangle Class Adapter:" << std::endl;
    std::cout << std::string(30, '-') << std::endl;

    auto rectangle = std::make_unique<RectangleAdapter>();
    rectangle->draw(10, 20, 100, 50);

    // Example 4: STL Container Adapter
    std::cout << "\n4. STL Container Adapter (Stack):" << std::endl;
    std::cout << std::string(30, '-') << std::endl;

    Stack<std::vector<int>> stack;

    std::cout << "Pushing elements: ";
    for (int i = 1; i <= 5; ++i) {
        stack.push(i);
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "Stack size: " << stack.size() << std::endl;

    std::cout << "Popping elements: ";
    while (!stack.empty()) {
        std::cout << stack.top() << " ";
        stack.pop();
    }
    std::cout << std::endl;

    // Advanced example: Multiple adapters
    std::cout << "\n5. Multiple Media Adapters:" << std::endl;
    std::cout << std::string(30, '-') << std::endl;

    class VideoPlayer {
    public:
        void playVideo(const std::string& fileName, const std::string& quality) {
            std::cout << "Playing video: " << fileName << " in " << quality << " quality" << std::endl;
        }
    };

    class VideoAdapter : public MediaPlayer {
    private:
        std::unique_ptr<VideoPlayer> videoPlayer;

    public:
        VideoAdapter() : videoPlayer(std::make_unique<VideoPlayer>()) {}

        void play(const std::string& audioType, const std::string& fileName) override {
            if (audioType == "avi" || audioType == "mkv") {
                videoPlayer->playVideo(fileName, "HD");
            } else {
                std::cout << "Video format " << audioType << " not supported" << std::endl;
            }
        }
    };

    class UniversalPlayer : public MediaPlayer {
    private:
        std::unique_ptr<AudioPlayer> audioPlayer;
        std::unique_ptr<VideoAdapter> videoAdapter;

    public:
        UniversalPlayer()
            : audioPlayer(std::make_unique<AudioPlayer>()),
              videoAdapter(std::make_unique<VideoAdapter>()) {}

        void play(const std::string& mediaType, const std::string& fileName) override {
            if (mediaType == "mp3" || mediaType == "mp4" || mediaType == "flac") {
                audioPlayer->play(mediaType, fileName);
            } else if (mediaType == "avi" || mediaType == "mkv") {
                videoAdapter->play(mediaType, fileName);
            } else {
                std::cout << "Media type " << mediaType << " not supported" << std::endl;
            }
        }
    };

    auto universalPlayer = std::make_unique<UniversalPlayer>();
    universalPlayer->play("mp3", "audio.mp3");
    universalPlayer->play("avi", "movie.avi");
    universalPlayer->play("mkv", "series.mkv");
    universalPlayer->play("wmv", "unsupported.wmv");

    return 0;
}