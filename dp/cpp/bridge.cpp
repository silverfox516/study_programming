#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

// Implementation interface (Bridge)
class DrawingAPI {
public:
    virtual ~DrawingAPI() = default;
    virtual void drawLine(double x1, double y1, double x2, double y2) = 0;
    virtual void drawCircle(double x, double y, double radius) = 0;
    virtual void drawRectangle(double x, double y, double width, double height) = 0;
    virtual void setColor(const std::string& color) = 0;
    virtual std::string getRendererName() const = 0;
};

// Concrete Implementation 1: OpenGL
class OpenGLRenderer : public DrawingAPI {
private:
    std::string currentColor = "white";

public:
    void drawLine(double x1, double y1, double x2, double y2) override {
        std::cout << "[OpenGL] Drawing line from (" << x1 << "," << y1
                  << ") to (" << x2 << "," << y2 << ") in " << currentColor << std::endl;
    }

    void drawCircle(double x, double y, double radius) override {
        std::cout << "[OpenGL] Drawing circle at (" << x << "," << y
                  << ") with radius " << radius << " in " << currentColor << std::endl;
    }

    void drawRectangle(double x, double y, double width, double height) override {
        std::cout << "[OpenGL] Drawing rectangle at (" << x << "," << y
                  << ") size " << width << "x" << height << " in " << currentColor << std::endl;
    }

    void setColor(const std::string& color) override {
        currentColor = color;
        std::cout << "[OpenGL] Color set to " << color << std::endl;
    }

    std::string getRendererName() const override {
        return "OpenGL";
    }
};

// Concrete Implementation 2: DirectX
class DirectXRenderer : public DrawingAPI {
private:
    std::string currentColor = "white";

public:
    void drawLine(double x1, double y1, double x2, double y2) override {
        std::cout << "[DirectX] Rendering line: (" << x1 << "," << y1
                  << ")->(" << x2 << "," << y2 << ") color=" << currentColor << std::endl;
    }

    void drawCircle(double x, double y, double radius) override {
        std::cout << "[DirectX] Rendering circle: center=(" << x << "," << y
                  << ") r=" << radius << " color=" << currentColor << std::endl;
    }

    void drawRectangle(double x, double y, double width, double height) override {
        std::cout << "[DirectX] Rendering rectangle: pos=(" << x << "," << y
                  << ") size=" << width << "x" << height << " color=" << currentColor << std::endl;
    }

    void setColor(const std::string& color) override {
        currentColor = color;
        std::cout << "[DirectX] Color changed to " << color << std::endl;
    }

    std::string getRendererName() const override {
        return "DirectX";
    }
};

// Concrete Implementation 3: SVG
class SVGRenderer : public DrawingAPI {
private:
    std::string currentColor = "black";
    std::ostringstream svgContent;

public:
    void drawLine(double x1, double y1, double x2, double y2) override {
        svgContent << "<line x1=\"" << x1 << "\" y1=\"" << y1
                   << "\" x2=\"" << x2 << "\" y2=\"" << y2
                   << "\" stroke=\"" << currentColor << "\"/>\n";
        std::cout << "[SVG] Added line element" << std::endl;
    }

    void drawCircle(double x, double y, double radius) override {
        svgContent << "<circle cx=\"" << x << "\" cy=\"" << y
                   << "\" r=\"" << radius << "\" fill=\"" << currentColor << "\"/>\n";
        std::cout << "[SVG] Added circle element" << std::endl;
    }

    void drawRectangle(double x, double y, double width, double height) override {
        svgContent << "<rect x=\"" << x << "\" y=\"" << y
                   << "\" width=\"" << width << "\" height=\"" << height
                   << "\" fill=\"" << currentColor << "\"/>\n";
        std::cout << "[SVG] Added rectangle element" << std::endl;
    }

    void setColor(const std::string& color) override {
        currentColor = color;
        std::cout << "[SVG] Color set to " << color << std::endl;
    }

    std::string getRendererName() const override {
        return "SVG";
    }

    std::string getSVGContent() const {
        return "<svg>\n" + svgContent.str() + "</svg>";
    }
};

// Abstraction: Shape
class Shape {
protected:
    std::unique_ptr<DrawingAPI> drawingAPI;
    double x, y;

public:
    Shape(std::unique_ptr<DrawingAPI> api, double x = 0, double y = 0)
        : drawingAPI(std::move(api)), x(x), y(y) {}

