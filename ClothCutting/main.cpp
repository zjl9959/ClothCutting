#define LIBNEST2D_BACKEND_CLIPPER
#define LIBNEST2D_OPTIMIZER_NLOPT

#include "algorithm/solver.h"
#include "algorithm/problem.h"
#include <libnest2d.h>
#include "./tools/svgtools.hpp"

//using namespace cloth_cutting;
//int main(int argc, char *argv[]) {
//	Input input;
//	for (auto i = 0; i < input.items.size(); ++i) {
//		input.items[i].res_coords = input.items[i].raw_coords;
//	}
//	OutPut output;
//	output.save(input.items);
//    // TODO:Ìí¼ÓÊµÏÖ
//    return 0;
//}




int main() {
	using namespace libnest2d;

	// Example polygons 
	std::vector<Item> input1(23,
		{
			{-5000000, 8954050},
			{5000000, 8954050},
			{5000000, -45949},
			{4972609, -568550},
			{3500000, -8954050},
			{-3500000, -8954050},
			{-4972609, -568550},
			{-5000000, -45949},
			{-5000000, 8954050},
		});
	std::vector<Item> input2(15,
		{
		   {-11750000, 13057900},
		   {-9807860, 15000000},
		   {4392139, 24000000},
		   {11750000, 24000000},
		   {11750000, -24000000},
		   {4392139, -24000000},
		   {-9807860, -15000000},
		   {-11750000, -13057900},
		   {-11750000, 13057900},
		});

	std::vector<Item> input;
	input.insert(input.end(), input1.begin(), input1.end());
	input.insert(input.end(), input2.begin(), input2.end());

	// Perform the nesting with a box shaped bin
	nest(input, Box(150000000, 150000000));

	// Retrieve resulting geometries
	using namespace svg;
	using SVGWriter = SVGWriter<PolygonImpl>;
	SVGWriter::Config conf;
	conf.mm_in_coord_units = 1000000;
	SVGWriter svgw(conf);
	svgw.setSize(Box(250000000, 210000000));
	svgw.writeItems(input.begin(), input.end());
	svgw.save("out");

	return EXIT_SUCCESS;
}