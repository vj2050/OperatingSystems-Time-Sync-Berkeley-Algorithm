CXX= g++
THR= lpthread
MAKE= make

compile : serverB_v6.cpp clientB_v6.cpp
	$(CXX) -o serverB_v6 serverB_v6.cpp -$(THR)

	$(CXX) -o clientB_v6 clientB_v6.cpp 
	
clean : 
	rm serverB_v6 clientB_v6
	ls -ltr
	$(info CLEAN SUCCESSFUL!!!)

run_server : 
	./serverB_v6

run_client :
	./clientB_v6
