#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>

// Template Method Pattern - Data Processing Framework
class DataProcessor {
public:
    // Template method - defines the algorithm skeleton
    void processData() {
        std::cout << "Starting data processing..." << std::endl;

        loadData();

        if (validateData()) {
            transformData();
            if (hasAdditionalProcessing()) {
                performAdditionalProcessing();
            }
            saveData();
        } else {
            std::cout << "Data validation failed!" << std::endl;
            handleValidationError();
        }

        cleanup();
        std::cout << "Data processing completed.\n" << std::endl;
    }

protected:
    // Abstract methods - must be implemented by subclasses
    virtual void loadData() = 0;
    virtual bool validateData() = 0;
    virtual void transformData() = 0;
    virtual void saveData() = 0;

    // Hook methods - can be overridden by subclasses
    virtual bool hasAdditionalProcessing() const { return false; }
    virtual void performAdditionalProcessing() {
        std::cout << "No additional processing needed" << std::endl;
    }

    virtual void handleValidationError() {
        std::cout << "Default validation error handling" << std::endl;
    }

    virtual void cleanup() {
        std::cout << "Performing default cleanup" << std::endl;
    }

public:
    virtual ~DataProcessor() = default;
    virtual std::string getProcessorType() const = 0;
};

// Concrete implementation for CSV data processing
class CSVDataProcessor : public DataProcessor {
private:
    std::vector<std::string> data;
    std::string filename;

public:
    explicit CSVDataProcessor(const std::string& filename) : filename(filename) {}

protected:
    void loadData() override {
        std::cout << "Loading CSV data from: " << filename << std::endl;
        // Simulate loading CSV data
        data = {"header1,header2,header3", "value1,value2,value3", "value4,value5,value6"};
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Loaded " << data.size() << " rows" << std::endl;
    }

    bool validateData() override {
        std::cout << "Validating CSV data..." << std::endl;

        if (data.empty()) {
            std::cout << "No data to validate" << std::endl;
            return false;
        }

        // Check if all rows have the same number of columns
        if (data.size() > 1) {
            size_t headerCols = std::count(data[0].begin(), data[0].end(), ',') + 1;
            for (size_t i = 1; i < data.size(); ++i) {
                size_t cols = std::count(data[i].begin(), data[i].end(), ',') + 1;
                if (cols != headerCols) {
                    std::cout << "Row " << i << " has inconsistent number of columns" << std::endl;
                    return false;
                }
            }
        }

        std::cout << "CSV data validation successful" << std::endl;
        return true;
    }

    void transformData() override {
        std::cout << "Transforming CSV data..." << std::endl;

        // Convert to uppercase (simple transformation)
        for (auto& row : data) {
            std::transform(row.begin(), row.end(), row.begin(), ::toupper);
        }

        std::cout << "CSV transformation completed" << std::endl;
    }

    void saveData() override {
        std::cout << "Saving processed CSV data to: " << filename + "_processed.csv" << std::endl;

        // Simulate saving
        for (const auto& row : data) {
            std::cout << "  " << row << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "CSV data saved successfully" << std::endl;
    }

    bool hasAdditionalProcessing() const override {
        return true; // CSV needs additional formatting
    }

    void performAdditionalProcessing() override {
        std::cout << "Performing CSV-specific formatting..." << std::endl;
        // Add quotes around values with commas
        for (auto& row : data) {
            std::cout << "  Formatted: \"" << row << "\"" << std::endl;
        }
    }

public:
    std::string getProcessorType() const override {
        return "CSV Data Processor";
    }
};

// Concrete implementation for JSON data processing
class JSONDataProcessor : public DataProcessor {
private:
    std::string jsonData;
    std::string endpoint;

public:
    explicit JSONDataProcessor(const std::string& endpoint) : endpoint(endpoint) {}

protected:
    void loadData() override {
        std::cout << "Loading JSON data from API: " << endpoint << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(800));