    virtual ~Shape() = default;
    virtual void draw() = 0;
    virtual void move(double newX, double newY) {
        x = newX;
        y = newY;
    }

    void setColor(const std::string& color) {
        drawingAPI->setColor(color);
    }

    std::string getRendererName() const {
        return drawingAPI->getRendererName();
    }
};

// Refined Abstraction: Circle
class Circle : public Shape {
private:
    double radius;

public:
    Circle(std::unique_ptr<DrawingAPI> api, double x, double y, double radius)
        : Shape(std::move(api), x, y), radius(radius) {}

    void draw() override {
        drawingAPI->drawCircle(x, y, radius);
    }

    void setRadius(double newRadius) {
        radius = newRadius;
    }

    double getRadius() const {
        return radius;
    }
};

// Refined Abstraction: Rectangle
class Rectangle : public Shape {
private:
    double width, height;

public:
    Rectangle(std::unique_ptr<DrawingAPI> api, double x, double y, double width, double height)
        : Shape(std::move(api), x, y), width(width), height(height) {}

    void draw() override {
        drawingAPI->drawRectangle(x, y, width, height);
    }

    void resize(double newWidth, double newHeight) {
        width = newWidth;
        height = newHeight;
    }
};

// Refined Abstraction: Line
class Line : public Shape {
private:
    double x2, y2;

public:
    Line(std::unique_ptr<DrawingAPI> api, double x1, double y1, double x2, double y2)
        : Shape(std::move(api), x1, y1), x2(x2), y2(y2) {}

    void draw() override {
        drawingAPI->drawLine(x, y, x2, y2);
    }

    void setEndPoint(double newX2, double newY2) {
        x2 = newX2;
        y2 = newY2;
    }
};

// Complex shape composed of basic shapes
class House : public Shape {
private:
    std::unique_ptr<Rectangle> base;
    std::unique_ptr<Rectangle> roof;
    std::unique_ptr<Rectangle> door;
    std::unique_ptr<Circle> window;

public:
    House(std::unique_ptr<DrawingAPI> api, double x, double y)
        : Shape(std::move(api), x, y) {
        // Create a copy of the drawing API for each component
        // Note: In a real implementation, you might want to share the API or use a different approach
    }

    void draw() override {
        std::cout << "Drawing house at (" << x << "," << y << ") using "
                  << drawingAPI->getRendererName() << std::endl;

        // Draw base
        drawingAPI->setColor("brown");
        drawingAPI->drawRectangle(x, y, 100, 80);

        // Draw roof
        drawingAPI->setColor("red");
        drawingAPI->drawRectangle(x - 10, y + 80, 120, 40);

        // Draw door
        drawingAPI->setColor("darkbrown");
        drawingAPI->drawRectangle(x + 40, y, 20, 50);

        // Draw window
        drawingAPI->setColor("lightblue");
        drawingAPI->drawCircle(x + 20, y + 60, 8);
        drawingAPI->drawCircle(x + 80, y + 60, 8);
    }
};

// Example with different rendering backends for notification system
class NotificationSender {
public:
    virtual ~NotificationSender() = default;
    virtual void sendNotification(const std::string& title, const std::string& message) = 0;
    virtual std::string getChannelName() const = 0;
};

class EmailSender : public NotificationSender {
public:
    void sendNotification(const std::string& title, const std::string& message) override {
        std::cout << "[EMAIL] To: user@example.com" << std::endl;
        std::cout << "[EMAIL] Subject: " << title << std::endl;
        std::cout << "[EMAIL] Body: " << message << std::endl;
    }

    std::string getChannelName() const override {
        return "Email";
    }
};

class SMSSender : public NotificationSender {
public:
    void sendNotification(const std::string& title, const std::string& message) override {
        std::cout << "[SMS] " << title << ": " << message << " (160 char limit)" << std::endl;
    }

    std::string getChannelName() const override {
        return "SMS";
    }
};

class PushNotificationSender : public NotificationSender {
public:
    void sendNotification(const std::string& title, const std::string& message) override {
        std::cout << "[PUSH] 📱 " << title << std::endl;
        std::cout << "[PUSH] " << message << std::endl;
    }

    std::string getChannelName() const override {
        return "Push Notification";
    }
};

// Abstraction for notifications
class Notification {
protected:
    std::unique_ptr<NotificationSender> sender;
    std::string title;
    std::string message;

public:
    Notification(std::unique_ptr<NotificationSender> sender,
                const std::string& title, const std::string& message)
        : sender(std::move(sender)), title(title), message(message) {}

