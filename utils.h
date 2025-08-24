#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <map>
using namespace std;

void makeDir(const string &path);

void writeFile(const string &path, const string &data);

string readFile(const string &path);

string normalizePath(const string &path);

string simpleHash(const string &s);

#endif // UTILS_H