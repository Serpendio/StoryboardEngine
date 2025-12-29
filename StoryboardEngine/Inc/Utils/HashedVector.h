#pragma once
#include "pch.h"

namespace StoryboardEngine
{
	// Fast access, fast iteration, slow removal
	template<typename KeyType, typename ValueType>
	class HashedVector
	{
	private:
		std::vector<ValueType> elements;
		std::unordered_map<KeyType, size_t> indexMap; // Maps key to index in elements

	public:
		HashedVector() = default;

		bool Contains(const KeyType& key) const
		{
			return indexMap.contains(key);
		}

		// Removes an element by key, preserving order. Returns true if removal was success
		bool Remove(const KeyType& key)
		{
			auto it = indexMap.find(key);
			if (it == indexMap.end())
			{
				StoryboardEngine::Logger::LogError("Key not found in HashedVector");
				return false;
			}

			size_t index = it->second;
			elements.erase(elements.begin() + index);
			indexMap.erase(it);

			// Update indices in the map
			for (auto& [k, v] : indexMap)
			{
				if (v > index)
				{
					v--;
				}
			}

			return true;
		}

		// Removes an element by swapping it with the last element and popping back. Does not preserve order. Returns true if removal was success
		bool RemoveFast(const KeyType& key)
		{
			auto it = indexMap.find(key);
			if (it == indexMap.end())
			{
				StoryboardEngine::Logger::LogError("Key not found in HashedVector");
				return false;
			}

			size_t index = it->second;
			size_t lastIndex = elements.size() - 1;

			if (index != lastIndex)
			{
				std::swap(elements[index], elements[lastIndex]);

				// Update indexMap for the swapped element
				for (auto& [k, v] : indexMap)
				{
					if (v == lastIndex)
					{
						v = index;
						break;
					}
				}
			}

			elements.pop_back();
			indexMap.erase(it);
			return true;
		}

		// Adds an element with the given key & returns a reference to it. Returns existing element if key already exists
		ValueType& Add(const KeyType& key, const ValueType& element)
		{
			if (indexMap.contains(key))
			{
				StoryboardEngine::Logger::LogError("Key already exists in HashedVector");
				return elements[indexMap[key]];
			}

			elements.push_back(element);
			indexMap[key] = elements.size() - 1;

			return elements.back();
		}

		// Adds an element with the given key & returns a reference to it. Returns existing element if key already exists
		ValueType& Add(const KeyType& key, ValueType&& element)
		{
			if (indexMap.contains(key))
			{
				StoryboardEngine::Logger::LogError("Key already exists in HashedVector");
				return elements[indexMap[key]];
			}

			elements.push_back(std::move(element));
			indexMap[key] = elements.size() - 1;
			return elements.back();
		}

		// Adds or replaces an element with the given key & returns a reference to it
		ValueType& AddOrReplace(const KeyType& key, const ValueType& element)
		{
			if (indexMap.contains(key))
			{
				elements[indexMap[key]] = element;
				return elements[indexMap[key]];
			}

			elements.push_back(element);
			indexMap[key] = elements.size() - 1;
			return elements.back();
		}

		// Adds or replaces an element with the given key & returns a reference to it
		ValueType& AddOrReplace(const KeyType& key, ValueType&& element)
		{
			if (indexMap.contains(key))
			{
				elements[indexMap[key]] = std::move(element);
				return elements[indexMap[key]];
			}

			elements.push_back(std::move(element));
			indexMap[key] = elements.size() - 1;
			return elements.back();
		}

		// Swaps two elements by their indices
		void Swap(size_t index1, size_t index2)
		{
			if (index1 >= elements.size() || index2 >= elements.size())
			{
				StoryboardEngine::Logger::LogError("Index out of bounds in HashedVector Swap");
			}
			std::swap(elements[index1], elements[index2]);
			// Update indexMap
			int swaps = 0;
			for (auto& [k, v] : indexMap)
			{
				if (v == index1)
				{
					v = index2;
					++swaps;
				}
				else if (v == index2)
				{
					v = index1;
					++swaps;
				}
				if (swaps == 2) break;
			}
		}

