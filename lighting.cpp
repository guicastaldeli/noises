#define LIGHTING_IMPLEMENTATION
#include <lighting.hpp>
#include <mesh.hpp>
#include <shader.hpp>
#include <stdexcept>
#include <cake.hpp>
#include <cstring>

// ---------------------------------------
//
// Lights
//
// ---------------------------------------
/******************************
 * Ambient Light
 ****************/
AmbientLight::AmbientLight(LightingManager& lightingManager) : Lighting(lightingManager) {
    color = Vec3(0.3f, 0.8f, 0.7f);
    intensity = 0.2f;
}

// Get Type
LightType AmbientLight::getType() const {
    return LightType::AMBIENT;
}

// Upload
void AmbientLight::upload(UniformBuffer& ubo) const {
    ubo.lights.ambientColor[0] = color.x;
    ubo.lights.ambientColor[1] = color.y;
    ubo.lights.ambientColor[2] = color.z;
    ubo.lights.ambientIntensity = getIntensity();
}

void PointLight::uploadCount(UniformBuffer& ubo, int count) {
    ubo.lights.pointCount = std::min(count, 32);
}

/****************
 * Point Light
 ****************/
PointLight::PointLight(LightingManager& lightingManager) : Lighting(lightingManager) {
    id = static_cast<uint32_t>(IdGen::next("plight"));
}

// Get Type
LightType PointLight::getType() const {
    return LightType::POINT;
}

// Get Id
uint32_t PointLight::getId() const {
    return id;
}

// Set Position
void PointLight::setPosition(float x, float y, float z) {
    position = Vec3(x, y, z);
}

// Set Radius
void PointLight::setRadius(float r) {
    radius = r;
}

// Get Position
Vec3 PointLight::getPosition() const {
    return position;
}

// Get Radius
float PointLight::getRadius() const {
    return radius;
}

// Upload
void PointLight::upload(UniformBuffer& ubo) const {
    const auto& pts = lightingManager.getPointLight();
    int i = -1;
    for(int idx = 0; idx < (int)pts.size(); ++idx) {
        if(pts[idx] == this) {
            i = idx;
            break;
        }
    }
    if(i < 0 || i >= 32) return;

    ubo.lights.pointLights[i].position[0] = position.x;
    ubo.lights.pointLights[i].position[1] = position.y;
    ubo.lights.pointLights[i].position[2] = position.z;
    ubo.lights.pointLights[i].color[0] = color.x;
    ubo.lights.pointLights[i].color[1] = color.y;
    ubo.lights.pointLights[i].color[2] = color.z;
    ubo.lights.pointLights[i].intensity = getIntensity();
}

/********************
 * Directional Light 
 * *******/
DirectionalLight::DirectionalLight(LightingManager& lightingManager) : Lighting(lightingManager) {
    id = static_cast<uint32_t>(IdGen::next("dlight"));
}

// Get Light Space Matrix
Mat4 DirectionalLight::getLightSpaceMatrix(const Vec3& center, float radius) const {
    Vec3 dir = direction;
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
    if(len > 1e-6f) { dir.x /= len; dir.y /= len; dir.z /= len; }
    
    // Light position must be ON THE OPPOSITE SIDE of the light direction
    Vec3 lightPos(center.x - dir.x * (radius * 2.0f),
                  center.y - dir.y * (radius * 2.0f),
                  center.z - dir.z * (radius * 2.0f));

    Vec3 up(0.0f, 1.0f, 0.0f);
    if(std::fabs(dir.y) > 0.99f) up = Vec3(0.0f, 0.0f, 1.0f);

    Mat4 lightView = Mat4::LookAt(lightPos, center, up);
    
    // Tight ortho box
    Mat4 lightProj = Mat4::Ortho(-radius, radius, 
                                 -radius, radius, 
                                 0.1f, radius * 4.0f);
    return lightProj * lightView;
}
// Get Type
LightType DirectionalLight::getType() const {
    return LightType::DIRECTIONAL;
}

// Get Id
uint32_t DirectionalLight::getId() const {
    return id;
}

// Set Direction
void DirectionalLight::setDirection(float x, float y, float z) {
    direction = Vec3(x, y, z);
}

// Get Direction
Vec3 DirectionalLight::getDirection() const {
    return direction;
}

// Upload
void DirectionalLight::upload(UniformBuffer& ubo) const {
    const auto& dirs = lightingManager.getDirectionalLight();
    int i = -1;
    for(int idx = 0; idx < (int)dirs.size(); ++idx) {
        if(dirs[idx] == this) {
            i = idx;
            break;
        }
    }
    if(i < 0 || i >= 4) return;

    ubo.lights.dirLights[i].direction[0] = direction.x;
    ubo.lights.dirLights[i].direction[1] = direction.y;
    ubo.lights.dirLights[i].direction[2] = direction.z;
    ubo.lights.dirLights[i].color[0] = color.x;
    ubo.lights.dirLights[i].color[1] = color.y;
    ubo.lights.dirLights[i].color[2] = color.z;
    ubo.lights.dirLights[i].intensity = getIntensity();
}

