#pragma once

#include <utils/vec3.hpp>
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <mesh.hpp>

/************** 
 * 
 * Collision Type
 * 
 * ***************/
enum class CollisionType {
    STATIC_COLLIDER,
    BOUNDARY_COLLIDER
};

// ---------------------------------------
//
// Colliders
//
// ---------------------------------------
class Collider;

/*********************
 * Static Collider
 * **********************/
class StaticCollider : public Collider {
private:
    BBox bbox;
    Vec3 position{ 0, 0, 0 };
    float halfX = 0, halfY = 0, halfZ = 0;
    std::function<BBox()> bboxProv;
    std::string id, type;
    bool hasBBox = false, hasPosition = false, hasProv = false;

    CollisionResult calcCollision(const BBox& a, const BBox& b) const;

public:
    StaticCollider(const BBox& bbox, std::string id, std::string type = "") :
                    bbox(bbox), id(std::move(id)), type(std::move(type)), hasBBox(true) {};

    BBox getBBox() const override;
    std::string getId() const override;
    std::string getType() const override;
    
    
    CollisionResult checkCollision(const BBox& other) const;
};

/*******************
 * Boundary Collider
 ***************/
class BoundaryCollider : public Collider {
private:
    float distance = 0.0f, thickness = 1.0f;
    float minHeight = 0.0f, maxHeight = 0.0f;
    Vec3 center{ 0, 0, 0 };
    bool active = true;

public:
    explicit BoundaryCollider(float distance) { setDistance(distance); }

    BBox getBBox() const override;
    std::string getId() const override;
    float getMinHeight() const;
    float getMaxHeight() const;
    const Vec3& getCenter() const;
    void setCenter(const Vec3& c);
    void setDistance(float d);
    float getBoundaryDistance() const;
    Vec3 getBoundaryNormal(const Vec3& p) const;
    float getBoundaryFar(const Vec3& p) const;
    bool isOutsideBoundary(const Vec3& p) const;

    void activate();
    void deactivate();
    bool isActive() const;
};
// ---------------------------------------

/******************************
 * 
 * BBox
 * 
 **********************/
class BBox {
public:
    float minX = 0, minY = 0, minZ = 0;
    float maxX = 0, maxY = 0, maxZ = 0;

    BBox() = default;
    BBox(float minX, float minY, float minZ,
        float maxX, float maxY, float maxZ) :
        minX(minX), minY(minY), minZ(minZ),
        maxX(maxX), maxY(maxY), maxZ(maxZ) {};

    float getSizeX() const;
    float getSizeY() const;
    float getSizeZ() const;

    void setFromCenter(const Vec3& c, const Vec3& size);
    static BBox fromCenter(const Vec3& c, const Vec3& s);
    bool intersects(const BBox& o) const;

};
/**
 * 
 */

/**************************
 * 
 * Collision Result
 * 
 **************************/
class Collider;
class CollisionResult {
public:
    bool collided = false;
    Vec3 normal{ 0, 0, 0 };
    float depth = 0.0f;
    Collider* otherCollider = nullptr;
    CollisionType type = CollisionType::STATIC_COLLIDER;

    CollisionResult() = default;
    CollisionResult(bool collided, 
                    const Vec3& normal, float depth,
                    Collider* other, CollisionType type) :
                    collided(collided),
                    normal(normal), depth(depth),
                    otherCollider(other), type(type) {};
};
/**
 * 
 */

/***************************
 * 
 * Collider
 * 
 */
class Collider {
protected:
    bool visible = true;

public:
    virtual ~Collider() = default;

    virtual BBox        getBBox()   const = 0;
    virtual std::string getId()     const = 0;
    virtual std::string getType()   const { return ""; }

    virtual void        onCollision(const CollisionResult&) {}
    virtual void        setVisible(bool v)                  { visible = v; }
    virtual bool        isVisible() const                   { return visible; }
};
/**
 * 
 */

/*****************************
 * 
 * Collision Manager
 * 
 */
class CollisionManager {
private:
    Mesh& mesh;

    std::unordered_map<std::string, BBox> localBoxCache;
    std::vector<std::unique_ptr<Collider>> colliders;
    std::unordered_set<std::string> colliderIdSet;
    std::vector<std::string> removedIds;

    const BBox& localBoxFor(const std::string& type);
    
public:
    CollisionManager(Mesh& mesh);
    ~CollisionManager();

    void set(uint32_t id, CollisionType type);

    bool colliderExists(const std::string& id) const;
    Collider* getCollider(const std::string& id);
    std::vector<Collider*> getColliders();

    std::vector<CollisionResult> checkCollision(const BBox& bbox);
    void resolveCollision(const BBox& bbox,
                        Vec3& position, Vec3& velocity,
                        std::vector<CollisionResult>& collisions);

    void removeCollider(const std::string& id);
    void clear();
};
/**
 * 
 */