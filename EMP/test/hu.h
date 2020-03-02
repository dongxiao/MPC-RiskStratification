#include <iostream>
#include <vector>
#include <fstream>
#include <emp-sh2pc/emp-sh2pc.h>
#include "emp-clinic/emp-clinic.h"
using std::cout;
using std::endl;

void parse_file(const char * filename, vector<uint64_t>& pid, vector<uint64_t>& flag){
	std::ifstream fin;
	fin.open(filename);
	char tmp;
	uint64_t tmp2,length;
	fin >> length;
	pid.clear();pid.resize(length);
	flag.clear();flag.resize(length);

	for (int i = 0; i < length; ++i) {
		fin >> pid[i];
		flag[i] = 0;
		for(int j = 0; j < 36; ++j) {
			flag[i] <<=1;
			fin>>tmp>>tmp2;
			if (tmp2 == 1)
				flag[i] = flag[i] | 0x1;
		}
	}
}
/*
//judge if the patient with data a & b should be output.
Bit judge(Integer &a, Integer & b) {
	if(a.size()!=b.size())error("size mismatch");

	Integer u = a | b;
	int coeff[] = {6,6,3,2,2,2,2,1,1,1,1,1,1,1,1,1,1};
	Integer res(7, 0, PUBLIC);
	for(int i = 0; i < 17; ++i) {
		Integer updatedres = res + Integer(7, coeff[i], PUBLIC);
		res = res.If(u[i], updatedres);
	}
	return res >= Integer(7, 9, PUBLIC);
}*/

//judge if the patient with data a & b should be output.

Bit judge(Integer &a, Integer & b) {
	if(a.size()!=b.size())error("size mismatch");

	Integer u = a | b;
	Integer v = a & b;

	Bit anyhit(PUBLIC, false);

	Integer S[36];
	Integer res(7, 0, PUBLIC);

	res = res.If(u[0], Integer(7, 1, PUBLIC));
	Integer v_update = res + Integer(7, 1, PUBLIC);
	S[0] = res.If(v[0], v_update);

	for (int i = 1; i < 36; ++i) {
		v_update = S[i-1] + Integer(7, 1, PUBLIC);
		res = res.If(u[i], v_update);
		v_update = res + Integer(7, 1, PUBLIC);
		res = res.If(v[i], v_update);		
		S[i] = res;
	}
	
	for (int i = 0; i < 24; ++i) {
		Integer hu_count = S[i+12] - S[i];
		anyhit = (hu_count >= Integer(7, 1, PUBLIC)) | anyhit;
	}
	return anyhit;
}


//judge if the patient with data a & b should be output.
/*
Bit judge(Integer &a, Integer & b) {
	if(a.size()!=b.size())error("size mismatch");
	Integer u = a | b;
	Integer v = a & b;
	//uint32_t flag = u.reveal<uint32_t>(BOB);
	//cout <<flag<<" ";	

	Bit anyhit(PUBLIC, false);
	
	for (int i = 0; i < 21; ++i) {
		Integer res(7, 0, PUBLIC);
		for(int j = 0; j < 12; ++j) {
			Integer updatedres = res + Integer(7, 1, PUBLIC);
			res = res.If(u[i+j], updatedres);
			updatedres = res + Integer(7, 1, PUBLIC);
			res = res.If(v[i+j], updatedres);
		}
		int RES = res.reveal<int>(BOB);

		anyhit = (res >= Integer(7, 5, PUBLIC)) | anyhit;
	}
	return anyhit;
}
*/

void compute_hu(vector<vector<Integer>>& AlicePID, vector<vector<Integer>>& AliceFlag,
					vector<Integer>& BobPID, vector<Integer>& BobFlag, int party) {
	if(AlicePID.size()!= AliceFlag.size())error("size 1!");
	if(AlicePID.size()!= BobPID.size())error("size 2!");
	if(BobPID.size()!= BobFlag.size())error("size 3!");

	int cmp_count = 0;
	for(int i = 0; i < AlicePID. size(); ++i) {
		uint64_t pid = BobPID[i].reveal<uint64_t>(BOB);
		Integer currentFlag = AliceFlag[i][0];
		Bit anyeq = Bit(false, PUBLIC);
		for (int j = 0; j < AlicePID[i].size(); ++j) {
			cmp_count ++;
			Bit eq = (AlicePID[i][j] == BobPID[i]);
			currentFlag = currentFlag.If(eq, AliceFlag[i][j]);
			anyeq = eq | anyeq;
		}
		Bit hit = anyeq & judge(currentFlag, BobFlag[i]);
		bool bi = hit.reveal(BOB);
		if(party == BOB and bi)
			cout << pid <<endl;
	}
	cout <<"Number of Comp\t"<<cmp_count<<endl;
}

