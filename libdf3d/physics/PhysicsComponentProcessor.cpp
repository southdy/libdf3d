#include "PhysicsComponentProcessor.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "impl/BulletInterface.h"
#include "PhysicsComponentCreationParams.h"
#include "PhysicsHelpers.h"
#include <libdf3d/math/AABB.h>
#include <libdf3d/math/BoundingSphere.h>
#include <libdf3d/math/ConvexHull.h>
#include <libdf3d/render/MeshData.h>
#include <libdf3d/3d/StaticMeshComponentProcessor.h>
#include <libdf3d/3d/SceneGraphComponentProcessor.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/base/DebugConsole.h>
#include <libdf3d/base/TimeManager.h>
#include <libdf3d/game/ComponentDataHolder.h>
#include <libdf3d/game/World.h>

namespace df3d {

static_assert(sizeof(int) >= sizeof(df3d::Entity::IdType), "Can't store user data in bullet user pointer");

class PhysicsComponentMotionState : public btMotionState
{
    World &m_world;
    Entity m_holder;
    btTransform m_transform;

public:
    PhysicsComponentMotionState(Entity e, World &w)
        : m_world(w),
        m_holder(e)
    {
        auto orientation = w.sceneGraph().getOrientation(m_holder);
        auto position = w.sceneGraph().getWorldPosition(m_holder);

        m_transform = btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), glmTobt(position));
    }

    ~PhysicsComponentMotionState()
    {

    }

    void getWorldTransform(btTransform &worldTrans) const
    {
        worldTrans = m_transform;
    }

    void setWorldTransform(const btTransform &worldTrans)
    {
        auto rot = worldTrans.getRotation();

        auto df3dPos = btToGlm(worldTrans.getOrigin());
        auto df3dOrient = glm::quat(rot.w(), rot.x(), rot.y(), rot.z());
        glm::mat4 ATTRIBUTE_ALIGNED16(df3dWorldTransf);
        worldTrans.getOpenGLMatrix(glm::value_ptr(df3dWorldTransf));

        // TODO_ecs: more fast lookup!
        m_world.sceneGraph().setTransform(m_holder, df3dPos, df3dOrient, df3dWorldTransf);

        m_transform = worldTrans;
    }
};

struct PhysicsComponentProcessor::Impl
{
    World &df3dWorld;

    btDefaultCollisionConfiguration *collisionConfiguration = nullptr;
    btCollisionDispatcher *dispatcher = nullptr;
    btBroadphaseInterface *overlappingPairCache = nullptr;
    btSequentialImpulseConstraintSolver *solver = nullptr;
    btDiscreteDynamicsWorld *dynamicsWorld = nullptr;

    physics_impl::BulletDebugDraw *debugDraw;

    struct Data
    {
        Entity holder;

        btRigidBody *body = nullptr;
        bool initialized = false;
        weak_ptr<MeshData> meshData;
        shared_ptr<PhysicsComponentCreationParams> params;
    };

    ComponentDataHolder<Data> data;

    Impl(World &w)
        : df3dWorld(w)
    {
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        overlappingPairCache = new btDbvtBroadphase();
        solver = new btSequentialImpulseConstraintSolver();
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

        dynamicsWorld->setGravity(btVector3(0, 0, 0));

        debugDraw = new physics_impl::BulletDebugDraw();
        dynamicsWorld->setDebugDrawer(debugDraw);
    }

    ~Impl()
    {
        data.clear();

        for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject *obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody *body = btRigidBody::upcast(obj);
            if (body && body->getMotionState())
                delete body->getMotionState();
            if (body && body->getCollisionShape())
                delete body->getCollisionShape();

            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfiguration;
        delete debugDraw;
    }

    void initialize(Data &data)
    {
        assert(!data.initialized);

        btCollisionShape *colShape = nullptr;
        switch (data.params->shape)
        {
        case CollisionShapeType::BOX:
        {
            auto aabb = svc().world().staticMesh().getAABB(data.holder);
            if (!aabb.isValid())
            {
                glog << "Can not create box shape for rigid body. AABB is invalid" << logwarn;
                return;
            }

            auto half = (aabb.maxPoint() - aabb.minPoint()) / 2.0f;
            colShape = new btBoxShape(btVector3(half.x, half.y, half.z));
        }
        break;
        case CollisionShapeType::SPHERE:
        {
            auto sphere = svc().world().staticMesh().getBoundingSphere(data.holder);
            if (!sphere.isValid())
            {
                glog << "Can not create sphere shape for rigid body. Bounding sphere is invalid" << logwarn;
                return;
            }

            auto radius = sphere.getRadius();
            colShape = new btSphereShape(radius);
        }
        break;
        case CollisionShapeType::CONVEX_HULL:
        {
            auto convexHull = data.meshData.lock()->getConvexHull();
            if (!convexHull || !convexHull->isValid())
            {
                glog << "Can not create convex hull shape for rigid body. Hull is invalid" << logwarn;
                return;
            }

            const auto &vertices = convexHull->getVertices();
            std::vector<btVector3> tempPoints;
            for (const auto &v : vertices)
                tempPoints.push_back({ v.x, v.y, v.z });

            colShape = new btConvexHullShape((btScalar*)tempPoints.data(), tempPoints.size());

            // FIXME: what to do if scale has changed?
            auto scale = svc().world().sceneGraph().getLocalScale(data.holder);
            colShape->setLocalScaling(glmTobt(scale));
        }
        break;
        default:
            assert(false);
            return;
        }

        btVector3 localInertia(0, 0, 0);
        if (!glm::epsilonEqual(data.params->mass, 0.0f, glm::epsilon<float>()))
            colShape->calculateLocalInertia(data.params->mass, localInertia);

        // Set motion state.
        auto myMotionState = createMotionState(data.holder);

        // Fill body properties.
        btRigidBody::btRigidBodyConstructionInfo rbInfo(data.params->mass, myMotionState, colShape, localInertia);
        rbInfo.m_friction = data.params->friction;
        rbInfo.m_restitution = data.params->restitution;
        rbInfo.m_linearDamping = data.params->linearDamping;
        rbInfo.m_angularDamping = data.params->angularDamping;

        data.body = new btRigidBody(rbInfo);

        if (data.params->group != -1 && data.params->mask != -1)
            dynamicsWorld->addRigidBody(data.body, data.params->group, data.params->mask);
        else
            dynamicsWorld->addRigidBody(data.body);

        if (data.params->disableDeactivation)
            data.body->setActivationState(DISABLE_DEACTIVATION);

        data.body->setUserIndex(data.holder.id);

        data.initialized = true;
        data.params.reset();
    }

