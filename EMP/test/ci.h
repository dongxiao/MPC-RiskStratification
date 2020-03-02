#include <iostream>
#include <vector>
#include <fstream>
#include <emp-sh2pc/emp-sh2pc.h>
#include "emp-clinic/emp-clinic.h"
using std::cout;
using std::endl;

void parse_file(const char * filename, vector<uint64_t>& pid, vector<uint32_t>& flag){
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
		for(int j = 0; j < 17; ++j) {
			flag[i] <<=1;
			fin>>tmp>>tmp2;
			if (tmp2 == 1)
				flag[i] = flag[i] | 0x1;
		}
	}
}
//judge if the patient with data a & b should be output.
Bit judge(Integer &a, Integer & b) {
	if(a.size()!=b.size())error("size mismatch");

	Integer u = a | b;
/*	uint32_t flag = u.reveal<uint32_t>(BOB);
	if(party == BOB)
		cout <<flag<<" ";	
*/
	int coeff[] = {6,6,3,2,2,2,2,1,1,1,1,1,1,1,1,1,1};
	Integer Coeff[17];
	Integer zero (4, 0, PUBLIC);
	for(int i = 0; i < 17; ++i) {
		Coeff[i] = Integer(4, coeff[i], PUBLIC);
		Coeff[i] = zero.If(u[i], Coeff[i]);
		Coeff[i].resize(7, false);
	}
	
	Integer res(7, 0, PUBLIC);
	for(int i = 0; i < 17; ++i)
		res = res + Coeff[i];
	
	return res >= Integer(7, 8, PUBLIC);
}

void compute_ci_ca(vector<vector<Integer>>& AlicePID, vector<vector<Integer>>& AliceFlag,
					vector<Integer>& BobPID, vector<Integer>& BobFlag, int party) {
	if(AlicePID.size()!= AliceFlag.size())error("size 1!");
	if(AlicePID.size()!= BobPID.size())error("size 2!");
	if(BobPID.size()!= BobFlag.size())error("size 3!");

	int cmp_count = 0;
	Integer res_ca(32, 0, PUBLIC);

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
		Integer resplus1 = res_ca + Integer(32, 1, PUBLIC);
		res_ca = If(hit, resplus1, res_ca);
	}
	cout << "SUM: " << res_ca.reveal<int>() << endl;
}

void compute_ci(vector<vector<Integer>>& AlicePID, vector<vector<Integer>>& AliceFlag,
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

void compute_ci_alice(vector<vector<uint64_t>>& pid, vector<vector<uint32_t>>& flag, NetIO * io) {
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
			AliceFlag[i].push_back(Integer(17, flag[i][j], ALICE));//32 -> 17
		}
		for(int j = 0; j < max_size - pid[i].size(); ++j) {
			AlicePID[i].push_back(Integer(64, 0, ALICE));
			AliceFlag[i].push_back(Integer(17, 0, ALICE));//32 -> 17
		}
	}

	for(int i = 0; i < pid.size(); ++i) {
		batcher.add<Integer>(64, 0);
		batcher.add<Integer>(17, 0);
	}
	batcher.make_semi_honest(BOB);
	for(int i = 0; i < pid.size(); ++i) {
		BobPID.push_back(batcher.next<Integer>());
		BobFlag.push_back(batcher.next<Integer>());
	}
	//Choose between the regular version and cardinality only version
	compute_ci(AlicePID, AliceFlag, BobPID, BobFlag, ALICE);
	//compute_ci_ca(AlicePID, AliceFlag, BobPID, BobFlag, ALICE);
}

void compute_ci_bob(vector<uint64_t>& pid, vector<uint32_t>& flag, NetIO* io) {
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
			AliceFlag[i].push_back(Integer(17, 0, ALICE));
		}
	}
	for(int i = 0; i < pid.size(); ++i) {
		batcher.add<Integer>(64, pid[i]);
		batcher.add<Integer>(17, flag[i]);
	}
	batcher.make_semi_honest(BOB);
	for(int i = 0; i < pid.size(); ++i) {
		BobPID.push_back(batcher.next<Integer>());
		BobFlag.push_back(batcher.next<Integer>());
	}
	//Choose between the regular version and cardinality only version
	compute_ci(AlicePID, AliceFlag, BobPID, BobFlag, BOB);
	//compute_ci_ca(AlicePID, AliceFlag, BobPID, BobFlag, BOB);
}

