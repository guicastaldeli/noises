#include <collision.hpp>
#include <camera.hpp>
#include <iostream>

// ---------------------------------------
//
// Colliders
//
// ---------------------------------------
/*********************
 * Static Collider
 * **********************/
// Get Id
std::string StaticCollider::getId() const {
    return id;
}

// Get Type
std::string StaticCollider::getType() const {
    return type;
}

// Get Cell Size
float StaticCollider::getCellSize() const {
    return cellSize;
}

// Set Cell Size
void StaticCollider::setCellSize(float s) {
    cellSize = s;
}

// Get Instances
const std::vector<StaticCollider::Instance>& StaticCollider::getInstances() const {
    return instances;
}

// Instance Count
size_t StaticCollider::instanceCount() const {
    return instances.size();
}

// Get BBox
BBox StaticCollider::getBBox() const {
    return localBox;
}

// Cell Of
StaticCollider::CellKey StaticCollider::cellOf(float x, float y, float z, float cell) {
    return { (int)std::floor(x / cell),
                (int)std::floor(y / cell),
                (int)std::floor(z / cell) };
}

// World Box
BBox StaticCollider::worldBox(const Instance& inst) const {
    return BBox(inst.position.x + localBox.minX, inst.position.y + localBox.minY, inst.position.z + localBox.minZ,
                inst.position.x + localBox.maxX, inst.position.y + localBox.maxY, inst.position.z + localBox.maxZ);
}

// Add Instance
void StaticCollider::addInstance(uint32_t instId, const Vec3& worldPos) {
    instances.push_back({ instId, worldPos });
}

// Clear Instances
void StaticCollider::clearInstances() {
    instances.clear();
    grid.clear();
}

// Build Grid
void StaticCollider::buildGrid() {
    grid.clear();
    for(uint32_t i = 0; i < instances.size(); ++i) {
        BBox b = worldBox(instances[i]);
        CellKey lo = cellOf(b.minX, b.minY, b.minZ, cellSize);
        CellKey hi = cellOf(b.maxX, b.maxY, b.maxZ, cellSize);
        for(int x = lo.x; x <= hi.x; ++x)
        for(int y = lo.y; y <= hi.y; ++y)
        for(int z = lo.z; z <= hi.z; ++z)
            grid[{x,y,z}].push_back(i);
    }
}

// Calc Collision
CollisionResult StaticCollider::calcCollision(const BBox& a, const BBox& b, const Instance& inst, uint32_t idx) const {
    float xO = std::min(a.maxX, b.maxX) - std::max(a.minX, b.minX);
    float yO = std::min(a.maxY, b.maxY) - std::max(a.minY, b.minY);
    float zO = std::min(a.maxZ, b.maxZ) - std::max(a.minZ, b.minZ);
    
    float m = std::min({ xO, yO, zO });
    Vec3 n{ 0, 0, 0 };
    if(m == xO)         n.x = (a.maxX > b.maxX) ? 1.0f : -1.0f;
    else if(m == yO)    n.y = (a.maxY > b.maxY) ? 1.0f : -1.0f;
    else                n.z = (a.maxZ > b.maxZ) ? 1.0f : -1.0f;

    return CollisionResult(true, n, m,
                        const_cast<StaticCollider*>(this),
                        CollisionType::STATIC_COLLIDER, 
                        b, inst.id);
}

// Check Collision
void StaticCollider::checkCollision(const BBox& other, std::vector<CollisionResult>& out) const {
    if(instances.empty()) return;

    CellKey lo = cellOf(other.minX, other.minY, other.minZ, cellSize);
    CellKey hi = cellOf(other.maxX, other.maxY, other.maxZ, cellSize);

    std::vector<uint32_t> c;
    for(int x = lo.x; x <= hi.x; ++x)
    for(int y = lo.y; y <= hi.y; ++y)
    for(int z = lo.z; z <= hi.z; ++z) {
        auto it = grid.find({x,y,z});
        if(it == grid.end()) continue;
        c.insert(c.end(), it->second.begin(), it->second.end());
    }

    std::sort(c.begin(), c.end());
    c.erase(std::unique(c.begin(), c.end()), c.end());

    for(uint32_t idx : c) {
        const Instance& inst = instances[idx];
        BBox wb = worldBox(inst);
        if(!other.intersects(wb)) continue;
        out.push_back(calcCollision(other, wb, inst, idx));
    }
}

