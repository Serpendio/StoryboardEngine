#pragma once

#include "nlohmann\json.hpp"
#include "SimpleMathJsonImpl.h"

// ToDo: Sort this messy file out
// ToDo: Hint file?

// Macro for serializing objects, adapted from the json macros. Note for default, the Type must be default constructable
#define TO_JSON(v1) StoryboardEngine::Logger::LogInfo("Serializing ", #v1); nlohmann_json_j[#v1] = nlohmann_json_t.v1;
#define FROM_JSON(v1) StoryboardEngine::Logger::LogInfo("Deserializing ", #v1); nlohmann_json_j.at(#v1).get_to<decltype(nlohmann_json_t.v1)>(nlohmann_json_t.v1);
#define TO_JSON_WITH_DEFAULT(v1) if (nlohmann_json_default_obj.v1 != nlohmann_json_t.v1) { StoryboardEngine::Logger::LogInfo("Serializing ", #v1); nlohmann_json_j[#v1] = nlohmann_json_t.v1; }
#define FROM_JSON_WITH_DEFAULT(v1) StoryboardEngine::Logger::LogInfo("Deserializing ", #v1); nlohmann_json_t.v1 = nlohmann_json_j.value(#v1, nlohmann_json_default_obj.v1);
#define POINTER_TO_JSON(v1) if (nlohmann_json_t.v1) { StoryboardEngine::Logger::LogInfo("Serializing ", #v1); nlohmann_json_j[#v1] = *nlohmann_json_t.v1; } else { nlohmann_json_j[#v1] = nullptr; }
#define POINTER_FROM_JSON(v1) if (nlohmann_json_j.at(#v1).is_null()) { nlohmann_json_t.v1 = nullptr; } else { StoryboardEngine::Logger::LogInfo("Deserializing ", #v1); v1 = new decltype(*nlohmann_json_t.v1)(nlohmann_json_j.at(#v1).get<decltype(*nlohmann_json_t.v1)>()); }

// Macro to save or load the data within the variable
#define JSON_VARIABLE(v1) \
	if constexpr (json_scene_saving) { TO_JSON(v1) } \
	else { FROM_JSON(v1) }

// Like JSON_VARIABLE but only saves if the data is different from the default constructed value, can reduce file size
#define JSON_VARIABLE_WITH_DEFAULT(v1) \
	if constexpr (json_scene_saving) { TO_JSON_WITH_DEFAULT(v1) } \
	else { FROM_JSON_WITH_DEFAULT(v1) }
// Macro to save or load the data within a pointer variable, assumes the pointer is owning and either nullptr or valid
#define JSON_POINTER(v1) \
	if constexpr (json_scene_saving) { POINTER_TO_JSON(v1) } \
	else { POINTER_FROM_JSON(v1) }

// ToDo: Support serialization with multiple base classes
// ToDo: The ADD_SERIALIZATION macros should use templated json types (see NLOHMANN_DEFINE_TYPE_INTRUSIVE)
// ToDo: Define JSON_USE_IMPLICIT_CONVERSIONS = 0

// Uses SFINAE to allow custom macros without causing compile errors

// Macro to add serialization to a class with a base class
#define ADD_SERIALIZATION(BaseType, Type, ...)  \
    template <bool json_scene_saving> \
    static void serialize(nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
		NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_EXPAND, __VA_ARGS__)) \
    } \
	virtual void serialize_json(nlohmann::ordered_json& nlohmann_json_j, const bool& to_json_bool) override { \
		BaseType::serialize_json(nlohmann_json_j, to_json_bool); \
		if (to_json_bool) \
			serialize<true>(nlohmann_json_j, *this); \
		else \
			serialize<false>(nlohmann_json_j, *this); \
	} \
    friend void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { \
        auto& non_const_obj = const_cast<Type&>(nlohmann_json_t); \
        non_const_obj.serialize_json(nlohmann_json_j, true); \
    } \
    friend void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
		auto& non_const_obj = const_cast<nlohmann::ordered_json&>(nlohmann_json_j); \
        nlohmann_json_t.serialize_json(non_const_obj, false); \
    }


