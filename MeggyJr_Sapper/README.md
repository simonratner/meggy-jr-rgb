Sapper
======
Minesweeper clone for the [Meggy Jr RGB](http://shop.evilmadscientist.com/productsmenu/tinykitlist/100-meggyjr).

Gameplay
--------
Number of surrounding mines at the current cursor position is indicated with
auxiliary LEDs. It is also shown with color, as follows:

      0 - black
      1 - blue
      2 - green
      3 - yellow
      4 - orange
      5 - bright blue
      6 - bright green
      7 - bright yellow
      8 - bright orange

Mines are red, flags are violet, your cursor is white.

Use directional keys to move the cursor, ``A`` to flag and ``B`` to reveal the
current cell. Pressing "B" on a cell that is already revealed will also reveal
all unflagged cells around it.

Changelog
---------
Version 1.1, 31-May-2009

* Added button repeat (can hold down d-buttons).
* Added first-click immunity.
* Now smaller and with less repeated code.

Version 1.0, 31-May-2009

* Initial version.

License
-------
Copyright (c) 2009 Simon Ratner. All right reserved.
 
This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library. If not, see <http://www.gnu.org/licenses/>.