		// Gets an element by key, returns true if successfully retrieved
		bool Get(const KeyType& key, ValueType& outElement) // ToDo: Check this keeps outElement linked to the element in the vector
		{
			auto it = indexMap.find(key);
			if (it == indexMap.end())
			{
				StoryboardEngine::Logger::LogError("Key not found in HashedVector");
				return false;
			}

			outElement = elements[it->second];
			return true;
		}

		/* /// Removed for now as failed accesses can't be checked for safety, also, causes confusion in the case of KeyType === size_t
		ValueType& operator[](const KeyType& key)
		{
			auto it = indexMap.find(key);
			if (it == indexMap.end())
			{
				StoryboardEngine::Logger::LogError("Key not found in HashedVector");
				return elements.back();
			}
			return elements[it->second];
		}
		*/

		// ToDo: Should be able to remove by index, and by iterator, foreach should give the iterator too

		const KeyType& GetKey(size_t index) const
		{
			if (index >= elements.size())
			{
				StoryboardEngine::Logger::LogError("Index out of bounds in HashedVector");
			}

			for (const auto& [k, v] : indexMap)
			{
				if (v == index)
				{
					return k;
				}
			}

			StoryboardEngine::Logger::LogError("Index not found in indexMap in HashedVector");
			static KeyType dummyKey{};
			return dummyKey;
		}

		ValueType& At(size_t index)
		{
			if (index >= elements.size())
			{
				StoryboardEngine::Logger::LogError("Index out of bounds in HashedVector");
			}
			return elements[index];
		}

		ValueType& operator[](size_t index)
		{
			return elements[index];
		}

		const ValueType& operator[](size_t index) const
		{
			return elements[index];
		}

		size_t Size() const
		{
			return elements.size();
		}

		std::vector<ValueType>::iterator begin()
		{
			return elements.begin();
		}

		std::vector<ValueType>::iterator end()
		{
			return elements.end();
		}

		std::vector<ValueType>::const_iterator begin() const
		{
			return elements.begin();
		}

		std::vector<ValueType>::const_iterator end() const
		{
			return elements.end();
		}

		std::vector<ValueType>::const_iterator cbegin() const
		{
			return elements.cbegin();
		}

		std::vector<ValueType>::const_iterator cend() const
		{
			return elements.cend();
		}

		std::vector<ValueType>::iterator find(const KeyType& key)
		{
			auto it = indexMap.find(key);
			if (it == indexMap.end())
			{
				return elements.end();
			}
			return elements.begin() + it->second;
		}

		std::vector<ValueType>::const_iterator find(const KeyType& key) const
		{
			auto it = indexMap.find(key);
			if (it == indexMap.end())
			{
				return elements.cend();
			}
			return elements.cbegin() + it->second;
		}

		const std::vector<ValueType>& GetAllElements() const
		{
			return elements;
		}

		void Clear()
		{
			elements.clear();
			indexMap.clear();
		}

		friend void to_json(nlohmann::ordered_json& nlohmann_json_j, const HashedVector<KeyType, ValueType>& nlohmann_json_t)
		{
			//nlohmann_json_j.clear();

			std::vector<std::pair<KeyType, ValueType>> items;

			items.reserve(nlohmann_json_t.elements.size());

			// Serialize maintaining order

			for (const auto& element : nlohmann_json_t.elements)
			{
				// Find the corresponding key
				for (const auto& [key, index] : nlohmann_json_t.indexMap)
				{
					if (nlohmann_json_t.elements[index] == element)
					{
						items.emplace_back(key, element);
						//nlohmann_json_j[key] = element;
						break;
					}
				}
			}

			nlohmann_json_j = items;
		}

		friend void from_json(const nlohmann::ordered_json& nlohmann_json_j, HashedVector<KeyType, ValueType>& nlohmann_json_t)
		{
			nlohmann_json_t.Clear();

			// Prevent reallocations which would move/destroy existing elements
			nlohmann_json_t.elements.reserve(nlohmann_json_j.size());

			for (int i = 0; i < nlohmann_json_j.size(); ++i)
			{
				auto& item = nlohmann_json_j.at(i);

				// Default-construct in-place then deserialize into that object (no temporary)
				nlohmann_json_t.elements.emplace_back();
				item.at(1).get_to(nlohmann_json_t.elements.back());

				nlohmann_json_t.indexMap[item.at(0).get<KeyType>()] = i;
			}
		}
	};
}

