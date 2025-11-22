#pragma once
#include "Enigma/Core/Types.h"

namespace Enigma::Engine {

	// Universally Unique ID, basically a uint64 wrapper
	class UUID {
	public:
		UUID();
		UUID(u64 uuid);
		UUID(const UUID& other) = default;

		operator u64() const { return m_UUID; }

	private:
		u64 m_UUID;
	};

}

namespace std {

	// UUID hash function
	template<>
	struct hash<Enigma::Engine::UUID> {
		std::size_t operator()(const Enigma::Engine::UUID& uuid) const {
			return hash<u64>()(static_cast<u64>(uuid));
		}
	};

}