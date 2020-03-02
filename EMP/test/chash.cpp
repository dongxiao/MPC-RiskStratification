#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-clinic/emp-clinic.h"

using namespace emp;
using namespace std;

void test_hash(int n) {

	int m = 1.5 * n;
	vector<uint64_t> hash_key_a(m, 0);
	vector<uint64_t> hash_value_a(m, 0);
	vector<uint64_t> empt;
	vector<vector<uint64_t>> hash_key_b(m, empt);
	vector<vector<uint64_t>> hash_value_b(m, empt);

	vector<uint64_t> key;
	vector<uint64_t> value;
	srand(time(NULL));
	for(int i = 0; i < n; ++i) {
		key.push_back((uint64_t)(rand()%1000000000));
		value.push_back((uint64_t)rand());
	}

	// ALICE
	CHash<uint64_t> ch_a(n);
	ch_a.chash(hash_key_a, hash_value_a, key, value);

	// BOB
	CHash<uint64_t> ch_b(n);
	ch_b.rhash(hash_key_b, hash_value_b, key, value);


	for(int i = 0; i < m; ++i) {
		uint64_t a0 =  hash_key_a[i];
	       	uint64_t a1 =  hash_value_a[i];
		bool findone = false;
		//std::cout << a0 << " " << a1 << std::endl;
		for(int j = 0; j < hash_key_b[i].size(); ++j) {
			//std::cout << hash_key_b[i][j] << " " << hash_value_b[i][j] << "\t";
			if(hash_key_b[i][j] == a0 && hash_value_b[i][j] == a1) {
				findone = true;
				break;
			}
		}
		//std::cout << std::endl;
		if(a0 != 0 && findone == false) {
			std::cout << "fail at " << i <<  std::endl;
			return;
		}
	}
	std::cout << "success" << std::endl;
}

int main(void) {
	int n = 500000;
	test_hash(n);

	return 0;
}
