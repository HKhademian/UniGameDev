#include<iostream>
#include<fstream>
//#include<conio.h>

using namespace std;

const char os_slash = '/';

string getFullFilename(string path) {
	string filename = "";
	for (int i = path.size() - 1; i >= 0; i--) {
		if (path[i] != os_slash)
			filename = path[i] + filename;
		else
			break;
	}

	return filename;
}

string getExt(string path) {
	string ext = "";
	for (int i = path.size() - 1; i >= 0; i--) {
		if (path[i] != '.')
			ext = path[i] + ext;
		else
			break;
	}
	return ext;
}

string getOnlyFileName(string filename) {
	string onlyFileName = "";
	for (char chr : filename) {
		if (chr != '.')
			onlyFileName += chr;
		else
			break;
	}
	return onlyFileName;
}

int main() {
	string line;
	string source_path_and_name, target_directory;

	cout << "Enter source file path(like '/home/test.txt'): ";
	getline(cin, source_path_and_name);
	cout << "Enter target file path(like '/home/'): ";
	getline(cin, target_directory);

	auto target_path = target_directory + getOnlyFileName(getFullFilename(source_path_and_name)) + "-copy." +
										 getExt(source_path_and_name);

	cout << target_path << endl;

	ifstream source_file(source_path_and_name);
	ofstream target_file(target_path);

	if (source_file && target_file)
		while (getline(source_file, line))
			target_file << line << "\n";
	else {
		cerr << "could't open the file(s)" << endl;
		exit(1);
	}
	cout << "copy finished!" << endl;
	source_file.close();
	target_file.close();
	return 0;
}