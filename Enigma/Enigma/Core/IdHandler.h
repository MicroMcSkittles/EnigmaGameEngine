#pragma once
#include "Core/Core.h"

#include <vector>
#include <map>

namespace Enigma {
	namespace Core {

		struct ID {
			uint32_t index;
			uint32_t generation;

			// Creates an invalid id
			static ID InvalidID() {
				return {
					(uint32_t)-1,
					(uint32_t)-1
				};
			}

			bool operator < (const ID other) const {
				return index < other.index;
			}
			operator std::string() const {
				return "( " + std::to_string(index) + ", " + std::to_string(generation) + " )";
			}
		};
		
		template<class T>
		class IDHandler {
		public:
			IDHandler() = default;

			std::vector<T*>& GetData() { return m_Data; }

			bool IsValid(ID id) {
				// Make sure id is registered in m_IDs
				if (!m_IDs.count(id)) return false;
				// Make sure id is in the same generation as the registered id
				if (m_IDs[id].second != id.generation) return false;
				// Make sure id has a valid index into m_Data
				return (m_IDs[id].first != -1);
			}
			ID Create(T* value) {

				// see if there are any open slots, then use the first open slot
				if (!m_Slots.empty()) {
					// Get first open slot
					ID slot = m_Slots[0];
					slot.generation += 1;

					// Remove slot from m_Slots
					m_Slots.erase(m_Slots.begin());

					m_IDs[slot].first = (uint32_t)m_Data.size();
					m_IDs[slot].second += 1;
					m_Data.push_back(value);
					return slot;
				}

				// Create new id then return
				ID id = { (uint32_t)m_IDs.size(), 0 };
				int dataId = m_Data.size();
				m_Data.push_back(value);
				m_IDs.insert({ id, { dataId, 0 } });
				return id;
			}
			void Delete(ID id) {
				// Make sure id is valid
				if (!IsValid(id)) return;

				// find where the id points to in m_Data then remove that index from m_Data
				int index = m_IDs[id].first;
				m_Data.erase(m_Data.begin() + index);
				m_IDs[id].first = -1;
				
				// Find every id that points to some where in m_Data after the removed 
				// index then update their pointer
				for (auto& [id, dataEntry] : m_IDs) {
					if (dataEntry.first >= index) {
						dataEntry.first -= 1;
					}
				}

				// Push id to m_Slots so id can be reused
				m_Slots.push_back(id);
			}

			T* Get(ID id) {
				// Make sure id is valid
				if (!IsValid(id)) {
					LOG_ERROR("Invalid ID ( " + std::to_string(id.index) + ", " + std::to_string(id.generation) + " )");
				}

				// Get id
				return m_Data[m_IDs[id].first];
			}

			void Clear() {

				// Delete every pointer
				for (auto d : m_Data) {
					delete d;
				}
				
				// Clear all lists
				m_Data.clear();
				m_IDs.clear();
				m_Slots.clear();
			}

		private:
			std::vector<T*> m_Data;
			std::map<ID, std::pair<int, uint32_t>> m_IDs;
			std::vector<ID> m_Slots;
		};

	}
}