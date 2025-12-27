#pragma once
#include <array>
#include <string>
#include "Utils/Serialization.h"

// Lightweight header frontend for UUID functionality
namespace StoryboardEngine
{
	class UUID
	{
	public:
		UUID() noexcept = default;
		UUID(const UUID& other) noexcept = default;
		UUID& operator=(const UUID& other) noexcept = default;
		constexpr UUID(const std::array<uint8_t, 16>& data) noexcept : data(data) 
        {
#ifdef _EDITOR
            dataAsString = ToString();
#endif
        }
		constexpr UUID(uint8_t(&arr)[16]) noexcept;

		static UUID Generate();
		const std::array<uint8_t, 16>& GetBytes() const noexcept;
        bool is_nil() const noexcept;
        static bool is_valid_uuid(const std::string& in_str) noexcept;
        static UUID from_string(const std::string& in_str) noexcept;
		std::string ToString() const;

		/*bool operator==(const UUID& other) const
		{
			return data == other.data;
		}
		bool operator!=(const UUID& other) const
		{
			return !(*this == other);
		}
		bool operator<(const UUID& other) const
		{
			return data < other.data;
		}*/

		auto operator<=>(const UUID& other) const = default;

		//ADD_SERIALIZATION_NO_PARENT(UUID, JSON_VARIABLE(data))
        template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> 
        friend void to_json(BasicJsonType& nlohmann_json_j, const UUID& nlohmann_json_t)
        {
            nlohmann_json_j = nlohmann_json_t.ToString();
        }

        template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> 
        friend void from_json(const BasicJsonType& nlohmann_json_j, UUID& nlohmann_json_t)
        {
            nlohmann_json_t = UUID::from_string(nlohmann_json_j.get<std::string>());
        }
	private:
		friend std::hash<UUID>;

		std::array<uint8_t, 16> data{ { 0 } };
#ifdef _EDITOR
        std::string dataAsString;
#endif
	};
}

namespace std
{
    template <>
    struct hash<StoryboardEngine::UUID>
    {
        using argument_type = StoryboardEngine::UUID;
        using result_type = std::size_t;

        [[nodiscard]] result_type operator()(argument_type const& uuid) const
        {
            uint64_t l =
                static_cast<uint64_t>(uuid.data[0]) << 56 |
                static_cast<uint64_t>(uuid.data[1]) << 48 |
                static_cast<uint64_t>(uuid.data[2]) << 40 |
                static_cast<uint64_t>(uuid.data[3]) << 32 |
                static_cast<uint64_t>(uuid.data[4]) << 24 |
                static_cast<uint64_t>(uuid.data[5]) << 16 |
                static_cast<uint64_t>(uuid.data[6]) << 8 |
                static_cast<uint64_t>(uuid.data[7]);
            uint64_t h =
                static_cast<uint64_t>(uuid.data[8]) << 56 |
                static_cast<uint64_t>(uuid.data[9]) << 48 |
                static_cast<uint64_t>(uuid.data[10]) << 40 |
                static_cast<uint64_t>(uuid.data[11]) << 32 |
                static_cast<uint64_t>(uuid.data[12]) << 24 |
                static_cast<uint64_t>(uuid.data[13]) << 16 |
                static_cast<uint64_t>(uuid.data[14]) << 8 |
                static_cast<uint64_t>(uuid.data[15]);

            if constexpr (sizeof(result_type) > 4)
            {
                return result_type(l ^ h);
            }
            else
            {
                uint64_t hash64 = l ^ h;
                return result_type(uint32_t(hash64 >> 32) ^ uint32_t(hash64));
            }
        }
    };
}