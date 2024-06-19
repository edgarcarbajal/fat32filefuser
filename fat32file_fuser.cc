#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;

void fuseFiles(vector<filesystem::directory_entry>* fileEntries, string outFilePath);
void msgExit(int code);
bool is_numeric_string(string str);
void fileErrorMsg(unsigned int filestate_bits, string filepath);

int main(int argc, char* argv[]) {
    cout << "*****-******-*****\n";
    cout << "fat32file_fuser - CLI Tool to merge large files into 1 that were split up due to fat32 file size limitations!\n Please make sure your files/directories have no whitespaces!\n\n";
    
    string filesDirPath;
    string fusedFilePath;
    if(argc == 1) {
        cout << "No arguments passsed in when running this program(fat32file_fuser), please provide input for the following prompts...\n";

        cout << "Please enter the directory(folder) of the split files: ";
        getline(cin, filesDirPath);


        cout << "Please enter the destination path of the newly fused file (The name of the file will be the same as the folder!): ";
        getline(cin, fusedFilePath);
    }
    else if(argc == 3) {
        filesDirPath = argv[1];
        fusedFilePath = argv[2];
    }
    else {
        cerr << "Error: Incorrect usage of program!\n\nHow to use: ./fat32file_fuser <input directory file path> <output directory file path>\nOr: ./fat32file_fuser\n\n";
        msgExit(-1);
    }
    cout << "\n\n";

    if(!filesystem::is_directory(filesDirPath)){
        cerr << "Error: filepath \"" << filesDirPath << "\" is not a valid directory path. Please re-run the program and try again.\n";
        msgExit(1);
    }
    if(!filesystem::is_directory(fusedFilePath)){
        cerr << "Error: filepath \"" << fusedFilePath << "\" is not a valid directory path. Please re-run the program and try again.\n";
        msgExit(1);
    }

    // format of filenames must be numbers (0-indexed) (ie: 00, 01, 02, 03...)
    cout << "Eligible mergable files in the directory: " << filesDirPath << '\n';
    vector<filesystem::directory_entry> fileparts;
    for(const auto& entry : filesystem::directory_iterator(filesDirPath)) {
        
        if(is_numeric_string(entry.path().filename())) {
            cout << entry.path() << '\n';
            fileparts.push_back(entry);
        }
    }


    //filesystem has an overload for '/' operator.
    auto outputPath = filesystem::canonical(filesystem::path(fusedFilePath)) / filesystem::path(filesDirPath).filename();
    if(filesystem::path(filesDirPath).filename() == "") {
        cout << "Error: Directory Name in path might contain whitespaces, so cannot obtain filename to append to new fused file! - Will use the file name : \"UnknownMergedFile(fat32file_fuser)\"\n";
        cout << "NOTE: This might cause overwritting other fused files that failed in a similar way(ie: have the same name), so please makesure to stop the program and rename the source directory with no whitespaces just in case!\n";
        fusedFilePath = outputPath.string() + "UnknownMergedFile(fat32file_fuser)";
    }
    else
        fusedFilePath = outputPath.string() + "[Merged]";

    sort(fileparts.begin(), fileparts.end());
    cout << "\n*\n*\nFusing files in directory: " << filesDirPath << " into the single file: " << fusedFilePath << '\n';
    fuseFiles(&fileparts, fusedFilePath);

    msgExit(0);
}


void fuseFiles(vector<filesystem::directory_entry>* fileEntries, string outFilePath) {
    uintmax_t bufferSize = 1024*1024;
    ifstream inputfile;
    ofstream outputfile;
    char* buffer = new char[bufferSize]; //1 MB(ie: 1024*1024) buffer (1 char == 1 byte)


    outputfile.open(outFilePath, fstream::binary);
    if(outputfile.fail()) {
        fileErrorMsg(outputfile.rdstate(), outFilePath);
        msgExit(2);
    }

    for(size_t i = 0; i < fileEntries->size(); i++) {
        inputfile.open(fileEntries->at(i).path(), fstream::binary);
        if(inputfile.fail()) {
            fileErrorMsg(inputfile.rdstate(), fileEntries->at(i).path());
            msgExit(2);
        }

        uintmax_t remainingBytes = fileEntries->at(i).file_size();
        uintmax_t blockSize = remainingBytes > bufferSize ? bufferSize : remainingBytes;
        cout << "Reading and fusing from file: " << fileEntries->at(i) << '\n';

        //read happens inside of the while cond statement!
        while(inputfile.read(buffer, blockSize) && remainingBytes) {
            //cout << "blockSize: " << blockSize << " | remainingBytes: " << remainingBytes << '\n'; 
            if(inputfile.bad()) {
                fileErrorMsg(inputfile.rdstate(), fileEntries->at(i).path());
                msgExit(2);
            }
            
            outputfile.write(buffer, blockSize);
            if(outputfile.bad()) {
                fileErrorMsg(outputfile.rdstate(), outFilePath);
                msgExit(2);
            }
            remainingBytes -= blockSize;
            blockSize = remainingBytes > bufferSize ? bufferSize : remainingBytes;

            uintmax_t currentReadBytes = fileEntries->at(i).file_size() - remainingBytes;
            double percentProgress = double(currentReadBytes) / double(fileEntries->at(i).file_size());
            printf("Progress: %2.1f%% - (%ju / %ju bytes)\n", percentProgress*100, currentReadBytes, fileEntries->at(i).file_size());
        }
        cout << "...Done!\n\n";

        inputfile.close();
    }

    outputfile.close();
    delete[] buffer;
    cout << "File fusing done!\n";
}


void msgExit(int code) {
    // -1: error in usage
    // 0: success
    // 1: error in dirpath
    // 2: file errors
    cout << "Terminating program...\n*****-******-*****\n";
    exit(code);
}


bool is_numeric_string(string str) {
    auto itr = str.begin();
    while(itr != str.end() && isdigit(*itr))
        ++itr;

    return !str.empty() && itr == str.end();
}

void fileErrorMsg(unsigned int filestate_bits, string filepath) {
    if((filestate_bits & fstream::failbit) != 0)
        cerr << "File Error: Failed to open file \"" << filepath << "\"\n";
    else if((filestate_bits & fstream::badbit) != 0)
        cerr << "File Error: Failed to read/write file \"" << filepath << "\"\n";
    else
        cerr << "File Error: Unknown file error while handling file \"" << filepath << "\"\n";
}