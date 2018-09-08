# Goat

Goat - something between Heidi (sql) and Toad.

An attempt in making alternative DB frontend using QT5.

This app was made for 2 reasons:
1. necessity: there is no app in native toolkit available for linux that has querying functionality (there is a plugin for kate in kde, but this just doesnt work for me; not sure about gnome)
2. learning: improve my qt skills. i'm not a c++ developer, so please excuse shitty code quality

Note: what i learned so far is that i shouldn't commit directly to master. From now on development will be done in the dev branch.

What works:
- connection manager
    - saves information (warning! connection manager saves unencrypted passwords. always!)
    - delete connections
    - based on the driver, set default port to what makes sense
- queries can be executed with ctrl-enter for current query (query where cursor is currently located), as well as for selection with a button (but not as as script yet)
- basic syntax highlighter
- opening and saving files
- queries can be cancelled
    - works currently only on postgres
- coloring of NULL values in results grid
- export data
    - as c/p from the results grid


Todos:
- support more database options
    - connection manager needs to be able to handle file to actual sqlite db
- execute query as a script
- further improve connection manager
    - option to save or not to save password (per connection)
- autosave settings
    - e.g. height of the bottom part of tab
- settings dialog
- export data
- import data
- general code quality improvement
    - refactoring of querying (move to separate class?)


To be considered:
- evaluate scintilla/qscintilla or ktextedit/ktexteditor
- move from qt classes to native libs (e.g. libpq)

# Installing

## Opensuse

Tumbleweed:
```
zypper ar https://download.opensuse.org/repositories/home:/misp/openSUSE_Tumbleweed/ goatrepo
zypper ref
zypper in goat
```

Leap 15:
```
zypper ar https://download.opensuse.org/repositories/home:/misp/openSUSE_Leap_15.0/ goatrepo
zypper ref
zypper in goat
```

## Ubuntu 18.04

```
sudo apt install build-essential g++ qt5-default
mkdir build
cd build
qmake ..
make
./goat
```

# Contributing

Contributions are very welcome!

Following is needed:
- logo
- packaging

and not to mention 

- Qt skills

# Recognitions

Icons used are called Silk. From [here](http://www.famfamfam.com/lab/icons/silk/).

# Screenshots

Main window and connection manager

[![Connection manager](https://i.imgur.com/XES59p0.png)](https://i.imgur.com/XES59p0.png)

Main window and export data dialog

[![Export data](https://i.imgur.com/A3W9qAL.png)](https://i.imgur.com/A3W9qAL.png)
