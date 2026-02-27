#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

// Flyweight interface
class CharacterFlyweight {
public:
    virtual ~CharacterFlyweight() = default;
    virtual void render(int x, int y, int size, const std::string& color) const = 0;
    virtual char getCharacter() const = 0;
    virtual std::string getFontFamily() const = 0;
};

// Concrete Flyweight
class ConcreteCharacterFlyweight : public CharacterFlyweight {
private:
    char character;      // Intrinsic state
    std::string fontFamily;  // Intrinsic state

public:
    ConcreteCharacterFlyweight(char character, const std::string& fontFamily)
        : character(character), fontFamily(fontFamily) {}

    void render(int x, int y, int size, const std::string& color) const override {
        std::cout << "Rendering '" << character << "' at (" << x << "," << y
                  << ") size=" << size << " color=" << color
                  << " font=" << fontFamily << std::endl;
    }

    char getCharacter() const override {
        return character;
    }

    std::string getFontFamily() const override {
        return fontFamily;
    }
};

// Flyweight Factory
class CharacterFlyweightFactory {
private:
    std::unordered_map<std::string, std::shared_ptr<CharacterFlyweight>> flyweights;

    std::string getKey(char character, const std::string& fontFamily) const {
        return std::string(1, character) + "_" + fontFamily;
    }

public:
    std::shared_ptr<CharacterFlyweight> getFlyweight(char character, const std::string& fontFamily) {
        std::string key = getKey(character, fontFamily);

        auto it = flyweights.find(key);
        if (it != flyweights.end()) {
            std::cout << "Reusing existing flyweight for '" << character << "' (" << fontFamily << ")" << std::endl;
            return it->second;
        }

        std::cout << "Creating new flyweight for '" << character << "' (" << fontFamily << ")" << std::endl;
        auto flyweight = std::make_shared<ConcreteCharacterFlyweight>(character, fontFamily);
        flyweights[key] = flyweight;
        return flyweight;
    }

    size_t getFlyweightCount() const {
        return flyweights.size();
    }

    void listFlyweights() const {
        std::cout << "Total flyweights created: " << flyweights.size() << std::endl;
        for (const auto& [key, flyweight] : flyweights) {
            std::cout << "  " << key << std::endl;
        }
    }
};

// Context class that uses flyweights
class TextCharacter {
private:
    std::shared_ptr<CharacterFlyweight> flyweight;
    int x, y;           // Extrinsic state
    int size;           // Extrinsic state
    std::string color;  // Extrinsic state

public:
    TextCharacter(std::shared_ptr<CharacterFlyweight> flyweight, int x, int y, int size, const std::string& color)
        : flyweight(flyweight), x(x), y(y), size(size), color(color) {}

    void render() const {
        flyweight->render(x, y, size, color);
    }

    void move(int newX, int newY) {
        x = newX;
        y = newY;
    }

    void resize(int newSize) {
        size = newSize;
    }

    void changeColor(const std::string& newColor) {
        color = newColor;
    }

    char getCharacter() const {
        return flyweight->getCharacter();
    }
};

// Document class
class Document {
private:
    std::vector<std::unique_ptr<TextCharacter>> characters;
    CharacterFlyweightFactory& factory;

public:
    explicit Document(CharacterFlyweightFactory& factory) : factory(factory) {}

    void addCharacter(char ch, const std::string& fontFamily, int x, int y, int size, const std::string& color) {
        auto flyweight = factory.getFlyweight(ch, fontFamily);
        characters.push_back(std::make_unique<TextCharacter>(flyweight, x, y, size, color));
    }

    void render() const {
        std::cout << "\nRendering document with " << characters.size() << " characters:" << std::endl;
        for (const auto& character : characters) {
            character->render();
        }
    }

    size_t getCharacterCount() const {
        return characters.size();
    }
};

