#include <iostream>
#include <list>
#include <vector>
#include <ctime>
#include <queue>
#include <fstream>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <stack>
#include <deque>
using namespace std;
void mainUser();
list<string> files;
void gotoRowColomn(int rpos, int cpos)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = cpos;
    scrn.Y = rpos;
    SetConsoleCursorPosition(hOuput, scrn);
}
enum class NodeType
{
    File,
    Directory
};
void timerr(time_t given_time)
{
    time_t time = static_cast<time_t>(given_time);
    tm *timeinfo = localtime(&time);
    string time_str = to_string(timeinfo->tm_hour) + ":" +
                      to_string(timeinfo->tm_min) + ":" +
                      to_string(timeinfo->tm_sec);
    cout << time_str << endl;
}
void timer(time_t given_time, int &hours, int &minutes, int &seconds)
{
    time_t currentTime = time(nullptr);
    double timePassed = difftime(currentTime, given_time);
    hours = static_cast<int>(timePassed) / 3600;
    minutes = static_cast<int>(timePassed) % 3600 / 60;
    seconds = static_cast<int>(timePassed) % 60;
}
class FileSystemEntity
{
public:
    string name;
    string path;
    time_t creation;
    time_t pushtime;
    FileSystemEntity(string n, string p)
        : name(n), path(p), creation(time(nullptr)) {}

    virtual void displayDetails() const
    {
        cout << path << name;
        timerr(creation);
        cout << endl;
    }
};
class File : public FileSystemEntity
{
public:
    string fileType;
    time_t creation;
    list<FileSystemEntity *> contents;
    File(string n, string p, string ft)
        : FileSystemEntity(n, p), creation(time(nullptr)), fileType(ft) {}

    void displayDetails() const override
    {
        cout << path << name << fileType << "     ";
        timerr(creation);
        cout << endl;
    }
};

class Directory : public FileSystemEntity
{
public:
    Directory *root;
    time_t creation;
    list<FileSystemEntity *> contents;
    Directory(string n, string p, Directory *par)
        : FileSystemEntity(n, p), creation(time(nullptr)), root(par) {}

    void displayDetails() const override
    {
        cout << name << path << "   ";
        timerr(creation);
        cout << endl;
    }
};
class FileSystem
{
public:
    class TreeNode
    {
    public:
        FileSystemEntity *data;
        NodeType type;
        TreeNode *left;
        TreeNode *right;

        TreeNode(FileSystemEntity *entity, NodeType t)
            : data(entity), type(t), left(nullptr), right(nullptr) {}
    };
    list<FileSystemEntity *> contents;
    priority_queue<File *> pq;
    queue<File *> qu;
    TreeNode *root;
    Directory *currentDirectory;
    FileSystem()
    {
        Directory *rootDirectory = new Directory("V", "V:/", nullptr);
        root = new TreeNode(rootDirectory, NodeType::Directory);
        currentDirectory = rootDirectory;
        contents.push_back(currentDirectory);
    }
    string SetPath(Directory *d)
    {
        string path = d->name;
        Directory *parent = d->root;

        while (parent != nullptr)
        {
            path = parent->name + '/' + path;
            parent = parent->root;
        }
        d->path = path;
        return path;
    }
    void print(string prompt = "/>")
    {
        if (currentDirectory->name == "V")
        {
            cout << "V:" << prompt;
        }
        else
        {
            cout << "V:" << '/' << currentDirectory->path.substr(2, currentDirectory->path.length()) << prompt;
        }
    }
    TreeNode *getRoot()
    {
        return root;
    }

    void insert(TreeNode *&node, FileSystemEntity *entity, NodeType type)
    {
        if (node == nullptr)
        {
            node = new TreeNode(entity, type);
            contents.push_back(entity);
        }
        else if (entity->name < node->data->name)
        {
            insert(node->left, entity, type);
            contents.push_back(entity);
        }
        else
        {
            insert(node->right, entity, type);
            contents.push_back(entity);
        }
    }
    void displayNodesFiles(TreeNode *node)
    {
        if (node == nullptr)
        {
            return;
        }

        displayNodesFiles(node->left);
        if (node->type == NodeType::File)
        {
            File *fileNode = dynamic_cast<File *>(node->data);
            if (fileNode != nullptr)
            {
                cout << "A\t\t\t";
                cout << fileNode->path << fileNode->name << fileNode->fileType << endl;
            }
        }
        displayNodesFiles(node->right);
    }

