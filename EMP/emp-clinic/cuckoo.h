#ifndef CUCKOO_H__
#define CUCKOO_H__

#include <emp-tool/emp-tool.h>
#include <queue>
#include <set>
#include <map>
#include <vector>

using namespace emp;

template <typename V>
class Cuckoo { public:
	int item_n;	// t
	int hash_n, m;

	bool *cuckoo_flag;

	CRH crh;
	block hash_seed;

	block *crh_val = nullptr;
	std::map<uint64_t, int> idx_pos_map;

	Cuckoo(int item_n, float sz_ratio) {
		this->item_n = item_n;

		m = sz_ratio * item_n;
		hash_n = 3;
		cuckoo_flag = new bool[m];
	}

	~Cuckoo() {
		delete[] cuckoo_flag;
		if(crh_val != nullptr)
			delete[] crh_val;
	}

	bool insert(std::vector<uint64_t> &hash_key, std::vector<V> &hash_value, 
			std::vector<uint64_t > &key, std::vector<V> &value) {
		crh_val = new block[item_n];
		hash_gen(key);
		memset(cuckoo_flag, false, m);
		std::queue<uint64_t> s;
		uint64_t idx_table[m];
		for(int i = 0; i < item_n; ++i)
			s.push(i);
		while(s.size() > 0) {
			int i = rand() % hash_n;
			int idx_out = s.front();
			uint64_t item = key[idx_out];
			s.pop();
			uint32_t *u32_pt = (uint32_t*)(&crh_val[idx_out]);
			uint32_t j = u32_pt[i] % m;
			if (!cuckoo_flag[j])	{
				hash_key[j] = item;
				hash_value[j] = value[idx_out];
				idx_table[j] = idx_out;
				cuckoo_flag[j] = true;
			} else {
				int evic_idx = idx_table[j];
				s.push(evic_idx);
				hash_key[j] = item;
				hash_value[j] = value[idx_out];
				idx_table[j] = idx_out;
			}
		} 
		
		return true;
	}

	block to_hash;
	block hash_res;
	uint32_t hash(int i, uint64_t item) {
		uint32_t *idx = (uint32_t*)(&hash_res);
		to_hash = _mm_set_epi64x((uint64_t)0, item);
		hash_res = crh.H(to_hash);
		return idx[i] % m;
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
