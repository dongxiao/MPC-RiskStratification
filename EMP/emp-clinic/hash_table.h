#ifndef HASH_TABLE_H__
#define HASH_TABLE_H__
	
#include <emp-tool/emp-tool.h>
#include <map>
#include "cuckoo.h"

using namespace emp;

template <typename V>
class Hash_table {
	public:
	int item_n, m, hash_n;

	int *hash_row_pt;	// how many items are in one row
	block *crh_val = nullptr;
	CRH crh;
	Hash_table(int item_n, float sz_ratio) {
		this->item_n = item_n;
		this->m = sz_ratio*item_n;
		this->hash_n = 3;
		hash_row_pt = new int[m]{0};
	}

	~Hash_table() {
		delete[] hash_row_pt;
		if(crh_val != nullptr) 
			delete[] crh_val;
	}
	
	void matrix_fill(std::vector<std::vector<uint64_t>> &hash_key, std::vector<std::vector<V>> &hash_value,
		       	std::vector<uint64_t> &key, std::vector<V> &value) {
		crh_val = new block[item_n];
		hash_gen(key);
		fill_in(hash_key, hash_value, key, value);
	}

	void fill_in(std::vector<std::vector<uint64_t>> &hash_key, std::vector<std::vector<V>> &hash_value,
			std::vector<uint64_t> &key, std::vector<V> &value) {
		for(int i = 0; i < item_n; ++i) {
			uint32_t *int32_pt = (uint32_t*)(&crh_val[i]);
			for(int j = 0; j < hash_n; ++j) {
				int idx = int32_pt[j] % m;
				hash_key[idx].push_back(key[i]);
				hash_value[idx].push_back(value[i]);
				hash_row_pt[idx]++;
			}
		}
	}

	void hash_gen(std::vector<uint64_t> &key) {
		block *to_hash = new block[item_n];
		for(int i = 0; i < item_n; ++i)
			to_hash[i] = _mm_set_epi64x((uint64_t)0, key[i]);
		crh.Hn(crh_val, to_hash, item_n);
		delete[] to_hash;
	}

};
#endif