    void displayAllFiles()
    {
        displayNodesFiles(root);
    }
    File *createFile(const string &name, const string &directoryName, const string &filetype, const time_t &create)
    {
        Directory *directory = findDirectoryByName(directoryName);
        if (directory != nullptr)
        {
            if (findFileByName(name, filetype) == nullptr)
            {

                File *newFile = new File(name, directory->path, filetype);
                directory->contents.push_back(newFile);
                contents.push_back(newFile);
                insert(root, newFile, NodeType::File);
                cout << "File created successfully." << endl;
                return newFile;
            }
            else
            {
                cout << "File already exist";
                return nullptr;
            }
        }
        else
        {
            cout << "Directory not found." << endl;
            return nullptr;
        }
    }
    Directory *createDirectory(const string &name, const string &path, const time_t &create)
    {
        Directory *parentDirectory = currentDirectory;
        if (parentDirectory != nullptr)
        {
            if (findDirectoryByName(name) == nullptr)
            {

                Directory *newDirectory = new Directory(name, currentDirectory->path + name, parentDirectory);
                parentDirectory->contents.push_back(newDirectory);
                contents.push_back(newDirectory);
                insert(root, newDirectory, NodeType::Directory);
                cout << "Parent directory  found." << endl;
                return newDirectory;
            }
            else
            {
                cout << "Directory already   found." << endl;
                return nullptr;
            }
        }
        else
        {
            cout << "Parent directory not found." << endl;
            return nullptr;
        }
    }
    Directory *findDirectoryByName(const string &name)
    {
        for (FileSystemEntity *entity : contents)
        {
            Directory *dir = dynamic_cast<Directory *>(entity);
            if (dir != nullptr && dir->name == name)
            {
                return dir;
            }
        }
        return nullptr;
    }
    File *findFileByName(const string &name, const string &type)
    {
        for (FileSystemEntity *entity : currentDirectory->contents)
        {
            File *file = dynamic_cast<File *>(entity);
            if (file != nullptr && file->name == name && file->fileType == type)
            {
                return file;
            }
        }
        return nullptr;
    }
    void changeDirectory(const string &directoryName)
    {
        Directory *foundDir = findDirectoryByName(directoryName);
        if (foundDir != nullptr)
        {
            currentDirectory = foundDir;
            string path = SetPath(currentDirectory);
        }
        else
        {
            cout << "Directory not found." << endl;
        }
    }
    void current()
    {
        print();
    }
    void previousDirectory()
    {
        if (currentDirectory->root != nullptr)
        {
            currentDirectory = currentDirectory->root;
        }
        else
        {
            cout << "No previous directory available." << endl;
        }
    }
    void rootDirectories()
    {
        currentDirectory = dynamic_cast<Directory *>(root->data);
    }
    void convertFileType(const string &fromType, const string &toType)
    {
        for (FileSystemEntity *entity : contents)
        {
            File *file = dynamic_cast<File *>(entity);
            if (file != nullptr && file->fileType == fromType)
            {
                file->fileType = toType;
            }
        }
        cout << "Converted file type from " << fromType << " to " << toType << endl;
    }
    File *findFileInDirectory(Directory *destDir, const string &name, const string &filetype)
    {
        for (FileSystemEntity *entity : destDir->contents)
        {
            File *file = dynamic_cast<File *>(entity);
            if (file != nullptr && file->name == name && file->fileType == filetype)
            {
                return file;
            }
        }
        return nullptr;
    }
    void copyFile(string &fileName, string &destDirName, string &filetype)
    {
        Directory *destDir = findDirectoryByName(destDirName);
        if (destDir == nullptr)
        {
            cout << "Destination directory not found." << endl;
            return;
        }
        File *sourceFile = findFileByName(fileName, filetype);
        if (sourceFile == nullptr)
        {
            cout << "Source file not found." << endl;
            return;
        }
        File *existingFileInDest = findFileInDirectory(destDir, fileName, filetype);
        if (existingFileInDest != nullptr)
        {
            cout << "File with the same name already exists in the destination directory." << endl;
            return;
        }
        File *copiedFile = new File(sourceFile->name, destDir->path + "/" + sourceFile->name, sourceFile->fileType);
        destDir->contents.push_back(copiedFile);
        cout << " 1 file(s) copied. " << endl;
    }

    void deleteFile(const string &name, const string &directoryName, const string &filetype)
    {
        Directory *directory = findDirectoryByName(directoryName);

        if (directory != nullptr)
        {
            File *fileToDelete = findFileByName(name, filetype);

            if (fileToDelete != nullptr)
            {
                if (fileToDelete->fileType == filetype)
                {

                    directory->contents.remove(fileToDelete);
                    root = removeNode(root, fileToDelete);

                    delete fileToDelete;
                    cout << "File '" << name << "' deleted successfully." << endl;
                }
                else
                {
                    cout << "File type mismatch for '" << name << "'." << endl;
                }
            }
            else
            {
                cout << "File '" << name << "' not found." << endl;
            }
        }
        else
        {
            cout << "Directory not found." << endl;
        }
    }

