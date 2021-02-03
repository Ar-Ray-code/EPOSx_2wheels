#include "two_wheels.hpp"

two_wheels::two_wheels(std::string abs_path, std::string configure_file)
{
    std::stringstream sub_name0, sub_name1;
    sub_name0 << abs_path << "/EPOS0.txt";
    sub_name1 << abs_path << "/EPOS1.txt";

    epos_base eb0("EPOS0", sub_name0.str(), configure_file);
    epos_base eb1("EPOS1", sub_name1.str(), configure_file);
    while(1)
	{
		sleep(1);
	}

}
two_wheels::~two_wheels()
{

}
