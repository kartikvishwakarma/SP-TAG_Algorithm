# SP-TAG_Algorithm
@author 
	kartik vishwakarma
	04-11-2018


Implementation of serial and parallel dijkstra algorithms
on SP-TAG graph using openmp.

Dataset, "tdEdgeId.src.sink.time.csv", spatial-temporal road network


parallel execution:
-------------------

compile:
	g++ -fopenmp parallel_dijkstra.cpp -o parallel_dijkstra -std=c++11 -g

run:
	./parallel_dijkstra

NOTE: "tdEdgeId.src.sink.time.csv" must be at same path location to execute code successfully.


serial execution:
------------------

compile:
	g++ serial_dijkstra.cpp -o serial_dijkstra -std=c++11 -g

run:
	./serial_dijkstra

NOTE: default "test.csv" as dataset, for "tdEdgeId.src.sink.time.csv" 
	run using,
			./serial_dijkstra tdEdgeId.src.sink.time.csv
