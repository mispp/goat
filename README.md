# Goat

Goat - something between Heidi (sql) and Toad.

An attempt in making alternative DB frontend using QT5.
This app was made for 2 reasons:
1. necessity: there is no app in native toolkit available for linux that has querying functionality (there is a plugin for kate in kde, but this just doesnt work for me; not sure about gnome)
2. learning: improve my qt skills. i'm not a c++ developer, so please excuse shitty code quality

Contributions are welcome!

What (partially) works:
- connection manager saves information
    - warning! connection manager saves passwords. always!
- simple queries can be executed with ctrl-enter for current query (query where cursor is currently located), as well as for selection with a button
- syntax highlighter (with some issues)

Todos:
- move from qt classes to libpq for better error reporting (should check if it would work better)
    - postgres is a priority here, mariadb/mysql should maybe stay with qt classes for now
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
- evaluate scintilla/qscintilla or ktextedit/ktexteditor
- general code quality improvement
    - connection handling moved to model (i have no idea how to do that now, so this is far, far away)




# Screenshots

Connection manager

[![Connection manager](https://i.imgur.com/YCDsRwq.png)](https://i.imgur.com/YCDsRwq.png)

Query results

[![Query resuls](https://i.imgur.com/IimqoT3.png)](https://i.imgur.com/IimqoT3.png)
