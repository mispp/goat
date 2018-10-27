# Goat

Database frontend written in QT5 using widgets. At some point it should provide an alternative to tools like mysql workbench and pgadmin.

Goal for the project is to be something between [HeidiSQL](https://www.heidisql.com/) and Toad.

You should probably also take a look at [Sqlgull](https://github.com/asw-dev/sqlgull).

# Introduction

This app was made for 2 reasons:
1. Necessity: there was no app in native toolkit available for linux that has querying functionality (there is a plugin for kate in kde, but this just doesnt work for me; not sure about gnome). 
2. Learning: obtain/improve my qt, c++ and git skills. I'm not a c++ developer, so please excuse shitty code quality.

Note: what i learned so far is that i shouldn't commit directly to master. 

What works:
- multiple database options
    - postgres
    - mysql
    - sqlite
    - odbc
- connection manager
    - warning: connection manager currently saves unencrypted passwords
- querying
    - ctrl-enter for current query (as in toad)
    - button click which executes selection
    - canceling long running queries
- basic syntax highlighter
- opening and saving files
- coloring
    - NULL values in results grid
    - results grid shows data in different color based on datatype
- export data
    - c/p from results grid
    - export dialog which re-executes query and saves data to csv
- autosave state
    - window size

Todos:
- execute query as a script
- db2 support
- windows version
- further improve connection manager
    - option to save or not to save password (per connection)
    - encrypted passwords
- settings dialog
- import data
- general code quality improvement


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

## Snap
```
snap install goat --edge --devmode
```

# Contributing

Contributions are very welcome!

Following is needed:
- logo
- packaging

and not to mention 

- Qt skills

# Recognitions

UI icons used are called Silk. From [here](http://www.famfamfam.com/lab/icons/silk/).

Database icons are Papirus. From [here](https://github.com/PapirusDevelopmentTeam/papirus-icon-theme).

# Screenshots

Main window and connection manager

[![Connection manager](https://i.imgur.com/7KQcKMS.png)](https://i.imgur.com/7KQcKMS.png)

Main window and export data dialog

[![Export data](https://i.imgur.com/2ZtAP1M.png)](https://i.imgur.com/2ZtAP1M.png)
