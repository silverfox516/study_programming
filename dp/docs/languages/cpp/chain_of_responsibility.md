# Chain of Responsibility Pattern - C++ Implementation

## 개요

Chain of Responsibility 패턴은 요청을 처리할 수 있는 객체들의 체인을 따라 요청을 전달하는 행동적 디자인 패턴입니다. 각 핸들러는 요청을 처리하거나 체인의 다음 핸들러로 전달할지를 결정합니다.

C++에서는 스마트 포인터, 함수 포인터, 람다 표현식을 활용하여 유연하고 효율적인 처리 체인을 구현할 수 있습니다.

## 구조

### 핵심 구성 요소

1. **Handler** (RequestHandler, SupportHandler, HTTPHandler)
   - 요청 처리를 위한 공통 인터페이스
   - 다음 핸들러에 대한 참조 유지
   - 처리 또는 전달 로직 포함

2. **Concrete Handler** (AuthenticationHandler, Level1SupportHandler, CacheHandler 등)
   - Handler 인터페이스의 구체적 구현
   - 특정 타입의 요청만 처리
   - 처리할 수 없으면 다음 핸들러로 전달

3. **Client**
   - 체인의 첫 번째 핸들러로 요청 전송
   - 어떤 핸들러가 처리할지 알 필요 없음

## C++ 구현

### 1. 기본 Handler 인터페이스

```cpp
class RequestHandler {
protected:
    std::unique_ptr<RequestHandler> nextHandler;

public:
    virtual ~RequestHandler() = default;

    void setNext(std::unique_ptr<RequestHandler> handler) {
        nextHandler = std::move(handler);  // 스마트 포인터로 안전한 소유권 이전
    }

    virtual void handleRequest(const std::string& request) {
        if (canHandle(request)) {
            processRequest(request);
        } else if (nextHandler) {
            std::cout << "Passing request to next handler..." << std::endl;
            nextHandler->handleRequest(request);
        } else {
            std::cout << "❌ No handler found for request: " << request << std::endl;
        }
    }

protected:
    virtual bool canHandle(const std::string& request) = 0;
    virtual void processRequest(const std::string& request) = 0;
    virtual std::string getHandlerName() const = 0;
};
```

### 2. 인증 체인 구현

#### 인증 핸들러
```cpp
class AuthenticationHandler : public RequestHandler {
private:
    std::string validUser = "admin";
    std::string validPassword = "password123";

protected:
    bool canHandle(const std::string& request) override {
        return request.find("auth:") == 0;
    }

    void processRequest(const std::string& request) override {
        std::cout << "🔐 " << getHandlerName() << " processing: " << request << std::endl;

        // 요청에서 자격증명 추출
        std::string credentials = request.substr(5); // "auth:" 제거
        size_t colonPos = credentials.find(':');

        if (colonPos != std::string::npos) {
            std::string user = credentials.substr(0, colonPos);
            std::string password = credentials.substr(colonPos + 1);

            if (user == validUser && password == validPassword) {
                std::cout << "✅ Authentication successful for user: " << user << std::endl;
            } else {
                std::cout << "❌ Authentication failed!" << std::endl;
            }
        }
    }

    std::string getHandlerName() const override {
        return "Authentication Handler";
    }
};
```

#### 권한 부여 핸들러
```cpp
class AuthorizationHandler : public RequestHandler {
private:
    std::vector<std::string> authorizedActions = {"read", "write", "delete"};

protected:
    bool canHandle(const std::string& request) override {
        return request.find("authorize:") == 0;
    }

    void processRequest(const std::string& request) override {
        std::cout << "🛡️ " << getHandlerName() << " processing: " << request << std::endl;

        std::string action = request.substr(10); // "authorize:" 제거

        auto it = std::find(authorizedActions.begin(), authorizedActions.end(), action);
        if (it != authorizedActions.end()) {
            std::cout << "✅ Action '" << action << "' is authorized" << std::endl;
        } else {
            std::cout << "❌ Action '" << action << "' is not authorized!" << std::endl;
        }
    }

    std::string getHandlerName() const override {
        return "Authorization Handler";
    }
};
```

### 3. 지원 티켓 시스템

#### 티켓 구조체
```cpp
enum class TicketPriority {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

struct SupportTicket {
    int id;
    std::string description;
    TicketPriority priority;
    std::string category;

    std::string toString() const {
        std::string priorityStr;
        switch (priority) {
            case TicketPriority::LOW: priorityStr = "LOW"; break;
            case TicketPriority::MEDIUM: priorityStr = "MEDIUM"; break;
            case TicketPriority::HIGH: priorityStr = "HIGH"; break;
            case TicketPriority::CRITICAL: priorityStr = "CRITICAL"; break;
        }
        return "Ticket #" + std::to_string(id) + " [" + priorityStr + "] " +
               category + ": " + description;
    }
};
```

