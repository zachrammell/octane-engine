#pragma once
#include "math.h"
#include "mass_data.h"

namespace Octane
{
    class RigidBody
    {
    public:
        RigidBody();
        ~RigidBody();

        void Integrate(float dt);

    private:
        //linear data - positional
        DirectX::XMFLOAT3 position_;
        DirectX::XMFLOAT3 linear_velocity_;
        DirectX::XMFLOAT3 force_accumulator_;
        DirectX::XMFLOAT3 linear_constraints_;

        //angular data - rotational
        DirectX::XMFLOAT4 orientation_;
        DirectX::XMFLOAT4 inverse_orientation_;
        DirectX::XMFLOAT3 angular_velocity_;
        DirectX::XMFLOAT3 torque_accumulator_;
        DirectX::XMFLOAT3 angular_constraints_;

        //mass data
        MassData            mass_data_;
        DirectX::XMFLOAT3   global_centroid_;
        DirectX::XMFLOAT3X3 global_inertia_;
        DirectX::XMFLOAT3X3 global_inverse_inertia_;
    };
}
