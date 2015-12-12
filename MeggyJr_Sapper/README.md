Sapper
======
Minesweeper clone for the [Meggy Jr RGB](http://shop.evilmadscientist.com/productsmenu/tinykitlist/100-meggyjr).

Gameplay
--------
Number of surrounding mines at the current cursor position is indicated with
auxiliary LEDs. It is also shown with increasing color intensity, from green
to yellow.

    0 - off
    1 - green
    ...
    8 - yellow

Mines are red, flags are blue, your cursor is white (or pink, if you have one
of the older boards with low intensity green LEDs).

Use directional keys to move the cursor, ``A`` to flag and ``B`` to reveal the
current cell. Pressing ``B`` on a revealed cell will also reveal all unflagged
cells around it.

Changelog
---------
Version 1.2, 11-Dec-2015

* Changed color mine indicators to use intensity instead of distinct colors.
* Updated to work with latest arduino version.
* Added makefile, build and upload with:
  * `make`
  * `make upload`
* Added separate license file.

Version 1.1, 31-May-2009

* Added button repeat (can hold down d-buttons).
* Added first-click immunity.
* Now smaller and with less repeated code.

Version 1.0, 31-May-2009

* Initial version.

License
-------
[GPL-3.0](LICENSE)