#### 레벨별 지원 핸들러
```cpp
class Level1SupportHandler : public SupportHandler {
protected:
    bool canHandle(const SupportTicket& ticket) override {
        return ticket.priority <= TicketPriority::MEDIUM &&
               (ticket.category == "General" || ticket.category == "Account");
    }

    void processTicket(const SupportTicket& ticket) override {
        std::cout << "🎧 " << getHandlerLevel() << " handling: " << ticket.toString() << std::endl;
        std::cout << "   Providing basic troubleshooting steps..." << std::endl;
        std::cout << "   ✅ Ticket resolved by Level 1 Support" << std::endl;
    }

    std::string getHandlerLevel() const override {
        return "Level 1 Support";
    }
};

class Level3SupportHandler : public SupportHandler {
protected:
    bool canHandle(const SupportTicket& ticket) override {
        return ticket.priority == TicketPriority::CRITICAL ||
               ticket.category == "Security" || ticket.category == "Infrastructure";
    }

    void processTicket(const SupportTicket& ticket) override {
        std::cout << "🚨 " << getHandlerLevel() << " handling: " << ticket.toString() << std::endl;
        std::cout << "   Engaging senior engineers..." << std::endl;
        std::cout << "   ✅ Critical issue handled by Level 3 Support" << std::endl;
    }

    std::string getHandlerLevel() const override {
        return "Level 3 Support";
    }
};
```

### 4. HTTP 요청 처리 체인

#### HTTP 요청 구조체
```cpp
struct HTTPRequest {
    std::string method;
    std::string path;
    std::string userAgent;
    std::string clientIP;
    std::map<std::string, std::string> headers;

    std::string toString() const {
        return method + " " + path + " from " + clientIP;
    }
};
```

#### 속도 제한 핸들러
```cpp
class RateLimitHandler : public HTTPHandler {
private:
    std::map<std::string, int> requestCounts;
    int maxRequestsPerMinute = 100;

protected:
    bool shouldHandle(const HTTPRequest& request) override {
        return true; // 항상 속도 제한 확인
    }

    bool processRequest(HTTPRequest& request) override {
        std::cout << "🚦 " << getHandlerName() << " checking: " << request.toString() << std::endl;

        requestCounts[request.clientIP]++;

        if (requestCounts[request.clientIP] > maxRequestsPerMinute) {
            std::cout << "   ❌ Rate limit exceeded for IP: " << request.clientIP << std::endl;
            return false; // 요청 차단
        }

        std::cout << "   ✅ Rate limit OK (" << requestCounts[request.clientIP]
                  << "/" << maxRequestsPerMinute << ")" << std::endl;
        return true; // 다음 핸들러로 계속
    }

    std::string getHandlerName() const override {
        return "Rate Limit Handler";
    }
};
```

#### 캐시 핸들러
```cpp
class CacheHandler : public HTTPHandler {
private:
    std::map<std::string, std::string> cache;

protected:
    bool shouldHandle(const HTTPRequest& request) override {
        return request.method == "GET";
    }

    bool processRequest(HTTPRequest& request) override {
        std::cout << "💾 " << getHandlerName() << " checking: " << request.toString() << std::endl;

        auto it = cache.find(request.path);
        if (it != cache.end()) {
            std::cout << "   ✅ Cache hit for: " << request.path << std::endl;
            std::cout << "   Returning cached response: " << it->second << std::endl;
            return false; // 캐시된 응답 반환, 처리 중단
        } else {
            std::cout << "   ❌ Cache miss for: " << request.path << std::endl;
            // 캐시에 추가하는 시뮬레이션
            cache[request.path] = "Cached response for " + request.path;
            return true; // 계속 처리
        }
    }

    std::string getHandlerName() const override {
        return "Cache Handler";
    }
};
```

### 5. 함수형 Chain of Responsibility

```cpp
template<typename Request>
class FunctionalChain {
private:
    using Handler = std::function<bool(const Request&)>;
    std::vector<Handler> handlers;

public:
    void addHandler(Handler handler) {
        handlers.push_back(handler);
    }

    bool process(const Request& request) {
        for (const auto& handler : handlers) {
            if (handler(request)) {
                return true; // 요청 처리됨
            }
        }
        return false; // 어떤 핸들러도 요청을 처리하지 못함
    }
};
```

## 사용 예제

### 1. 인증/권한 체인
```cpp
auto authHandler = std::make_unique<AuthenticationHandler>();
auto authzHandler = std::make_unique<AuthorizationHandler>();

// 체인 구축
authHandler->setNext(std::move(authzHandler));

// 요청 처리
authHandler->handleRequest("auth:admin:password123");
authHandler->handleRequest("authorize:read");
```

### 2. 지원 티켓 처리
```cpp
auto level1 = std::make_unique<Level1SupportHandler>();
auto level2 = std::make_unique<Level2SupportHandler>();
auto level3 = std::make_unique<Level3SupportHandler>();

// 체인 구축
level1->setNext(std::move(level2));
level2->setNext(std::move(level3));

// 티켓 처리
SupportTicket ticket{1, "Password reset", TicketPriority::LOW, "Account"};
level1->handleTicket(ticket);
```

