#include "repository.h"
#include "utils.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>
using namespace std;
namespace fs = std::filesystem;

// repository constants
const string repoDir = ".legit";
const string objectsDir = ".legit/objects";
const string headFile = ".legit/HEAD";
const string indexFile = ".legit/index";
const string refsDir = ".legit/refs/heads";
const string ignoreFile = ".legit/ignore";
const string tagsDir = ".legit/refs/tags";

map<string, string> readIndex()
{
    map<string, string> idx;
    stringstream ss(readFile(indexFile));
    string file, hash;
    while (ss >> file >> hash)
        idx[file] = hash;
    return idx;
}

void writeIndex(const map<string, string> &idx)
{
    stringstream ss;
    for (auto &p : idx)
        ss << p.first << " " << p.second << "\n";
    writeFile(indexFile, ss.str());
}

string resolveHead()
{
    string head = readFile(headFile);
    if (head.rfind("ref: ", 0) == 0)
    {
        string refPath = head.substr(5);
        return readFile(refPath);
    }
    return head;
}

string currentBranch()
{
    string head = readFile(headFile);
    if (head.rfind("ref: ", 0) == 0)
    {
        string refPath = head.substr(5);
        fs::path p(refPath);
        return p.filename().string();
    }
    return "";
}

void updateHead(const string &commitHash)
{
    string head = readFile(headFile);
    if (head.rfind("ref: ", 0) == 0)
    {
        string refPath = head.substr(5);
        writeFile(refPath, commitHash);
    }
    else
    {
        writeFile(headFile, commitHash);
    }
}

vector<string> readIgnore()
{
    vector<string> ignores;
    string data = readFile(ignoreFile);
    stringstream ss(data);
    string line;
    while (getline(ss, line))
    {
        if (!line.empty())
            ignores.push_back(line);
    }
    return ignores;
}

bool isIgnored(const string &path)
{
    vector<string> ignores = readIgnore();
    for (auto &ig : ignores)
    {
        string normIg = normalizePath(ig);
        string normPath = normalizePath(path);
        if (normPath == normIg)
            return true;
        if (normPath.rfind(normIg + "/", 0) == 0)
            return true;
    }
    return false;
}

void writeTag(const string &branch, const string &tag, const string &commitHash)
{
    string path = tagsDir + "/" + branch + "/" + tag;
    writeFile(path, commitHash);
}

string readTag(const string &branch, const string &tag)
{
    string path = tagsDir + "/" + branch + "/" + tag;
    if (fs::exists(path))
        return readFile(path);
    return "";
}

void restoreCommit(const string &commitHash, bool staged, bool updateHeadToo)
{
    string commitData = readFile(objectsDir + "/" + commitHash);
    if (commitData.empty())
    {
        cerr << "fatal: unknown commit " << commitHash << endl;
        return;
    }

    auto pos = commitData.find("\nMSG:");
    stringstream ss(commitData.substr(0, pos));
    string file, hash;
    map<string, string> newIndex;
    while (ss >> file >> hash)
        newIndex[file] = hash;

    map<string, string> oldIndex = readIndex();
    for (auto &p : oldIndex)
    {
        if (!newIndex.count(p.first))
        {
            fs::remove(p.first);
        }
    }

    for (auto &p : newIndex)
    {
        if (p.second != "__deleted__")
        {
            string content = readFile(objectsDir + "/" + p.second);
            writeFile(p.first, content);
        }
    }

    if (staged)
        writeIndex(newIndex);

    if (updateHeadToo)
        updateHead(commitHash);
}