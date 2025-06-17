#pragma once
#include "Core/Core.h"

#include <vector>
#include <map>

namespace Enigma {
	namespace Core {

		struct ID {
			uint32_t index;
			uint32_t generation;

			bool operator < (const ID& other) {
				return index < other.index;
			}
			bool operator < (const ID other) const {
				return index < other.index;
			}
		};
		
		template<class T>
		class IDHandler {
		public:
			IDHandler() = default;

			std::vector<T>& GetData() { return m_Data; }

			bool IsValid(ID& id) {
				if (!m_IDs.count(id)) return false;
				if (m_IDs[id].second != id.generation) return false;
				return (m_IDs[id].first != -1);
			}
			ID Create(T value) {
				if (!m_Slots.empty()) {
					ID slot = m_Slots[0];
					m_Slots.erase(m_Slots.begin());
					m_IDs[slot].first = m_Data.size();
					m_IDs[slot].second += 1;
					m_Data.push_back(value);
					slot.generation += 1;
					return slot;
				}

				ID id = { m_IDs.size(), 0 };
				int dataId = m_Data.size();
				m_Data.push_back(value);
				m_IDs.insert({ id, { dataId, 0 } });
				return id;
			}
			void Delete(ID& id) {
				if (!IsValid(id)) return;
				m_Data.erase(m_Data.begin() + m_IDs[id].first);
				m_IDs[id].first = -1;
				m_Slots.push_back(id);
			}

			T Get(ID& id) {
				if (!IsValid(id)) {
					LOG_ERROR("Invalid ID ( " + std::to_string(id.index) + ", " + std::to_string(id.generation) + " )");
				}
				return m_Data[m_IDs[id].first];
			}

		private:
			std::vector<T> m_Data;
			std::map<ID, std::pair<int, uint32_t>> m_IDs;
			std::vector<ID> m_Slots;
		};

	}
}