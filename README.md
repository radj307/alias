# alias
This is a simple executable that executes a single pre-set command.  

This allows you to create command aliases or command macros on windows, or execute shell scripts (`.bat`/`.ps1`) using an executable (`.exe`).

# Features
- Same return code as executing the command manually
- Argument Forwarding
- Name-based configuration; you can copy-paste the executable in the same directory and rename it to create a different alias.
- Live Output
- Fully Configurable

# Usage
1.  Download the [latest release](https://github.com/radj307/Command-Alias/releases).
2.  Rename the executable to whatever you want.
3.  Run the executable once, which will create the default configuration file next to the executable.
4.  Edit the generated `.ini` file & set the command to execute.

__Example:__  
Say you wanted to execute the command `ls -Force` by entering `lsf`.  
First, rename `alias.exe` to `lsf.exe`, and place it somewhere on your PATH.  
Then run `lsf.exe` once to generate `lsf.ini` in the same directory, and open it in a text editor.  
Set the following values:  
```ini
[target]
command = "ls -Force"
forward_args = true
```
Now you can use the `lsf` command to execute `ls -Force`

# How it Works

When the program is called, it finds its current location and searches that directory for an `ini` file with the same name.  

It then uses the __popen__ function to open a process pipe that it uses to emulate that process.  
The executed command's return code is forwarded by the program when it exits as well.  

_Note: __popen__ uses `cmd.exe` by default; to execute PowerShell commands you must prepend `powershell -Command` to the command string in the config._
