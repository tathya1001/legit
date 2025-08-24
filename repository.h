#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include <vector>
#include <map>
using namespace std;

extern const string repoDir;
extern const string objectsDir;
extern const string headFile;
extern const string indexFile;
extern const string refsDir;
extern const string ignoreFile;
extern const string tagsDir;

map<string, string> readIndex();
void writeIndex(const map<string, string> &idx);

string resolveHead();
string currentBranch();
void updateHead(const string &commitHash);

vector<string> readIgnore();
bool isIgnored(const string &path);

void writeTag(const string &branch, const string &tag, const string &commitHash);
string readTag(const string &branch, const string &tag);

void restoreCommit(const string &commitHash, bool staged, bool updateHeadToo);

#endif // REPOSITORY_H
