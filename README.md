## MPC-RiskStratification
  MPC-RiskStratification - software demo for the following paper submission - Studying Patient Risk Stratification Using Garbled Circuits, submitted to AMIA 2020 Annual Symposium, Chicago, IL, November, 2020.
  <br/>
## Installation
  <br/>
  Set up EMP-Toolkit, in this demo we use the semi-honest mode instance - emp-sh2pc. 
  <br/>Follow the instruction from: https://github.com/emp-toolkit/emp-sh2pc
  <br/>Alternatively, use the docker container provided from: https://github.com/MPC-SoK/frameworks/tree/master/emp
  <br/><br/>
  - ComorbidityIndex <br/>
  - HighUtilizer <br/>
  - EMP <br/>
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- emp-clinic <br/>
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- test  <br/>
  - README.md <br/><br/>
  1) Copy the two folders emp-clinic and test into ~/emp-toolkit/emp-sh2pc as ~/emp-toolkit/emp-sh2pc/emp-clinic/ and ~/emp-toolkit/emp-sh2pc/test/
  <br/><br/>  
  2) Put the two lines at the bottom of ~/emp-toolkit/emp-sh2pc/CMakeLists.txt   <br/>
add_test (hu)  <br/>
add_test (ci2)  <br/><br/>
  3) Copy the two data folder - ComorbidityIndex and HighUtilizer into ~/emp-toolkit/emp-sh2pc/, as ~/emp-toolkit/emp-sh2pc/ComorbidityIndex/ and ~/emp-toolkit/emp-sh2pc/HighUtilizer/
  <br/><br/>  
  4) Open a terminal and make files using ~/emp-toolkit/emp-sh2pc/build$make
  <br/><br/>  
  5) To switch between the cardinality mode (report only patient count) and regular mode (report individuals), use correct versions in compute_hu() and compute_ci() functions. See line 149, 181 in /EMP/test/hu.h for the High utilizer protoco, or line 140, 172 in /EMP/test/ci.h for Charlson index protocol. Notice changes need to be applied to both parties.
  <br/><br/> 
  6) To adjust the thresholds in figure 3,4, see the judge functions in ci.h and hu.h. Currently, the threshold for Charlson index value is set to 8 for PSI-CI, and total ED visits is set to 4 for PSI-HU. More specifically, at line 55 in the hu.h, anyhit = (hu_count >= Integer(7, 4, PUBLIC)) | anyhit, 7 here refers to an 7 bit integer to hold the result for the jointly computed totally number ED visits, and 4 is the threshold >= 4. Similarly, at line 51 in the ci.h, return res >= Integer(7, 8, PUBLIC), 8 is the threshold for jointly computed Charlson index value, the protocol only reports over 8 here.
  <br/><br/>    i
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
  1) Download synthea package from https://github.com/synthetichealth/synthea
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
  3) Use the method described in the Data Preparation section to generate datasets for institution A and instituion B
  