void compute_hu_ca(vector<vector<Integer>>& AlicePID, vector<vector<Integer>>& AliceFlag,
					vector<Integer>& BobPID, vector<Integer>& BobFlag, int party) {
	if(AlicePID.size()!= AliceFlag.size())error("size 1!");
	if(AlicePID.size()!= BobPID.size())error("size 2!");
	if(BobPID.size()!= BobFlag.size())error("size 3!");

	int cmp_count = 0;
	Integer res_ca(32, 0, PUBLIC);

	for(int i = 0; i < AlicePID. size(); ++i) {
		uint64_t pid = BobPID[i].reveal<uint64_t>(BOB);
		Bit anyhit(PUBLIC, false);
		for (int j = 0; j < AlicePID[i].size(); ++j) {
			cmp_count ++;
			Bit eq = (AlicePID[i][j] == BobPID[i]);
			Bit ju = judge(AliceFlag[i][j], BobFlag[i]);
			Bit res = eq & ju;
			Integer resplus1 = res_ca + Integer(32, 1, PUBLIC);
			res_ca = If(res, resplus1, res_ca);
		}
	}
	cout << "SUM: " << res_ca.reveal<int>() << endl;
}

void compute_hu_alice(vector<vector<uint64_t>>& pid, vector<vector<uint64_t>>& flag, NetIO * io) {
	vector<vector<Integer>> AlicePID;
	vector<vector<Integer>> AliceFlag;
	vector<Integer> BobPID;
	vector<Integer> BobFlag;
	Batcher batcher;
	cout <<"ALICE "<<pid.size()<<" "<<flag.size()<<endl;
	int max_size = 0;
	for(int i = 0; i < pid.size(); ++i)
		if (pid[i].size() > max_size)
			max_size = pid[i].size();
	io->send_data(&max_size, 4);
	cout << max_size;
	for(int i = 0; i < pid.size(); ++i) {
		AlicePID.push_back(vector<Integer>());	
		AliceFlag.push_back(vector<Integer>());
		for(int j = 0; j < pid[i].size(); ++j) {
			AlicePID[i].push_back(Integer(64, pid[i][j], ALICE));
			AliceFlag[i].push_back(Integer(36, flag[i][j], ALICE));//32 -> 17
		}
		for(int j = 0; j < max_size - pid[i].size(); ++j) {
			AlicePID[i].push_back(Integer(64, 0, ALICE));
			AliceFlag[i].push_back(Integer(36, 0, ALICE));//32 -> 17
		}
	}

	for(int i = 0; i < pid.size(); ++i) {
		batcher.add<Integer>(64, 0);
		batcher.add<Integer>(36, 0);
	}
	batcher.make_semi_honest(BOB);
	for(int i = 0; i < pid.size(); ++i) {
		BobPID.push_back(batcher.next<Integer>());
		BobFlag.push_back(batcher.next<Integer>());
	}

	compute_hu(AlicePID, AliceFlag, BobPID, BobFlag, ALICE);
	//compute_hu_ca(AlicePID, AliceFlag, BobPID, BobFlag, ALICE);
}

void compute_hu_bob(vector<uint64_t>& pid, vector<uint64_t>& flag, NetIO* io) {
	vector<vector<Integer>> AlicePID;
	vector<vector<Integer>> AliceFlag;
	vector<Integer> BobPID;
	vector<Integer> BobFlag;
	Batcher batcher;
	cout <<"BOB "<<pid.size()<<" "<<flag.size()<<endl;
	int max_size = 0;
	io->recv_data(&max_size, 4);

	for(int i = 0; i < pid.size(); ++i) {
		AlicePID.push_back(vector<Integer>());	
		AliceFlag.push_back(vector<Integer>());	
		for(int j = 0; j < max_size; ++j) {
			AlicePID[i].push_back(Integer(64, 0, ALICE));
			AliceFlag[i].push_back(Integer(36, 0, ALICE));
		}
	}
	for(int i = 0; i < pid.size(); ++i) {
		batcher.add<Integer>(64, pid[i]);
		batcher.add<Integer>(36, flag[i]);
	}
	batcher.make_semi_honest(BOB);
	for(int i = 0; i < pid.size(); ++i) {
		BobPID.push_back(batcher.next<Integer>());
		BobFlag.push_back(batcher.next<Integer>());
	}
	compute_hu(AlicePID, AliceFlag, BobPID, BobFlag, BOB);
	//compute_hu_ca(AlicePID, AliceFlag, BobPID, BobFlag, BOB);
}



