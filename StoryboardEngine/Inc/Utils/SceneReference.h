#pragma once
#include <memory>
#include <unordered_map>
#include "nameof.hpp"
#include <nlohmann/json.hpp>
#include "Utils/Logger.h"
#include "Internal/UUID.h"
#include "Utils/ApplicationUtils.h"
#include "Core/SerializableObject.h"

namespace StoryboardEngine
{
	// Non-templated base for SceneReference so we can store heterogeneous references
	class SceneReferenceBase
	{
	protected:
		friend class Scene;

		// Map of UUID -> SceneReferenceBase* to fix-up references after deserialization.
		static std::vector<std::pair<UUID, SceneReferenceBase*>> objectsToLink;
	public:
		virtual ~SceneReferenceBase() = 0;
		virtual void Resolve(const std::shared_ptr<SerializableObject>& target) = 0;

	};

	// A reference to a serializable object, basically a weak pointer with forced serialization support
	template<typename T>
	class SceneReference final : public SceneReferenceBase // : public std::enable_shared_from_this<SceneReference<T>>
	{
		// ToDo: Find a way to use this static assert without circular dependency issues
		//static_assert(std::is_base_of<SerializableObject, T>::value, "SceneReference can only be used with SerializableObject derived types");
	private:
		std::weak_ptr<T> ptr;

		friend class Scene;
	public:
		SceneReference() : ptr() {}
		SceneReference(std::shared_ptr<T> p) : ptr(p) {}
		SceneReference(std::weak_ptr<T> p) : ptr(p) {}
		SceneReference(std::nullptr_t) : ptr() {}
		SceneReference(const SceneReference<T>& other) : ptr(other.ptr) {}
		SceneReference<T>& operator=(const SceneReference<T>& other)
		{
			ptr = other.ptr;
			return *this;
		}

		~SceneReference()
		{
			// Might be possible to only need this when in editor, keep for safety for now

			if (!ApplicationUtils::IsDeserializingScene()) // If we're not deserializing a scene, we shouldn't have any unlinked references
				return;

			for (auto it = SceneReferenceBase::objectsToLink.begin(); it != SceneReferenceBase::objectsToLink.end(); )
			{
				if (it->second == static_cast<SceneReferenceBase*>(this))
				{
					Logger::LogWarning("SceneReference<", NAMEOF_SHORT_TYPE(T), "> with UUID ", it->first.ToString(), " destroyed without being linked");
					it = SceneReferenceBase::objectsToLink.erase(it);
					break;
				}
				else
				{
					++it;
				}
			}
		}

		// Templated constructor for converting between base and derived types
		template<typename U>
		SceneReference(const SceneReference<U>& other)
		{
			static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "SceneReference can only be constructed from derived & base types");

			if (!other)
				return;

			std::shared_ptr<T> thisAsShared = std::dynamic_pointer_cast<T>(other.ptr.lock());

			if (!thisAsShared)
			{
				// ToDo: Decide if we want to throw this error, it should never happen due to the static_assert
				// Logger::LogError("Invalid SceneReference conversion from ", NAMEOF_SHORT_TYPE(U), " to ", NAMEOF_SHORT_TYPE(T));
				return;
			}

			ptr = thisAsShared;
		}

		void Resolve(const std::shared_ptr<SerializableObject>& target) override
		{
			//ptr = std::static_pointer_cast<T>(target);
			//static_assert(std::is_base_of<SerializableObject, T>::value, "SceneReference can only be used with SerializableObject derived types");

			if (!target)
			{
				Logger::LogWarning("Null target provided when resolving SceneReference<", NAMEOF_SHORT_TYPE(T), ">");
				ptr = std::weak_ptr<T>();
				return;
			}

			// Can't use _pointer_cast as T might not be defined when this object is defined
			// This constructor will keep the shared_ptr's reference, but still allow us to have a weak_ptr of the correct type
			ptr = std::shared_ptr<T>(target, reinterpret_cast<T*>(target.get()));

			if (ptr.expired())
			{
				Logger::LogError("Failed to resolve SceneReference<", NAMEOF_SHORT_TYPE(T), "> to object with id ", target->GetUUID().ToString());
			}
		}