// Example 2: Game Particle System
class ParticleFlyweight {
public:
    virtual ~ParticleFlyweight() = default;
    virtual void update(double x, double y, double vx, double vy, double life) const = 0;
    virtual std::string getType() const = 0;
};

class FireParticle : public ParticleFlyweight {
public:
    void update(double x, double y, double vx, double vy, double life) const override {
        std::cout << "🔥 Fire particle at (" << x << "," << y << ") vel=(" << vx << "," << vy << ") life=" << life << std::endl;
    }

    std::string getType() const override {
        return "Fire";
    }
};

class SmokeParticle : public ParticleFlyweight {
public:
    void update(double x, double y, double vx, double vy, double life) const override {
        std::cout << "💨 Smoke particle at (" << x << "," << y << ") vel=(" << vx << "," << vy << ") life=" << life << std::endl;
    }

    std::string getType() const override {
        return "Smoke";
    }
};

class SparkParticle : public ParticleFlyweight {
public:
    void update(double x, double y, double vx, double vy, double life) const override {
        std::cout << "✨ Spark particle at (" << x << "," << y << ") vel=(" << vx << "," << vy << ") life=" << life << std::endl;
    }

    std::string getType() const override {
        return "Spark";
    }
};

class ParticleFlyweightFactory {
private:
    std::unordered_map<std::string, std::shared_ptr<ParticleFlyweight>> flyweights;

public:
    std::shared_ptr<ParticleFlyweight> getFlyweight(const std::string& type) {
        auto it = flyweights.find(type);
        if (it != flyweights.end()) {
            return it->second;
        }

        std::shared_ptr<ParticleFlyweight> flyweight;
        if (type == "Fire") {
            flyweight = std::make_shared<FireParticle>();
        } else if (type == "Smoke") {
            flyweight = std::make_shared<SmokeParticle>();
        } else if (type == "Spark") {
            flyweight = std::make_shared<SparkParticle>();
        }

        if (flyweight) {
            flyweights[type] = flyweight;
            std::cout << "Created new " << type << " particle flyweight" << std::endl;
        }

        return flyweight;
    }

    size_t getFlyweightCount() const {
        return flyweights.size();
    }
};

class Particle {
private:
    std::shared_ptr<ParticleFlyweight> flyweight;
    double x, y;        // Extrinsic state
    double vx, vy;      // Extrinsic state
    double life;        // Extrinsic state

public:
    Particle(std::shared_ptr<ParticleFlyweight> flyweight, double x, double y, double vx, double vy, double life)
        : flyweight(flyweight), x(x), y(y), vx(vx), vy(vy), life(life) {}

    void update(double deltaTime) {
        x += vx * deltaTime;
        y += vy * deltaTime;
        life -= deltaTime;
        flyweight->update(x, y, vx, vy, life);
    }

    bool isAlive() const {
        return life > 0;
    }

    std::string getType() const {
        return flyweight->getType();
    }
};

class ParticleSystem {
private:
    std::vector<std::unique_ptr<Particle>> particles;
    ParticleFlyweightFactory& factory;
    std::mt19937 rng;

public:
    explicit ParticleSystem(ParticleFlyweightFactory& factory)
        : factory(factory), rng(std::random_device{}()) {}

    void emitParticles(const std::string& type, int count, double x, double y) {
        std::uniform_real_distribution<double> velDist(-5.0, 5.0);
        std::uniform_real_distribution<double> lifeDist(1.0, 3.0);

        for (int i = 0; i < count; ++i) {
            auto flyweight = factory.getFlyweight(type);
            if (flyweight) {
                particles.push_back(std::make_unique<Particle>(
                    flyweight, x, y, velDist(rng), velDist(rng), lifeDist(rng)
                ));
            }
        }
    }

    void update(double deltaTime) {
        std::cout << "\nUpdating " << particles.size() << " particles:" << std::endl;
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [deltaTime](auto& particle) {
                    particle->update(deltaTime);
                    return !particle->isAlive();
                }),
            particles.end()
        );
    }

    size_t getParticleCount() const {
        return particles.size();
    }
};

