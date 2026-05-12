# Neon Interpreter

## Introduction

You have just downloaded the Neon interpreter, a cross‑platform, multitasking interpreted programming language. In this folder you will find complete documentation and installation files for Linux, Windows, and the TI‑83 Premium CE [Python Edition].

For more information: https://langage-neon.raphaael.fr

## Neon Installation Instructions

These files include executable binaries for Linux, Windows, and the TI‑83 Premium CE / TI‑84 Plus CE, as well as installation packages for Linux and Windows.

The Neon executables are designed for portable use; otherwise, I recommend installing them.

### Windows
To install Neon on Windows, simply run the installer files and follow the prompts. Neon can then be launched from the console by typing neon after you have added the executable’s path to the PATH, or you can start it like any other application.

### Linux
To install it on Linux, grant execution permission to the file `Neon_installer.lpk`, then run it.

### TI‑83 Premium CE [Python Edition]
To use Neon on the calculator:

Transfer the files `NEONINSTALL.8xp`, `NEON.0.8xv`, `NEON.1.8xv`, `NEON.2.8xv`, and `NEON.3.8xv` to your calculator.
To create and edit Neon programs, I recommend using the Python app. Neon is compatible with files created by the Python app.
You have two ways to access Neon: via the console or by running a file.

To open the console, place "" in Ans (or Rép). To do this, go to the home screen (the calculator’s main screen), type "", then press ENTER.
To run a file (an AppVar), put the AppVar name in Ans (or Rép) in the same way: "EXAMPLE" then ENTER. The file name must follow program‑naming rules (only uppercase letters or digits, and starting with an uppercase letter).
If you want to convert a .ne file into an executable AppVar for Neon, go to the TI-83 Premium CE/ne-to-8xv folder in this archive. Place the .ne file you wish to convert in the same folder, then run the command:

```
python3 ne-to-8xv.py <path-to-ne-file> <program-name-on-calculator>
```

When finished, you will have a .8xv file to transfer to the calculator.