void DirectionalLight::uploadCount(UniformBuffer& ubo, int count) {
    ubo.lights.dirCount = std::min(count, 4);
}
// ---------------------------------------

/*****************
 * 
 * Lighting
 * 
 *  ******************/
Lighting::Lighting(LightingManager& lightingManager) : lightingManager(lightingManager) {};

// Set Color
void Lighting::setColor(float r, float g, float b) {
    color = Vec3(r, g, b);
}

void Lighting::setColor(const std::string& hex) {
    color = Color::HexToVec3(hex);
}

// Set Intensity
void Lighting::setIntensity(float i) {
    intensity = i;
}
/**
 * 
 */

/***************************
 * 
 * Lighting Manager
 * 
 */
LightingManager::LightingManager(Main& main, Shader& shader, Mesh& mesh) : 
    main(main),
    shader(shader),
    mesh(mesh) {};
LightingManager::~LightingManager() {};

/**
 * Get Mesh
 */
const Mesh& LightingManager::getMesh() const {
    return mesh;
}

/**
 * Set Color
 */
LightingManager& LightingManager::setColor(float r, float g, float b) {
    if(!current) throw std::runtime_error("LightingManager: no current light to set color");
    current->setColor(r, g, b);
    return *this;
}

LightingManager& LightingManager::setColor(const std::string& hex) {
    if(!current) throw std::runtime_error("LightingManager: no current light to set color");
    current->setColor(hex);
    return *this;
}

/**
 * Set Intensity
 */
LightingManager& LightingManager::setIntensity(float i) {
    if(!current) throw std::runtime_error("LightingManager: no current light to set intensity");
    current->setIntensity(i);
    return *this;
}

/**
 * Set Position
 */
LightingManager& LightingManager::setPosition(float x, float y, float z) {
    if(!current) throw std::runtime_error("LightingManager: no current light to set position");
    if(current->getType() != LightType::POINT) throw std::runtime_error("LightingManager: setPosition only works on PointLight");
    static_cast<PointLight*>(current)->setPosition(x, y, z);
    return *this;
}

/**
 * Set Direction
 */
LightingManager& LightingManager::setDirection(float x, float y, float z) {
    if(!current) throw std::runtime_error("LightingManager: no current light to set direction");
    if(current->getType() != LightType::DIRECTIONAL) throw std::runtime_error("LightingManager: setDirection only works on DirectionalLight");

    static_cast<DirectionalLight*>(current)->setDirection(x, y, z);
    return *this;
}

/**
 * 
 * Add
 * 
 */
LightingManager& LightingManager::add(LightType type) {
    switch(type) {
        // Ambient
        case LightType::AMBIENT: {
            if(!ambientLight) {
                auto light = std::make_unique<AmbientLight>(*this);
                ambientLight = light.get();
                current = light.get();
                lights.push_back(std::move(light));
            }
            break;
        }
        // Point
        case LightType::POINT: {
            auto light = std::make_unique<PointLight>(*this);
            pointLight.push_back(light.get());
            current = light.get();
            lights.push_back(std::move(light));
            break;
        }
        // Directional
        case LightType::DIRECTIONAL: {
            auto light = std::make_unique<DirectionalLight>(*this);
            directionalLight.push_back(light.get());
            current = light.get();
            lights.push_back(std::move(light));
            break;
        }
    }
    return *this;
}

/**
 * 
 * Upload
 * 
 */
void LightingManager::upload(UniformBuffer& ubo) {
    // Ambient Light
    if(ambientLight) {
        ambientLight->upload(ubo);
    } else {
        ubo.lights.ambientIntensity = 0.0f;
    }

    // Point Light
    PointLight::uploadCount(ubo, (int)pointLight.size());
    for(auto* p : pointLight) p->upload(ubo);

    // Directional Light
    DirectionalLight::uploadCount(ubo, (int)directionalLight.size());
    for(auto& d : directionalLight) d->upload(ubo);
    if(!directionalLight.empty()) {
        Vec3 sceneCenter(15.0f, 0.0f, 15.0f);
        float sceneRadius = 21.2f; 
        // TODO: change this later to fit inside the
        // Boundary collider size... and calc the center
        // dynamically of course...
        Mat4 ls = directionalLight[0]->getLightSpaceMatrix(sceneCenter, sceneRadius);
        for(int r = 0; r < 4; ++r)
        for(int c = 0; c < 4; ++c) {
            ubo.lights.lightSpaceMatrix.m[r][c] = ls.m[c][r]; 
        }
    }

    ubo.lights.shadowMapWidth = SHADOW_MAP_WIDTH;
    ubo.lights.shadowMapHeight = SHADOW_MAP_HEIGHT;
}

/**
 * 
 * Init
 * 
 */
void LightingManager::init() {
    ShadowMap::init(main);
}

/**
 * 
 * Create
 * 
 */
void LightingManager::create() {
    ShadowMap::create(main, shader, mesh);
}

/**
 * Clear
 */
void LightingManager::clear() {
    lights.clear();
    pointLight.clear();
    directionalLight.clear();
    ambientLight = nullptr;
    current = nullptr;
    ShadowMap::clear(main.device);
}