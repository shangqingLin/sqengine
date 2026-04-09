
export const ObjectType = {
    Node: 1,
    Scene: 2,
    PyhsicsSystem: 3,
    TweenSystem: 4,
    CommandBufferBridge: 5,
    Mesh: 6,
    VertexBuffer: 7,
    IndexBuffer: 8,
    MeshInstance: 9
}

export const ComponentType = {
    Transform2DComponent: 1,
    SpriteComponent: 2,
    CameraComponent: 3,
    canvas: 4,
    RigidBody2DComponent: 5,
    BoxCollision2DComponent: 6,
    CircleCollision2DComponent: 7,
    PolygonCollision2DComponent: 8,
    GraphicsComponent: 9,
    ChainCollision2DComponent: 10,
    CapsuleCollision2DComponent: 11,
    LabelComponent: 12,
    ParticlePhysics2DComponent: 13,
    UIContentComponent: 14,
    Character2DComponent: 15,
    SpineComponent: 16,
    DistanceJoint2DComponent: 17,
    WheelJoint2DComponent: 18,
    MotorJoint2DComponent: 19,
    MouseJoint2DComponent: 20,
    PrismaticJoint2DComponent: 21,
    RevoluteJoint2DComponent: 22,
    WeldJoint2DComponent: 23,
    Ragdoll2DComponent: 24,
    PBD2DComponent: 25,
    JointConnect2DComponent: 26,
    Particle2DComponent: 27,
    Renderer: 28,
    WallSlide2DComponent: 30,
}

export enum NativeObjectType {
    gl = 1,
    physics = 2,
    tween = 3,
    Node = 4,
    Truck = 5
}