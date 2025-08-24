#include "commands.h"
#include "repository.h"
#include "utils.h"
#include <iostream>
#include <filesystem>
#include <sstream>
using namespace std;

namespace fs = std::filesystem;

void cmdInit()
{
    makeDir(repoDir);
    makeDir(objectsDir);
    makeDir(refsDir);
    writeFile(indexFile, "");
    writeFile(refsDir + "/master", "");
    writeFile(headFile, "ref: " + refsDir + "/master");
    cout << "Initialized empty legit repo in " << repoDir << "/" << endl;
}

void cmdAdd(const string &path)
{
    if (path == ".")
    {
        for (auto &entry : fs::recursive_directory_iterator("."))
        {
            if (!entry.is_regular_file())
                continue;
            string fname = normalizePath(entry.path().string());
            if (isIgnored(fname) || fname.rfind(repoDir, 0) == 0)
                continue;
            cmdAdd(fname);
        }
        return;
    }

    if (fs::is_directory(path))
    {
        for (auto &entry : fs::recursive_directory_iterator(path))
        {
            if (entry.is_regular_file())
                cmdAdd(entry.path().string());
        }
        return;
    }

    map<string, string> idx = readIndex();
    string normalized = normalizePath(path);

    if (!fs::exists(path))
    {
        idx[normalized] = "__deleted__";
        writeIndex(idx);
        cout << "Staged deletion of '" << normalized << "'" << endl;
        return;
    }

    string content = readFile(path);
    string hash = simpleHash(content);
    string head = resolveHead();
    map<string, string> lastCommit;

    if (!head.empty())
    {
        string data = readFile(objectsDir + "/" + head);
        size_t pos = data.find("\nMSG:");
        stringstream ss(data.substr(0, pos));
        string f, h;
        while (ss >> f >> h)
            lastCommit[f] = h;
    }

    if (idx.count(normalized) && idx[normalized] == hash && lastCommit.count(normalized))
    {
        cout << "No changes to add for " << normalized << endl;
        return;
    }

    if (readFile(objectsDir + "/" + hash).empty())
        writeFile(objectsDir + "/" + hash, content);

    idx[normalized] = hash;
    writeIndex(idx);
    cout << "Staged '" << normalized << "' with hash " << hash.substr(0, 7) << endl;
}

void cmdCommit(const string &msg, const string &tag)
{
    map<string, string> idx = readIndex();
    if (idx.empty())
    {
        cout << "Nothing to commit!" << endl;
        return;
    }

    stringstream commitData;
    for (auto &p : idx)
        commitData << p.first << " " << p.second << "\n";
    commitData << "\nMSG:" << msg;
    string parent = resolveHead();
    if (!parent.empty())
        commitData << "\nPARENT:" << parent;

    string commitContent = commitData.str();
    string hash = simpleHash(commitContent);
    writeFile(objectsDir + "/" + hash, commitContent);
    updateHead(hash);

    map<string, string> cleaned;
    for (auto &p : idx)
    {
        if (p.second != "__deleted__")
            cleaned[p.first] = p.second;
    }
    writeIndex(cleaned);
    cout << "Committed as " << hash << endl;

    if (!tag.empty())
    {
        string branch = currentBranch();
        if (branch.empty())
        {
            cerr << "fatal: cannot tag in detached HEAD" << endl;
        }
        else
        {
            makeDir(tagsDir + "/" + branch);
            writeTag(branch, tag, hash);
            cout << "Tagged commit as '" << tag << "' on branch " << branch << endl;
        }
    }
}

void cmdLog()
{
    string commitHash = resolveHead();
    if (commitHash.empty())
    {
        cout << "No commits yet." << endl;
        return;
    }

    while (!commitHash.empty())
    {
        string data = readFile(objectsDir + "/" + commitHash);
        if (data.empty())
            break;

        cout << "Commit: " << commitHash.substr(0, 6) << endl;
        auto msgPos = data.find("\nMSG:");
        auto parentPos = data.find("\nPARENT:");
        if (msgPos != string::npos)
        {
            size_t msgStart = msgPos + 5;
            size_t msgEnd = (parentPos != string::npos) ? parentPos : data.length();
            cout << "Message: " << data.substr(msgStart, msgEnd - msgStart) << endl;
        }
        cout << endl;

        if (parentPos != string::npos)
        {
            commitHash = data.substr(parentPos + 8);
        }
        else
        {
            break;
        }
    }
}