// Macro to add serialization to a class with a base class but no member variables
#define ADD_SERIALIZATION_PURE(BaseType, Type) \
	virtual void serialize_json(nlohmann::ordered_json& nlohmann_json_j, const bool& to_json_bool) override { \
		BaseType::serialize_json(nlohmann_json_j, to_json_bool); \
	} \
    friend void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { \
        auto& non_const_obj = const_cast<Type&>(nlohmann_json_t); \
        non_const_obj.serialize_json(nlohmann_json_j, true); \
    } \
    friend void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
        auto& non_const_obj = const_cast<nlohmann::ordered_json&>(nlohmann_json_j); \
        nlohmann_json_t.serialize_json(non_const_obj, false); \
    }


#define ADD_SERIALIZATION_WITH_DEFAULT(BaseType, Type, ...)  \
    template <bool json_scene_saving> \
    static void serialize(nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
		const Type nlohmann_json_default_obj{}; \
		NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_EXPAND, __VA_ARGS__)) \
    } \
	virtual void serialize_json(nlohmann::ordered_json& nlohmann_json_j, const bool& to_json_bool) { \
		BaseType::serialize_json(nlohmann_json_j, to_json_bool); \
		if (to_json_bool) \
			serialize<true>(nlohmann_json_j, *this); \
		else \
			serialize<false>(nlohmann_json_j, *this); \
	} \
    friend void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { \
        auto& non_const_obj = const_cast<Type&>(nlohmann_json_t); \
        non_const_obj.serialize_json(nlohmann_json_j, true); \
    } \
    friend void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
		auto& non_const_obj = const_cast<nlohmann::ordered_json&>(nlohmann_json_j); \
        nlohmann_json_t.serialize_json(non_const_obj, false); \
    }

// Macro to add serialization to a class with no base class
#define ADD_SERIALIZATION_NO_PARENT(Type, ...)  \
    template <bool json_scene_saving> \
    static void serialize(nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
		NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_EXPAND, __VA_ARGS__)) \
    } \
	virtual void serialize_json(nlohmann::ordered_json& nlohmann_json_j, const bool& to_json_bool) { \
		if (to_json_bool) \
			serialize<true>(nlohmann_json_j, *this); \
		else \
			serialize<false>(nlohmann_json_j, *this); \
	} \
    friend void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { \
        auto& non_const_obj = const_cast<Type&>(nlohmann_json_t); \
        non_const_obj.serialize_json(nlohmann_json_j, true); \
    } \
    friend void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
		auto& non_const_obj = const_cast<nlohmann::ordered_json&>(nlohmann_json_j); \
        nlohmann_json_t.serialize_json(non_const_obj, false); \
    }

// Macro to add serialization to a class with no base class but no member variables
#define ADD_SERIALIZATION_NO_PARENT_PURE(Type)  \
	virtual void serialize_json(nlohmann::ordered_json& nlohmann_json_j, const bool& to_json_bool) {} \
    friend void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { } \
    friend void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { }

// Macro to add serialization to a class with no base class, needed when using default values
#define ADD_SERIALIZATION_NO_PARENT_WITH_DEFAULT(Type, ...)  \
    template <bool json_scene_saving> \
    static void serialize(nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
        auto& non_const_obj = const_cast<Type&>(nlohmann_json_t); \
		NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_EXPAND, __VA_ARGS__)) \
    } \
    virtual void serialize_json(nlohmann::ordered_json& nlohmann_json_j, const bool& to_json_bool) {} \
    friend void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { \
        auto& non_const_obj = const_cast<Type&>(nlohmann_json_t); \
        Type::serialize<true>(nlohmann_json_j, non_const_obj); \
    } \
    friend void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { \
		auto& non_const_obj = const_cast<nlohmann::ordered_json&>(nlohmann_json_j); \
        Type::serialize<false>(non_const_obj, nlohmann_json_t); \
    }

