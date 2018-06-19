# Goat

Goat - something between Heidi (sql) and Toad.

An attempt in making alternative DB frontend using QT5.

Todos:
- further improve connection manager
    - delete connections
    - option to save or not to save password (per connection)
    - driver auto-selection currently doesnt work
    - based on the driver, set default port to what makes sense
- syntax highlighter
- autosave some things
    - height of the bottom part of tab
- settings dialog
- export data
- import data
- general code quality improvement
    - connection handling moved to model (i have no idea how to do that now, so this is far, far away)


Warning! Connection manager saves password. Always.

[![Screenshot](https://i.imgur.com/ATAl83M.png)](https://i.imgur.com/ATAl83M.png)
