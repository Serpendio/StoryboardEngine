#include "pch.h"
#include "Internal/UUID.h"
#define UUID_SYSTEM_GENERATOR
#include <stduuid/uuid.h>

static StoryboardEngine::UUID toStoryboardEngineUUID(const uuids::uuid& in_uuid)
{
	std::array<uint8_t, 16> out_uuid{};
	auto in_bytes = in_uuid.as_bytes();
	std::memcpy(out_uuid.data(), in_bytes.data(), out_uuid.size());
	return out_uuid;
}

static uuids::uuid toUUIDLibraryUUID(const StoryboardEngine::UUID& in_uuid)
{
	std::array<uint8_t, 16> in_bytes = in_uuid.GetBytes();
	return uuids::uuid(in_bytes);
}

constexpr StoryboardEngine::UUID::UUID(uint8_t(&arr)[16]) noexcept
{
	std::copy(std::cbegin(arr), std::cend(arr), std::begin(data));
#ifdef _EDITOR
	dataAsString = ToString();
#endif
}

StoryboardEngine::UUID StoryboardEngine::UUID::Generate()
{
	return toStoryboardEngineUUID(uuids::uuid_system_generator{}());
}

const std::array<uint8_t, 16>& StoryboardEngine::UUID::GetBytes() const noexcept
{
	return data;
}

bool StoryboardEngine::UUID::is_nil() const noexcept
{
	for (size_t i = 0; i < data.size(); ++i) if (data[i] != 0) return false;
	return true;
}

bool StoryboardEngine::UUID::is_valid_uuid(const std::string& in_str) noexcept
{
	return uuids::uuid::is_valid_uuid(in_str);
}

StoryboardEngine::UUID StoryboardEngine::UUID::from_string(const std::string& in_str) noexcept
{
	auto opt_uuid = uuids::uuid::from_string(in_str);
	if (opt_uuid.has_value())
	{
		return toStoryboardEngineUUID(opt_uuid.value());
	}
	return UUID{};
}

std::string StoryboardEngine::UUID::ToString() const
{
	auto uuid_lib_uuid = toUUIDLibraryUUID(*this);
	return uuids::to_string(uuid_lib_uuid);
}