// Example 3: Forest Simulation with Trees
class TreeType {
public:
    virtual ~TreeType() = default;
    virtual void render(double x, double y, double scale, const std::string& season) const = 0;
    virtual std::string getSpecies() const = 0;
};

class OakTree : public TreeType {
public:
    void render(double x, double y, double scale, const std::string& season) const override {
        std::cout << "🌳 Oak tree at (" << x << "," << y << ") scale=" << scale << " season=" << season << std::endl;
    }

    std::string getSpecies() const override {
        return "Oak";
    }
};

class PineTree : public TreeType {
public:
    void render(double x, double y, double scale, const std::string& season) const override {
        std::cout << "🌲 Pine tree at (" << x << "," << y << ") scale=" << scale << " season=" << season << std::endl;
    }

    std::string getSpecies() const override {
        return "Pine";
    }
};

class BirchTree : public TreeType {
public:
    void render(double x, double y, double scale, const std::string& season) const override {
        std::cout << "🌿 Birch tree at (" << x << "," << y << ") scale=" << scale << " season=" << season << std::endl;
    }

    std::string getSpecies() const override {
        return "Birch";
    }
};

class TreeTypeFactory {
private:
    std::unordered_map<std::string, std::shared_ptr<TreeType>> treeTypes;

public:
    std::shared_ptr<TreeType> getTreeType(const std::string& species) {
        auto it = treeTypes.find(species);
        if (it != treeTypes.end()) {
            return it->second;
        }

        std::shared_ptr<TreeType> treeType;
        if (species == "Oak") {
            treeType = std::make_shared<OakTree>();
        } else if (species == "Pine") {
            treeType = std::make_shared<PineTree>();
        } else if (species == "Birch") {
            treeType = std::make_shared<BirchTree>();
        }

        if (treeType) {
            treeTypes[species] = treeType;
            std::cout << "Created new " << species << " tree type flyweight" << std::endl;
        }

        return treeType;
    }

    size_t getTreeTypeCount() const {
        return treeTypes.size();
    }
};

class Tree {
private:
    std::shared_ptr<TreeType> treeType;
    double x, y;        // Extrinsic state
    double scale;       // Extrinsic state

public:
    Tree(std::shared_ptr<TreeType> treeType, double x, double y, double scale)
        : treeType(treeType), x(x), y(y), scale(scale) {}

    void render(const std::string& season) const {
        treeType->render(x, y, scale, season);
    }

    std::string getSpecies() const {
        return treeType->getSpecies();
    }
};

class Forest {
private:
    std::vector<std::unique_ptr<Tree>> trees;
    TreeTypeFactory& factory;

public:
    explicit Forest(TreeTypeFactory& factory) : factory(factory) {}

    void plantTree(const std::string& species, double x, double y, double scale) {
        auto treeType = factory.getTreeType(species);
        if (treeType) {
            trees.push_back(std::make_unique<Tree>(treeType, x, y, scale));
        }
    }

    void renderForest(const std::string& season) const {
        std::cout << "\nRendering forest in " << season << " with " << trees.size() << " trees:" << std::endl;
        for (const auto& tree : trees) {
            tree->render(season);
        }
    }

