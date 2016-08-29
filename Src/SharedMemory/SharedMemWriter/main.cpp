//
// boost 라이브러리를 이용해서, 공유메모리를 생성하고, 정보를 저장한다.
// 매 루프를 돌면서, 공유메모리에 정보를 쓴다.
// 
// 공유 메모리 이름은 "Hello" 다.
//

#define _CRT_SECURE_NO_WARNINGS


#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <iostream>
#include <sstream>
#include <windows.h>

using namespace boost::interprocess;


int main()
{
	try
	{
		shared_memory_object::remove("Hello");

		shared_memory_object sharedmem1(create_only, "Hello", read_write);

		// setting the size of the shared memory
		sharedmem1.truncate(65536);

		// map the shared memory to current process 
		mapped_region mmap(sharedmem1, read_write, 0, 256);

		int cnt = 0;
		while (1)
		{
			Sleep(1000);

			std::stringstream ss;
			ss << "Hello World! ";
			ss << cnt++;
			
			// access the mapped region using get_address
			//strcpy(static_cast<char*>(mmap.get_address()), "Hello World!\n");
			strcpy(static_cast<char*>(mmap.get_address()), ss.str().c_str());

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
