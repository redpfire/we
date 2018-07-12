
linux:
	g++ -lboost_system -lboost_filesystem --std=c++11 -o we *.cpp

clean:
	rm -f we
