#include <iostream>
#include <chrono>
#include <ctime>

int main(int argc, char const *argv[])
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

	int sum = 0;

	for (int i = 3; i < 1000; ++i)
	{
		if(i % 3 == 0 || i % 5 == 0) sum += i;
	}

    end = std::chrono::system_clock::now();
    long elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "Elapsed Milliseconds: " << elapsed_millis << std::endl;

	std::cout << sum << std::endl;
	return 0;
}