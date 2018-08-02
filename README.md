# Goat

Goat - something between Heidi (sql) and Toad.

An attempt in making alternative DB frontend using QT5.

This app was made for 2 reasons:
1. necessity: there is no app in native toolkit available for linux that has querying functionality (there is a plugin for kate in kde, but this just doesnt work for me; not sure about gnome)
2. learning: improve my qt skills. i'm not a c++ developer, so please excuse shitty code quality

Contributions are welcome!

What works:
- connection manager saves information
    - warning! connection manager saves passwords. always!
- queries can be executed with ctrl-enter for current query (query where cursor is currently located), as well as for selection with a button (but not as as script yet)
- basic syntax highlighter

Todos:
- change how results are fetched from database, now it doesn't work with big amounts of data
- execute query as a script
- further improve connection manager
    - delete connections
    - option to save or not to save password (per connection)
    - driver auto-selection currently doesnt work
    - based on the driver, set default port to what makes sense
- opening and saving files
- autosave settings
    - e.g. height of the bottom part of tab
- settings dialog
- export data
- import data
- general code quality improvement
    - connection handling moved to model (i have no idea how to do that now, so this is far, far away)

To be considered:
- move from qt classes to libpq for better error reporting (should check if it would work better)
    - postgres is a priority here, mariadb/mysql should maybe stay with qt classes for now
- evaluate scintilla/qscintilla or ktextedit/ktexteditor

# Installing

## Ubuntu 18.04

```
sudo apt install build-essential g++ qt5-default
mkdir build
cd build
qmake ..
make
./Goat
```

# Recognitions

Icons used are called Silk. From [here](http://www.famfamfam.com/lab/icons/silk/).

# Screenshots

Connection manager

[![Connection manager](https://i.imgur.com/YCDsRwq.png)](https://i.imgur.com/YCDsRwq.png)

Main window

[![Main window](https://i.imgur.com/NJjdtkH.png)](https://i.imgur.com/NJjdtkH.png)
