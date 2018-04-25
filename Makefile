all:	main.cpp ConnectionManager.hpp Globals.hpp GridOutlierDetection.hpp
		g++  -std=c++11 -pthread -o P2 GridOutlierDetection.cpp ConnectionManager.cpp main.cpp
debug:	main.cpp ConnectionManager.hpp Globals.hpp GridOutlierDetection.hpp
		g++ -g -std=c++11 -pthread -o P2 GridOutlierDetection.cpp ConnectionManager.cpp main.cpp
clean:
		rm -rf *.o *~ P2 core
