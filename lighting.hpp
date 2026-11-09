#pragma once

#include <utils/vec3.hpp>
#include <utils/color.hpp>
#include <utils/idgen.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#define SHADOW_MAP_WIDTH 2048
#define SHADOW_MAP_HEIGHT 2048

struct UniformBuffer;

/********************
 * 
 * Light Data
 * 
 */
struct LightData {
    float position[3];  float padding1;
    float color[3];     float intensity;
};

struct DirLightData {
    float direction[3]; float padding1;
    float color[3];     float intensity;
};

struct Lights {
    float ambientColor[3];
    float ambientIntensity;

    int pointCount;
    int dirCount;
    float padding1[2];

    LightData pointLights[32];
    DirLightData dirLights[4];

    Mat4 lightSpaceMatrix;
    int shadowMapWidth;
    int shadowMapHeight;
    float padding2[2];
};
/**
 * 
 */

class Main;
class Mesh;

/************************
 * 
 * Light Type
 * 
 */
class LightingManager;
enum class LightType {
    AMBIENT,
    POINT,
    DIRECTIONAL
};

/*****************
 * 
 * Lighting
 * 
 *  ******************/
class Lighting {
protected:
    Vec3 color{ 1.0f, 1.0f, 1.0f };
    float intensity = 1.0f;

public:
    Lighting(LightingManager& lightingManager);
    virtual ~Lighting() = default;

    LightingManager& lightingManager;

    virtual uint32_t getId() const { return 0; }
    virtual LightType getType() const = 0;
    virtual void setColor(float r, float g, float b);
    virtual void setColor(const std::string& hex);
    virtual void setIntensity(float i);

    virtual void upload(UniformBuffer& ubo) const = 0;

    Vec3 getColor() const { return color; }
    float getIntensity() const { return intensity; }
};
/**
 * 
 */

// ---------------------------------------
//
// Lights
//
// ---------------------------------------
/******************************
 * Ambient Light
 ****************/
class AmbientLight : public Lighting {
public:
    AmbientLight(LightingManager& lightingManager);
    LightType getType() const override;

    void upload(UniformBuffer& ubo) const override;
};

/****************
 * Point Light
 ****************/
class PointLight : public Lighting {
private:
    uint32_t id;
    Vec3 position{ 0.0f, 0.0f, 0.0f };
    float radius = 1.0f;

public:
    PointLight(LightingManager& lightingManager);
    
    LightType getType() const override;
    uint32_t getId() const override;
    void setPosition(float x, float y, float z);
    void setRadius(float r);
    Vec3 getPosition() const;
    float getRadius() const;

    void upload(UniformBuffer& ubo) const override;
    static void uploadCount(UniformBuffer& ubo, int count);
};

/********************
 * Directional Light 
 * *******/
class DirectionalLight : public Lighting {
private:
    uint32_t id;
    Vec3 direction{ -0.5f, -1.0f, -0.3f };

public:
    DirectionalLight(LightingManager& lightingManager);

    Mat4 getLightSpaceMatrix(const Vec3& sceneCenter, float sceneRadius) const;

    LightType getType() const override;
    uint32_t getId() const override;
    void setDirection(float x, float y, float z);
    Vec3 getDirection() const;

    void upload(UniformBuffer& ubo) const override;
    static void uploadCount(UniformBuffer& ubo, int count);
};
// ---------------------------------------

/***************************
 * 
 * Lighting Manager
 * 
 */
class LightingManager {
private:
    Mesh& mesh;

    std::vector<std::unique_ptr<Lighting>> lights;
    Lighting* current = nullptr;

    AmbientLight* ambientLight = nullptr;
    std::vector<PointLight*> pointLight;
    std::vector<DirectionalLight*> directionalLight;

public:
    LightingManager(Mesh& mesh);
    ~LightingManager();

    LightingManager& add(LightType type);
    void upload(UniformBuffer& ubo);

    const Mesh& getMesh() const;

    LightingManager& setColor(float r, float g, float b);
    LightingManager& setColor(const std::string& hex);
    LightingManager& setIntensity(float i);
    LightingManager& setPosition(float x, float y, float z);
    LightingManager& setDirection(float x, float y, float z);

    void clear();

    AmbientLight* getAmbientLight() const { return ambientLight; }
    const std::vector<PointLight*>& getPointLight() const { return pointLight; }
    const std::vector<DirectionalLight*>& getDirectionalLight() const { return directionalLight; }
};
/**
 * 
 */