#ifndef FILEMANIP_H
#define FILEMANIP_H

#include "includes.h"
#include "thread_pool.h"

namespace fs = std::filesystem;
typedef std::shared_ptr<std::ifstream> file_ptr;
extern std::mutex mtx;

struct file_task
{
    file_ptr dir_file;
    bool is_checked;
    std::uintmax_t size;

    file_task(file_ptr dir_file, bool is_checked, std::uintmax_t size);
};

class filemanip
{
private:
    std::vector<file_task> dir_files;
    std::string dir;
    std::string target_line;
    int count_of_files;
    double total_dir_size;

    file_task open_file(const std::string& filepath, std::uintmax_t file_size) const;
    bool process_dictionary(file_task& fproc);

public:
    filemanip(std::string dir, std::string target_line);
    void dir_traversal();
    void bruteforce(int threads_count);
    void print_dir_files_info() const;
    double get_total_dir_size() const;
    int get_count_of_files() const;
    void print_progress() const;
    void calculate_average_brutforce_speed();
};

#endif
