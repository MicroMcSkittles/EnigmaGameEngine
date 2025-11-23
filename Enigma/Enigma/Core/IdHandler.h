#pragma once
#include <vector>
#include <algorithm>
#include <limits>

#include "Enigma/Core/Core.h"
#include "Enigma/Core/Types.h"
#include "Enigma/Core/SparseSet.h"

// TODO: update docs

namespace Enigma::Core {

	struct ID {
		u64 index = std::numeric_limits<u64>::max();
		u64 generation = std::numeric_limits<u64>::max();

		bool operator <  (const ID other) const { return index < other.index; }
		bool operator == (const ID other) const { return (index == other.index) && (generation == other.generation); }
		bool operator != (const ID other) const { return !(*this == other); }
		
		std::string ToString() { 
			return std::string("( " + std::to_string(index) + ", " + std::to_string(generation) + " )"); 
		}
	};
	constexpr ID InvalidID = { std::numeric_limits<u64>::max(), std::numeric_limits<u64>::max() };

	template<class T>
	class IDHandler {
	public:
		IDHandler() { }
		~IDHandler() { Clear(); }

		std::vector<T>& GetData() { 
			return m_Data.GetData();
		};

		bool Contains(ID id) {
			if (id.index == InvalidID.index) return false;
			if (id.index >= m_LargestID) return false;
			if (m_Generations.Get(id.index) != id.generation) return false;
			return m_Data.Contains(id.index);
		}
		ID Create(T value) {
			// Find an available id
			u64 availableID;
			if (!m_AvailableIDs.empty()) {
				availableID = *std::min_element(m_AvailableIDs.begin(), m_AvailableIDs.end());
			}
			else {
				availableID = m_LargestID;
				m_LargestID += 1;
				// Create a new generation entry
				m_Generations.Create(availableID, std::numeric_limits<u64>::max());
			}

			m_Generations.Get(availableID) += 1;
			m_Data.Create(availableID, value);

			ID id;
			id.index = availableID;
			id.generation = m_Generations.Get(availableID);

			return id;
		}
		void Delete(ID id) {
			LOG_ASSERT(!Contains(id), "Failed to delete ID %s", id.ToString().c_str());

			m_Data.Remove(id.index);
			m_AvailableIDs.push_back(id.index);
		}

		// Returns the value at id
		T& Get(ID id) {
			LOG_ASSERT(!Contains(id), "Failed to get ID %s", id.ToString().c_str());
			return m_Data.Get(id.index);
		}

		// Returns the id of an element
		ID Get(const T& value) {
			for (u64 index : m_Data.GetIDs()) {
				if (m_Data.Get(index) == value) {
					ID id;
					id.index = index;
					id.generation = m_Generations.Get(index);
					return id;
				}
			}
			return InvalidID;
		}

		// Returns the id of a element at index
		ID Get(u64 index) {
			LOG_ASSERT(index >= m_Generations.GetIDs().size(), "Failed to get id at index ( %ull )", static_cast<u64>(index));
			
			ID id;
			id.index = m_Generations.GetIDs()[index];
			id.generation = m_Generations.Get(id.index);

			return id;
		}

		void Clear() {
			m_AvailableIDs.clear();
			m_Data.Clear();
			m_Generations.Clear();
		}

	private:
		u64 m_LargestID;
		std::vector<u64> m_AvailableIDs;

		SparseSet<T> m_Data;
		SparseSet<u64> m_Generations;
	};

}