### 3. HTTP 요청 처리
```cpp
auto rateLimiter = std::make_unique<RateLimitHandler>();
auto httpAuth = std::make_unique<AuthenticationHTTPHandler>();
auto cache = std::make_unique<CacheHandler>();
auto router = std::make_unique<RouteHandler>();

// HTTP 처리 체인 구축
rateLimiter->setNext(std::move(httpAuth));
// ... 체인 연결

HTTPRequest request{"GET", "/api/users", "curl/7.68.0", "192.168.1.1",
                   {{"Authorization", "Bearer valid_token"}}};
rateLimiter->handle(request);
```

### 4. 함수형 체인 (람다 사용)
```cpp
FunctionalChain<std::string> functionalChain;

// 이메일 핸들러 추가
functionalChain.addHandler([](const std::string& request) -> bool {
    if (request.find("email:") == 0) {
        std::string email = request.substr(6);
        std::cout << "📧 Email handler processing: " << email << std::endl;
        if (email.find("@") != std::string::npos) {
            std::cout << "   ✅ Valid email format" << std::endl;
            return true;
        } else {
            std::cout << "   ❌ Invalid email format" << std::endl;
            return false;
        }
    }
    return false;
});

// 사용
functionalChain.process("email:user@example.com");
```

### 실행 결과
```
Processing request: auth:admin:password123
🔐 Authentication Handler processing: auth:admin:password123
✅ Authentication successful for user: admin

New ticket: Ticket #3 [CRITICAL] Infrastructure: Server down
🚨 Level 3 Support handling: Ticket #3 [CRITICAL] Infrastructure: Server down
   Engaging senior engineers...
   ✅ Critical issue handled by Level 3 Support

Processing HTTP request: GET /api/users from 192.168.1.2
🚦 Rate Limit Handler checking: GET /api/users from 192.168.1.2
   ✅ Rate limit OK (1/100)
🔑 Authentication Handler checking: GET /api/users from 192.168.1.2
   ✅ Authentication successful
```

## C++의 장점

### 1. 스마트 포인터를 통한 안전한 메모리 관리
```cpp
std::unique_ptr<RequestHandler> nextHandler;  // 자동 메모리 관리
nextHandler = std::move(handler);             // 안전한 소유권 이전
```

### 2. 템플릿을 활용한 재사용 가능한 함수형 체인
```cpp
template<typename Request>
class FunctionalChain { /* ... */ };
```

### 3. 람다와 std::function으로 유연한 핸들러 정의
```cpp
using Handler = std::function<bool(const Request&)>;
```

### 4. STL 컨테이너 활용
```cpp
std::vector<Handler> handlers;           // 핸들러 목록
std::map<std::string, int> requestCounts; // 상태 관리
```

### 5. enum class로 타입 안전한 열거형
```cpp
enum class TicketPriority { LOW = 1, MEDIUM = 2, HIGH = 3, CRITICAL = 4 };
```

### 6. 가상 소멸자로 안전한 다형성
```cpp
virtual ~RequestHandler() = default;  // 안전한 다형적 소멸
```

### 7. 자동 타입 추론
```cpp
auto authHandler = std::make_unique<AuthenticationHandler>();
```

## 적용 상황

### 1. 요청 처리 파이프라인
- 웹 서버의 미들웨어 체인
- 인증, 권한 부여, 로깅, 캐싱
- API 게이트웨이

### 2. 이벤트 처리
- GUI 이벤트 처리 체인
- 게임의 입력 처리
- 시스템 이벤트 핸들링

### 3. 데이터 검증
- 입력 데이터 유효성 검사
- 다단계 필터링
- 데이터 변환 파이프라인

### 4. 지원 시스템
- 고객 지원 티켓 라우팅
- 에러 처리 및 복구
- 알림 전송 체인

### 5. 파싱 및 컴파일러
- 구문 분석기
- 코드 생성 파이프라인
- 최적화 패스

### 6. 게임 개발
- 명령 처리 체인
- AI 의사결정 트리
- 충돌 감지 및 처리

## 장점과 단점

### 장점
1. **느슨한 결합**: 발신자와 수신자의 분리
2. **동적 구성**: 런타임에 체인 변경 가능
3. **단일 책임 원칙**: 각 핸들러는 하나의 책임만
4. **확장성**: 새로운 핸들러 쉽게 추가
5. **유연성**: 처리 순서 변경 용이

### 단점
1. **실행 보장 없음**: 요청이 처리되지 않을 수 있음
2. **런타임 성능**: 체인을 따라 순회하는 오버헤드
3. **디버깅 어려움**: 실제 처리 위치 추적 곤란
4. **설정 복잡성**: 올바른 체인 구성 필요

## 관련 패턴

- **Command**: 요청을 객체로 캡슐화 (Chain과 함께 사용 가능)
- **Composite**: 트리 구조에서 요청 처리 (vs Chain: 선형 구조)
- **Decorator**: 객체에 기능 추가 (vs Chain: 처리 위임)
- **Strategy**: 알고리즘 선택 (vs Chain: 처리 체인)

Chain of Responsibility 패턴은 복잡한 요청 처리 로직을 단순하고 유연하게 만들어주며, 특히 미들웨어 아키텍처나 이벤트 처리 시스템에서 매우 유용합니다.