// C++ Standard headers
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
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
            { "txt",   "Text File"         },
            { "c"  ,   "C Source File"     },
            { "cpp",   "C++ Source File"   },
            { "h"  ,   "C/C++ Header File" },
            { "lnk",   "Windows Shortcut"  },
            { "java",  "Java Source File"  },
            { "class", "Java Class File"   },
            { "sh",    "Shell script"      },
            { "pdf",   "PDF"               },
            { "crx",   "Chrome Extension"  },
            { "mp3",   "MP3 File"          },
            { "mp4",   "MP4 File"          },
            { "zip",   "ZIP Archive"       },
            { "ico",   "Computer ICO File" },
            { "gif",   "GIF"               },
            { "jpg",   "JPG Image"         },
            { "png",   "PNG Image"         },
            { "iso",   "ISO Live Disk"     }
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


std::string identify_file_type(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return "Unable to open file";
    }

    constexpr int MAX_HEXBYTES_READ = 16;
    char buffer[MAX_HEXBYTES_READ + 1] = {0}; // + NUL
    file.read(buffer, MAX_HEXBYTES_READ);
    file.close();


    if (buffer[0] == 0x23 && buffer[1] == 0x21) {
        // 23 21
        return "Shell script";
    }
    if (buffer[0] == 0x53 && buffer[1] == 0x51 && buffer[2] == 0x4C
        && buffer[3] == 0x69 && buffer[4] == 0x74 && buffer[5] == 0x65
        && buffer[6] == 0x20 && buffer[7] == 0x66 && buffer[8] == 0x6F
        && buffer[9] == 0x72 && buffer[10] == 0x6D && buffer[11] == 0x61
        && buffer[12] == 0x74 && buffer[13] == 0x20 && buffer[14] == 0x33
        && buffer[15] == 0x00) {
        // 53 51 4C 69 74 65 20 66 6F 72 6D 61 74 20 33 00
        return "SQLite database";
    }
    if (buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0x01 && buffer[3] == 0x00) {
        return "Computer ICO icon file";
    }
    if (buffer[0] == 0x47 && buffer[1] == 0x49 && buffer[2] == 0x46
        && buffer[3] == 0x38 && buffer[5] == 0x61) {
        return "GIF";
    }
    if (buffer[0] == 0xFF && buffer[1] == 0xD8 && buffer[2] == 0xFF
        && buffer[3] == 0xEE) {
        return "JPG";
    }
    if (buffer[0] == 0x52 && buffer[1] == 0x61 && buffer[2] == 0x72
        && buffer[3] == 0x21 && buffer[4] == 0x1A && buffer[5] == 0x07) {
        return "RAR";
    }
    if (buffer[0] == 0x89 && buffer[1] == 0x50 && buffer[2] == 0x4E
        && buffer[3] == 0x47 && buffer[4] == 0x0D && buffer[5] == 0x0A
        && buffer[6] == 0x1A && buffer[7] == 0x0A) {
        return "PNG";
    }
    if (buffer[0] == 0xCA && buffer[1] == 0xFE && buffer[2] == 0xBA && buffer[3] == 0xBE) {
        // CA FE BA BE
        return "Java .class";
    }
    if (buffer[0] == 0x25 && buffer[1] == 0x50 && buffer[2] == 0x44 && buffer[3] == 0x46) {
        return "PDF";
    }
    if (buffer[0] == 0x49 && buffer[1] == 0x44 && buffer[2] == 0x33) {
        return "MP3";
    }
    if (buffer[0] == 0x43 && buffer[1] == 0x44 && buffer[2] == 0x30
        && buffer[3] == 0x30 && buffer[4] == 0x31) {
        // 43 44 30 30 31
        return "ISO";
    }
    if (buffer[0] == 0x43 && buffer[1] == 0x72 && buffer[2] == 0x32
        && buffer[3] == 0x34) {
        // 43 72 32 34
        return "Google Chrome Extension (CRX)";
    }
    if (buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0x00 && buffer[3] == 0x18) {
        return "MP4";
    }
    return "Unknown";
}


template <typename T>
void get_file_metadata_impl(const T& path)
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
    oss << "Kind of file: " << identify_file_type(path) << '\n';
    oss << '\n';

    std::cout << oss.str();
}


template <typename T, typename... Args,
    typename std::enable_if<std::is_constructible<std::string, T>::value>::type* = nullptr>
void get_file_metadata(const T& first, const Args&... args)
{
    /**
     * enable_if and is_constructible ensure that only string types are accepted.
     */

    get_file_metadata_impl(first);
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
