// C++ Standard headers
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

// C headers
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>


std::string get_file_name(const std::string& path) {
    /**
     * This takes a file's path as argument
     * and returns its unqualified name.
     */

    return path.substr(path.find_last_of('/') + 1);
}


std::string get_file_extension(const std::string& path) {
    /**
     * This returns the final file extension
     * from a given path, without the '.' character.
     */
    size_t last_dot = path.find_last_of('.');
    if (last_dot == std::string::npos) {
        return "";
    }

    return path.substr(last_dot + 1);
}


std::string get_file_extension_type(const std::string& extension)
{
    /**
     * This returns the type of a file based on the
     * extension.
     */

    static const std::unordered_map<std::string, std::string>
        extension_to_type {
            { "txt",   "Text File"          },
            { "c"  ,   "C Source File"      },
            { "cpp",   "C++ Source File"    },
            { "h"  ,   "C/C++ Header File"  },
            { "lnk",   "Windows Shortcut"   },
            { "java",  "Java Source File"   },
            { "class", "Java Class File"    }
        };

    auto it = extension_to_type.find(extension);
    if (it != extension_to_type.end()) {
        return it->second;
    }
    return "Unknown";
}


off_t get_file_size(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        std::perror("stat");
        std::exit(EXIT_FAILURE);
    }
    return st.st_size;
}


time_t get_file_creation_time(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        std::perror("stat");
        std::exit(EXIT_FAILURE);
    }
    return st.st_ctime;
}


time_t get_file_modification_time(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        std::perror("stat");
        std::exit(EXIT_FAILURE);
    }
    return st.st_mtime;
}


std::string get_file_type(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        std::perror("stat");
        std::exit(EXIT_FAILURE);
    }

    if (S_ISDIR(st.st_mode)) {
        return "directory";
    } else if (S_ISREG(st.st_mode)) {
        return "regular file";
    } else if (S_ISLNK(st.st_mode)) {
        return "symlink";
    } else {
        return "unknown";
    }
}


std::string get_file_permissions(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        std::perror("stat");
        std::exit(EXIT_FAILURE);
    }

    char permissions[11];
    mode_t mode = st.st_mode & 0777;
    // Permissions string e.g.: -rwxrwxrwx

    permissions[0] = (S_ISDIR(mode)) ? 'd' : (S_ISLNK(mode)) ? 'l' : '-';
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';

    return std::string { permissions };
}


std::string get_file_line_ending(const std::string& path) {
    /**
     * This takes a filepath as an argument, and returns
     * the line ending it uses:
     * * CR   for Classic Mac OS,
     * * LF   for Unix (Linux, macOS, BSD, etc.),
     * * CRLF for Windows
     */

    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        std::perror("open");
        std::exit(EXIT_FAILURE);
    }

    char prev_char = '\0';
    char current_char;
    std::string line_ending;

    while (read(fd, &current_char, 1) == 1) {
        if (prev_char == '\r' && current_char == '\n') {
            line_ending = "CRLF";
            break;

        } else if (current_char == '\n') {
            line_ending = "LF";
            break;

        } else if (prev_char == '\r') {
            line_ending = "CR";
            break;
        }

        prev_char = current_char;
    }

    close(fd);
    return line_ending;
}


template <typename T>
void get_file_metadata(const T& path)
{
    std::ostringstream oss;
    oss << "File name: " << get_file_name(path) << '\n';
    std::string file_ext = get_file_extension(path);
    oss << "File extension: " << file_ext;
    oss << " ("  << get_file_extension_type(file_ext) << ")\n";

    oss << "File size: " << get_file_size(path) << " bytes\n";

    time_t creation_time = get_file_creation_time(path);
    oss << "File creation time: " << ctime(&creation_time);
    time_t modification_time = get_file_modification_time(path);
    oss << "File modification time: " << ctime(&modification_time);

    oss << "File type: " << get_file_type(path) << '\n';
    oss << "File permissions: " << get_file_permissions(path) << '\n';
    oss << "Line ending: " << get_file_line_ending(path) << '\n';
    oss << '\n';

    std::cout << oss.str();
}


template <typename T, typename... Args>
void get_file_metadata(const T& first, const Args&... args)
{
    get_file_metadata(first);
    if constexpr (sizeof...(args) > 0) {
        get_file_metadata(args...);
    }
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path1> [<path2> ...]" << '\n';
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        get_file_metadata(argv[i]);
    }

    return EXIT_SUCCESS;
}