        // Simulate JSON data
        jsonData = R"({"users": [{"name": "John", "age": 30}, {"name": "Jane", "age": 25}]})";
        std::cout << "JSON data loaded" << std::endl;
    }

    bool validateData() override {
        std::cout << "Validating JSON data..." << std::endl;

        // Simple JSON validation (check for braces)
        if (jsonData.empty() || jsonData.front() != '{' || jsonData.back() != '}') {
            std::cout << "Invalid JSON format" << std::endl;
            return false;
        }

        std::cout << "JSON validation successful" << std::endl;
        return true;
    }

    void transformData() override {
        std::cout << "Transforming JSON data..." << std::endl;

        // Simple transformation - add processing timestamp
        size_t insertPos = jsonData.find_last_of('}');
        if (insertPos != std::string::npos) {
            jsonData.insert(insertPos, R"(, "processed_at": "2024-01-01T10:00:00Z")");
        }

        std::cout << "JSON transformation completed" << std::endl;
    }

    void saveData() override {
        std::cout << "Saving processed JSON data..." << std::endl;
        std::cout << "  " << jsonData << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        std::cout << "JSON data saved to database" << std::endl;
    }

    void handleValidationError() override {
        std::cout << "JSON validation error - sending alert to admin" << std::endl;
    }

public:
    std::string getProcessorType() const override {
        return "JSON Data Processor";
    }
};

// Example 2: Game Level Template
class GameLevel {
public:
    // Template method
    void playLevel() {
        std::cout << "\n=== Starting Level: " << getLevelName() << " ===" << std::endl;

        initializeLevel();
        showIntroduction();

        while (!isLevelComplete()) {
            processPlayerInput();
            updateGameState();
            renderFrame();

            if (shouldShowHint()) {
                showHint();
            }
        }

        if (isPlayerSuccessful()) {
            showSuccessMessage();
            unlockNextLevel();
        } else {
            showFailureMessage();
            if (allowRetry()) {
                offerRetry();
            }
        }

        cleanup();
        std::cout << "=== Level Complete ===" << std::endl;
    }

protected:
    // Abstract methods
    virtual void initializeLevel() = 0;
    virtual void processPlayerInput() = 0;
    virtual void updateGameState() = 0;
    virtual void renderFrame() = 0;
    virtual bool isLevelComplete() const = 0;
    virtual bool isPlayerSuccessful() const = 0;
    virtual std::string getLevelName() const = 0;

    // Hook methods
    virtual void showIntroduction() {
        std::cout << "Welcome to " << getLevelName() << "!" << std::endl;
    }

    virtual bool shouldShowHint() const { return false; }
    virtual void showHint() {
        std::cout << "Hint: Keep trying!" << std::endl;
    }

    virtual void showSuccessMessage() {
        std::cout << "🎉 Level completed successfully!" << std::endl;
    }

    virtual void showFailureMessage() {
        std::cout << "💀 Level failed!" << std::endl;
    }

    virtual void unlockNextLevel() {
        std::cout << "🔓 Next level unlocked!" << std::endl;
    }

    virtual bool allowRetry() const { return true; }

    virtual void offerRetry() {
        std::cout << "🔄 Press R to retry" << std::endl;
    }

    virtual void cleanup() {
        std::cout << "Cleaning up level resources" << std::endl;
    }

public:
    virtual ~GameLevel() = default;
};

// Concrete game levels
class TutorialLevel : public GameLevel {
private:
    int playerActions = 0;
    bool completed = false;

protected:
    void initializeLevel() override {
        std::cout << "Setting up tutorial environment..." << std::endl;
        playerActions = 0;
        completed = false;
    }

    void processPlayerInput() override {
        std::cout << "Processing tutorial input..." << std::endl;
        playerActions++;
    }

    void updateGameState() override {
        std::cout << "Updating tutorial state (action " << playerActions << ")" << std::endl;
        if (playerActions >= 3) {
            completed = true;
        }
    }

