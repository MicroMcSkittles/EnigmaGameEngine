#pragma once
#include <limits>
#include <vector>
#include <array>
#include <algorithm>

#include "Enigma/Core/Types.h"
#include "Enigma/Core/Core.h"

namespace Enigma::Core {

	// My implementation of the sparse set concept.
	// Used to keep all of the data compact to take advantage of cpu caching
	// Create and Remove instructions have a constant speed O(1)
	template<typename T, u64 PageSize = 64>
	class SparseSet {
	public:

		SparseSet() { }
		SparseSet(const SparseSet& other) { 
			m_Dense = std::vector<T>(other.m_Dense.begin(), other.m_Dense.end());
			m_IDMap = std::vector<u64>(other.m_IDMap.begin(), other.m_IDMap.end());
			m_SparsePages = std::vector<std::array<u64, PageSize>>(other.m_SparsePages.begin(), other.m_SparsePages.end());
		}
		~SparseSet() { Clear(); }

		void Create(u64 id) {
			SetDenseIndex(id, m_Dense.size());
			m_IDMap.push_back(id);
			m_Dense.push_back(T());
		}
		void Create(u64 id, T value) {
			SetDenseIndex(id, m_Dense.size());
			m_IDMap.push_back(id);
			m_Dense.push_back(value);
		}

		void Remove(u64 id) {
			u64 index = GetDenseIndex(id);
			LOG_ASSERT(index == InvalidIndex, "Failed to remove value from sparse set, invalid id ( %d )", static_cast<i64>(index));

			// Update indices
			SetDenseIndex(m_IDMap.back(), index);
			SetDenseIndex(id, InvalidIndex);

			// Swap the index to remove with the last element
			std::swap(m_Dense[index], m_Dense.back());
			std::swap(m_IDMap[index], m_IDMap.back());

			// Remove the component
			m_Dense.pop_back();
			m_IDMap.pop_back();
		}

		bool Contains(u64 id) {
			if (m_SparsePages.size() * PageSize <= id) return false;
			if (GetDenseIndex(id) == InvalidIndex) return false;
			return true;
		}

		T& Get(u64 id) {
			LOG_ASSERT(m_SparsePages.size() * PageSize <= id, "Failed to get value from sparse set, id out of sparse list bounds ( %d )", static_cast<int>(id));

			u64 index = GetDenseIndex(id);
			LOG_ASSERT(index == InvalidIndex, "Failed to get value from sparse set, invalid id ( %d )", static_cast<int>(id));

			return m_Dense[index];
		}

		void Clear() {
			m_Dense.clear();
			m_IDMap.clear();
			m_SparsePages.clear();
		}

		// Swaps the memory in the dense list at the given indices
		void Swap(u64 first, u64 second) {
			// Get Dense indices
			u64 firstDenseIndex = GetDenseIndex(first);
			u64 secondDenseIndex = GetDenseIndex(second);

			// Swap Dense indices
			SetDenseIndex(first, secondDenseIndex);
			SetDenseIndex(second, firstDenseIndex);

			// Swap Dense data
			std::swap(m_Dense[firstDenseIndex], m_Dense[secondDenseIndex]);
		}

		bool Empty() { return m_Dense.empty(); }
		u64 Size() { return m_Dense.size(); }
		std::vector<u64>& GetIDs() { return m_IDMap; }
		std::vector<T>& GetData() { return m_Dense; }

		inline u64 GetDenseIndex(u64 id) {
			// Find what page id is on and where on that page its on
			u64 page = id / PageSize;
			u64 sparseIndex = id % PageSize;

			LOG_ASSERT(page >= m_SparsePages.size(), "Failed to get sparse set dense index");

			return m_SparsePages[page][sparseIndex];
		}
	private:
		inline void SetDenseIndex(u64 id, u64 index) {
			// Find what page id is on and where on that page its on
			u64 page = id / PageSize;
			u64 sparseIndex = id % PageSize;

			// if the page doesn't exist than create it
			if (page >= m_SparsePages.size()) {
				m_SparsePages.resize(page + 1);
				m_SparsePages[page].fill(InvalidIndex);
			}

			m_SparsePages[page][sparseIndex] = index;
		}

	private:
		static constexpr u64 InvalidIndex = std::numeric_limits<u64>::max();

		std::vector<T> m_Dense;
		std::vector<u64> m_IDMap;
		std::vector<std::array<u64, PageSize>> m_SparsePages;
	};

}