    virtual ~Notification() = default;
    virtual void send() = 0;

    std::string getChannelName() const {
        return sender->getChannelName();
    }
};

// Simple notification
class SimpleNotification : public Notification {
public:
    SimpleNotification(std::unique_ptr<NotificationSender> sender,
                      const std::string& title, const std::string& message)
        : Notification(std::move(sender), title, message) {}

    void send() override {
        sender->sendNotification(title, message);
    }
};

// Urgent notification with different formatting
class UrgentNotification : public Notification {
public:
    UrgentNotification(std::unique_ptr<NotificationSender> sender,
                      const std::string& title, const std::string& message)
        : Notification(std::move(sender), title, message) {}

    void send() override {
        std::string urgentTitle = "🚨 URGENT: " + title;
        std::string urgentMessage = "⚠️ " + message + " ⚠️";
        sender->sendNotification(urgentTitle, urgentMessage);
    }
};

int main() {
    std::cout << "=== Bridge Pattern Demo ===" << std::endl;

    // Example 1: Graphics rendering with different APIs
    std::cout << "\n1. Graphics Rendering Bridge:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    std::vector<std::unique_ptr<Shape>> shapes;

    // Create shapes with different rendering backends
    shapes.push_back(std::make_unique<Circle>(
        std::make_unique<OpenGLRenderer>(), 10, 10, 5));
    shapes.push_back(std::make_unique<Rectangle>(
        std::make_unique<DirectXRenderer>(), 20, 20, 15, 10));
    shapes.push_back(std::make_unique<Line>(
        std::make_unique<SVGRenderer>(), 0, 0, 50, 50));

    for (auto& shape : shapes) {
        std::cout << "\nUsing " << shape->getRendererName() << ":" << std::endl;
        shape->setColor("blue");
        shape->draw();
    }

    // Complex shape example
    std::cout << "\n2. Complex Shape (House):" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    auto house1 = std::make_unique<House>(std::make_unique<OpenGLRenderer>(), 100, 100);
    auto house2 = std::make_unique<House>(std::make_unique<DirectXRenderer>(), 200, 100);

    house1->draw();
    std::cout << std::endl;
    house2->draw();

    // SVG content example
    std::cout << "\n3. SVG Content Generation:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    auto svgRenderer = std::make_unique<SVGRenderer>();
    auto svgRendererPtr = svgRenderer.get(); // Keep a pointer for later use

    auto svgCircle = std::make_unique<Circle>(std::move(svgRenderer), 50, 50, 25);
    svgCircle->setColor("red");
    svgCircle->draw();

    std::cout << "\nGenerated SVG:" << std::endl;
    std::cout << svgRendererPtr->getSVGContent() << std::endl;

    // Example 2: Notification system bridge
    std::cout << "\n4. Notification System Bridge:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    std::vector<std::unique_ptr<Notification>> notifications;

    // Create different types of notifications with different senders
    notifications.push_back(std::make_unique<SimpleNotification>(
        std::make_unique<EmailSender>(),
        "Welcome",
        "Thank you for signing up!"));

    notifications.push_back(std::make_unique<UrgentNotification>(
        std::make_unique<SMSSender>(),
        "Security Alert",
        "Suspicious login detected"));

    notifications.push_back(std::make_unique<SimpleNotification>(
        std::make_unique<PushNotificationSender>(),
        "New Message",
        "You have 3 unread messages"));

    notifications.push_back(std::make_unique<UrgentNotification>(
        std::make_unique<EmailSender>(),
        "System Maintenance",
        "Service will be down for 2 hours"));

    for (auto& notification : notifications) {
        std::cout << "\nSending via " << notification->getChannelName() << ":" << std::endl;
        notification->send();
    }

    // Runtime renderer switching example
    std::cout << "\n5. Runtime Renderer Switching:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    auto circle = std::make_unique<Circle>(std::make_unique<OpenGLRenderer>(), 25, 25, 10);

    std::cout << "Original renderer:" << std::endl;
    circle->setColor("green");
    circle->draw();

    // Note: In a real implementation, you might want to provide a method to change the renderer
    // For demonstration, we'll create a new circle with a different renderer
    std::cout << "\nSwitching to DirectX renderer:" << std::endl;
    auto circle2 = std::make_unique<Circle>(std::make_unique<DirectXRenderer>(), 25, 25, 10);
    circle2->setColor("green");
    circle2->draw();

    return 0;
}