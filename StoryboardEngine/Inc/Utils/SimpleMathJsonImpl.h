#pragma once
#include "nlohmann/json.hpp"
#include "directxtk/SimpleMath.h"
using namespace DirectX::SimpleMath;

namespace DirectX::SimpleMath
{
	/*inline void to_json(nlohmann::json& j, const Vector2& v)
	{
		j = nlohmann::ordered_json{ {"x", v.x}, {"y", v.y}};
	}

	inline void from_json(const nlohmann::json& j, Vector2& v)
	{
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
	}

	inline void to_json(nlohmann::json& j, const Vector3& v)
	{
		j = nlohmann::ordered_json{ {"x", v.x}, {"y", v.y}, {"z", v.z}};
	}

	inline void from_json(const nlohmann::json& j, Vector3& v)
	{
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
		j.at("z").get_to(v.z);
	}

	inline void to_json(nlohmann::json& j, const Vector4& v)
	{
		j = nlohmann::ordered_json{ {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
	}

	inline void from_json(const nlohmann::json& j, Vector4& v)
	{
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
		j.at("z").get_to(v.z);
		j.at("w").get_to(v.w);
	}

	inline void to_json(nlohmann::json& j, const Quaternion& q)
	{
		j = nlohmann::ordered_json{ {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
	}

	inline void from_json(const nlohmann::json& j, Quaternion& q)
	{
		j.at("x").get_to(q.x);
		j.at("y").get_to(q.y);
		j.at("z").get_to(q.z);
		j.at("w").get_to(q.w);
	}

	inline void to_json(nlohmann::json& j, const Matrix& m)
	{
		j = nlohmann::ordered_json{
			{"m00", m.m[0][0]}, {"m01", m.m[0][1]}, {"m02", m.m[0][2]}, {"m03", m.m[0][3]},
			{"m10", m.m[1][0]}, {"m11", m.m[1][1]}, {"m12", m.m[1][2]}, {"m13", m.m[1][3]},
			{"m20", m.m[2][0]}, {"m21", m.m[2][1]}, {"m22", m.m[2][2]}, {"m23", m.m[2][3]},
			{"m30", m.m[3][0]}, {"m31", m.m[3][1]}, {"m32", m.m[3][2]}, {"m33", m.m[3][3]}
		};
	}

	inline void from_json(const nlohmann::json& j, Matrix& m)
	{
		j.at("m00").get_to(m.m[0][0]);
		j.at("m01").get_to(m.m[0][1]);
		j.at("m02").get_to(m.m[0][2]);
		j.at("m03").get_to(m.m[0][3]);
		j.at("m10").get_to(m.m[1][0]);
		j.at("m11").get_to(m.m[1][1]);
		j.at("m12").get_to(m.m[1][2]);
		j.at("m13").get_to(m.m[1][3]);
		j.at("m20").get_to(m.m[2][0]);
		j.at("m21").get_to(m.m[2][1]);
		j.at("m22").get_to(m.m[2][2]);
		j.at("m23").get_to(m.m[2][3]);
		j.at("m30").get_to(m.m[3][0]);
		j.at("m31").get_to(m.m[3][1]);
		j.at("m32").get_to(m.m[3][2]);
		j.at("m33").get_to(m.m[3][3]);
	}*/

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Vector2, x, y)
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Vector3, x, y, z)
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Vector4, x, y, z, w)
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Quaternion, x, y, z, w)
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Matrix, 
			_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44)
}
