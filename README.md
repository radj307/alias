# alias
This is a simple executable that executes a single set command.  

This allows you to create command aliases or command macros on windows, or execute shell scripts _(`bat`, `ps1`)_ using an executable.

# Features
- Same return code as executing the command manually
- Argument Forwarding
- Filename-Based, you can have any number of aliases in a single directory.
- Live Output
- Fully Configurable

# How it Works
The program keeps track of its current name & location, and checks its directory for an `.ini` configuration file with the same name.  
It then uses the __popen__ function to execute the command set in the config, prints any text from STDOUT/STDERR to either STDOUT or a file, then returns the result.  

_Note: __popen__ uses `cmd.exe`, so powershell commands will not work unless you prepend `powershell -Command` to the command._

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
