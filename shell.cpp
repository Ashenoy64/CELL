#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem> // for filesystem operations (C++17)

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

using namespace std;
namespace fs = std::filesystem; // alias for filesystem operations
string getParentDirectory(const string &path)
{
    fs::path p(path);
    return p.parent_path().string();
}

vector<string> split(const string &str, char delim = ' ') {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}


string join(const vector<string> &parts, char delim = '/')
{
    if (parts.empty())
        return "";
    string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i)
    {
        result += delim + parts[i];
    }
    return result;
}

void printHistory(const vector<string> &history)
{
    for (const auto &cmd : history)
    {
        cout << cmd << endl;
    }
}


void changeDirectory(const string &newDir, string &currentPath) {
    if (newDir == "..") {
        fs::path parentPath = fs::path(currentPath).parent_path();
        if (!parentPath.empty()) {
            currentPath = parentPath.string();
        } else {
            cerr << "Error: Already at the root directory." << endl;
        }
    } else {
        fs::path absolutePath;
        if (fs::path(newDir).is_absolute()) {
            absolutePath = fs::absolute(newDir);
        } else {
            absolutePath = fs::absolute(fs::path(currentPath) / newDir);
        }

        if (!fs::exists(absolutePath)) {
            cerr << "Error: Directory does not exist." << endl;
        } else if (!fs::is_directory(absolutePath)) {
            cerr << "Error: Not a directory." << endl;
        } else {
            currentPath = absolutePath.string();
        }
    }
}



void touch(string &dir, const string &filename)
{
    string newfile = dir + "/" + filename;
    // if file already exists
    if (fs::exists(newfile))
    {
        cout << "Error: File already exists\n";
        return;
    }
    ofstream file(newfile);
    file.close();
    cout << "File created\n";
}

void copyFile(const string &sourceDir, const string &filename, const string &destDir)
{
    string sourceFile = sourceDir + "/" + filename;
    string destFile = destDir + "/" + filename;

    if (!fs::exists(sourceFile))
    {
        cout << "Error: No such file or directory\n";
        return;
    }

    if (!fs::exists(destDir))
    {
        cout << "Error: Destination directory does not exist\n";
        return;
    }

    fs::copy(sourceFile, destFile);
    cout << "File copied\n";
}

void listDirectoryContents(const string &path)
{
    for (const auto &entry : fs::directory_iterator(path))
    {
        cout << entry.path().filename().string() << endl;
    }
}

int main(int argc, char **argv)
{
    string cmd;
    clearScreen();
    vector<string> history;
    string currentPath = fs::current_path().string(); 

    while (true)
    {
        cout << currentPath << ">";
        getline(cin,cmd);
        vector<string> splitCmd = split(cmd);
        int length = splitCmd.size();


        if (length == 0)
        {
            continue;
        }

        history.push_back(cmd);
        string control = splitCmd[0];

        if (control == "cd")
        {
            
           changeDirectory(splitCmd[1],currentPath);
        }
        else if (control == "ls")
        {
            {
                listDirectoryContents(currentPath);
            }
        }
        else if (control == "cls")
        {
            clearScreen();
        }
        else if (control == "touch")
        {
            if (length > 1)
            {
                touch(currentPath, splitCmd[1]);
            }
            else
            {
                cerr << "Error: No filename specified." << endl;
            }
        }
        else if (control == "cp")
        {
            if (length < 3)
            {
                cerr << "Error: Insufficient arguments. Usage: cp <source> <destination>" << endl;
            }
            else
            {
                copyFile(currentPath, splitCmd[1], splitCmd[2]);
            }
        }
        else if (control == "hist")
        {
            printHistory(history);
        }
        else if (control == "clrhist")
        {
            history.clear();
        }
        else if (control == "exit")
        {
            exit(0);
        }
        else
        {
            string command_string = "sh -c ";

            int returnCode = system(cmd.c_str());

            // Check if the command was executed successfully
            if (returnCode == 0)
            {
                std::cout << "Command executed successfully." << std::endl;
            }
            else
            {
                std::cout << "Command execution failed or returned non-zero: " << returnCode << std::endl;
            }
        }
    }
    return 0;
}