    TreeNode *removeNode(TreeNode *node, File *fileToDelete)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (fileToDelete->name < node->data->name)
        {
            node->left = removeNode(node->left, fileToDelete);
        }
        else if (fileToDelete->name > node->data->name)
        {
            node->right = removeNode(node->right, fileToDelete);
        }
        else
        {
            if (node->type == NodeType::File && node->data == fileToDelete)
            {
                Directory *dir = dynamic_cast<Directory *>(node->data);
                if (dir != nullptr)
                {
                    dir->contents.remove(fileToDelete);
                }
                delete fileToDelete;
                cout << "File '" << fileToDelete->name << "' deleted successfully." << endl;
                return nullptr;
            }
            node->left = removeNode(node->left, fileToDelete);
            node->right = removeNode(node->right, fileToDelete);
        }
        return node;
    }
    void deleteNode(TreeNode *&node, FileSystemEntity *entity)
    {
        if (node == nullptr)
        {
            return;
        }

        if (entity->name < node->data->name)
        {
            deleteNode(node->left, entity);
        }
        else if (entity->name > node->data->name)
        {
            deleteNode(node->right, entity);
        }
        else
        {
            if (node->type == NodeType::File)
            {
                File *file = dynamic_cast<File *>(entity);
                File *fileNode = dynamic_cast<File *>(node->data);
                if (file != nullptr && fileNode != nullptr && file->name == fileNode->name && file->fileType == fileNode->fileType)
                {
                    node = removeFromTree(node);
                }
            }
            else if (node->type == NodeType::Directory)
            {
                Directory *directory = dynamic_cast<Directory *>(entity);
                Directory *dirNode = dynamic_cast<Directory *>(node->data);
                if (directory != nullptr && dirNode != nullptr && directory->name == dirNode->name)
                {
                    node = removeFromTree(node);
                }
            }
        }
    }

    TreeNode *removeFromTree(TreeNode *node)
    {
        if (node->left == nullptr)
        {
            TreeNode *rightChild = node->right;
            delete node->data;
            delete node;
            return rightChild;
        }
        else if (node->right == nullptr)
        {
            TreeNode *leftChild = node->left;
            delete node->data;
            delete node;
            return leftChild;
        }
        else
        {
            TreeNode *successorParent = node;
            TreeNode *successor = node->right;
            while (successor->left != nullptr)
            {
                successorParent = successor;
                successor = successor->left;
            }

            if (successorParent != node)
            {
                successorParent->left = successor->right;
            }
            else
            {
                successorParent->right = successor->right;
            }

            node->data->name = successor->data->name;
            delete successor->data;
            delete successor;
            return node;
        }
    }
    void filesInDirectory(string dirName)
    {
        Directory *directory = findDirectoryByName(dirName);
        if (directory == nullptr)
        {
            cout << "Directory not found." << endl;
            return;
        }
        if (!directory->contents.empty())
        {
            for (FileSystemEntity *entity : directory->contents)
            {
                File *file = dynamic_cast<File *>(entity);
                if (file != nullptr)
                {
                    cout << "\t\t\t\t" << file->name << file->fileType << "    ";
                    timerr(file->creation);
                    cout << endl;
                }
                else
                {
                    cout << "No file found" << endl;
                }
                Directory *directory = dynamic_cast<Directory *>(entity);
                if (directory != nullptr)
                {
                    cout << "\t\t <DIR> \t\t" << directory->name << "    ";
                    timerr(directory->creation);
                    cout << endl;
                }
            }
        }
        else
        {
            cout << "No data in directory." << endl;
        }
    }
    void attrib()
    {
        Directory *directory = currentDirectory;
        if (directory == nullptr)
        {
            cout << "Directory not found." << endl;
            return;
        }
        if (!directory->contents.empty())
        {
            for (FileSystemEntity *entity : directory->contents)
            {
                File *file = dynamic_cast<File *>(entity);
                if (file != nullptr)
                {

                    cout << "A\t\t\t";
                    cout << file->path << file->name << file->fileType << endl;
                }
            }
        }
        else
        {
            cout << "No file found" << endl;
        }
    }
    void moveFile(const string &fileName, const string &destDirName, const string &fileType)
    {
        Directory *destDir = findDirectoryByName(destDirName);
        if (destDir == nullptr)
        {
            cout << "Destination directory not found." << endl;
            return;
        }
        File *sourceFile = findFileByName(fileName, fileType);
        if (sourceFile == nullptr)
        {
            cout << "Source file not found." << endl;
            return;
        }
        File *existingFileInDest = findFileInDirectory(destDir, fileName, fileType);
        if (existingFileInDest != nullptr)
        {
            cout << "File with the same name already exists in the destination directory." << endl;
            return;
        }
        currentDirectory->contents.remove(sourceFile);
        sourceFile->path = destDir->path + '/';
        destDir->contents.push_back(sourceFile);

        cout << "File moved successfully." << endl;
    }
    void printTree(TreeNode *node, int depth)
    {
        if (node == nullptr)
        {
            return;
        }
        else
        {
            if (node->type == NodeType::Directory)
            {
                Directory *directory = dynamic_cast<Directory *>(node->data);
                cout << "+ " << directory->name << endl;
                for (FileSystemEntity *entity : directory->contents)
                {
                    File *file = dynamic_cast<File *>(entity);
                    if (file != nullptr)
                    {
                        cout << "- " << file->name << file->fileType << endl;
                    }
                }
            }
        }
        printTree(node->left, depth + 1);
        printTree(node->right, depth + 1);
    }

    void printFileSystemStructure()
    {
        cout << "File System Structure:" << endl;
        printTree(root, 0);
    }
    void printing(string &name, string &filetype)
    {
        File *file = findFileInDirectory(currentDirectory, name, filetype);
        if (file != nullptr)
        {
            file->pushtime = time(nullptr);
            pq.push(file);
            cout << "File pushed to priority queue" << endl;
        }
        else
        {
            cout << "File not found" << endl;
        }
    }
    void printTimeQueue()
    {
        cout << "Priority queue" << endl;
        priority_queue<File *> tempQueue = pq;

        while (!tempQueue.empty())
        {
            File *file = tempQueue.top();
            cout << file->name << file->fileType << "    ";
            timerr(file->pushtime);
            cout << endl;

            int hours, minutes, secs;
            timer(file->pushtime, hours, minutes, secs);
            cout << "TimeLeft: " << hours << "hours:" << minutes << " minutes:" << secs << "seconds" << endl;

            tempQueue.pop();
        }
    }
    void convertTime(double seconds, int &hours, int &minutes, int &secs)
    {
        hours = seconds / 3600;
        minutes = (seconds - (hours * 3600)) / 60;
        secs = seconds - (hours * 3600) - (minutes * 60);
    }
    void printQueue()
    {
        cout << "Priority queue" << endl;
        priority_queue<File *> tempQueue = pq;

        while (!tempQueue.empty())
        {
            cout << tempQueue.top()->name << tempQueue.top()->fileType << "     ";
            timerr(tempQueue.top()->pushtime);
            cout << endl;
            tempQueue.pop();
        }
    }
    void emptyDirectory()
    {
        Directory *dir = currentDirectory;

        for (FileSystemEntity *entity : dir->contents)
        {
            File *file = dynamic_cast<File *>(entity);
            if (file != nullptr)
            {
                deleteNode(root, file);
                contents.remove(file);
                delete file;
                cout << "File deleted successfully" << endl;
            }
            else
            {
                cout << "No file found" << endl;
            }
            Directory *dir = dynamic_cast<Directory *>(entity);
            if (dir != nullptr)
            {
                deleteNode(root, dir);
                contents.remove(dir);
                delete dir;
                cout << "Directory deleted successfully" << endl;
            }
            else
            {
                cout << "No Directory found" << endl;
            }
        }
    }
    void printQ(string &name, string &filetype)
    {
        File *file = findFileInDirectory(currentDirectory, name, filetype);
        if (file != nullptr)
        {
            file->pushtime = time(nullptr);
            qu.push(file);
            cout << "File pushed to queue" << endl;
        }
        else
        {
            cout << "File not found" << endl;
        }
    }
    void QueueOfPrintingFile()
    {
        cout << "Queue" << endl;
        queue<File *> tempQueue = qu;

        while (!tempQueue.empty())
        {
            cout << tempQueue.front()->name << tempQueue.front()->fileType << "    ";
            timerr(tempQueue.front()->pushtime);
            cout << endl;
            tempQueue.pop();
        }
    }

    void printTime()
    {
        cout << "Priority queue" << endl;
        queue<File *> tempQueue = qu;

        while (!tempQueue.empty())
        {
            File *file = tempQueue.front();
            cout << file->name << file->fileType << "       ";
            timerr(file->pushtime);
            cout << endl;
            int hours, minutes, secs;
            timer(file->pushtime, hours, minutes, secs);
            cout << "TimeLeft: " << hours << "hours:" << minutes << " minutes:" << secs << "seconds" << endl;

            tempQueue.pop();
        }
    }
    void printWorkingDirectory()
    {
        for (FileSystemEntity *entity : currentDirectory->contents)
        {
            Directory *dirEntity = dynamic_cast<Directory *>(entity);
            if (dirEntity != nullptr)
            {
                cout << "\t\t\t"
                     << " <Directory>" << dirEntity->name << endl;
            }
            else
            {
                File *fileEntity = dynamic_cast<File *>(entity);
                if (fileEntity != nullptr)
                {
                    cout << "\t\t\t" << fileEntity->name << fileEntity->fileType << endl;
                }
            }
        }
    }
    void renameFile(string &name, string &filetype, string &newname, string &filetype2)
    {
        File *file = findFileInDirectory(currentDirectory, name, filetype);
        if (file != nullptr)
        {
            file->name = newname;
            file->fileType = filetype2;
            file->path = currentDirectory->path + '/';
            file->displayDetails();
        }
        else
        {
            cout << "No file found" << endl;
        }
    }
    bool removesDirectory(string &name)
    {
        Directory *dir = findDirectoryByName(name);
        if (dir != nullptr)
        {
            deleteNode(root, dir);
            contents.remove(dir);
            if (dir == currentDirectory)
            {
                previousDirectory();
            }
            if (name == "V")
            {
                cout << "Root directory deleted" << endl;
                return false;
            }
            delete dir;
            cout << "Directory deleted successfully" << endl;
        }
        else
        {
            cout << "No Directory found" << endl;
        }
        return true;
    }
    Directory *findDirectoryByPath(const string &path)
    {
        for (FileSystemEntity *entity : contents)
        {
            Directory *dir = dynamic_cast<Directory *>(entity);
            if (dir != nullptr && dir->path == path)
            {
                return dir;
            }
        }
        return nullptr;
    }
    void t(time_t given_time, ofstream &outFile)
    {
        time_t time = static_cast<time_t>(given_time);
        tm *timeinfo = localtime(&time);
        string time_str = to_string(timeinfo->tm_hour) + ":" +
                          to_string(timeinfo->tm_min) + ":" +
                          to_string(timeinfo->tm_sec);
        outFile << time_str << endl;
    }
    void FileWrite(TreeNode *node, ofstream &outFile, int depth)
    {
        if (node == nullptr)
        {
            return;
        }
        if (node->type == NodeType::Directory)
        {
            Directory *directory = dynamic_cast<Directory *>(node->data);

            outFile << "Directory," << directory->name << "," << directory->path << ",";
            t(directory->creation, outFile);
            for (FileSystemEntity *entity : directory->contents)
            {
                File *file = dynamic_cast<File *>(entity);
                if (file != nullptr)
                {
                    outFile << "File," << file->name << "," << file->path << "," << file->fileType << ",";
                    t(file->creation, outFile);
                }
            }
        }
        FileWrite(node->left, outFile, depth + 1);
        FileWrite(node->right, outFile, depth + 1);
    }
    void writeFileSystemStructureToFile(FileSystem &fs, const string &filename)
    {
        ofstream outFile(filename, ios::out); // Open in truncation mode

        if (!outFile.is_open())
        {
            cout << "Error opening file for writing." << endl;
            return;
        }

        FileWrite(fs.root, outFile, 0);
        outFile.close();
    }

    string extractDirectoryName(const string &filePath)
    {
        size_t found = filePath.find_last_of('/');
        if (found != string::npos)
        {
            string directoryName = filePath.substr(found + 1);
            return directoryName;
        }
        return "";
    }
    void loadFileSystemStructureFromFile(const string &filename)
    {
        ifstream inFile(filename);
        if (!inFile.is_open())
        {
            cout << "Error opening file for reading." << endl;
            return;
        }

        string line;
        while (getline(inFile, line))
        {
            istringstream iss(line);
            string item;
            vector<string> tokens;

            while (getline(iss, item, ','))
            {
                tokens.push_back(item);
            }

            if (tokens.size() == 4 || tokens.size() == 5)
            {
                try
                {
                    string type = tokens[0];
                    string name = tokens[1];
                    string path = tokens[2];

                    string timeStr = tokens[tokens.size() - 1];
                    time_t creationTime = stoi(timeStr);

                    if (type == "Directory")
                    {
                        createDirectory(name, path, creationTime);
                    }
                    else if (type == "File")
                    {
                        string fileType = tokens[3];

                        string directoryName = extractDirectoryName(path);
                        Directory *directory = findDirectoryByName(directoryName);
                        if (directory != nullptr)
                        {
                            createFile(name, directoryName, fileType, creationTime);
                        }
                        else
                        {
                            cout << "Directory not found for file: " << name << endl;
                        }
                    }
                }
                catch (const invalid_argument &e)
                {
                    cout << "Invalid data format: " << line << endl;
                }
            }
            else
            {
                cout << "Invalid data format: " << line << endl;
            }
        }

        inFile.close();
        cout << "File System Structure loaded from file successfully." << endl;
    }
    void searchInFiles(const string &searchString)
    {
        for (FileSystemEntity *entity : currentDirectory->contents)
        {
            File *fileEntity = dynamic_cast<File *>(entity);
            if (fileEntity != nullptr)
            {
                string f = fileEntity->name + fileEntity->fileType;
                ifstream file(f);
                string line;

                if (file.is_open())
                {
                    while (getline(file, line))
                    {
                        if (line.find(searchString) != string::npos)
                        {
                            cout << f << endl;
                        }
                        else
                        {
                            cout << "No files found containing the string." << endl;
                        }
                    }
                    file.close();
                }
            }
        }
    }
};

