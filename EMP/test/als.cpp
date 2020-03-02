#include <typeinfo>
#include "test/als.h"
#include <algorithm>
using namespace emp;
using namespace std;

int main(int argc, char** argv) {
	NetIO * io;
	int port, party;
	parse_party_and_port(argv, &party, &port);
	io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);
	setup_semi_honest(io, party);	
	vector<pair<uint64_t,uint32_t>> data;
	int bob_size, alice_size;
	parse_file(argv[3], data);
	if(party == ALICE) {
		alice_size = data.size();
		io->recv_data(&bob_size, 4);
		io->send_data(&alice_size, 4);
	} else {
		bob_size = data.size();
		io->send_data(&bob_size, 4);
		io->recv_data(&alice_size, 4);
	}
	io->flush();
	for(int i = 0; i < data.size(); ++i) {
		data[i].first = data[i].first << 1;
		if(party == ALICE)
			data[i].first = data[i].first + 1;
	}
	sort(data.begin(), data.end());

	cout <<"start\n";
	compute_ci_pre(alice_size, bob_size, party, data, io);
	if(party == ALICE) {
		cout << "ALICE Network:\t"<<io->counter<<" Bytes"<<endl;
	} else {
		cout << "BOB Network:\t"<<io->counter<<" Bytes"<<endl;
		HalfGateEva<NetIO> * t = (HalfGateEva<NetIO> *)(CircuitExecution::circ_exec);
		cout << "Gate:\t"<< (t->gid) <<" AND gates"<<endl;
	}

	io->flush();
	delete io;
	return 0;
}
