#include "two_wheels.hpp"
#include "epos_single/argparse.hpp"
 
int main(int argc, char** argv)
{
	argparse::ArgumentParser parser("./epos_wheels", "EPOS4 communicate program for ROS", "Apache License 2.0");

	parser.addArgument({"--yaml_file"}, "yaml file.");
	parser.addArgument({"--wheel_spd_path"}, "Wheel speed file folder path.");

	auto args = parser.parseArgs(argc, argv);

	auto yaml = args.get<std::string>("yaml_file");
	auto spd_path = args.get<std::string>("wheel_spd_path");

	two_wheels tw( spd_path, yaml);

	return 0;
}
