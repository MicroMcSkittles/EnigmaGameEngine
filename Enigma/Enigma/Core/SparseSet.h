#pragma once
#include "Enigma/Core/Core.h"
#include <limits>
#include <vector>
#include <array>
#include <algorithm>

namespace Enigma::Core {

	// My implementation of the sparse set concept.
	// Used to keep all of the data compact to take advantage of cpu caching
	// Create and Remove instructions have a constant speed O(1)
	template<typename T, size_t PageSize = 64>
	class SparseSet {
	public:

		SparseSet() { }
		~SparseSet() { Clear(); }

		void Create(size_t id) {
			SetDenseIndex(id, m_Dense.size());
			m_IDMap.push_back(id);
			m_Dense.push_back(T());
		}
		void Create(size_t id, T value) {
			SetDenseIndex(id, m_Dense.size());
			m_IDMap.push_back(id);
			m_Dense.push_back(value);
		}

		void Remove(size_t id) {
			size_t index = GetDenseIndex(id);
			LOG_ASSERT(index == InvalidIndex, "Failed to remove value from sparse set, invalid id ( %d )", (int)index);

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

		bool Contains(size_t id) {
			if (m_SparsePages.size() * PageSize <= id) return false;
			if (GetDenseIndex(id) == InvalidIndex) return false;
			return true;
		}

		T& Get(size_t id) {
			LOG_ASSERT(m_SparsePages.size() * PageSize <= id, "Failed to get value from sparse set, id out of sparse list bounds ( %d )", (int)id);

			size_t index = GetDenseIndex(id);
			LOG_ASSERT(index == InvalidIndex, "Failed to get value from sparse set, invalid id ( %d )", (int)id);

			return m_Dense[index];
		}

		void Clear() {
			m_Dense.clear();
			m_IDMap.clear();
			m_SparsePages.clear();
		}

		bool Empty() { return m_Dense.empty(); }
		size_t Size() { return m_Dense.size(); }
		std::vector<size_t>& GetIDs() { return m_IDMap; }
		std::vector<T>& GetData() { return m_Dense; }

	private:
		inline void SetDenseIndex(size_t id, size_t index) {
			// Find what page id is on and where on that page its on
			size_t page = id / PageSize;
			size_t sparseIndex = id % PageSize;

			// if the page doesn't exist than create it
			if (page >= m_SparsePages.size()) {
				m_SparsePages.resize(page + 1);
				m_SparsePages[page].fill(InvalidIndex);
			}

			m_SparsePages[page][sparseIndex] = index;
		}
		inline size_t GetDenseIndex(size_t id) {
			// Find what page id is on and where on that page its on
			size_t page = id / PageSize;
			size_t sparseIndex = id % PageSize;

			LOG_ASSERT(page >= m_SparsePages.size(), "Failed to get sparse set dense index");

			return m_SparsePages[page][sparseIndex];
		}

	private:
		static constexpr size_t InvalidIndex = std::numeric_limits<size_t>::max();

		std::vector<T> m_Dense;
		std::vector<size_t> m_IDMap;
		std::vector<std::array<size_t, PageSize>> m_SparsePages;
	};

}