    void renderFrame() override {
        std::cout << "Rendering tutorial frame" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    bool isLevelComplete() const override {
        return completed;
    }

    bool isPlayerSuccessful() const override {
        return completed;
    }

    std::string getLevelName() const override {
        return "Tutorial";
    }

    bool shouldShowHint() const override {
        return playerActions == 1; // Show hint after first action
    }

    void showHint() override {
        std::cout << "💡 Hint: Try moving around and interacting with objects!" << std::endl;
    }

    void showIntroduction() override {
        std::cout << "🎮 Welcome to the tutorial! Learn the basics here." << std::endl;
    }
};

class BossLevel : public GameLevel {
private:
    int bossHealth = 100;
    int playerHealth = 100;
    int turn = 0;

protected:
    void initializeLevel() override {
        std::cout << "Initializing boss arena..." << std::endl;
        bossHealth = 100;
        playerHealth = 100;
        turn = 0;
    }

    void processPlayerInput() override {
        std::cout << "Player attacks boss!" << std::endl;
        bossHealth -= 20;
        turn++;
    }

    void updateGameState() override {
        if (bossHealth > 0 && playerHealth > 0) {
            std::cout << "Boss attacks player!" << std::endl;
            playerHealth -= 15;
        }
        std::cout << "Boss HP: " << bossHealth << ", Player HP: " << playerHealth << std::endl;
    }

    void renderFrame() override {
        std::cout << "Rendering epic boss battle!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    bool isLevelComplete() const override {
        return bossHealth <= 0 || playerHealth <= 0;
    }

    bool isPlayerSuccessful() const override {
        return bossHealth <= 0 && playerHealth > 0;
    }

    std::string getLevelName() const override {
        return "Boss Battle";
    }

    bool shouldShowHint() const override {
        return turn == 2 && bossHealth > 60;
    }

    void showHint() override {
        std::cout << "💡 Hint: Try using special attacks for more damage!" << std::endl;
    }

    void showIntroduction() override {
        std::cout << "⚔️ A mighty boss appears! Prepare for battle!" << std::endl;
    }

    void showSuccessMessage() override {
        std::cout << "🏆 Boss defeated! You are victorious!" << std::endl;
    }

    void showFailureMessage() override {
        std::cout << "💀 You have been defeated by the boss!" << std::endl;
    }
};

// Example 3: Cooking Recipe Template
class Recipe {
public:
    // Template method
    void cookDish() {
        std::cout << "\n🍳 Cooking: " << getDishName() << std::endl;
        std::cout << "Estimated time: " << getEstimatedTime() << " minutes" << std::endl;

        if (requiresPreparation()) {
            prepareIngredients();
        }

        preHeatOven();

        cookMainDish();

        if (needsSideDish()) {
            prepareSideDish();
        }

        if (requiresGarnish()) {
            addGarnish();
        }

        plateDish();

        std::cout << "✅ " << getDishName() << " is ready to serve!" << std::endl;
    }

protected:
    // Abstract methods
    virtual std::string getDishName() const = 0;
    virtual void prepareIngredients() = 0;
    virtual void cookMainDish() = 0;
    virtual void plateDish() = 0;
    virtual int getEstimatedTime() const = 0;

    // Hook methods
    virtual bool requiresPreparation() const { return true; }
    virtual bool needsSideDish() const { return false; }
    virtual bool requiresGarnish() const { return false; }

    virtual void preHeatOven() {
        std::cout << "🔥 Preheating oven to 350°F" << std::endl;
    }

    virtual void prepareSideDish() {
        std::cout << "🥗 Preparing side dish" << std::endl;
    }

    virtual void addGarnish() {
        std::cout << "🌿 Adding garnish" << std::endl;
    }

public:
    virtual ~Recipe() = default;
};

class PastaRecipe : public Recipe {
protected:
    std::string getDishName() const override {
        return "Spaghetti Carbonara";
    }