void cmdStatus()
{
    string branch = currentBranch();
    string head = resolveHead();
    if (!branch.empty())
        cout << "On branch " << branch << endl;
    else
        cout << "HEAD detached at " << head.substr(0, 6) << endl;

    map<string, string> idx = readIndex();
    cout << "Changes to be committed:" << endl;
    map<string, string> lastCommit;
    if (!head.empty())
    {
        string commitData = readFile(objectsDir + "/" + head);
        size_t pos = commitData.find("\nMSG:");
        stringstream ss(commitData.substr(0, pos));
        string f, h;
        while (ss >> f >> h)
            lastCommit[f] = h;
    }
    for (auto &p : idx)
    {
        if (p.second == "__deleted__")
            cout << "  deleted: " << p.first << endl;
        else if (!lastCommit.count(p.first) || lastCommit[p.first] != p.second)
            cout << "  staged: " << p.first << endl;
    }

    cout << "\nChanges not staged for commit:" << endl;
    for (auto &p : idx)
    {
        if (p.second == "__deleted__")
            continue;
        if (!fs::exists(p.first))
        {
            cout << "  deleted: " << p.first << endl;
            continue;
        }
        string content = readFile(p.first);
        string curHash = simpleHash(content);
        if (curHash != p.second)
            cout << "  modified: " << p.first << endl;
    }

    cout << "\nUntracked files:" << endl;
    for (auto &entry : fs::recursive_directory_iterator("."))
    {
        if (!entry.is_regular_file())
            continue;
        string fname = normalizePath(entry.path().string());
        if (isIgnored(fname) || fname.rfind(repoDir, 0) == 0)
            continue;
        if (!idx.count(fname))
            cout << "  " << fname << endl;
    }
}

void cmdIgnore(const string &path)
{
    string norm = normalizePath(path);
    string existing = readFile(ignoreFile);
    stringstream ss(existing);
    string line;
    while (getline(ss, line))
    {
        if (line == norm)
        {
            cout << "'" << norm << "' is already ignored" << endl;
            return;
        }
    }
    existing += norm + "\n";
    writeFile(ignoreFile, existing);
    cout << "Added '" << norm << "' to ignore list" << endl;
}

void cmdBranch(const vector<string> &args)
{
    if (args.empty())
    {
        string current = currentBranch();
        for (auto &entry : fs::directory_iterator(refsDir))
        {
            string branch = entry.path().filename().string();
            if (branch == current)
                cout << "* " << branch << endl;
            else
                cout << "  " << branch << endl;
        }
        return;
    }

    if (args.size() == 1)
    {
        string name = args[0];
        string path = refsDir + "/" + name;
        if (fs::exists(path))
        {
            cerr << "fatal: branch '" << name << "' already exists" << endl;
            return;
        }
        string head = resolveHead();
        if (head.empty())
        {
            cerr << "fatal: no commits yet" << endl;
            return;
        }
        writeFile(path, head);
        cout << "Created branch " << name << endl;
        return;
    }

    if (args.size() == 2 && args[0] == "-d")
    {
        string name = args[1];
        string path = refsDir + "/" + name;
        if (!fs::exists(path))
        {
            cerr << "fatal: branch '" << name << "' not found" << endl;
            return;
        }
        if (name == currentBranch())
        {
            cerr << "fatal: cannot delete branch currently checked out" << endl;
            return;
        }
        fs::remove(path);
        cout << "Deleted branch " << name << endl;
        return;
    }
    cerr << "fatal: invalid branch command" << endl;
}

void cmdCheckout(const string &arg, bool newBranch)
{
    string branchPath = refsDir + "/" + arg;
    if (newBranch)
    {
        string cur = resolveHead();
        if (readFile(branchPath) != "")
        {
            cerr << "fatal: branch exists" << endl;
            return;
        }
        writeFile(branchPath, cur);
        writeFile(headFile, "ref: " + branchPath);
        restoreCommit(cur, true, false);
        cout << "Switched to new branch '" << arg << "'" << endl;
        return;
    }

    if (fs::exists(branchPath))
    {
        string hash = readFile(branchPath);
        writeFile(headFile, "ref: " + branchPath);
        restoreCommit(hash, true, false);
        cout << "Switched to branch '" << arg << "'" << endl;
        return;
    }

    string branch = currentBranch();
    if (!branch.empty())
    {
        string taggedHash = readTag(branch, arg);
        if (!taggedHash.empty())
        {
            restoreCommit(taggedHash, true, true);
            cout << "Checked out tag '" << arg << "' (detached)" << endl;
            return;
        }
    }

    restoreCommit(arg, true, true);
    cout << "Checked out commit " << arg.substr(0, 7) << " (detached)" << endl;
}

void cmdMerge(const string &branch)
{
    string curBranch = currentBranch();
    if (curBranch.empty())
    {
        cerr << "fatal: cannot merge in detached HEAD" << endl;
        return;
    }

    string branchPath = refsDir + "/" + branch;
    if (!fs::exists(branchPath))
    {
        cerr << "fatal: branch '" << branch << "' not found" << endl;
        return;
    }

    string targetHash = readFile(branchPath);
    if (targetHash.empty())
    {
        cerr << "fatal: branch '" << branch << "' has no commits" << endl;
        return;
    }

    string curHash = resolveHead();
    if (curHash == targetHash)
    {
        cout << "Already up to date." << endl;
        return;
    }

    restoreCommit(targetHash, true, false);
    map<string, string> idx = readIndex();
    stringstream commitData;
    for (auto &p : idx)
        commitData << p.first << " " << p.second << "\n";
    commitData << "\nMSG:Merge branch '" << branch << "'";
    if (!curHash.empty())
        commitData << "\nPARENT:" << curHash;
    commitData << "\nPARENT2:" << targetHash;

    string commitContent = commitData.str();
    string hash = simpleHash(commitContent);
    writeFile(objectsDir + "/" + hash, commitContent);
    updateHead(hash);

    cout << "Merged branch '" << branch << "' into " << curBranch
         << " (commit " << hash.substr(0, 7) << ")" << endl;
}