#include <lighting.hpp>
#include <mesh.hpp>
#include <stdexcept>
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
LightingManager::LightingManager(Mesh& mesh) : mesh(mesh) {};
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
    // Ambient light
    if(ambientLight) {
        ambientLight->upload(ubo);
    } else {
        ubo.lights.ambientIntensity = 0.0f;
    }

    // Point light
    PointLight::uploadCount(ubo, (int)pointLight.size());
    for(auto* p : pointLight) p->upload(ubo);

    // Directional light
    DirectionalLight::uploadCount(ubo, (int)directionalLight.size());
    for(auto& d : directionalLight) d->upload(ubo);
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
}