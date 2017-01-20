#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

#include "dir_list.h"


FileListing::FileListing(string & s, time_t t)
{
    name = s;
    date = t;
}

ostream& operator<< (ostream & out, const FileListing & in)
{
    out << '>' << in.name << " | " << in.date;
}


vector<string> dir_list;



// walk through the specified path, storing the details in the provided FileListing vector
// can recurse into subdirectories if needed
// also sends status messages to a provided ostream (defaults to cout)

void get_dir_files(vector<FileListing> & file_list, string & path_name, ostream & out, bool recursion)
{
    dir_list.push_back(path_name);
    out << "entering " << path_name << endl;

    DIR * dir_stream = opendir(path_name.c_str());
    if (dir_stream) {
        struct dirent * entry;
        while (entry = readdir (dir_stream)) {
            string filename = path_name + entry->d_name;
            struct stat sbuf;
            if (stat(filename.c_str(),&sbuf) == -1) {
                // cout << strerror(errno) << " |ERROR| " << filename << endl;
                continue;
            }
            if ( S_ISREG(sbuf.st_mode) ){         // normal file
                    file_list.push_back(FileListing(filename,sbuf.st_mtime));
            }
            if (S_ISDIR(sbuf.st_mode)) {        // directory
                //  Ignore the automatic '.' and '..' directories
                if (entry->d_name[0]=='.') continue;
                else if (recursion) {
					filename += '\\';
					get_dir_files(file_list, path_name, out, recursion);
				}
            }
        }
        closedir(dir_stream);
    }
}

// same as get_dir_files, except that it only stores files that have filenames that contain _filter_
void get_filtered_files(vector<FileListing> & file_list, string & path_name, const string & filter, ostream & out, bool recursion)
{
    dir_list.push_back(path_name);
    out << "entering " << path_name << endl;

    DIR * dir_stream = opendir(path_name.c_str());
    if (dir_stream) {
        struct dirent * entry;
        while (entry = readdir (dir_stream)) {
            string filename = path_name + entry->d_name;
            struct stat sbuf;
            if (stat(filename.c_str(),&sbuf) == -1) {
                // cout << strerror(errno) << " |ERROR| " << filename << endl;
                continue;
            }
            if (S_ISREG(sbuf.st_mode)){         // normal file
				// only store file if it matches the filter
				if (filename.find(filter) != string::npos)
                    file_list.push_back(FileListing(filename, sbuf.st_mtime));
            }
            if (S_ISDIR(sbuf.st_mode)) {        // directory
                //  Ignore the automatic '.' and '..' directories
                if (entry->d_name[0]=='.') continue;
                else if (recursion) {
					filename += '\\';
					get_filtered_files(file_list, path_name, filter, out, recursion);
				}
            }
        }
        closedir(dir_stream);
    }
}