/*******************
 * Boundary Collider
 ***************/
// Activate
void BoundaryCollider::activate() {
    active = true;
}

// Deactivate
void BoundaryCollider::deactivate() {
    active = false;
}

// Is Active
bool BoundaryCollider::isActive() const {
    return active;
}

// Get Id
std::string BoundaryCollider::getId() const {
    return "";
}

// Get Min Height
float BoundaryCollider::getMinHeight() const {
    return minHeight;
}

// Get Max Height
float BoundaryCollider::getMaxHeight() const {
    return maxHeight;
}

// Set Center
void BoundaryCollider::setCenter(const Vec3& c) {
    center = Vec3(c.x, 0.0f, c.z);
}

// Get Center
const Vec3& BoundaryCollider::getCenter() const {
    return center;
}

// Set Distance
void BoundaryCollider::setDistance(float d) {
    distance = d;
    minHeight = -d;
    maxHeight = d;
}

// Get Boundary Distance
float BoundaryCollider::getBoundaryDistance() const {
    return distance;
}

// Get Boundary Normal
Vec3 BoundaryCollider::getBoundaryNormal(const Vec3& p) const {
    Vec3 n{ 0, 0, 0 };
    Vec3 l = p - center;

    if(std::fabs(l.x) > distance)       n.x = (l.x > 0) ? 1.0f : -1.0;
    else if(std::fabs(l.z) > distance)  n.z = (l.z > 0) ? -1.0f : 1.0f;

    return n;
}

// Get Boundary Far
float BoundaryCollider::getBoundaryFar(const Vec3& p) const {
    Vec3 l = p - center;
    float xF = std::max(0.0f, std::fabs(l.x) - distance);
    float zF = std::max(0.0f, std::fabs(l.z) - distance);
    return std::max(xF, zF);
}

// Get BBox
BBox BoundaryCollider::getBBox() const {
    float v = 1e9f;
    return BBox(center.x - distance - thickness, -v, center.z - distance - thickness,
                center.x + distance + thickness, v, center.z + distance + thickness);
}

// Is Outside Boundary
bool BoundaryCollider::isOutsideBoundary(const Vec3& p) const {
    if(!active) return false;
    return std::fabs(p.x - center.x) > distance ||
            std::fabs(p.z - center.z) > distance;
}
// ---------------------------------------

/******************************
 * 
 * BBox
 * 
 **********************/
// Get Size X
float BBox::getSizeX() const {
    return maxX - minX;
}

// Get Size Y
float BBox::getSizeY() const {
    return maxY - minY;
}

// Get Size Z
float BBox::getSizeZ() const {
    return maxZ - minZ;
}

// Set From Center
void BBox::setFromCenter(const Vec3& c, const Vec3& size) {
    float hx = size.x * 0.5f, hy = size.y * 0.5f, hz = size.z * 0.5f;

    minX = c.x - hx; maxX = c.x + hx;
    minY = c.y - hy; maxY = c.y + hy;
    minZ = c.z - hz; maxZ = c.z + hz;
}

// From Center
static BBox fromCenter(const Vec3& c, const Vec3& s) {
    BBox b;
    b.setFromCenter(c, s);
    return b;
}

// Intersects
bool BBox::intersects(const BBox& o) const {
    return (minX <= o.maxX && maxX >= o.minX) &&
            (minY <= o.maxY && maxY >= o.minY) &&
            (minZ <= o.maxZ && maxZ >= o.minZ);
}
/**
 * 
 */

/***************************
 * 
 * Collider
 * 
 */
// Set Visible
void Collider::setVisible(bool v) {
    visible = v;
}

// Is Visible
bool Collider::isVisible() const {
    return visible;
}
/**
 * 
 */

/*****************************
 * 
 * Collision Manager
 * 
 */
