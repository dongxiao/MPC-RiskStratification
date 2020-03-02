#include <typeinfo>
#include "test/ci.h"
using namespace emp;
using namespace std;

int main(int argc, char** argv) {
	NetIO * io;
	int port, party;
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);
	setup_semi_honest(io, party);	
	vector<uint64_t> pid;
	vector<uint32_t> flag;
	parse_file(argv[3], pid, flag);

	vector<vector<uint64_t>> vv_pid;
	vector<vector<uint32_t>> vv_flag;
	vector<uint64_t> v_pid;
	vector<uint32_t> v_flag;

	if(party == ALICE) {
		for(int i = 0; i < 5; ++i) {
			vv_pid.push_back(pid);
			vv_flag.push_back(flag);
		}
		compute_ci_alice(vv_pid, vv_flag, io);
	} else {
		compute_ci_bob(pid, flag, io);
	}

	io->flush();
	delete io;
	return 0;
}

