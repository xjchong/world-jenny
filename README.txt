This is a dungeon/world generator that outputs a layout
in ASCII form, similar to roguelikes such as Angband.

The algorithm used follows these guidelines:

1) Start with a block of uncarved space.
2) Attempt to carve a given number of rooms, that are of a 
given dimension, on average.
3) Fill in uncarved spaces with perfect corridors, possibly
breaking down walls between twists if spaciness is desired.
4) Add doors to connect rooms and corridors into one connected
region, possibly with more doors than are necessary.
5) Fill in dead ends, so that every path leads to at least a
room (though that room may be a dead end).
6) Remove/keep a percentage of single tile pillars as desired. 
7) Finished! Returns a string.

This generator is a work in progress, and will hopefully have
more flavour over time. Feel free to use it if you find it useful.


UNDERSTANDING THE EXECUTABLE:

Prompts have a default recommended value in ().
 |
 V

ROWS = number of rows, made odd
COLUMNS = number of columns, made odd
ROOM DENSITY = number of attempts to carve a room
ROOM WIDTH = average width of a room
ROOM HEIGHT = average height of a room
ROOM DEVIATION = deviation of room dimensions (normal dist.)
EXTRA DOOR CHANCE = chance of creating extra door, percentage
MAX-DOORS/AREA = max number of extra doors created
SPACINESS = chance of breaking down a wall when making corridors
PILLARS = percentage of pillars to keep (singleton walls)


*Note: there is no error handling, so invalid input will have
undefined behaviour.
