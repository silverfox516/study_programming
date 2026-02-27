# Proxy Pattern

## 개요
Proxy 패턴은 다른 객체에 대한 접근을 제어하기 위해 대리자나 자리채움자 역할을 하는 구조 패턴입니다. 실제 객체에 대한 참조를 제공하면서 추가적인 기능을 제공할 수 있습니다.

## C 언어 구현
```c
typedef struct Image {
    void (*display)(struct Image* self);
    void (*destroy)(struct Image* self);
} Image;

typedef struct RealImage {
    Image base;
    char* filename;
} RealImage;

typedef struct ImageProxy {
    Image base;
    char* filename;
    RealImage* real_image;
} ImageProxy;

void real_image_display(Image* self) {
    RealImage* real = (RealImage*)self;
    printf("Displaying image: %s\n", real->filename);
}

void real_image_load(RealImage* real) {
    printf("Loading image from disk: %s\n", real->filename);
    // 실제로는 파일에서 이미지 로딩
}

RealImage* create_real_image(const char* filename) {
    RealImage* real = malloc(sizeof(RealImage));
    real->base.display = real_image_display;
    real->base.destroy = real_image_destroy;
    real->filename = malloc(strlen(filename) + 1);
    strcpy(real->filename, filename);

    real_image_load(real);  // 생성시 로딩
    return real;
}

void image_proxy_display(Image* self) {
    ImageProxy* proxy = (ImageProxy*)self;

    // 지연 로딩: 실제로 필요할 때만 RealImage 생성
    if (proxy->real_image == NULL) {
        proxy->real_image = create_real_image(proxy->filename);
    }

    proxy->real_image->base.display((Image*)proxy->real_image);
}

ImageProxy* create_image_proxy(const char* filename) {
    ImageProxy* proxy = malloc(sizeof(ImageProxy));
    proxy->base.display = image_proxy_display;
    proxy->base.destroy = image_proxy_destroy;
    proxy->filename = malloc(strlen(filename) + 1);
    strcpy(proxy->filename, filename);
    proxy->real_image = NULL;  // 지연 로딩을 위해 NULL로 초기화

    return proxy;
}
```

## 사용 예제
```c
// 프록시 생성 (실제 이미지는 아직 로딩되지 않음)
Image* image1 = (Image*)create_image_proxy("photo1.jpg");
Image* image2 = (Image*)create_image_proxy("photo2.jpg");

printf("프록시 생성됨\n");

// 실제 사용할 때 이미지가 로딩됨
image1->display(image1);  // Loading image from disk: photo1.jpg
                         // Displaying image: photo1.jpg

image2->display(image2);  // Loading image from disk: photo2.jpg
                         // Displaying image: photo2.jpg
```

## 적용 상황
- **지연 로딩**: 리소스가 실제로 필요할 때까지 로딩을 지연
- **접근 제어**: 원본 객체에 대한 접근 권한 검사
- **캐싱**: 비용이 많이 드는 연산 결과를 캐시
- **로깅**: 원본 객체 호출을 기록