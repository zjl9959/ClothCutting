#include "algorithm/solver.h"
#include "algorithm/problem.h"

using namespace cloth_cutting;


int main(int argc, char *argv[]) {
	Input input;
	for (auto i = 0; i < input.items.size(); ++i) {
		input.items[i].res_coords = input.items[i].raw_coords;
	}

	Solver sol(input);
	sol.run();

	Output output;
	output.save(input.items);
    
	system("Pause");
    return 0;
}
