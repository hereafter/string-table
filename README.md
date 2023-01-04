A simple developer [command line tool](https://en.wikipedia.org/wiki/Command-line_interface) to extract string table from native dlls. Strings then could be easily searched.
***

**SYNTAX**

`STRINGTABLE [drive:][path][filename]`

  * [drive:][path][filename]

       Specifies drive, directory and file name to a dll file


**EXAMPLE**

The following example walks through the steps to search "Pin To Taskbar" id in the Windows shell dll.

1. open command prompt at a working directory

![image](https://user-images.githubusercontent.com/1483954/210475727-5f15eda2-9d42-4c8f-9aa7-e4ac0095edb9.png)

2. copy a dll to inspect 

`copy %windir%\System32\shell32.dll .`

![image](https://user-images.githubusercontent.com/1483954/210475862-0450da08-0a55-4eb6-8d73-a76449580d91.png)

3. Use "StringTable" command

`stringtable shell32.dll`

The command prompts 2506 strings extracted. 
use 

`dir` 

to verify a csv file is created in current directory.

![image](https://user-images.githubusercontent.com/1483954/210476025-8b39c0a8-5d3d-4730-8296-593aa60758e6.png)


4. Use 'Visual Studio Code' as the editor to search for "Pin To Taskbar"

`code shell32.csv`

![image](https://user-images.githubusercontent.com/1483954/210476301-0d02fe3c-5ed6-4f5b-a457-a703f31702f4.png)


Search "Pin to tas"

![image](https://user-images.githubusercontent.com/1483954/210476369-b1a40e81-9c3f-4cdb-8506-7ae2cbb457bc.png)

reveals that the string is located with id 5386 decimal.
You may use "Notepad" to do the job as well.


5. Import csv into Excel

Create a blank excel sheet
Goto "Data" Tab Use command "From Text/CSV"

![image](https://user-images.githubusercontent.com/1483954/210476688-80c34ff7-9a8d-4346-9f67-b3aa534486af.png)

Browse to the file shell32.csv to import csv into an spreadsheet.
Choose delimiter to be "Custom"  the text encoding is 65001 UTF8

![image](https://user-images.githubusercontent.com/1483954/210476823-1255ed71-54da-401b-bc72-2ca0999748e6.png)


Hit "Load"

You may process the strings inside excel

![image](https://user-images.githubusercontent.com/1483954/210477107-2cf84b02-9028-4b5e-a56c-94bec868fe19.png)

