# LeGit - A Toy Git in C++

**Download for Windows**: [legit.exe](https://github.com/tathya1001/legit/releases/download/v1.0.0/legit.exe)

---

Welcome to Legit! This is a lightweight, command-line version control system built in C++ that mimics some of the basic functionalities of Git.

This guide will walk you through the installation and basic usage of the `legit.exe` command-line tool on Windows.

---

## 💾 Installation

Follow these steps to get LeGit up and running on your system.

### Step 1: Get the Executable

You have two options: download the pre-compiled file or compile it yourself from the source code.

#### Option A: Download the Pre-compiled Executable

1.  **Download the file**: Get the latest `legit.exe` from [**this link**](https://github.com/tathya1001/legit/releases/download/v1.0.0/legit.exe).
2.  **Antivirus Warning**: Your browser or antivirus software might flag the downloaded `.exe` file as a potential threat. This is a common false positive for unsigned executables from unknown developers. If this happens, you can proceed with Option B.

#### Option B: Compile from Source (If Virus is Detected or for latest version)

If you prefer to build the executable yourself, you'll need Git and a C++ compiler like G++/MinGW.

1.  **Prerequisites**: Make sure you have [Git](https://git-scm.com/downloads) and [MinGW-w64](https://www.mingw-w64.org/) (which provides `g++`) installed and configured in your system's PATH.
2.  **Clone the repository**:
    ```bash
    git clone https://<your-repo-url>/legitv2.git
    cd legitv2
    ```
3.  **Compile the source code**: Run the following command in the project's root directory. The `-static` flag ensures the executable is self-contained and doesn't rely on external DLLs.
    ```bash
    g++ *.cpp -static -o legit.exe
    ```
    This will create a fresh `legit.exe` file in the current folder.

### Step 2: Place `legit.exe` in a Permanent Folder

1.  Create a dedicated folder on any drive where you want to store the program permanently. It's best to choose a simple, space-free path.
    * **Example**: `C:\legit`

2.  Move or paste your `legit.exe` file into this newly created folder.

### Step 3: Add the Folder to Windows PATH

To run the `legit` command from any directory in your terminal, you must add its location to the Windows `PATH` environment variable.

1.  Press the **Windows Key** and search for "**Edit the system environment variables**". Click to open it.
    

2.  In the System Properties window that opens, click the **Environment Variables...** button at the bottom.

3.  In the new window, find the `Path` variable under the **System variables** list. Select it and click **Edit...**.
    

4.  Click **New** and paste the full path to the folder where you saved `legit.exe`.
    * **Example**: `C:\legit`

5.  Click **OK** on all three windows to save and close them.

### Step 4: Verify the Installation

1.  **Open a new terminal**. Any terminals you had open *before* changing the environment variables will not recognize the new path. You must open a new Command Prompt, PowerShell, or Windows Terminal window.

2.  Run a simple command to check if your system can find the executable. For example, you can navigate to any working folder and initialize a new repository:
    ```bash
    # Navigate to a test folder
    cd C:\Users\YourUser\Desktop\my-test-project

    # Run the init command
    legit init
    ```
    If it works, you will see the message: `Initialized empty legit repo in .legit/`

3.  **Troubleshooting**: If the `legit` command is not found, try **restarting your computer**. This ensures the new environment variables are loaded system-wide.

---

## 🚀 Basic Usage

Here is a quick example of how to start tracking a project.

1.  **Initialize a repository** in your project folder:
    ```bash
    legit init
    ```

2.  **Create a file** and add some content.
    ```bash
    echo "Hello World" > test.txt
    ```

3.  **Stage the file** to prepare it for the next commit (the command is `stage`, not `add`):
    ```bash
    legit stage test.txt
    ```

4.  **Commit the staged changes** with a message (the command is `checkpoint`, not `commit`):
    ```bash
    legit checkpoint -m "Initial commit"
    ```

5.  **Check the log** to see your commit history:
    ```bash
    legit log
    ```