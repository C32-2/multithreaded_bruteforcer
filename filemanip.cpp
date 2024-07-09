#include "filemanip.h"

std::mutex mtx;

file_task::file_task(file_ptr dir_file, bool is_checked, std::uintmax_t size)
{
    this->dir_file = dir_file;
    this->is_checked = is_checked;
    this->size = size;

    this->dir_file->clear();
    this->dir_file->seekg(0, std::ios::beg);
}

filemanip::filemanip(std::string dir, std::string target_line) : dir(dir), count_of_files(0), total_dir_size(0), target_line(target_line) { }

void filemanip::dir_traversal()
{
    std::string extension;
    for (const auto& dir_entry : fs::recursive_directory_iterator(this->dir))
    {
        if (fs::is_regular_file(dir_entry))
        {
            extension = fs::path(dir_entry).extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            if (extension == ".txt")
            {
                std::uintmax_t file_size = fs::file_size(dir_entry);
                this->dir_files.emplace_back(open_file(dir_entry.path().string(), file_size));
                count_of_files += 1;
                total_dir_size += file_size;
            }
        }
    }
}

file_task filemanip::open_file(const std::string& filepath, std::uintmax_t file_size) const
{
    auto input_file = std::make_shared<std::ifstream>(filepath);
    if (!input_file->is_open())
    {
        return file_task(nullptr, true, 0);
    }
    return file_task(input_file, false, file_size);
}

bool filemanip::process_dictionary(file_task& fproc)
{
    std::string line;
    while (std::getline(*fproc.dir_file, line))
    {
        if (line == target_line)
        {
            return true;
        }
    }

    return false;
}


void filemanip::bruteforce(int threads_count)
{
    int threads = threads_count;

    if (threads_count <= 0 || threads_count > 12)
    {
        std::cout << "Invalid count of threads. Using " << std::thread::hardware_concurrency() << " threads";
        threads = std::thread::hardware_concurrency();
    }

    thread_pool pool(threads);

    for (auto& file_task : dir_files)
    {
        pool.enqueue([&file_task, this]()
        {
            if (this->process_dictionary(file_task))
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "Line found: " << this->target_line << "\n";
            }
            file_task.is_checked = true;
            this->print_progress();
        });
    }
}

void filemanip::print_dir_files_info() const
{
    for (auto& file_process : dir_files)
    {
        std::cout << file_process.dir_file << "\n";
        std::cout << "\n";

        std::string line;
    }
}

double filemanip::get_total_dir_size() const
{
    return this->total_dir_size;
}

int filemanip::get_count_of_files() const
{
    return this->count_of_files;
}

void filemanip::print_progress() const
{
    std::lock_guard<std::mutex> lock(mtx);
    int completed_count = 0;
    double checked_size = 0;
    for (const auto& task : dir_files)
    {
        if (task.is_checked)
        {
            completed_count += 1;
            checked_size += task.size;
        }
    }
    std::cout << "\rProgress: " << (completed_count * 100 / dir_files.size()) 
        << "% " << "(" << completed_count << " / " << this->get_count_of_files() << " files) " << std::flush;
}