struct state
{
    list<list<char>> text;
    list<list<char>>::iterator rowItr;
    list<char>::iterator colItr;
    int row, col;
};
class textEditor
{
public:
    int currentRow, currentCol; // current indices
    list<list<char>> text;
    list<list<char>>::iterator rowItr;
    list<char>::iterator colItr;
    deque<state> undo;
    stack<state> redo;
    FileSystem fs;
    textEditor()
    {
        text.push_back(list<char>());
        rowItr = text.begin();
        (*rowItr).push_back('\n');
        colItr = (*rowItr).begin();
        currentRow = 0;
        currentCol = 0;
    }
    state getState()
    {
        state *s = new state;
        s->text.push_back(list<char>());
        auto r_itr = s->text.begin(); // iterator of state s
        s->row = currentRow;
        s->col = currentCol;
        s->colItr = colItr;
        s->rowItr = rowItr;
        // Iterate through each row in the currentFile's text(deep copy)
        for (auto row = text.begin(); row != text.end(); r_itr++, ++row)
        {
            // Create a new row in the state's text
            s->text.push_back(list<char>());

            // Iterate through each character in the row
            for (auto col = (*row).begin(); col != (*row).end(); ++col)
            {
                // Add a new character to the state's row
                char ch = *col;
                (*r_itr).push_back(*col);
            }
        }

        // Find the row in the state's text that corresponds to the currentRow
        s->rowItr = s->text.begin();
        advance(s->rowItr, currentRow); // loop

        // Find the column in the state's row that corresponds to the currentCol
        s->colItr = (*s->rowItr).begin();
        advance(s->colItr, currentCol); // loop

        // Set the row and column indices in the state

        return *s;
    }
    void loadState(state s)
    {
        text = s.text;
        rowItr = text.begin();
        currentRow = s.row;
        currentCol = s.col;
        advance(rowItr, currentRow);
        colItr = (*rowItr).begin();
        advance(colItr, currentCol);
    }
    void debug()
    {
        cout << "currentRow: " << currentRow << endl;
        cout << "currentCol: " << currentCol << endl;
        cout << "colItr: " << *colItr << endl;
        cout << "rowItr: " << (*rowItr).size() << endl;
        for (auto itr = text.begin(); itr != text.end(); itr++)
        {
            for (auto itr2 = (*itr).begin(); itr2 != (*itr).end(); itr2++)
            {
                cout << *itr2;
            }
            cout << endl;
        }
        getch();
    }
    void undoOperation()
    {
        if (undo.empty())
        {
            // cout << "No undo operation available" << endl;
            return;
        }
        redo.push(getState());
        state s = undo.back();
        undo.pop_back();
        loadState(s);
    }
    void redoOperation()
    {
        if (redo.empty())
        {
            // cout << "No redo operation available" << endl;
            return;
        }
        updateUndo();
        state s = redo.top();
        redo.pop();
        loadState(s);
    }
    void updateUndo()
    {
        if (undo.size() > 5)
        {
            undo.erase(undo.begin());
        }
        state s = getState();
        undo.push_back(s);
    }
    // delete key
    void deleteOperation()
    {
        updateUndo();
        if (currentCol == (*rowItr).size() - 1)
        {
            if (currentRow == text.size() - 1) // rowItr == text.end() not working
            {
                // cout << "No character to delete" << endl;
                return;
            }
            else
            {
                (*rowItr).pop_back(); // removes last endline character
                auto nextRowItr = next(rowItr);
                (*rowItr).splice((*rowItr).end(), *nextRowItr);
                text.erase(nextRowItr);
            }
        }
        else
        {
            (*rowItr).erase(colItr);
            colItr = (*rowItr).begin();
            advance(colItr, currentCol);
        }
        gotoRowColomn(currentRow, currentCol);
    }
    // left arrow key
    void leftOperation()
    {

        if (colItr == (*rowItr).begin())
        {
            if (rowItr == text.begin())
            {
                // cout << "No character to move left" << endl;
                return;
            }
            else
            {
                rowItr--;
                colItr = (*rowItr).begin();
                currentRow--;
                advance(colItr, (*rowItr).size() - 1);
                currentCol = (*rowItr).size() - 1;
            }
        }
        else
        {
            colItr--;
            currentCol--;
        }
        gotoRowColomn(currentRow, currentCol);
    }
    // right arrow key
    void rightOperation()
    {
        colItr++;
        if (colItr == (*rowItr).end())
        {
            if (currentRow == text.size() - 1) // rowItr==text.end() not working
            {
                // cout << "No character to move right" << endl;
                colItr--;
                return;
            }
            else
            {
                rowItr++;
                colItr = (*rowItr).begin();
                currentRow++;
                currentCol = 0;
            }
        }
        else
        {
            // colItr++;done it at start
            currentCol++;
        }
        gotoRowColomn(currentRow, currentCol);
    }
    // up arrow key
    void upOperation()
    {

        if (rowItr == text.begin())
        {
            // cout<<"No character to move up";
            return;
        }
        rowItr--;
        if (currentCol > (*rowItr).size() - 1)
        {
            currentCol = (*rowItr).size() - 1;
            advance(colItr, currentCol);
        }
        else if (currentCol < (*rowItr).size())
        {
            colItr = (*rowItr).begin();
            advance(colItr, currentCol);
        }
        currentRow--;
        gotoRowColomn(currentRow, currentCol);
    }
    // down key
    void downOperation()
    {

        if (currentRow >= text.size() - 1) // already at the last row
        {
            // cout<<"No character to move down";
            return;
        }

        rowItr++;
        if (currentCol > (*rowItr).size() - 1)
        {
            currentCol = (*rowItr).size() - 1;
            colItr = (*rowItr).begin();
            advance(colItr, currentCol);
        }
        else if (currentCol < (*rowItr).size())
        {
            colItr = (*rowItr).begin();
            advance(colItr, currentCol);
        }
        currentRow++;
        gotoRowColomn(currentRow, currentCol);
    }
    // enter key
    void newLineOperation()
    {

        auto nextRowItr = next(rowItr);
        list<char> newRow(colItr, (*rowItr).end());
        (*rowItr).erase(colItr, (*rowItr).end());
        (*rowItr).push_back('\n');
        text.insert(nextRowItr, newRow);

        // Update the iterators to point to the beginning of the next line

        currentRow++;
        rowItr = text.begin();
        advance(rowItr, currentRow);

        currentCol = 0;
        gotoRowColomn(currentRow, currentCol);
    }
    // backspace key
    void backSpaceOperation()
    {
        updateUndo();

        if (colItr == (*rowItr).begin())
        {
            if (rowItr != text.begin())
            {
                auto prevRowItr = prev(rowItr);
                (*prevRowItr).pop_back(); // remove the end line character
                // Move the cursor to the end of the previous line
                colItr = (*prevRowItr).begin();
                currentCol = (*prevRowItr).size();

                // Merge the current line into the previous line
                (*prevRowItr).splice((*prevRowItr).end(), *rowItr);

                // Erase the current line
                text.erase(rowItr);

                // Update iterators and counters
                advance(colItr, currentCol);
                rowItr = prevRowItr;
                currentRow--;
            }
        }
        else
        {
            colItr--;
            colItr = (*rowItr).erase(colItr);
            currentCol--;
        }
    }
    // main input function
    void input(string fileName, string fileType)
    {
        char ch;
        while (true)
        {
            printText();
            ch = _getch();
            if (ch == 26) // ctrl+z
            {
                undoOperation();
            }
            else if (ch == 25) // ctrl+y
            {
                redoOperation();
            }
            if (ch == 19) // ctrl+s
            {
                save(fileName, fileType);
            }
            else if (ch == 13) // enter key
            {
                newLineOperation();
            }
            else if (ch == 8) // backspace
            {
                backSpaceOperation();
            }
            if (ch == 27) // escape
            {
                break;

                fs.print();
            }
            else if (ch == -32)
            {
                ch = _getch();
                if (ch == 75) // left arrow
                {
                    leftOperation();
                }
                else if (ch == 77) // right arrow
                {
                    rightOperation();
                }
                else if (ch == 72) // up arrow
                {
                    upOperation();
                }
                else if (ch == 80) // down arrow
                {
                    downOperation();
                }
                else if (ch == 83) // delete key
                {
                    deleteOperation();
                }
            }
            else
            {
                insertOperation(ch);
            }
        }
    }
    // insert in notepad
    void insertOperation(char ch)
    {
        if (isValidInput(ch))
        {
            updateUndo();
            size_t offset = distance((*rowItr).begin(), colItr);

            (*rowItr).insert(colItr, ch);
            colItr = (*rowItr).begin();
            colItr = next(colItr, offset); // points to the same character that is inserted
            colItr++;
            currentCol++;
        }
    }
    void save(string filename, string fileType)
    {
        ofstream file;
        string ff = filename + fileType;
        file.open(ff);
        for (auto itr = text.begin(); itr != text.end(); itr++)
        {
            for (auto itr2 = (*itr).begin(); itr2 != (*itr).end(); itr2++)
            {
                if (*itr2 != '\n')
                    file << *itr2;
            }
            // Check if it's the last row before adding a newline
            if (next(itr) != text.end())
            {
                file << '\n';
            }
        }
        file.close();
    }
    void load(string filename, string fileType)
    {
        ifstream reader;
        string line;
        string f = filename + fileType;
        reader.open(f);
        rowItr = text.begin();
        (*rowItr).pop_back(); // remove the end line character
        colItr = (*rowItr).begin();
        while (getline(reader, line))
        {
            // insert char by char in row iterator
            for (size_t ch = 0; ch < line.size(); ch++)
            {
                (*rowItr).insert(colItr, line[ch]);
            }

            (*rowItr).insert(colItr, '\n');                 // last character must be end line character
            text.push_back(list<char>());                   // create new row
            size_t offset = distance(text.begin(), rowItr); // current row iterator
            rowItr = text.begin();                          // to avoid any error
            advance(rowItr, offset + 1);                    // next row of current row
            colItr = (*rowItr).begin();
        }
        text.pop_back(); // last row is empty
        reader.close();
        updateItrToLast();
    }
    void updateItrToLast()
    {
        rowItr = text.begin();
        currentRow = text.size() - 1;
        advance(rowItr, currentRow);
        colItr = (*rowItr).begin();
        advance(colItr, (*rowItr).size() - 1); // pointing to the end line character '\n'
        currentCol = (*rowItr).size() - 1;
    }
    void printText()
    {
        system("cls");
        for (auto itr = text.begin(); itr != text.end(); itr++)
        {
            for (auto itr2 = (*itr).begin(); *itr2 != '\n'; itr2++)
            {
                cout << *itr2;
            }
            cout << '\n';
        }
        gotoRowColomn(currentRow, currentCol);
    }
    void printRow(list<list<char>>::iterator rowItr)
    {
        gotoRowColomn(currentRow, 0);
        for (auto colItr = rowItr->begin(); colItr != rowItr->end(); colItr++)
        {
            cout << *colItr;
        }
    }
    bool isValidInput(char ch)
    {
        return isupper(ch) || islower(ch) || isdigit(ch) || isspace(ch) || isSpecialCharacter(ch);
    }
    bool isSpecialCharacter(char ch)
    {
        const char specialChars[] = "`~!@#$%^&*()_+=-|}{\":?<>[]/';/.,\"";

        for (char specialChar : specialChars)
        {
            if (ch == specialChar)
            {
                return true;
            }
        }
        return false;
    }
};
void mainUser()
{

    string name = "Saba Shahdin";
    string number = "2022-CS-112";
    string subject = "Data Structure and Algorithm";
    cout << "Version : [1.0.0.0]" << endl;
    cout << name << endl;
    cout << number << endl;
    cout << subject << endl;
}
string toLower(string op)
{
    for (int i = 0; i < op.size(); i++)
    {
        op[i] = tolower(op[i]);
    }
    return op;
}

