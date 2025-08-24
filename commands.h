#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
using namespace std;
void cmdInit();
void cmdAdd(const string &path);
void cmdCommit(const string &msg, const string &tag = "");
void cmdLog();
void cmdStatus();
void cmdIgnore(const string &path);
void cmdBranch(const vector<string> &args);
void cmdCheckout(const string &arg, bool newBranch = false);
void cmdMerge(const string &branch);

#endif // COMMANDS_H