CollisionManager::CollisionManager(Tick& tick, Mesh& mesh) : 
    tick(tick),    
    mesh(mesh) {};
CollisionManager::~CollisionManager() {};

/**
 * Collider Exists
 */
bool CollisionManager::colliderExists(const std::string& id) const {
    if(std::find(removedIds.begin(), removedIds.end(), id) != removedIds.end()) return false;
    return colliderIdSet.count(id) > 0;
}

/**
 * Get Collider
 */
Collider* CollisionManager::getCollider(const std::string& id) {
    for(Collider* c : colliders) if(c->getId() == id) return c;
    return nullptr;
}

/**
 * Get Colliders
 */
std::vector<Collider*> CollisionManager::getColliders() {
    return colliders;
}

/**
 * Rebuild
 */
void CollisionManager::build() {
    for(Collider* c : colliders) {
        if(c) c->build();
    }
}

/**
 * Local Box For
 */
const BBox& CollisionManager::localBoxFor(const std::string& type) {
    auto it = localBoxCache.find(type);
    if(it != localBoxCache.end()) return it->second;

    float v = 1e30f;
    float xMin =  v, yMin =  v, zMin =  v;
    float xMax = -v, yMax = -v, zMax = -v;

    if(const std::vector<float>* verts = mesh.getVertices(type)) {
        const size_t stride = Mesh::kVertexStride;
        for(size_t i = 0; i+2 < verts->size(); i += stride) {
            xMin = std::min(xMin, (*verts)[i+0]); xMax = std::max(xMax, (*verts)[i+0]);
            yMin = std::min(yMin, (*verts)[i+1]); yMax = std::max(yMax, (*verts)[i+1]);
            zMin = std::min(zMin, (*verts)[i+2]); zMax = std::max(zMax, (*verts)[i+2]);
        }
    }

    if(xMax < xMin) {
        xMin = yMin = zMin = 0;
        xMax = xMax = zMax = 0;
    }

    BBox box(xMin, yMin, zMin, xMax, yMax, zMax);
    auto [inserted, _] = localBoxCache.emplace(type, box);
    return inserted->second;
}

/**
 * Set
 */
void CollisionManager::set(uint32_t id, CollisionType type) {
    const std::string typeName = mesh.getType(id);
    if(typeName.empty()) return;

    const std::string sid = typeName;
    StaticCollider* sc = nullptr;

    switch(type) {
        // Static
        case CollisionType::STATIC_COLLIDER: {
            auto it = colliderByKey.find(sid);
            if(it != colliderByKey.end()) {
                sc = it->second;
            } else {
                const BBox& local = localBoxFor(typeName);
                auto owned = std::make_unique<StaticCollider>(local, sid, typeName);

                sc = owned.get();
                ownedColliders.push_back(std::move(owned));
                colliders.push_back(sc);
                colliderByKey[sid] = sc;
                colliderIdSet.insert(sid);
            }

            sc->addInstance(id, mesh.getPosition(id));
            break;
        }
        // Boundary
        case CollisionType::BOUNDARY_COLLIDER: {
            break;
        }
    }
}

void CollisionManager::set(RigidBody& rigidBody, CollisionType type) {
    for(const Entry& e : dynamicRigidBody) {
        if(e.rigidBody == &rigidBody) return;
    }
    dynamicRigidBody.push_back({ &rigidBody, type });
}

/**
 * Has RigidBody
 */
bool CollisionManager::hasRigidBody(const RigidBody& rigidBody) const {
    for(const Entry& e : dynamicRigidBody) {
        if(e.rigidBody == &rigidBody) return true;
    }
    return false;
}

/**
 * Check Collision
 */
std::vector<CollisionResult> CollisionManager::checkCollision(const BBox& bbox) {
    std::vector<CollisionResult> results;
    
    for(Collider* c : colliders) {
        if(auto* b = dynamic_cast<BoundaryCollider*>(c)) {
            if(b->isActive()) {
                Vec3 pos((bbox.minX + bbox.maxX) * 0.5f,
                        (bbox.minY + bbox.maxY) * 0.5f,
                        (bbox.minZ + bbox.maxZ) * 0.5f);
                if(b->isOutsideBoundary(pos)) {
                    results.push_back(CollisionResult(true,
                        b->getBoundaryNormal(pos), b->getBoundaryFar(pos), b,
                        CollisionType::BOUNDARY_COLLIDER
                    ));
                }
            }
        } else if(auto* s = dynamic_cast<StaticCollider*>(c)) {
            s->checkCollision(bbox, results);
        }
    }

    return results;
}

