#include "pch.h"

// Defining the declared but not defined simple math definitions (E.g. Vector3::UnitY)

const DirectX::SimpleMath::Vector2 DirectX::SimpleMath::Vector2::Zero = DirectX::SimpleMath::Vector2(0, 0);
const DirectX::SimpleMath::Vector2 DirectX::SimpleMath::Vector2::One  = DirectX::SimpleMath::Vector2(1, 1);

const DirectX::SimpleMath::Vector2 DirectX::SimpleMath::Vector2::UnitX = DirectX::SimpleMath::Vector2(1, 0);
const DirectX::SimpleMath::Vector2 DirectX::SimpleMath::Vector2::UnitY = DirectX::SimpleMath::Vector2(0, 1);



const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::Zero = DirectX::SimpleMath::Vector3(0, 0, 0);
const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::One  = DirectX::SimpleMath::Vector3(1, 1, 1);

const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::UnitX = DirectX::SimpleMath::Vector3(1, 0, 0);
const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::UnitY = DirectX::SimpleMath::Vector3(0, 1, 0);
const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::UnitZ = DirectX::SimpleMath::Vector3(0, 0, 1);

const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::Up       = DirectX::SimpleMath::Vector3(0,  1, 0);
const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::Down     = DirectX::SimpleMath::Vector3(0, -1, 0);
const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::Right    = DirectX::SimpleMath::Vector3( 1, 0, 0);
const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::Left     = DirectX::SimpleMath::Vector3(-1, 0, 0);
const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::Forward  = DirectX::SimpleMath::Vector3(0, 0,  1);
const DirectX::SimpleMath::Vector3 DirectX::SimpleMath::Vector3::Backward = DirectX::SimpleMath::Vector3(0, 0, -1);



const DirectX::SimpleMath::Vector4 DirectX::SimpleMath::Vector4::Zero = DirectX::SimpleMath::Vector4(0, 0, 0, 0);
const DirectX::SimpleMath::Vector4 DirectX::SimpleMath::Vector4::One  = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
const DirectX::SimpleMath::Vector4 DirectX::SimpleMath::Vector4::UnitX = DirectX::SimpleMath::Vector4(1, 0, 0, 0);
const DirectX::SimpleMath::Vector4 DirectX::SimpleMath::Vector4::UnitY = DirectX::SimpleMath::Vector4(0, 1, 0, 0);
const DirectX::SimpleMath::Vector4 DirectX::SimpleMath::Vector4::UnitZ = DirectX::SimpleMath::Vector4(0, 0, 1, 0);
const DirectX::SimpleMath::Vector4 DirectX::SimpleMath::Vector4::UnitW = DirectX::SimpleMath::Vector4(0, 0, 0, 1);



const DirectX::SimpleMath::Matrix DirectX::SimpleMath::Matrix::Identity = DirectX::SimpleMath::Matrix();



const DirectX::SimpleMath::Quaternion DirectX::SimpleMath::Quaternion::Identity = DirectX::SimpleMath::Quaternion();
