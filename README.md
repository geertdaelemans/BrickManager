# BrickManager
A tool to manage my LEGO brick collection.

In order to get this code snippet to work, you have to first set BrickLink key/secret pairs in the settings diaglog (Extras > Settings... > Connection).
You can apply for these values on the Bricklink.com site (https://www.bricklink.com/v2/api/register_consumer.page).

In order to get the software to compile you will need ssleay32.dll and libeay32.dll in your workspace (required for SSL support).

Currently supported features:
* Open BrickStock files (*.bsx)
* Save As... BrickStock files (*.bsx)
* Import your complete inventory from BrickLink
* Import your current orders from BricklLink
* Import your filed orders from BrickLink
* Get inventory of each order you imported
* Add items to any inventory (limited functionality)
* Filter on available colors in Add menu
* Import part catalog database (using BrickLink log-in)
* All data stored in local SQL database (local file)
* Pop-up window to (un)hide columns

This is still a work in progress... many features still need to be implemented.

Good luck!
