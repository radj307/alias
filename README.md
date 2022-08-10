# alias

A filename-based Windows executable that emulates the specified command in a terminal, similar to `bash` aliases.

This allows you to create command aliases or command macros on windows, or execute shell scripts (`.bat`/`.ps1`) using an executable (`.exe`).


## Features

- Same return code as executing the command manually
- Forwards commandline arguments
- Filename-based configuration allows you to have any number of aliases in the same directory
- Live-updating output


## Usage

 1. Download the [latest release](https://github.com/radj307/Command-Alias/releases).
 2. Rename the executable to whatever you want.
 3. Run the executable once, which will create the default configuration file in the same directory as the executable.
 4. Edit the generated `.ini` file & set the command to execute.


### Example

Say you wanted to create an alias `lsf` for the PowerShell command `Get-ChildItem -Force`.  

 1. Download *(or copy-paste)* `alias.exe` & move it to a [directory on your PATH](https://stackoverflow.com/a/44272417/8705305). 
 2. Rename `alias.exe` to `lsf.exe`, then run it once to generate the config file.
 3. Edit `lsf.ini`:  
    ```ini
    [target]
    command = "PowerShell -Command 'Get-ChildItem -Force'"
    forward_args = true
    ```  
    Now you can run the `lsf` command to execute `Get-ChildItem -Force`

# How it Works

When the program is called, it finds its current location and searches that directory for an `ini` file with the same name.  

It then uses the __popen__ function to open a process pipe that it uses to emulate that process.  
The executed command's return code is forwarded by the program when it exits as well.  

_Note: __popen__ uses `cmd.exe` by default; to execute PowerShell commands you must prepend `powershell -Command` to the command string in the config._
