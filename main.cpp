#include "filemanip.h"
#include <chrono>

int main()
{
	std::string dir = "";
	std::string target = "";
	int threads = 0;

	std::cout << "Select directory with .txt password dictionaries\n->";
	std::cin >> dir;

	std::cout << "Type target line\n->";
	std::cin >> target;

	std::cout << "Type count of threads\n->";
	std::cin >> threads;

	filemanip fm(dir, target);

	fm.dir_traversal();
	std::cout << "Brutforcing now... using " << threads << " threads\n";
	std::cout << "Total directory size: " << fm.get_total_dir_size() / (1024 * 1024) << " mb\n";
	std::cout << "Count of files: " << fm.get_count_of_files() << "\n";

	auto start = std::chrono::high_resolution_clock::now();
	fm.bruteforce(threads);
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end - start;
	double elapsed_minutes = elapsed.count() / 60.0;

	// Вывод результата
	std::cout << "Execution time: " << elapsed_minutes << " minutes" << std::endl;

}