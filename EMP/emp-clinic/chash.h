#ifndef CHASH_H__
#define CHASH_H__

#include "cuckoo.h"
#include "hash_table.h"

template <typename V>
class CHash { public:
	int n;
	float sz_ratio;
	int sz;
	CHash(int n) {
		this->n = n;
		this->sz_ratio = 1.5;
		this->sz = n * sz_ratio;
	}
	
	int tau() const {
		return 3;
	}
	int size() {
		return sz;
	}
	
	//key and value arrays are of size "size()"
	void chash(vector<uint64_t> & hash_key, vector<V> & hash_value, vector<uint64_t> & key, vector<V> & value) {
		Cuckoo<V> cuckoo(n, sz_ratio);
		cuckoo.insert(hash_key, hash_value, key, value);
	}

	void rhash(vector<vector<uint64_t>> & hash_key, vector<vector<V>> & hash_value, vector<uint64_t> & key, vector<V> & value) {
		Hash_table<V> hash_tab(n, sz_ratio);
		hash_tab.matrix_fill(hash_key, hash_value, key, value);
	}
};
#endif// HASH_H__