    btMotionState* createMotionState(Entity e)
    {
        return new PhysicsComponentMotionState(e, df3dWorld);
    }
};

void PhysicsComponentProcessor::update()
{
    for (auto &data : m_pimpl->data.rawData())
    {
        if (!data.initialized)
        {
            if (data.meshData.lock()->isInitialized())
                m_pimpl->initialize(data);
        }
    }

    m_pimpl->dynamicsWorld->stepSimulation(svc().timer().getFrameDelta(TimeChannel::GAME), 10);
}

void PhysicsComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    m_pimpl->data.cleanStep(deleted);
    m_pimpl->debugDraw->clean();
}

void PhysicsComponentProcessor::draw(RenderQueue *ops)
{
    if (auto console = svc().debugConsole())
    {
        if (!console->getCVars().get<bool>(CVAR_DEBUG_DRAW))
            return;
    }

    // Collect render operations.
    m_pimpl->dynamicsWorld->debugDrawWorld();
    // Append to render queue.
    m_pimpl->debugDraw->flushRenderOperations(ops);
}

PhysicsComponentProcessor::PhysicsComponentProcessor(World *w)
    : m_pimpl(new Impl(*w))
{
    auto physicsWorld = m_pimpl->dynamicsWorld;
    m_pimpl->data.setDestructionCallback([this, physicsWorld](const Impl::Data &data) {
        if (data.body)
        {
            physicsWorld->removeRigidBody(data.body);
            auto motionState = data.body->getMotionState();
            delete motionState;
            auto shape = data.body->getCollisionShape();
            delete shape;
            delete data.body;
        }
    });
}

PhysicsComponentProcessor::~PhysicsComponentProcessor()
{
    //glog << "PhysicsComponentProcessor::~PhysicsComponentProcessor alive entities" << m_pimpl->data.rawData().size() << logdebug;
}

btRigidBody* PhysicsComponentProcessor::getBody(Entity e)
{
    return m_pimpl->data.getData(e).body;
}

glm::vec3 PhysicsComponentProcessor::getCenterOfMass(Entity e)
{
    auto body = m_pimpl->data.getData(e).body;
    assert(body);
    return btToGlm(body->getCenterOfMassPosition());
}

void PhysicsComponentProcessor::teleportPosition(Entity e, const glm::vec3 &pos)
{
    auto body = getBody(e);
    if (body)
    {
        auto tr = body->getWorldTransform();
        tr.setOrigin(glmTobt(pos));

        body->setWorldTransform(tr);

        m_pimpl->dynamicsWorld->synchronizeSingleMotionState(body);
    }
    else
    {
        m_pimpl->df3dWorld.sceneGraph().setPosition(e, pos);
    }
}

void PhysicsComponentProcessor::teleportOrientation(Entity e, const glm::quat &orient)
{
    auto body = getBody(e);
    if (body)
    {
        auto tr = body->getWorldTransform();
        tr.setRotation(btQuaternion(orient.x, orient.y, orient.z, orient.w));

        body->setWorldTransform(tr);

        m_pimpl->dynamicsWorld->synchronizeSingleMotionState(body);
    }
    else
    {
        m_pimpl->df3dWorld.sceneGraph().setOrientation(e, orient);
    }
}

void PhysicsComponentProcessor::add(Entity e, const PhysicsComponentCreationParams &params, shared_ptr<MeshData> meshData)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has an physics component" << logwarn;
        return;
    }

    Impl::Data data;
    data.meshData = meshData;
    data.holder = e;
    data.params = make_shared<PhysicsComponentCreationParams>(params);

    if (meshData->isInitialized())
        m_pimpl->initialize(data);

    m_pimpl->data.add(e, data);
}

void PhysicsComponentProcessor::add(Entity e, btRigidBody *body, short group, short mask)
{
    assert(body);

    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has an physics component" << logwarn;
        return;
    }

    Impl::Data data;
    data.holder = e;
    data.body = body;
    data.initialized = true;

    if (group != -1 && mask != -1)
        m_pimpl->dynamicsWorld->addRigidBody(body, group, mask);
    else
        m_pimpl->dynamicsWorld->addRigidBody(body);

    data.body->setUserIndex(e.id);

    m_pimpl->data.add(e, data);
}

void PhysicsComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove physics component from an entity. Component is not attached" << logwarn;
        return;
    }

    m_pimpl->data.remove(e);
}

bool PhysicsComponentProcessor::has(Entity e)
{
    return m_pimpl->data.lookup(e).valid();
}

btDynamicsWorld* PhysicsComponentProcessor::getPhysicsWorld()
{
    return m_pimpl->dynamicsWorld;
}

btMotionState* PhysicsComponentProcessor::createMotionState(Entity e)
{
    return m_pimpl->createMotionState(e);
}

}
