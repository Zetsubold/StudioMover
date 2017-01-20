#ifndef DIR_LIST_H
#define DIR_LIST_H

class FileListing {
	// For storing the details about a particular file
public:
    string name;
    time_t date;
    FileListing(string & s, time_t t);
};

ostream& operator<< (ostream & out, const FileListing & in);

void get_dir_files(vector<FileListing> & file_list, string & path_name, ostream & out = cout, bool recursion = false);

void get_filtered_files(vector<FileListing> & file_list, string & path_name, const string & filter, ostream & out = cout, bool recursion = false);

#endif