    void prepareIngredients() override {
        std::cout << "🧄 Chopping garlic, dicing bacon, grating cheese" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    void cookMainDish() override {
        std::cout << "🍝 Boiling pasta and cooking bacon" << std::endl;
        std::cout << "🥚 Creating egg and cheese mixture" << std::endl;
        std::cout << "🍳 Combining all ingredients" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void plateDish() override {
        std::cout << "🍽️ Plating pasta with fresh black pepper" << std::endl;
    }

    int getEstimatedTime() const override {
        return 20;
    }

    bool requiresGarnish() const override {
        return true;
    }

    void addGarnish() override {
        std::cout << "🌿 Adding fresh parsley and extra parmesan" << std::endl;
    }
};

class SteakRecipe : public Recipe {
protected:
    std::string getDishName() const override {
        return "Grilled Ribeye Steak";
    }

    void prepareIngredients() override {
        std::cout << "🧂 Seasoning steak with salt and pepper" << std::endl;
        std::cout << "🌿 Preparing herb butter" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    void cookMainDish() override {
        std::cout << "🔥 Grilling steak to medium-rare" << std::endl;
        std::cout << "🧈 Basting with herb butter" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }

    void plateDish() override {
        std::cout << "🍽️ Plating steak with mashed potatoes" << std::endl;
    }

    int getEstimatedTime() const override {
        return 25;
    }

    bool needsSideDish() const override {
        return true;
    }

    void prepareSideDish() override {
        std::cout << "🥔 Preparing creamy mashed potatoes and grilled vegetables" << std::endl;
    }

    void preHeatOven() override {
        std::cout << "🔥 Preheating grill to high heat" << std::endl;
    }
};

// Function to demonstrate polymorphic usage
void processDataSources(std::vector<std::unique_ptr<DataProcessor>>& processors) {
    for (auto& processor : processors) {
        std::cout << "\nUsing: " << processor->getProcessorType() << std::endl;
        processor->processData();
    }
}

void playGameLevels(std::vector<std::unique_ptr<GameLevel>>& levels) {
    for (auto& level : levels) {
        level->playLevel();
    }
}

void cookMeals(std::vector<std::unique_ptr<Recipe>>& recipes) {
    for (auto& recipe : recipes) {
        recipe->cookDish();
    }
}

int main() {
    std::cout << "=== Template Method Pattern Demo ===" << std::endl;

    // Example 1: Data Processing
    std::cout << "\n1. Data Processing Framework:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::vector<std::unique_ptr<DataProcessor>> processors;
    processors.push_back(std::make_unique<CSVDataProcessor>("sales_data.csv"));
    processors.push_back(std::make_unique<JSONDataProcessor>("https://api.example.com/users"));

    processDataSources(processors);

    // Example 2: Game Levels
    std::cout << "\n\n2. Game Level Framework:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::vector<std::unique_ptr<GameLevel>> levels;
    levels.push_back(std::make_unique<TutorialLevel>());
    levels.push_back(std::make_unique<BossLevel>());

    playGameLevels(levels);

    // Example 3: Cooking Recipes
    std::cout << "\n\n3. Cooking Recipe Framework:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::vector<std::unique_ptr<Recipe>> recipes;
    recipes.push_back(std::make_unique<PastaRecipe>());
    recipes.push_back(std::make_unique<SteakRecipe>());

    cookMeals(recipes);

    // Benefits demonstration
    std::cout << "\n\n4. Template Method Pattern Benefits:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "✓ Defines algorithm skeleton in base class" << std::endl;
    std::cout << "✓ Subclasses implement specific steps" << std::endl;
    std::cout << "✓ Code reuse through inheritance" << std::endl;
    std::cout << "✓ Hook methods provide optional customization" << std::endl;
    std::cout << "✓ Inversion of control - framework calls user code" << std::endl;
    std::cout << "✓ Common algorithm structure across implementations" << std::endl;
    std::cout << "✓ Easy to add new implementations" << std::endl;

    return 0;
}