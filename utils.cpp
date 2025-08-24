#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

void makeDir(const string &path)
{
    if (!fs::exists(path))
        fs::create_directories(path);
}

void writeFile(const string &path, const string &data)
{
    fs::path p(path);
    if (p.has_parent_path())
        fs::create_directories(p.parent_path());
    ofstream f(path, ios::binary);
    f << data;
}

string readFile(const string &path)
{
    ifstream f(path, ios::binary);
    if (!f.is_open())
        return "";
    stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

string normalizePath(const string &path)
{
    fs::path p = fs::relative(fs::absolute(path), fs::current_path());
    return p.generic_string();
}

string simpleHash(const string &s)
{
    hash<string> h;
    stringstream ss;
    ss << hex << h(s);
    return ss.str();
}