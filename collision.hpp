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
#include <tick.hpp>
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
    BBox otherWorldBox{};
    uint32_t instanceId = 0;

    CollisionResult() = default;
    CollisionResult(bool collided, 
                    const Vec3& normal, float depth,
                    Collider* other, CollisionType type,
                    const BBox& box = {}, uint32_t instId = 0) :
                    collided(collided),
                    normal(normal), depth(depth),
                    otherCollider(other), type(type),
                    otherWorldBox(box), instanceId(instId) {};
};
/**
 * 
 */

/***************************
 * 
 * Collider
 * 
 */
class RigidBody;
class Collider {
protected:
    bool visible = true;

public:
    virtual ~Collider() = default;

    virtual BBox        getBBox()   const = 0;
    virtual std::string getId()     const = 0;
    virtual std::string getType()   const { return ""; }

    virtual void        onCollision(const CollisionResult&) {}
    virtual void        setVisible(bool v);
    virtual bool        isVisible() const;

    virtual RigidBody*  getRigidBody()                      { return nullptr; }
    virtual const RigidBody* getRigidBody() const           { return nullptr; }

    virtual void clearInstances() {}
    virtual void build() {}
};
/**
 * 
 */

// ---------------------------------------
//
// Colliders
//
// ---------------------------------------
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
    BBox localBox;

public:
    struct Instance {
        uint32_t id;
        Vec3 position;
    };

    struct CellKey {
        int x, y, z;
        bool operator==(const CellKey& o) const { return x==o.x && y==o.y && z==o.z; }
    };
    struct CellHash {
        size_t operator()(const CellKey& k) const {
            size_t h = size_t(k.x) * 73856093u ^
                        size_t(k.y) * 19349663u ^
                        size_t(k.z) * 83492791u;
            return h;
        }
    };

    StaticCollider(const BBox& bbox, std::string id, std::string type = "") :
                    bbox(bbox), localBox(bbox), id(std::move(id)), type(std::move(type)), hasBBox(true) {};

    std::vector<Instance> instances;
    std::unordered_map<CellKey, std::vector<uint32_t>, CellHash> grid;
    float cellSize = 2.0f;

    BBox getBBox() const override;
    std::string getId() const override;
    std::string getType() const override;

    void addInstance(uint32_t instId, const Vec3& worldPos);
    const std::vector<Instance>& getInstances() const;
    void clearInstances() override;
    void build() override { buildGrid(); }
    void buildGrid();
    size_t instanceCount() const;

    float getCellSize() const;
    void setCellSize(float s);
    
    static CellKey cellOf(float x, float y, float z, float cell);
    BBox worldBox(const Instance& inst) const;

    CollisionResult calcCollision(const BBox& a, const BBox& b, const Instance& inst, uint32_t idx) const;
    void checkCollision(const BBox& other, std::vector<CollisionResult>& out) const;
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

/*****************************
 * 
 * Collision Manager
 * 
 */
class CollisionManager {
private:
    /*********
     * Entry
     */
    struct Entry {
        RigidBody*      rigidBody;
        CollisionType   type;
    };
    /** */

    Tick& tick;
    Mesh& mesh;

    std::unordered_map<std::string, BBox> localBoxCache;
    std::vector<Collider*> colliders;
    std::vector<std::unique_ptr<Collider>> ownedColliders;
    std::vector<Entry> dynamicRigidBody;
    std::unordered_set<std::string> colliderIdSet;
    std::vector<std::string> removedIds;
    std::unordered_map<std::string, StaticCollider*> colliderByKey;

    const BBox& localBoxFor(const std::string& type);
    
public:
    CollisionManager(Tick& tick, Mesh& mesh);
    ~CollisionManager();

    void set(uint32_t id, CollisionType type);
    void set(RigidBody& rigidBody, CollisionType type);

    bool colliderExists(const std::string& id) const;
    bool hasRigidBody(const RigidBody& rigidBody) const;
    Collider* getCollider(const std::string& id);
    std::vector<Collider*> getColliders();

    std::vector<CollisionResult> checkCollision(const BBox& bbox);
    void resolveCollision(Collider& collider, std::vector<CollisionResult>& collisions);
    void resolveCollision(RigidBody& rigidBody, const BBox& bbox, std::vector<CollisionResult>& collisions);

    void update();
    void build();

    void clear();
    void clearColliders();
};
/**
 * 
 */