		template<typename U>
		SceneReference<U> As() const
		{
			static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, 
				"SceneReference can only be converted to derived & base types");
			if (!ptr.expired())
			{
				std::shared_ptr<U> castedPtr = std::dynamic_pointer_cast<U>(ptr.lock());
				if (castedPtr)
				{
					return SceneReference<U>(castedPtr);
				}
				else
				{
					// ToDo: Decide if we want to throw this error
					//Logger::LogError("Invalid SceneReference conversion from ", NAMEOF_SHORT_TYPE(T), " to ", NAMEOF_SHORT_TYPE(U));
				}
			}
			return SceneReference<U>();
		}

		// Like As, but only requires forward declaration of U. Returns a null SceneReference if U & T are not related
		template<typename U>
		SceneReference<U> TryAs() const
		{
			if (!ptr.expired())
			{
				auto castedPtr = std::shared_ptr<U>(ptr.lock(), reinterpret_cast<U*>(ptr.lock().get()));

				if (castedPtr)
				{
					return SceneReference<U>(castedPtr);
				}
			}
			return SceneReference<U>();
		}

		// Assignment operators
		T* get() const 
		{ 
			if (auto sp = ptr.lock()) 
				return sp.get();

			return nullptr;
		}

		T& operator*() const
		{
			static_assert(!std::is_void<T>::value, "Cannot use operator* on SceneReference of void type");

			auto sp = ptr.lock();
			if (sp) 
				return *sp; 

			Logger::LogError("Attempting to dereference a null SceneReference of type ", NAMEOF_SHORT_TYPE(T));
			throw std::runtime_error("Dereferencing null SceneReference");
		}

		T* operator->() const
		{
			static_assert(!std::is_void<T>::value, "Cannot use operator-> on SceneReference of void type");

			auto sp = ptr.lock();
			if (sp) 
				return sp.get(); 
			
			Logger::LogError("Attempting to access a null SceneReference of type ", NAMEOF_SHORT_TYPE(T));
			throw std::runtime_error("Dereferencing null SceneReference");
		}

		bool operator==(const SceneReference<T>& other) const
		{
			// Function needs to be explicitly defined because without, == chooses the bool() operator instead of <=> :shrug:

			return ptr.lock() == other.ptr.lock();
		}

		operator bool() const
		{
			return !ptr.expired();
		}

		auto operator<=>(const SceneReference<T>& other) const
		{
			return ptr.lock() <=> other.ptr.lock();
		}

		// Serialization
		template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
		friend void to_json(BasicJsonType& nlohmann_json_j, const SceneReference<T>& nlohmann_json_t)
		{
			if (auto sp = nlohmann_json_t.TryAs<SerializableObject>())
			{
				nlohmann_json_j = sp->GetUUID();
			}
			else
			{
				nlohmann_json_j = nullptr;
			}
		}

		template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
		friend void from_json(const BasicJsonType& nlohmann_json_j, SceneReference<T>& nlohmann_json_t)
		{
			if (nlohmann_json_j.is_null())
			{
				// ToDo: Usually this is fine (e.g. root's parent, or references that just aren't set)
				// but it would be nice to give more info just in case it's unexpected
				Logger::LogInfo("Loaded SceneReference<", NAMEOF_SHORT_TYPE(T), "> as Null");
				return;
			}

			StoryboardEngine::UUID id = nlohmann_json_j.get<StoryboardEngine::UUID>();
			SceneReferenceBase::objectsToLink.emplace_back(id, static_cast<SceneReferenceBase*>(&nlohmann_json_t));
			Logger::LogInfo("Registered SceneReference<", NAMEOF_SHORT_TYPE(T), "> for linking with UUID ", id.ToString());
		}

	private:
		// For the derived class conversion constructor
		template<typename U>
		friend class SceneReference;
	};
}
