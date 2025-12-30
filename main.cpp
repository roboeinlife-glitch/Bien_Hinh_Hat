#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <random>
const int WIDTH = 1200;
const int HEIGHT = 800;
const float PI = 3.14159265358979323846f;
struct Particle3D {
    sf::Vector3f position;
    sf::Vector3f originalPosition;
    sf::Vector3f velocity;
    sf::Color color;
    float size;
    float angle;
    float speed;
    bool isOrbiting;
    sf::Vector3f orbitCenter;
    float orbitRadius;
    float orbitAngle;
    float orbitSpeed;
};
struct TrailPoint {
    sf::Vector3f position;
    sf::Color color;
    float lifetime;
};
class ParticleMorph3D {
private:
    sf::RenderWindow window;
    sf::Clock clock;
    sf::Font font;
    sf::Text infoText;
    std::vector<Particle3D> particles;
    std::vector<TrailPoint> trails;
    std::vector<sf::VertexArray> trailRender;
    enum ShapeType {
        SPHERE_3D,
        HOLLOW_CUBE,
        FIGURE8_SPIRAL,
        ATOMIC_MODEL,
        HEART_3D,
        DOUBLE_HELIX,
        TOTAL_SHAPES
    };
    ShapeType currentShape;
    float shapeTransition;
    bool isTransitioning;
    // Camera
    float cameraDistance;
    float cameraAngleX, cameraAngleY;
    float cameraAngleZ;
    bool autoRotate;
    sf::Vector2f lastMousePos;
    bool isMouseDragging;
    // Zoom và scale
    float zoomFactor;
    float shapeScale;
    // Màu sắc
    bool colorCycleEnabled;
    float hueOffset;
    // Hiệu ứng
    float time;
    float pulse;
    // Biến dạng
    float distortionAmount;
    sf::Vector3f distortionAxis;
    // UI
    bool showTransformUI;
    sf::RectangleShape transformButton;
    sf::Text transformButtonText;
    // Hình dạng cụ thể
    struct {
        float sphereRadius;
        float cubeSize;
        float figure8Scale;
        float atomNucleusSize;
        float heartScale;
        float helixRadius;
    } shapeParams;
public:
    ParticleMorph3D() :
        window(sf::VideoMode(WIDTH, HEIGHT), "3D Particle Morph - Advanced Visualizer", sf::Style::Close),
        currentShape(SPHERE_3D),
        shapeTransition(0.0f),
        isTransitioning(false),
        cameraDistance(500.0f),
        cameraAngleX(0.5f),
        cameraAngleY(0.3f),
        cameraAngleZ(0.0f),
        autoRotate(true),
        isMouseDragging(false),
        zoomFactor(1.0f),
        shapeScale(1.0f),
        colorCycleEnabled(true),
        hueOffset(0.0f),
        time(0.0f),
        pulse(0.0f),
        distortionAmount(0.0f),
        distortionAxis(0.0f, 1.0f, 0.0f),
        showTransformUI(false)
    {
        window.setFramerateLimit(60);
        // Khởi tạo font
        if (!font.loadFromFile("arial.ttf")) {
            std::cout << "Font not found, continuing without text\n";
        }
        // Khởi tạo tham số hình dạng
        shapeParams.sphereRadius = 120.0f;
        shapeParams.cubeSize = 150.0f;
        shapeParams.figure8Scale = 100.0f;
        shapeParams.atomNucleusSize = 40.0f;
        shapeParams.heartScale = 80.0f;
        shapeParams.helixRadius = 100.0f;
        setupUI();
        generateCurrentShape();
    }
    void setupUI() {
        infoText.setFont(font);
        infoText.setCharacterSize(16);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(20, 20);
        // Transform Button
        transformButton.setSize(sf::Vector2f(200, 40));
        transformButton.setPosition(WIDTH - 220, 20);
        transformButton.setFillColor(sf::Color(50, 100, 200, 200));
        transformButton.setOutlineColor(sf::Color(100, 150, 255));
        transformButton.setOutlineThickness(2);
        transformButtonText.setFont(font);
        transformButtonText.setCharacterSize(18);
        transformButtonText.setFillColor(sf::Color::White);
        transformButtonText.setString("Transform Shape (T)");
        transformButtonText.setPosition(
            transformButton.getPosition().x + 20,
            transformButton.getPosition().y + 10
        );
    }
    // Hàm tạo hình cầu 3D RỖNG (hollow) - Cải thiện: Thêm nhiều lớp hơn, màu sắc gradient mượt mà hơn, thêm hiệu ứng glow
    void generateSphere3D() {
        particles.clear();
        int numLayers = 12; // Tăng số lớp cho độ mịn hơn
        int particlesPerLayer = 300; // Tăng số hạt mỗi lớp
        for (int layer = 0; layer < numLayers; layer++) {
            float radiusRatio = 0.2f + (layer / (float)numLayers) * 0.8f;
            float currentRadius = shapeParams.sphereRadius * radiusRatio;
            for (int i = 0; i < particlesPerLayer; i++) {
                Particle3D p;
                float phi = acos(1.0f - 2.0f * (i + 0.5f) / particlesPerLayer);
                float theta = PI * (1.0f + sqrt(5.0f)) * i;
                p.position.x = currentRadius * sin(phi) * cos(theta);
                p.position.y = currentRadius * sin(phi) * sin(theta);
                p.position.z = currentRadius * cos(phi);
                p.originalPosition = p.position;
                p.size = 2.0f + 1.0f * sin(theta * 4.0f); // Variation kích thước
                p.velocity = sf::Vector3f(0, 0, 0);
                float hue = (layer * 30.0f + hueOffset) + sin(theta) * 10.0f; // Thêm variation hue
                float saturation = 0.85f + 0.15f * cos(phi);
                float lightness = 0.5f + 0.3f * sin(layer * 1.5f);
                p.color = hslToColor(hue, saturation, lightness);
                p.color.a = 160 + 80 * (layer % 2); // Xen kẽ alpha
                p.isOrbiting = false;
                particles.push_back(p);
            }
        }
        // Tăng connections cho lưới dày hơn
        int connections = 800;
        for (int i = 0; i < connections; i++) {
            Particle3D p;
            float t = (rand() % 1000) / 1000.0f;
            int layer1 = rand() % numLayers;
            int layer2 = (layer1 + 1 + rand() % 2) % numLayers; // Kết nối xa hơn
            float r1 = shapeParams.sphereRadius * (0.2f + (layer1 / (float)numLayers) * 0.8f);
            float r2 = shapeParams.sphereRadius * (0.2f + (layer2 / (float)numLayers) * 0.8f);
            float phi = acos(1.0f - 2.0f * t);
            float theta = 2.0f * PI * t * 12.0f;
            float currentRadius = r1 * (1.0f - t) + r2 * t;
            p.position.x = currentRadius * sin(phi) * cos(theta);
            p.position.y = currentRadius * sin(phi) * sin(theta);
            p.position.z = currentRadius * cos(phi);
            p.originalPosition = p.position;
            p.size = 1.0f + 0.5f * sin(i * 0.1f);
            p.velocity = sf::Vector3f(0, 0, 0);
            p.color = sf::Color(200, 255, 255, 80 + rand() % 40); // Màu cyan mờ variation
            p.isOrbiting = false;
            particles.push_back(p);
        }
    }
    // Hình hộp rỗng 3D - Cải thiện: Thêm hạt ở mặt để tạo cảm giác khối hơn, màu sắc đa dạng hơn
    void generateHollowCube() {
        particles.clear();
        int particlesPerEdge = 50; // Tăng số hạt
        float size = shapeParams.cubeSize * shapeScale;
        // 12 cạnh
        for (int edge = 0; edge < 12; edge++) {
            for (int i = 0; i < particlesPerEdge; i++) {
                Particle3D p;
                float t = static_cast<float>(i) / particlesPerEdge;
                switch(edge) {
                    case 0: p.position = sf::Vector3f(-size, -size, -size) * (1.0f - t) + sf::Vector3f(size, -size, -size) * t; break;
                    case 1: p.position = sf::Vector3f(size, -size, -size) * (1.0f - t) + sf::Vector3f(size, size, -size) * t; break;
                    case 2: p.position = sf::Vector3f(size, size, -size) * (1.0f - t) + sf::Vector3f(-size, size, -size) * t; break;
                    case 3: p.position = sf::Vector3f(-size, size, -size) * (1.0f - t) + sf::Vector3f(-size, -size, -size) * t; break;
                    case 4: p.position = sf::Vector3f(-size, -size, size) * (1.0f - t) + sf::Vector3f(size, -size, size) * t; break;
                    case 5: p.position = sf::Vector3f(size, -size, size) * (1.0f - t) + sf::Vector3f(size, size, size) * t; break;
                    case 6: p.position = sf::Vector3f(size, size, size) * (1.0f - t) + sf::Vector3f(-size, size, size) * t; break;
                    case 7: p.position = sf::Vector3f(-size, size, size) * (1.0f - t) + sf::Vector3f(-size, -size, size) * t; break;
                    case 8: p.position = sf::Vector3f(-size, -size, -size) * (1.0f - t) + sf::Vector3f(-size, -size, size) * t; break;
                    case 9: p.position = sf::Vector3f(size, -size, -size) * (1.0f - t) + sf::Vector3f(size, -size, size) * t; break;
                    case 10: p.position = sf::Vector3f(size, size, -size) * (1.0f - t) + sf::Vector3f(size, size, size) * t; break;
                    case 11: p.position = sf::Vector3f(-size, size, -size) * (1.0f - t) + sf::Vector3f(-size, size, size) * t; break;
                }
                p.originalPosition = p.position;
                p.size = 2.0f + 0.5f * sin(t * PI * 4); // Variation size
                p.velocity = sf::Vector3f(0, 0, 0);
                float hue = (edge * 30.0f + hueOffset);
                p.color = hslToColor(hue, 0.8f, 0.6f);
                p.color.a = 220;
                p.isOrbiting = false;
                particles.push_back(p);
            }
        }
        // Thêm hạt ở mặt để tạo khối (mờ hơn)
        int faceParticles = 100; // Per face
        for (int face = 0; face < 6; face++) {
            for (int i = 0; i < faceParticles; i++) {
                Particle3D p;
                float u = (rand() % 1000) / 1000.0f;
                float v = (rand() % 1000) / 1000.0f;
                switch(face) {
                    case 0: p.position = sf::Vector3f(size * (2*u-1), size * (2*v-1), -size); break; // Front
                    case 1: p.position = sf::Vector3f(size * (2*u-1), size * (2*v-1), size); break; // Back
                    case 2: p.position = sf::Vector3f(size * (2*u-1), -size, size * (2*v-1)); break; // Bottom
                    case 3: p.position = sf::Vector3f(size * (2*u-1), size, size * (2*v-1)); break; // Top
                    case 4: p.position = sf::Vector3f(-size, size * (2*u-1), size * (2*v-1)); break; // Left
                    case 5: p.position = sf::Vector3f(size, size * (2*u-1), size * (2*v-1)); break; // Right
                }
                p.originalPosition = p.position;
                p.size = 1.5f;
                p.velocity = sf::Vector3f(0, 0, 0);
                p.color = hslToColor(face * 60.0f + hueOffset, 0.7f, 0.5f);
                p.color.a = 80; // Mờ để không che cạnh
                p.isOrbiting = false;
                particles.push_back(p);
            }
        }
    }
    // Hình số 8 xoắn 3D DẠNG KHỐI - Cải thiện: Tăng slices, thêm variation thickness, màu rainbow
    void generateFigure8Spiral() {
        particles.clear();
        int numSlices = 15; // Tăng slices
        int particlesPerSlice = 250;
        for (int slice = 0; slice < numSlices; slice++) {
            float zOffset = (slice - numSlices/2.0f) * 12.0f;
            for (int i = 0; i < particlesPerSlice; i++) {
                Particle3D p;
                float t = static_cast<float>(i) / particlesPerSlice * 4.0f * PI;
                float scale = shapeParams.figure8Scale * shapeScale;
                float a = scale * sqrt(2.0f * cos(2.0f * t) + 0.1f * sin(t * 3)); // Variation a
                float x = a * cos(t);
                float y = a * sin(t);
                float z = zOffset + scale * 0.15f * sin(t * 4.0f + time);
                float thickness = 6.0f + 2.0f * sin(slice * PI / numSlices);
                float offsetAngle = t * 3.0f + slice * 0.2f;
                float offsetX = thickness * cos(offsetAngle);
                float offsetY = thickness * sin(offsetAngle);
                p.position = sf::Vector3f(x + offsetX, y + offsetY, z);
                p.originalPosition = p.position;
                p.size = 1.8f + 1.2f * sin(t * 6.0f + slice * 0.6f);
                p.velocity = sf::Vector3f(0, 0, 0);
                float hue = (t * 90.0f + slice * 20.0f + hueOffset);
                p.color = hslToColor(hue, 0.95f, 0.65f);
                p.color.a = 190 - slice * 8;
                p.isOrbiting = false;
                particles.push_back(p);
            }
        }
        // Tăng connections
        int connections = 500;
        for (int i = 0; i < connections; i++) {
            Particle3D p;
            float t = (rand() % 1000) / 1000.0f * 4.0f * PI;
            int slice1 = rand() % numSlices;
            int slice2 = (slice1 + 1 + rand() % 3) % numSlices;
            float scale = shapeParams.figure8Scale * shapeScale;
            float a = scale * sqrt(2.0f * cos(2.0f * t));
            float x = a * cos(t);
            float y = a * sin(t);
            float z1 = (slice1 - numSlices/2.0f) * 12.0f;
            float z2 = (slice2 - numSlices/2.0f) * 12.0f;
            float interp = (rand() % 1000) / 1000.0f;
            float z = z1 * (1.0f - interp) + z2 * interp;
            p.position = sf::Vector3f(x, y, z);
            p.originalPosition = p.position;
            p.size = 1.0f;
            p.velocity = sf::Vector3f(0, 0, 0);
            p.color = sf::Color(255, 255, 200, 60 + rand() % 40);
            p.isOrbiting = false;
            particles.push_back(p);
        }
    }
    // Mô hình nguyên tử - Cải thiện: Thêm nhiều orbit hơn, variation nucleus, trails dài hơn
    void generateAtomicModel() {
        particles.clear();
        // Hạt nhân
        int nucleusParticles = 300; // Tăng
        float nucleusSize = shapeParams.atomNucleusSize * shapeScale;
        for (int i = 0; i < nucleusParticles; i++) {
            Particle3D p;
            float phi = acos(1.0f - 2.0f * (i + 0.5f) / nucleusParticles);
            float theta = PI * (1.0f + sqrt(5.0f)) * i;
            float r = nucleusSize * (0.6f + 0.4f * sin(time + theta * 2.0f));
            p.position.x = r * sin(phi) * cos(theta);
            p.position.y = r * sin(phi) * sin(theta);
            p.position.z = r * cos(phi);
            p.originalPosition = p.position;
            p.size = 2.0f + 1.5f * sin(theta * 6.0f + time);
            p.velocity = sf::Vector3f(0, 0, 0);
            float hue = 0.0f + 30.0f * sin(theta);
            p.color = hslToColor(hue, 0.9f, 0.6f);
            p.color.a = 240;
            p.isOrbiting = false;
            particles.push_back(p);
        }
        // Orbits
        int orbits = 4; // Tăng
        int electronsPerOrbit = 10;
        float orbitSpeeds[] = {1.2f, 0.8f, 0.5f, 0.3f};
        float orbitRadii[] = {200.0f, 140.0f, 100.0f, 60.0f};
        sf::Color orbitColors[] = {
            sf::Color(80, 180, 255, 210),
            sf::Color(80, 255, 180, 210),
            sf::Color(255, 180, 80, 210),
            sf::Color(180, 80, 255, 210)
        };
        for (int orbit = 0; orbit < orbits; orbit++) {
            for (int i = 0; i < electronsPerOrbit; i++) {
                Particle3D p;
                float angle = static_cast<float>(i) / electronsPerOrbit * 2.0f * PI + time * orbitSpeeds[orbit];
                float radius = orbitRadii[orbit] * shapeScale;
                float tilt = orbit * 0.3f; // Tilt different orbits
                float x = radius * cos(angle);
                float y = radius * sin(angle) * cos(tilt);
                float z = radius * sin(angle) * sin(tilt);
                p.position = sf::Vector3f(x, y, z);
                p.originalPosition = p.position;
                p.size = 2.5f + 0.5f * orbit;
                p.velocity = sf::Vector3f(0, 0, 0);
                p.color = orbitColors[orbit];
                p.isOrbiting = true;
                p.orbitCenter = sf::Vector3f(0, 0, 0);
                p.orbitRadius = radius;
                p.orbitAngle = angle;
                p.orbitSpeed = orbitSpeeds[orbit];
                particles.push_back(p);
            }
        }
    }
    // Hình trái tim 3D DẠNG KHỐI - Cải thiện: Tăng layers, inner particles dày hơn, màu gradient mượt
    void generateHeart3D() {
        particles.clear();
        int numLayers = 12; // Tăng
        int particlesPerLayer = 400;
        for (int layer = 0; layer < numLayers; layer++) {
            float layerFactor = (layer - numLayers/2.0f) / (numLayers/2.0f);
            for (int i = 0; i < particlesPerLayer; i++) {
                Particle3D p;
                float u = static_cast<float>(i) / particlesPerLayer * 2.0f * PI;
                float scale = shapeParams.heartScale * shapeScale;
                float x = 16.0f * pow(sin(u), 3);
                float y = 13.0f * cos(u) - 5.0f * cos(2.0f * u) - 2.0f * cos(3.0f * u) - cos(4.0f * u);
                float thickness = 8.0f * layerFactor * (0.7f + 0.3f * cos(layer * 3.0f));
                p.position = sf::Vector3f(
                    x * scale * 0.08f + 2.0f * sin(u * 5.0f),
                    -y * scale * 0.08f + 2.0f * cos(u * 5.0f),
                    thickness * (0.6f + 0.4f * sin(u * 3.0f))
                );
                p.originalPosition = p.position;
                p.size = 1.8f + 1.0f * sin(u * 8.0f + layer * 0.5f);
                p.velocity = sf::Vector3f(0, 0, 0);
                float redIntensity = 0.6f + 0.4f * (1.0f - fabs(layerFactor));
                float pinkFactor = fabs(layerFactor) * 0.6f;
                float hue = 330.0f + 30.0f * layerFactor;
                p.color = hslToColor(hue, 0.8f, redIntensity * 0.5f + pinkFactor * 0.5f);
                p.color.a = 170 + 80 * (layer % 2);
                p.isOrbiting = false;
                particles.push_back(p);
            }
        }
        // Inner particles dày hơn
        int innerParticles = 800;
        for (int i = 0; i < innerParticles; i++) {
            Particle3D p;
            float r = (rand() % 1000) / 1000.0f;
            float theta = (rand() % 1000) / 1000.0f * 2.0f * PI;
            float phi = (rand() % 1000) / 1000.0f * PI;
            float scale = shapeParams.heartScale * shapeScale * 0.6f;
            p.position.x = scale * r * sin(phi) * cos(theta) * 0.4f;
            p.position.y = scale * r * sin(phi) * sin(theta) * 0.4f;
            p.position.z = scale * r * cos(phi) * 0.25f;
            float heartX = p.position.x / (scale * 0.08f);
            float heartY = -p.position.y / (scale * 0.08f);
            float heartVal = pow(heartX*heartX + heartY*heartY - 1, 3) - heartX*heartX * heartY*heartY*heartY;
            if (heartVal < 0.15f) { // Mở rộng vùng
                p.originalPosition = p.position;
                p.size = 1.2f + 0.8f * sin(i * 0.05f);
                p.velocity = sf::Vector3f(0, 0, 0);
                p.color = hslToColor(340.0f + rand() % 20, 0.7f, 0.6f);
                p.color.a = 100 + rand() % 40;
                p.isOrbiting = false;
                particles.push_back(p);
            }
        }
    }
    // Xoắn kép (DNA-like) - Cải thiện: Thêm bonds giữa strands, variation radius, màu gradient
    void generateDoubleHelix() {
        particles.clear();
        int numParticles = 1200; // Tăng
        float radius = shapeParams.helixRadius * shapeScale;
        float height = 350.0f * shapeScale;
        for (int i = 0; i < numParticles; i++) {
            float t = static_cast<float>(i) / numParticles;
            float z = height * (t - 0.5f);
            for (int strand = 0; strand < 2; strand++) {
                Particle3D p;
                float angle = t * 10.0f * PI + (strand * PI) + sin(t * PI) * 0.5f; // Variation angle
                float localRadius = radius * (0.8f + 0.2f * sin(t * 6.0f * PI));
                float x = localRadius * cos(angle);
                float y = localRadius * sin(angle);
                p.position = sf::Vector3f(x, y, z);
                p.originalPosition = p.position;
                p.size = 2.5f + 0.5f * cos(t * PI * 10);
                p.velocity = sf::Vector3f(0, 0, 0);
                float hue = (strand == 0 ? 0.0f : 240.0f) + t * 60.0f + hueOffset;
                p.color = hslToColor(hue, 0.9f, 0.7f);
                p.color.a = 230;
                p.isOrbiting = false;
                particles.push_back(p);
            }
        }
        // Thêm bonds giữa strands
        int bonds = numParticles / 2;
        for (int i = 0; i < bonds; i++) {
            float t = static_cast<float>(i) / bonds;
            float z = height * (t - 0.5f);
            float angle = t * 10.0f * PI;
            sf::Vector3f pos1(radius * cos(angle), radius * sin(angle), z);
            sf::Vector3f pos2 = pos1 * -1.0f; // Opposite
            int numBondParticles = 10;
            for (int j = 0; j < numBondParticles; j++) {
                Particle3D p;
                float interp = static_cast<float>(j) / (numBondParticles - 1);
                p.position = pos1 * (1.0f - interp) + pos2 * interp;
                p.originalPosition = p.position;
                p.size = 1.5f;
                p.velocity = sf::Vector3f(0, 0, 0);
                p.color = sf::Color(200, 200, 200, 150);
                p.isOrbiting = false;
                particles.push_back(p);
            }
        }
    }
    void generateCurrentShape() {
        switch(currentShape) {
            case SPHERE_3D: generateSphere3D(); break;
            case HOLLOW_CUBE: generateHollowCube(); break;
            case FIGURE8_SPIRAL: generateFigure8Spiral(); break;
            case ATOMIC_MODEL: generateAtomicModel(); break;
            case HEART_3D: generateHeart3D(); break;
            case DOUBLE_HELIX: generateDoubleHelix(); break;
        }
    }
    sf::Color hslToColor(float h, float s, float l) {
        h = fmod(h, 360.0f);
        float c = (1.0f - fabs(2.0f * l - 1.0f)) * s;
        float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = l - c / 2.0f;
        float r, g, b;
        if (h < 60) { r = c; g = x; b = 0; }
        else if (h < 120) { r = x; g = c; b = 0; }
        else if (h < 180) { r = 0; g = c; b = x; }
        else if (h < 240) { r = 0; g = x; b = c; }
        else if (h < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        return sf::Color(
            static_cast<sf::Uint8>((r + m) * 255),
            static_cast<sf::Uint8>((g + m) * 255),
            static_cast<sf::Uint8>((b + m) * 255),
            220
        );
    }
    void update(float deltaTime) {
        time += deltaTime;
        pulse = sin(time * 2.0f) * 0.5f + 0.5f;
        if (isTransitioning) {
            shapeTransition += deltaTime * 2.0f;
            if (shapeTransition >= 1.0f) {
                shapeTransition = 0.0f;
                isTransitioning = false;
            }
        }
        if (autoRotate) {
            cameraAngleY += deltaTime * 0.3f;
        }
        if (colorCycleEnabled) {
            hueOffset += deltaTime * 30.0f;
        }
        if (currentShape == ATOMIC_MODEL) {
            for (auto& p : particles) {
                if (p.isOrbiting) {
                    p.orbitAngle += deltaTime * p.orbitSpeed;
                    float tilt = (p.orbitRadius / 60.0f - 1.0f) * 0.3f; // Dựa vào radius
                    float x = p.orbitRadius * cos(p.orbitAngle);
                    float y = p.orbitRadius * sin(p.orbitAngle) * cos(tilt);
                    float z = p.orbitRadius * sin(p.orbitAngle) * sin(tilt);
                    p.position = sf::Vector3f(x, y, z);
                    // Trails
                    TrailPoint trail;
                    trail.position = p.position;
                    trail.color = p.color;
                    trail.color.a = 120;
                    trail.lifetime = 0.8f; // Dài hơn
                    trails.push_back(trail);
                }
            }
        }
        for (auto& trail : trails) {
            trail.lifetime -= deltaTime;
        }
        trails.erase(std::remove_if(trails.begin(), trails.end(),
            [](const TrailPoint& t) { return t.lifetime <= 0.0f; }),
            trails.end());
        if (particles.size() > 100) {
            int trailSamples = std::min(10, static_cast<int>(particles.size() / 100));
            for (int i = 0; i < trailSamples; i++) {
                int idx = rand() % particles.size();
                const auto& p = particles[idx];
                float speed = sqrt(p.velocity.x*p.velocity.x + p.velocity.y*p.velocity.y + p.velocity.z*p.velocity.z);
                if (speed > 0.1f) {
                    TrailPoint trail;
                    trail.position = p.position;
                    trail.color = p.color;
                    trail.color.a = 60;
                    trail.lifetime = 0.4f;
                    trails.push_back(trail);
                }
            }
        }
        // Cải thiện distortion: Làm mượt hơn, thêm multi-axis
        if (fabs(distortionAmount) > 0.001f) {
            for (auto& p : particles) {
                sf::Vector3f toCenter = p.originalPosition;
                float distance = sqrt(toCenter.x*toCenter.x + toCenter.y*toCenter.y + toCenter.z*toCenter.z);
                if (distance > 0.1f) {
                    float wave = sin(distance * 0.05f + time * 2.0f) * distortionAmount;
                    float twist = cos(distance * 0.03f + time) * distortionAmount * 0.5f;
                    p.position = p.originalPosition + distortionAxis * wave;
                    // Thêm twist
                    float tempX = p.position.x * cos(twist) - p.position.y * sin(twist);
                    float tempY = p.position.x * sin(twist) + p.position.y * cos(twist);
                    p.position.x = tempX;
                    p.position.y = tempY;
                }
            }
        }
        updateInfoText();
    }
    void updateInfoText() {
        std::string shapeNames[TOTAL_SHAPES] = {
            "3D Hollow Sphere",
            "Hollow Cube",
            "3D Figure-8 Spiral",
            "Atomic Model",
            "3D Heart",
            "Double Helix"
        };
        std::stringstream info;
        info << "CONTROLS:\n";
        info << "• T or Transform Button: Transform Shape\n";
        info << "• Mouse Drag: Rotate 3D View\n";
        info << "• Mouse Wheel: Zoom Camera\n";
        info << "• Shift + Wheel: Scale Shape Size\n";
        info << "• Shift + Drag: Distort Shape\n";
        info << "• R: Reset View\n";
        info << "• C: Toggle Color Cycle " << (colorCycleEnabled ? "[ON]" : "[OFF]") << "\n";
        info << "• Space: Toggle Auto-Rotate " << (autoRotate ? "[ON]" : "[OFF]") << "\n";
        info << "• +/-: Adjust Particle Size\n";
        info << "• ESC: Exit\n\n";
        info << "Camera Distance: " << static_cast<int>(cameraDistance) << "\n";
        info << "Rotation: " << (autoRotate ? "Auto" : "Manual") << "\n";
        infoText.setString(info.str());
    }
    sf::Vector3f rotatePoint(sf::Vector3f point) {
        point *= shapeScale;
        float y1 = point.y * cos(cameraAngleX) - point.z * sin(cameraAngleX);
        float z1 = point.y * sin(cameraAngleX) + point.z * cos(cameraAngleX);
        float x2 = point.x * cos(cameraAngleY) + z1 * sin(cameraAngleY);
        float z2 = -point.x * sin(cameraAngleY) + z1 * cos(cameraAngleY);
        float x3 = x2 * cos(cameraAngleZ) - y1 * sin(cameraAngleZ);
        float y3 = x2 * sin(cameraAngleZ) + y1 * cos(cameraAngleZ);
        return sf::Vector3f(x3, y3, z2);
    }
    sf::Vector2f projectPoint(sf::Vector3f point) {
        float fov = 400.0f; // Fixed fov, cameraDistance affects position
        float z = point.z + cameraDistance;
        if (z < 0.1f) z = 0.1f;
        float scale = fov / z;
        float x = point.x * scale + WIDTH / 2.0f;
        float y = point.y * scale + HEIGHT / 2.0f;
        return sf::Vector2f(x, y);
    }
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                handleKeyPress(event.key.code);
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                float delta = event.mouseWheelScroll.delta;
                float factor = (delta > 0) ? 1.1f : 0.9f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    // Scale shape
                    shapeScale *= factor;
                    shapeScale = std::max(0.1f, std::min(5.0f, shapeScale));
                    generateCurrentShape(); // Re-generate to apply scale
                } else {
                    // Zoom camera
                    cameraDistance *= (delta > 0 ? 0.9f : 1.1f);
                    cameraDistance = std::max(50.0f, std::min(2000.0f, cameraDistance));
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    float mx = static_cast<float>(event.mouseButton.x);
                    float my = static_cast<float>(event.mouseButton.y);
                    // Check if click on transform button
                    sf::FloatRect buttonBounds = transformButton.getGlobalBounds();
                    if (buttonBounds.contains(mx, my)) {
                        transformShape();
                    } else {
                        lastMousePos = sf::Vector2f(mx, my);
                    }
                }
            }
            else if (event.type == sf::Event::MouseMoved) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2f currentPos(
                        static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y)
                    );
                    sf::Vector2f delta = currentPos - lastMousePos;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                        // Distort
                        distortionAmount += delta.x * 0.005f;
                        distortionAxis.x += delta.y * 0.002f;
                        distortionAxis.y += delta.x * 0.001f;
                        // Normalize axis
                        float len = sqrt(distortionAxis.x*distortionAxis.x + distortionAxis.y*distortionAxis.y + distortionAxis.z*distortionAxis.z);
                        if (len > 0.001f) distortionAxis /= len;
                    } else {
                        // Rotate
                        cameraAngleY += delta.x * 0.005f;
                        cameraAngleX += delta.y * 0.005f;
                    }
                    lastMousePos = currentPos;
                }
            }
        }
    }
    void transformShape() {
        currentShape = static_cast<ShapeType>((currentShape + 1) % TOTAL_SHAPES);
        isTransitioning = true;
        shapeTransition = 0.0f;
        generateCurrentShape();
    }
    void handleKeyPress(sf::Keyboard::Key key) {
        switch(key) {
            case sf::Keyboard::PageUp:
                shapeScale *= 1.1f;
                shapeScale = std::min(5.0f, shapeScale);
                generateCurrentShape();
                break;
            case sf::Keyboard::PageDown:
                shapeScale *= 0.9f;
                shapeScale = std::max(0.1f, shapeScale);
                generateCurrentShape();
                break;
            case sf::Keyboard::Escape:
                window.close();
                break;
            case sf::Keyboard::T:
                transformShape();
                break;
            case sf::Keyboard::R:
                cameraDistance = 500.0f;
                cameraAngleX = 0.5f;
                cameraAngleY = 0.3f;
                cameraAngleZ = 0.0f;
                distortionAmount = 0.0f;
                distortionAxis = sf::Vector3f(0.0f, 1.0f, 0.0f);
                shapeScale = 1.0f;
                generateCurrentShape();
                break;
            case sf::Keyboard::Space:
                autoRotate = !autoRotate;
                break;
            case sf::Keyboard::C:
                colorCycleEnabled = !colorCycleEnabled;
                break;
            case sf::Keyboard::Add:
            case sf::Keyboard::Equal:
                for (auto& p : particles) {
                    p.size = std::min(10.0f, p.size + 0.1f);
                }
                break;
            case sf::Keyboard::Subtract:
            case sf::Keyboard::Dash:
                for (auto& p : particles) {
                    p.size = std::max(1.0f, p.size - 0.1f);
                }
                break;
        }
    }
    void render() {
        window.clear(sf::Color(5, 10, 20));
        trailRender.clear();
        for (const auto& trail : trails) {
            if (trail.lifetime > 0.0f) {
                sf::Vector3f rotated = rotatePoint(trail.position);
                sf::Vector2f projected = projectPoint(rotated);
                float depth = rotated.z + cameraDistance;
                if (depth > 0 && depth < 2000.0f) {
                    sf::VertexArray trailVerts(sf::Points);
                    sf::Vertex vertex(projected);
                    sf::Color trailColor = trail.color;
                    trailColor.a = static_cast<sf::Uint8>(trail.color.a * (trail.lifetime / 0.8f));
                    vertex.color = trailColor;
                    trailVerts.append(vertex);
                    trailRender.push_back(trailVerts);
                }
            }
        }
        for (const auto& trail : trailRender) {
            window.draw(trail);
        }
        for (const auto& p : particles) {
            sf::Vector3f rotated = rotatePoint(p.position);
            sf::Vector2f projected = projectPoint(rotated);
            float depth = rotated.z + cameraDistance;
            if (depth > 0 && depth < 2000.0f) {
                float projScale = 400.0f / depth; // Scale size with distance for perspective
                float size = p.size * projScale;
                size = std::max(0.5f, std::min(10.0f, size));
                sf::CircleShape particle(size);
                particle.setPosition(projected.x - size, projected.y - size);
                sf::Color depthColor = p.color;
                float depthFactor = 1.0f - (depth / 2000.0f); // Adjusted for farther fade
                depthColor.a = static_cast<sf::Uint8>(p.color.a * (0.4f + 0.6f * depthFactor));
                particle.setFillColor(depthColor);
                sf::Color glowColor = depthColor;
                glowColor.a = 60;
                particle.setOutlineColor(glowColor);
                particle.setOutlineThickness(1.5f);
                window.draw(particle);
            }
        }
        if (isTransitioning) {
            float alpha = sin(shapeTransition * PI) * 100.0f;
            sf::RectangleShape transition(sf::Vector2f(WIDTH, HEIGHT));
            transition.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
            window.draw(transition);
        }
        window.draw(infoText);
        window.draw(transformButton);
        window.draw(transformButtonText);
        window.display();
    }
    void run() {
        sf::Clock frameClock;
        while (window.isOpen()) {
            float deltaTime = frameClock.restart().asSeconds();
            handleEvents();
            update(deltaTime);
            render();
        }
    }
};
int main() {
    ParticleMorph3D app;
    app.run();
    return 0;
}