/**
 * Resolve Collision
 */
void CollisionManager::resolveCollision(RigidBody& rigidBody,
                                        const BBox& bbox,
                                        std::vector<CollisionResult>& collisions) 
{
    if(collisions.empty()) return;
    std::sort(collisions.begin(), collisions.end(),
            [](const CollisionResult& a, const CollisionResult& b) {
                return b.depth < a.depth;
            });

    for(const CollisionResult& c : collisions) {
        if(!c.otherCollider) continue;

        Vec3 pos = rigidBody.getPosition();

        // Boundary
        if(c.type == CollisionType::BOUNDARY_COLLIDER) {
            if(auto* b = dynamic_cast<BoundaryCollider*>(c.otherCollider)) {
                float dist = b->getBoundaryDistance();
                const Vec3& center = b->getCenter();
    
                float localX = pos.x - center.x;
                float localZ = pos.z - center.z;
    
                if(std::fabs(localX) > dist) pos.x = center.x + std::copysign(dist, localX);
                if(std::fabs(localZ) > dist) pos.z = center.z + std::copysign(dist, localZ);
    
                Vec3 vel = rigidBody.getVelocity();
                if(c.normal.x != 0.0f) vel.x = 0.0f;
                if(c.normal.z != 0.0f) vel.z = 0.0f;
                rigidBody.setVelocity(vel);
                rigidBody.setPosition(pos);
            }

            continue;
        }
        // Static
        if(c.type == CollisionType::STATIC_COLLIDER) {
            if(c.normal.x != 0) pos.x += c.normal.x * c.depth;
            if(c.normal.z != 0) pos.z += c.normal.z * c.depth;

            if(c.normal.y > 0.5f) {
                float halfH = (bbox.maxY - bbox.minY) * 0.5f;
                pos.y = c.otherWorldBox.maxY + halfH;
                Vec3 v = rigidBody.getVelocity();
                v.y = 0;
                rigidBody.setVelocity(v);
            }

            rigidBody.setPosition(pos);
        }
    }
}

void CollisionManager::resolveCollision(Collider& collider, std::vector<CollisionResult>& collisions) {
    if(collisions.empty()) return;

    RigidBody* rigidBody = collider.getRigidBody();
    if(!rigidBody) return;

    const BBox bounds = collider.getBBox();
    resolveCollision(*rigidBody, bounds, collisions);
}

/**
 * 
 * Update
 * 
 */
void CollisionManager::update() {
    const std::vector<Collider*> colliderSnapshot = colliders;
    for(Collider* collider : colliderSnapshot) {
        RigidBody* rigidBody = collider->getRigidBody();
        if(!rigidBody) continue;
        rigidBody->update();

        BBox bounds = collider->getBBox();
        std::vector<CollisionResult> results = checkCollision(bounds);
        resolveCollision(*collider, results);
    }
    const std::vector<Entry> rigidBodySnapshot = dynamicRigidBody;
    for(const Entry& entry : rigidBodySnapshot) {
        RigidBody* rigidBody = entry.rigidBody;
        rigidBody->update();

        BBox bounds;
        bounds.setFromCenter(rigidBody->getPosition(), rigidBody->getSize());

        std::vector<CollisionResult> results = checkCollision(bounds);
        resolveCollision(*rigidBody, bounds, results);
    }
}

/**
 * Clear
 */
void CollisionManager::clear() {
    colliders.clear();
    ownedColliders.clear();
    colliderIdSet.clear();
    removedIds.clear();
    localBoxCache.clear();
}

/**
 * Clear Colliders
 */
void CollisionManager::clearColliders() {
    for(auto& owned : ownedColliders) {
        if(owned) owned->clearInstances();
    }
}
/**
 * 
 */