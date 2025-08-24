#include <iostream>
#include <string>
#include <vector>
#include "commands.h"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: legit <cmd>..." << endl;
        return 1;
    }

    string cmd = argv[1];

    if (cmd == "init")
    {
        cmdInit();
    }
    else if (cmd == "stage" && argc > 2)
    {
        cmdAdd(argv[2]);
    }
    else if (cmd == "checkpoint")
    {
        string msg;
        string tag;
        for (int i = 2; i < argc; i++)
        {
            if (string(argv[i]) == "-m" && i + 1 < argc)
                msg = argv[++i];
            else if (string(argv[i]) == "-t" && i + 1 < argc)
                tag = argv[++i];
        }
        if (msg.empty())
        {
            cerr << "fatal: commit requires -m <msg>" << endl;
            return 1;
        }
        cmdCommit(msg, tag);
    }
    else if (cmd == "log")
    {
        cmdLog();
    }
    else if (cmd == "status")
    {
        cmdStatus();
    }
    else if (cmd == "ignore" && argc > 2)
    {
        cmdIgnore(argv[2]);
    }
    else if (cmd == "branch")
    {
        vector<string> args;
        for (int i = 2; i < argc; i++)
            args.push_back(argv[i]);
        cmdBranch(args);
    }
    else if (cmd == "merge" && argc > 2)
    {
        cmdMerge(argv[2]);
    }
    else if (cmd == "load" && argc > 2)
    {
        if (argc == 4 && string(argv[2]) == "-b")
            cmdCheckout(argv[3], true);
        else
            cmdCheckout(argv[2], false);
    }
    else
    {
        cerr << "Unknown command" << endl;
        return 1;
    }

    return 0;
}