--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
											Run Instructions
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
There are in total 4 files name as DoctorWho_parallel, DoctorWho_serial, Email_parallel, Email_serial the 
First two are for Finding Kth Shortest Path for Data Set DoctorWho.csv, classic-who.csv, new-who.csv. And
Second two are for finding the kth Shortest Path for Data Set Email-EuAll, Email-Enron. The Algorithm Used
is Modified Dijkastra's for finding kth Shortest Path for all 3 datasets.
  


---> DoctorWho_parallel
	This .cpp File Contains the code for finding kth shortest path for .csv files parallel execution
root will send the 10 randomly generated start and end to all other processes and those processes are 
responsible for finding k number of shortest paths from their respective assigned start and end, given 
below is the running instructions this file processes.

			--> mpic++ DoctorWho_parallel.cpp -o DoctorWho_parallel <--
			-->  mpiexec -n 10 -f machinefile ./DoctorWho_parallel  <--

---> DoctorWho_serial
	This .cpp File Contains the code for finding kth shortest path for .csv files serial execution will 
run the code in the loop 10 times and only one root/master is responsible for finding k number of shortest 
paths from their respective assigned start and endcfor that iteration, given below is the running instructions 
this file processes.

			--> g++ DoctorWho_serial.cpp -o DoctorWho_serial <--
			--> 		   ./DoctorWho_serial		 <--
 
 
---> Email_parallel
	This .cpp File Contains the code for finding kth shortest path for .txt files parallel execution
root will send the 10 randomly generated start and end to all other processes and those processes are 
responsible for finding k number of shortest paths from their respective assigned start and end, given 
below is the running instructions this file processes.

			-->     mpic++ Email_parallel.cpp -o Email_parallel         <--
			-->    mpiexec -n 10 -f machinefile ./Email_parallel        <--


---> Email_serial 
	This .cpp File Contains the code for finding kth shortest path for .txt files serial execution will 
run the code in the loop 10 times and only one root/master is responsible for finding k number of shortest 
paths from their respective assigned start and end for that iteration, given below is the running instructions 
this file processes.

			-->    g++ Email_serial.cpp -o Email_serial <--
			--> 		   ./Email_serial          <--


