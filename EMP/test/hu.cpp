#include <typeinfo>
#include "test/hu.h"
#include <chrono>
using namespace emp;
using namespace std;

int main(int argc, char** argv) {
	NetIO * io;
	int port, party;
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);
	setup_semi_honest(io, party);
	vector<uint64_t> pid;
	vector<uint64_t> flag;
	int bob_size;
	parse_file(argv[3], pid, flag);
	if(party == ALICE) {
		io->recv_data(&bob_size, 4);
	} else {
		bob_size = pid.size();
		io->send_data(&bob_size, 4);
	}

	auto t1 = std::chrono::high_resolution_clock::now();

	CHash<uint64_t> hash(bob_size);
	vector<vector<uint64_t>> vv_pid;
	vector<vector<uint64_t>> vv_flag;
	vector<uint64_t> v_pid(hash.size(), 0);
	vector<uint64_t> v_flag(hash.size(), 0);
	for (int i = 0; i < hash.size(); ++i) {
		vv_pid.push_back(vector<uint64_t>());
		vv_flag.push_back(vector<uint64_t>());
	}

	if(party == ALICE) {
		hash.rhash(vv_pid, vv_flag, pid, flag);
		cout <<"start\n";
		compute_hu_alice(vv_pid, vv_flag, io);
		cout << "Network:\t"<<io->counter<<" Bytes" <<endl;
	} else {
		hash.chash(v_pid, v_flag, pid, flag);
		cout <<"start\n";
		compute_hu_bob(v_pid, v_flag, io);
		cout << "Network:\t"<<io->counter<<" Bytes" <<endl;
		HalfGateEva<NetIO> * t = (HalfGateEva<NetIO> *)(CircuitExecution::circ_exec);
		cout << "Gate:\t"<< (t->gid) <<" AND gates" <<endl;
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	std::cout << duration;
	io->flush();

	delete io;
	return 0;
}

