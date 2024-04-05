// C++ Standard headers
#include <cstdlib>
#include <iostream>
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
     * This takes an `std::string` with a file's path
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
            { "txt", "Text File"          },
            { "c"  , "C Source File"      },
            { "cpp", "C++ Source File"    },
            { "h"  , "C/C++ Header File"  }
        };

    auto it = extension_to_type.find(extension);
    if (it != extension_to_type.end()) {
        return it->second;

    } else {
        return "Unknown";
    }
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

    mode_t mode = st.st_mode & 0777;
    char permissions[10];
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';

    return permissions;
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



int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << '\n';
        return EXIT_FAILURE;
    }

    std::string path { argv[1] };

    std::cout << "File name: " << get_file_name(path) << '\n';
    std::string file_ext = get_file_extension(path);
    std::cout << "File extension: " << file_ext;
    std::cout << " ("  << get_file_extension_type(file_ext) << ")\n";

    std::cout << "File size: " << get_file_size(path) << " bytes\n";

    time_t creation_time = get_file_creation_time(path);
    std::cout << "File creation time: " << ctime(&creation_time);
    time_t modification_time = get_file_modification_time(path);
    std::cout << "File modification time: " << ctime(&modification_time);

    std::cout << "File type: " << get_file_type(path) << '\n';
    std::cout << "File permissions: " << get_file_permissions(path) << '\n';
    std::cout << "Line ending: " << get_file_line_ending(path) << '\n';

    return EXIT_SUCCESS;
}
