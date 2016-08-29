#define _CRT_SECURE_NO_WARNINGS


#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <iostream>
#include <sstream>
#include <windows.h>

using namespace boost::interprocess;

struct Point
{
	int x, y;
};

struct sSharedData
{
	int id;
// 	int count;
// 	Point pts[3];
};


int main()
{
	try
	{
		shared_memory_object::remove("killing_space_camera");

		shared_memory_object sharedmem1(create_only, "killing_space_camera", read_write);

		// setting the size of the shared memory
		sharedmem1.truncate(65536);

		// map the shared memory to current process 
		mapped_region mmap(sharedmem1, read_write, 0, 256);

		sSharedData data;
		data.id = 0;
//		data.count = 0;

		int cnt = 0;
		while (1)
		{
			Sleep(1000);

			std::stringstream ss;
			ss << "Hello World! ";
			ss << cnt++;

			// access the mapped region using get_address
			data.id++;
// 			data.count = (data.count > 0)? 0 : 1;
// 			data.pts[0] = { 960, 540 };
			*(sSharedData*)mmap.get_address() = data;

			std::cout << "write " << cnt << std::endl;
		}

	}
	catch (interprocess_exception&e)
	{
		// .. .  clean up 
		std::cout << e.what() << std::endl;
		std::cout << "client1 error\n";
	}

	return 0;
}