bool fileExist(string fileName, string fileType)
{
    string f = fileName + fileType;
    for (string file : files)
    {
        if (file == f)
        {
            return true;
        }
    }
    return false;
}
void readFiles(string fileName = "files", string fileType = ".txt")
{
    ifstream file;
    string f = fileName + fileType;
    file.open(f);
    string line;
    while (getline(file, line))
    {
        files.push_back(line);
    }
    file.close();
}
void writeFiles(string newFile, string type, string fileName = "files", string fileType = ".txt")
{
    ofstream file;
    string f = fileName + fileType;
    file.open(f, ios::app);
    file << newFile << type << endl;
    file.close();
}
void printBox(int x, int y, int gotox, int gotoy)
{
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            gotoRowColomn(gotox + j, gotoy + i);
            if (i == 0 || i == y - 1 || j == 0 || j == x - 1)
            {
                cout << "#";
            }
            else
            {
                cout << " ";
            }
        }
    }
}
string getFileName()
{
    printBox(5, 50, 10, 10);
    {
        gotoRowColomn(12, 12);
        cout << "Enter File Name ";
        string fileName;
        string fileType = "";
        cin >> fileName;
        size_t dotPosition = fileName.find_last_of('.');
        if (dotPosition != string::npos)
        {
            fileType = fileName.substr(dotPosition);
            fileName = fileName.substr(0, dotPosition);

            return fileName;
        }
    }
}
void del(string fileName, string fileType)
{
    string fil = fileName + fileType;
    remove(fil.c_str());
}
void help()
{
    cout << "cd name of directory : change directory  to the name of directory" << endl;
    cout << "attrib : Shows list of files in the current Directory" << endl;
    cout << "cd/ : changes directory to root" << endl;
    cout << "cd. : prints working directory" << endl;
    cout << "cd.. : Change directory to previous" << endl;
    cout << "convert : Changes type from one typr to other" << endl;
    cout << "copy : Copy one file in current directory to other directory" << endl;
    cout << "create : Create a file" << endl;
    cout << "del: Delete a file" << endl;
    cout << "dir : Display a list of files and subdirectories in a directory." << endl;
    cout << "edit : Edit a file" << endl;
    cout << "exit : Exits a file" << endl;
    cout << "find : Find a file" << endl;
    cout << "Load : Load dtaa from file " << endl;
    cout << "mkdir : Makes a directory" << endl;
    cout << "move : Move a file from on directory to other" << endl;
    cout << "pprint : text file to priority based queueu" << endl;
    cout << "prompt :Change prompt" << endl;
    cout << "print : add text file to queue" << endl;
    cout << "pqueue : shows time left for each file to print" << endl;
    cout << "pwd : prints working directory" << endl;
    cout << "queue : shows time left to each file to print" << endl;
    cout << "rename : rename a file whose name is provided" << endl;
    cout << "rmdir : remove a directory whose name is provided" << endl;
    cout << "save : save a file to disk " << endl;
    cout << "tree : display the complete directory" << endl;
    cout << "ver : dispalys version of program" << endl;
}
void makeFile(string fileName, string fileType)
{
    ofstream file;
    string f = fileName + fileType;
    file.open(f);
    file << '\n';
    file.close();
}
vector<string> find(string str, string fileName, string fileType)
{
    vector<string> lines;
    ifstream file;
    string fil = fileName + fileType;
    file.open(fil);
    string line;
    int lineNo = 0;
    while (getline(file, line))
    {
        lineNo++;
        if (line.find(str) != string::npos)
        {
            lines.push_back(line);
        }
    }
    file.close();
    return lines;
}
int main()
{
    system("cls");
    FileSystem fs;
    Directory *t;
    File *f;
    textEditor editor = textEditor();
    // fs.createDirectory("dir1", "V:/", time(nullptr));
    // fs.createFile("file1", "dir1", ".txt", time(nullptr));
    // fs.createDirectory("dir2", "V:/", time(nullptr));
    // fs.createFile("file2", "dir2", ".txt", time(nullptr));
    // fs.writeFileSystemStructureToFile(fs, "tree.txt");
    // if (!fileExist("file1", ".txt"))
    // {
    //     writeFiles("file1", ".txt");
    //     makeFile("file1", ".txt");
    // }
    // if (!fileExist("file2", ".txt"))
    // {
    //     writeFiles("file2", ".txt");
    //     makeFile("file2", ".txt");
    // }
    fs.printFileSystemStructure();
    fs.loadFileSystemStructureFromFile("tree.txt");
    fs.printFileSystemStructure();
    getch();
    system("cls");
    mainUser();
    FileSystem::TreeNode *root = fs.getRoot();
    fs.print();
    string choice;
    getline(cin, choice);
    string opr = choice.substr(0, choice.find(' '));
    opr = toLower(opr);
    bool flag = true;
    while (flag == true)
    {
        opr = choice.substr(0, choice.find(' '));
        opr = toLower(opr);

        if (opr == "attrib")
        {
            fs.attrib();
            fs.print();
        }
        else if (opr == "cd")
        {
            string folder = choice.substr(3, choice.length());
            fs.changeDirectory(folder);
            fs.print();
        }
        else if (opr == "cd.")
        {
            string folder = choice.substr(3, '.');
            fs.current();
        }
        else if (opr == "cd..")
        {
            fs.previousDirectory();
            fs.print();
        }
        else if (opr == "cd/")
        {
            fs.rootDirectories();
            fs.print();
        }
        else if (opr == "convert")
        {
            string fileTypes = choice.substr(8);
            istringstream iss(fileTypes);
            vector<string> tokens;
            string token;
            while (getline(iss, token, ' '))
            {
                tokens.push_back(token);
            }
            if (tokens.size() == 2)
            {
                string filetype1 = tokens[0];
                string filetype2 = tokens[1];
                fs.convertFileType(filetype1, filetype2);
                fs.displayAllFiles();
            }
            else
            {
                cout << "Invalid input format for file types." << endl;
            }

            fs.print();
        }
        else if (opr == "copy")
        {
            istringstream iss(choice);
            vector<string> tokens;
            string token;
            while (getline(iss, token, ' '))
            {
                tokens.push_back(token);
            }

            if (tokens.size() >= 3)
            {
                string fileName = tokens[1];
                string destDirName = tokens[2];
                string fileType = "";
                size_t dotPosition = fileName.find_last_of('.');
                if (dotPosition != string::npos)
                {
                    fileType = fileName.substr(dotPosition);
                    fileName = fileName.substr(0, dotPosition);
                }

                fs.copyFile(fileName, destDirName, fileType);
            }
            else
            {
                cout << "Invalid input for copy operation." << endl;
            }

            fs.print();
        }
        if (opr == "create")
        {
            string fileName = choice.substr(7, choice.length());
            string file = fs.currentDirectory->name;
            string fileType = "";
            size_t dotPosition = fileName.find_last_of('.');
            if (dotPosition != string::npos)
            {
                fileType = fileName.substr(dotPosition);
                fileName = fileName.substr(0, dotPosition);
                if (fileType == ".txt")
                {
                    f = fs.createFile(fileName, file, fileType, time(nullptr));
                    if (!fileExist(fileName, fileType))
                    {
                        writeFiles(fileName, fileType);
                        makeFile(fileName, fileType);
                    }
                    fs.writeFileSystemStructureToFile(fs, "tree.txt");
                }
                else
                {
                    cout << "Wrong output " << endl;
                }
            }
            else
            {
                cout << "Invalid input for create operation." << endl;
            }
            fs.print();
        }
        else if (opr == "del")
        {
            string fileName = choice.substr(4, choice.length());
            string fileType = "";
            size_t dotPosition = fileName.find_last_of('.');
            if (dotPosition != string::npos)
            {
                fileType = fileName.substr(dotPosition);
                fileName = fileName.substr(0, dotPosition);
                fs.deleteFile(fileName, fs.currentDirectory->name, fileType);
                del(fileName , fileType);
                
                if (f == nullptr)
                {
                    cout << "File not found" << endl;
                }
            }
            else
            {
                cout << "Invalid input for delete operation." << endl;
            }
            fs.print();
        }
        else if (opr == "dir")
        {
            fs.filesInDirectory(fs.currentDirectory->name);
            fs.print();
        }
        else if (opr == "exit")
        {
            flag = false;
            cout << "Exit from dos shell";
            break;
        }
        else if (opr == "find")
        {
            string fileName = choice.substr(5, choice.length());
            string fileType = "";
            size_t dotPosition = fileName.find_last_of('.');
            if (dotPosition != string::npos)
            {
                fileType = fileName.substr(dotPosition);
                fileName = fileName.substr(0, dotPosition);
            }
            File *f = fs.findFileInDirectory(fs.currentDirectory, fileName, fileType);
            if (f != nullptr)
            {
                cout << "File found" << endl;
                f->displayDetails();
            }
            else
            {
                cout << "No file found" << endl;
            }
            fs.print();
        }
        else if (opr == "format")
        {
            fs.emptyDirectory();
        }
        else if (opr == "mkdir")
        {
            if (choice.length() > 6)
            {
                string folder = choice.substr(6);
                fs.createDirectory(folder, fs.currentDirectory->path, time(nullptr));
            }
            else
            {
                cout << "Please provide a folder name." << endl;
            }
            fs.print();
        }

        else if (opr == "move")
        {
            istringstream iss(choice);
            vector<string> tokens;
            string token;
            while (getline(iss, token, ' '))
            {
                tokens.push_back(token);
            }
            if (tokens.size() >= 3)
            {
                string fileName = tokens[1];
                string destpath = tokens[2];
                string fileType = "";
                size_t dotPosition = fileName.find_last_of('.');
                if (dotPosition != string::npos)
                {
                    fileType = fileName.substr(dotPosition);
                    fileName = fileName.substr(0, dotPosition);
                }
                fs.moveFile(fileName, destpath, fileType);
            }
            else
            {
                cout << "Invalid input for move operation." << endl;
            }

            fs.print();
        }
        else if (opr == "pprint")
        {
            istringstream iss(choice);
            vector<string> tokens;
            string token;
            while (getline(iss, token, ' '))
            {
                tokens.push_back(token);
            }
            if (tokens.size() >= 2)
            {
                string fileName = tokens[1];
                string fileType = "";
                size_t dotPosition = fileName.find_last_of('.');
                if (dotPosition != string::npos)
                {
                    fileType = fileName.substr(dotPosition);
                    fileName = fileName.substr(0, dotPosition);
                }
                fs.printing(fileName, fileType);
                fs.printQueue();
            }
            else
            {
                cout << "Wrong form of input" << endl;
            }
            fs.print();
        }
        else if (opr == "prompt")
        {
            string prompt = "/$";
            fs.print(prompt);
        }
        else if (opr == "print")
        {
            istringstream iss(choice);
            vector<string> tokens;

            string token;
            while (getline(iss, token, ' '))
            {
                tokens.push_back(token);
            }
            if (tokens.size() >= 2)
            {
                string fileName = tokens[1];
                string fileType = "";
                size_t dotPosition = fileName.find_last_of('.');
                if (dotPosition != string::npos)
                {
                    fileType = fileName.substr(dotPosition);
                    fileName = fileName.substr(0, dotPosition);
                }
                fs.printQ(fileName, fileType);
                fs.QueueOfPrintingFile();
            }
            else
            {
                cout << "Wrong form of input" << endl;
            }
            fs.print();
        }
        else if (opr == "pqueue")
        {
            fs.printTimeQueue();
            fs.print();
        }
        else if (opr == "pwd")
        {
            fs.printWorkingDirectory();
            fs.print();
        }
        else if (opr == "queue")
        {
            fs.printTime();
            fs.print();
        }
        else if (opr == "rename")
        {
            istringstream iss(choice);
            vector<string> tokens;
            string token;
            while (getline(iss, token, ' '))
            {
                tokens.push_back(token);
            }
            if (tokens.size() >= 2)
            {
                string fileName = tokens[1];
                string fileType = "";
                string newname = tokens[2];
                string fileType2 = "";
                size_t dotPosition = fileName.find_last_of('.');
                if (dotPosition != string::npos)
                {
                    fileType = fileName.substr(dotPosition);
                    fileName = fileName.substr(0, dotPosition);
                }
                fs.renameFile(fileName, fileType, newname, fileType2);
            }
            else
            {
                cout << "Invalid input for rename operation." << endl;
            }

            fs.print();
        }
        else if (opr == "rmdir")
        {
            if (choice.length() > 6)
            {
                string folder = choice.substr(6);
                flag = fs.removesDirectory(folder);
                if (flag == false)
                {
                    cout << "Exit from dos shell" << endl;
                    break;
                }
            }
            else
            {
                cout << "Please provide a folder name." << endl;
            }
            fs.print();
        }
        else if (opr == "tree")
        {
            fs.printFileSystemStructure();
            fs.print();
        }
        else if (opr == "ver")
        {
            cout << endl;
            cout << "Current Version is";
            cout << "1.0.0" << endl;
            fs.print();
        }
        else if (opr == "load")
        {
            fs.loadFileSystemStructureFromFile("tree.txt");
            fs.print();
        }
        else if (opr == "edit")
        {

            string fileName;
            string fileType = ".txt";
            system("cls");
            cout << "Welcome to Notepad" << endl;
            fileName = getFileName();
            readFiles();
            if (!fileExist(fileName, fileType))
            {
                writeFiles(fileName, fileType);
                makeFile(fileName, fileType);
            }

            editor.load(fileName, fileType);
            editor.input(fileName, fileType);

            system("cls");
            mainUser();
            fs.print();
        }
        else if (opr == "save")
        {
            string fileName;
            string fileType = ".txt";
            system("cls");
            cout << "Welcome to Notepad" << endl;
            fileName = getFileName();
            readFiles();
            if (!fileExist(fileName, fileType))
            {
                writeFiles(fileName, fileType);
                makeFile(fileName, fileType);
            }
            editor.load(fileName, fileType);
            editor.save(fileName, fileType);

            system("cls");
            mainUser();
            cout << "File saved to disk" << endl;
            fs.print();
        }
        else if (opr == "help")
        {
            help();
            fs.print();
        }
        else if (opr != "attrib" && opr != "cd" && opr != "findstr" && opr != "findf" && opr != "cd." && opr != "cd.." && opr != "cd/" && opr != "convert" && opr != "copy" && opr != "create" && opr != "del" && opr != "edit" && opr != "exit" && opr != "find" && opr != "findf" && opr != "findf" && opr != "findstr" && opr != "help" && opr != "load" && opr != "mkdir" && opr != "move" && opr != "print" && opr != "prompt" && opr != "print" && opr != "pqueue" && opr != "pwd" && opr != "queue" && opr != "rename" && opr != "rmdir" && opr != "save" && opr != "tree" && opr != "ver")
        {
            cout << "Invalid Command " << endl;
            fs.print();
        }
        else if (opr == "findf")
        {
            string fileName = choice.substr(6, choice.length());
            string fileType = "";
            size_t dotPosition = fileName.find_last_of('.');
            vector<string> output;

            if (dotPosition != string::npos)
            {
                fileType = fileName.substr(dotPosition);
                fileName = fileName.substr(0, dotPosition);
                f = fs.findFileInDirectory(fs.currentDirectory, fileName, fileType);
                if (f != nullptr)
                {
                    cout << endl;
                    string str;
                    cout << "Enter string to find: ";
                    cin >> str;
                    output = find(str, fileName, fileType);

                    if (!output.empty())
                    {
                        cout << "Found" << endl;
                    }
                    else
                    {
                        cout << "Not found" << endl;
                    }
                }
            }
            else
            {
                cout << "Invalid file name." << endl;
            }
            fs.print();
        }
        else if (opr == "findstr")
        {
            string searchString;
            cout << "Enter the string to search: ";
            cin >> searchString;
            fs.searchInFiles(searchString);

            fs.print();
        }
        getline(cin, choice);
    }
}