    void getStatistics() const {
        std::unordered_map<std::string, int> speciesCount;
        for (const auto& tree : trees) {
            speciesCount[tree->getSpecies()]++;
        }

        std::cout << "\nForest Statistics:" << std::endl;
        std::cout << "Total trees: " << trees.size() << std::endl;
        std::cout << "Tree types (flyweights): " << factory.getTreeTypeCount() << std::endl;
        std::cout << "Species distribution:" << std::endl;
        for (const auto& [species, count] : speciesCount) {
            std::cout << "  " << species << ": " << count << " trees" << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Flyweight Pattern Demo ===" << std::endl;

    // Example 1: Text Document System
    std::cout << "\n1. Text Document System:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    CharacterFlyweightFactory charFactory;
    Document document(charFactory);

    // Add text "Hello World!" with different fonts and styles
    std::string text = "Hello World!";
    std::vector<std::string> fonts = {"Arial", "Times", "Courier"};
    std::vector<std::string> colors = {"red", "blue", "green", "black"};

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> fontDist(0, fonts.size() - 1);
    std::uniform_int_distribution<int> colorDist(0, colors.size() - 1);
    std::uniform_int_distribution<int> sizeDist(12, 24);

    for (size_t i = 0; i < text.length(); ++i) {
        document.addCharacter(
            text[i],
            fonts[fontDist(rng)],
            static_cast<int>(i * 10),
            10,
            sizeDist(rng),
            colors[colorDist(rng)]
        );
    }

    std::cout << "\nDocument created with " << document.getCharacterCount() << " characters" << std::endl;
    charFactory.listFlyweights();

    // Render a sample
    std::cout << "\nSample rendering (first 5 characters):" << std::endl;
    document.render();

    // Example 2: Particle System
    std::cout << "\n\n2. Game Particle System:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    ParticleFlyweightFactory particleFactory;
    ParticleSystem particleSystem(particleFactory);

    // Emit different types of particles
    particleSystem.emitParticles("Fire", 3, 100.0, 50.0);
    particleSystem.emitParticles("Smoke", 2, 110.0, 60.0);
    particleSystem.emitParticles("Spark", 4, 90.0, 40.0);
    particleSystem.emitParticles("Fire", 2, 105.0, 55.0); // Reuses Fire flyweight

    std::cout << "\nParticle system created with " << particleSystem.getParticleCount()
              << " particles using " << particleFactory.getFlyweightCount() << " flyweights" << std::endl;

    // Update particles
    particleSystem.update(0.5);

    // Example 3: Forest Simulation
    std::cout << "\n\n3. Forest Simulation:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    TreeTypeFactory treeFactory;
    Forest forest(treeFactory);

    // Plant many trees
    std::vector<std::string> species = {"Oak", "Pine", "Birch"};
    std::uniform_int_distribution<int> speciesDist(0, species.size() - 1);
    std::uniform_real_distribution<double> posDist(0.0, 100.0);
    std::uniform_real_distribution<double> scaleDist(0.5, 2.0);

    // Plant 20 trees randomly
    for (int i = 0; i < 20; ++i) {
        forest.plantTree(
            species[speciesDist(rng)],
            posDist(rng),
            posDist(rng),
            scaleDist(rng)
        );
    }

    forest.getStatistics();

    // Render forest in different seasons
    forest.renderForest("Spring");

    // Memory efficiency demonstration
    std::cout << "\n\n4. Memory Efficiency Demonstration:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::cout << "Without Flyweight Pattern:" << std::endl;
    std::cout << "- Each character object would store font family, character, position, size, color" << std::endl;
    std::cout << "- For " << document.getCharacterCount() << " characters: ~"
              << document.getCharacterCount() * 50 << " bytes (estimated)" << std::endl;

    std::cout << "\nWith Flyweight Pattern:" << std::endl;
    std::cout << "- Character flyweights: " << charFactory.getFlyweightCount() << " objects" << std::endl;
    std::cout << "- Context objects: " << document.getCharacterCount() << " objects" << std::endl;
    std::cout << "- Memory saved by sharing intrinsic state across similar characters" << std::endl;

    std::cout << "\nSame principle applies to:" << std::endl;
    std::cout << "- Particle system: " << particleFactory.getFlyweightCount()
              << " flyweights for " << particleSystem.getParticleCount() << " particles" << std::endl;
    std::cout << "- Forest simulation: " << treeFactory.getTreeTypeCount()
              << " tree types for 20 trees" << std::endl;

    return 0;
}
