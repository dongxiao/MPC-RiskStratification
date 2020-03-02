#include <iostream>
#include <vector>
#include <fstream>
#include <emp-sh2pc/emp-sh2pc.h>
#include "emp-clinic/emp-clinic.h"
using std::cout;
using std::endl;

void parse_file(const char * filename, vector<std::pair<uint64_t, uint32_t>>& data) {
	std::ifstream fin;
	fin.open(filename);
	char tmp;
	uint64_t length;
	fin >> length;
	data.clear();data.resize(length);

	for (int i = 0; i < length; ++i) {
		uint64_t pid;
		float floatt;
		uint32_t ll;
		fin >> pid>>tmp;
		fin >> floatt>>tmp>>ll;
		uint32_t flag = floatt*10;
		flag <<=5;
		flag = flag | ll;
		data[i] = std::pair<uint64_t,uint32_t>(pid%(1ULL<<62), flag);
	}
}
//judge if the patient with data a & b should be output.
Bit judge(Integer a, Integer b, int party = BOB) {
	Integer A1 = a>>5;
	Integer L1 = a & Integer(17, (1<<5)-1, PUBLIC);
	Integer A2 = b>>5;
	Integer L2 = b & Integer(17, (1<<5)-1, PUBLIC);
	Integer Left = A1*((L2<<1)-L1);
	Integer Right = (A2*L2)*Integer(17, 3, PUBLIC);
	return Left > Right;
}

void compute_ci_pre(int alice_size, int bob_size, int party, vector<std::pair<uint64_t,uint32_t>>& data, NetIO * io) {
	Integer * PID = new Integer[alice_size + bob_size];
	Integer * Flag = new Integer[alice_size + bob_size];
	Batcher batcher;
	if(party == ALICE) {
		for(int i = 0; i < alice_size; ++i) {
			PID[i] = Integer(64, data[i].first, ALICE);
			Flag[i] = Integer(17, data[i].second, ALICE);
		}
		for(int i = 0; i < bob_size; ++i) {
			batcher.add<Integer>(64, 0);
			batcher.add<Integer>(17, 0);
		}
	
	} else {
		for(int i = 0; i < alice_size; ++i) {
			PID[i] = Integer(64, 0, ALICE);
			Flag[i] = Integer(17, 0, ALICE);
		}
		for(int i = 0; i < bob_size; ++i) {
			batcher.add<Integer>(64, data[i].first);
			batcher.add<Integer>(17, data[i].second);
		}
	}
	batcher.make_semi_honest(BOB);
	for(int i = 0; i < bob_size; ++i) {
		PID[alice_size + bob_size - i -1] = batcher.next<Integer>();
		Flag[alice_size + bob_size - i -1] =  batcher.next<Integer>();
	}
	bitonic_merge<Integer, Integer>(PID, Flag, 0, alice_size + bob_size, Bit(false, PUBLIC));
	for(int i = 0; i < alice_size + bob_size; ++i)
		PID[i] = PID[i] >> 1;
	Integer * Dum = new Integer[alice_size + bob_size];
	for(int i = 0; i < alice_size + bob_size - 1; ++i) {
		uint64_t pid = PID[i].reveal<uint64_t>();
		Bit eq = (PID[i] == PID[i+1]);
		Bit match = judge(Flag[i], Flag[i+1], party);
		Bit hit = eq & match;
		Dum[i] = Integer(2, 0, PUBLIC);
		Dum[i][0] = hit;
	}
	sort(Dum, alice_size + bob_size-1, PID, Bit(false, PUBLIC));
	int size = 0;
	for(int i = 0; i < alice_size + bob_size-1; ++i) {
		bool bit = Dum[i][0].reveal(BOB);
		size += bit;
	}
	if (party == BOB) {
		io->send_data(&size, 4);
	} else {
		io->recv_data(&size, 4);
	}
	for(int i = 0; i < size; ++i) {
		uint64_t pid = PID[i].reveal<uint64_t>(BOB);
		if(party == BOB)
			cout << pid<<endl;
	}
}
