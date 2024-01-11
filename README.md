# dsc_as_kShortestPaths
[dsc][graph] top k shortest paths &amp; max flow

**DESCRIPTION:** This is a practice program which adopts Dijkstra's algorithm and Yen's algorithm to find top k shortest paths between 2 nodes on a given directed graph. It also tries to follow the Push-Relabel algorithm to find the max flow.

**[COMPILE ON WINDOWS]**

***g++ -o <output_program> Dijkstra.cpp Yen.cpp Graph.cpp main.cpp***

***./<output_program> <input_configuration>***

e.g:

g++ -o run Dijkstra.cpp Yen.cpp Graph.cpp main.cpp

./run input/input.cfg

**[CHANGE INPUT]**

User can change the input configuration file at "input/input.cfg". Its format is "<start_point> <end_point>", which indicates the 2 points that we want to find top k shortest paths.
