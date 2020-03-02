## MPC-RiskStratification
  MPC-RiskStratification repo contains the software demo for the following paper submission:<br/><br/>
  Dong X, Randolph D, Weng C, Wang X. *Studying Patient Risk Stratification Using Garbled Circuits*,
  submitted to AMIA 2020 Annual Symposium, Chicago, IL, November, 2020.
  <br/>
## Installation
  <br/>
  Set up EMP-Toolkit, in this demo we use the semi-honest mode instance - emp-sh2pc. 
  <br/>Follow the instruction from: https://github.com/emp-toolkit/emp-sh2pc
  <br/>Alternatively, use the docker container provided from: https://github.com/MPC-SoK/frameworks/tree/master/emp
  <br/><br/>
  1) Copy the two folders under /EMP/emp-tool/ and /EMP/emp-ot/ into ~/emp-toolkit/emp-sh2pc/ as ~/emp-toolkit/emp-sh2pc/emp-clinic/ and ~/emp-toolkit/emp-sh2pc/test/
  <br/><br/>  
  2) Put the three lines at the bottom of ~/emp-toolkit/emp-sh2pc/CMakeLists.txt   <br/>
add_test (hu)  <br/>
add_test (ci2)  <br/>
add_test (chash)  <br/><br/>
  3) Copy the two data folders into ~/emp-toolkit/emp-sh2pc/, as ~/emp-toolkit/emp-sh2pc/ComorbidityIndex/ and ~/emp-toolkit/emp-sh2pc/HighUtilizer/
  <br/><br/>  
  4) Open a terminal and make files using ~/emp-toolkit/emp-sh2pc/build$make
  <br/><br/>  
  
## Performance Test
  <br/>
  1) To run the Comorbidity Index Protocol, open two terminals, and type:
  <br/>
  ~/emp-toolkit/emp-sh2pc/build$ ./bin/ci2 1 12345 ../ComorbidityIndex/Realistic/cmbd1.64.dat
  <br/>
  ~/emp-toolkit/emp-sh2pc/build$ ./bin/ci2 2 12345 ../ComorbidityIndex/Realistic/cmbd2.64.dat
  <br/><br/>  
  2) To run the High Utilizer Protocol, open two terminals, and type:
  <br/>
  ~/emp-toolkit/emp-sh2pc/build$ ./bin/hu 1 12345 ../HighUtilizer/Realistic/hu1.64.dat
  <br/>
  ~/emp-toolkit/emp-sh2pc/build$ ./bin/hu 2 12345 ../HighUtilizer/Realistic/hu2.64.dat
  <br/><br/>  
  To simulate network latency or traffic bandwidth, use the Linux *tc* commands, or utilities contributed by github members such as https://gist.github.com/bidulock/11233356  <br/><br/>
  
  Running time are 193 seconds for PSI-CI and 422 seconds respectively for PSI-CI and PSI-HU in simulated a WAN setting (96ms round trip latency, 50MBps bandwidth), the experiments were performed on a system has an Intel i7-8750H 2.2 GHz CPU. The cardinality only version (PSI-CI-CA, PSI-HU-CA) runs ~10 seconds slower. To run the cardinality only version, check /test/ci.h and /test/hu.h for details.
    <br/><br/>  
    
  ## Synthetic Data Generation
  <br/>
  download synthea package from https://github.com/synthetichealth/synthea
  <br/>
  ./run_synthea -s 1234 -p 2700000 Illinois "Chicago"
  <br/>
  note: enable csv output in /src/main/resources/synthea.properties
  <br/><br/>  
  2) Load synthetic data into encounter and demographics tables 
  <br/>
  ENCOUNTERCLASS = 'emergency', these are ED visits
  <br/>
  column LAT and LON from the demographics tables records the exact coordinates
  <br/><br/>  
  
