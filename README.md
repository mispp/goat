# Goat

Goat - something between Heidi (sql) and Toad.

An attempt in making alternative DB frontend using QT5.
This app was made for 2 reasons:
1. necessity: there is no app in native toolkit available for linux that has querying functionality (there is a plugin for kate in kde, but this just doesnt work for me; not sure about gnome)
2. learning: improve my qt skill. i'm not a c++ developer, so please excuse shitty code quality

Contributions are welcome!

What works:
- connection manager saves information
    - warning! Connection manager saves passwords. always!
- simple queries can be executed with ctrl-enter. buttons will be added later.


Todos:
- further improve connection manager
    - delete connections
    - option to save or not to save password (per connection)
    - driver auto-selection currently doesnt work
    - based on the driver, set default port to what makes sense
- opening and saving files
- syntax highlighter
- autosave settings
    - height of the bottom part of tab
- settings dialog
- export data
- import data
- general code quality improvement
    - connection handling moved to model (i have no idea how to do that now, so this is far, far away)




[![Screenshot](https://i.imgur.com/ATAl83M.png)](https://i.imgur.com/ATAl83M.png)
