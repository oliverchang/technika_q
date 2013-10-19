DJMax Technika Q Song Format
============================

Header
------
* 4 bytes unknown (always zeros?)
* 4 bytes offset to song information

Song information (usually at the very end of the file)
------------------------------------------------------
* 2 bytes number of sounds
* 2 bytes number of tracks?
* 2 bytes number of positions per measure?
* 4 bytes initial BPM (float)
* 4 bytes unknown
* 4 bytes unknown (another float?)
* 4 bytes unknown
* 4 bytes unknown

Sounds 
----------------------------
* Starts at offset 8
* An array of entries that are 0x43 bytes in size each:
    * 2 bytes index
    * 1 byte unknown
    * 0x40 bytes filename

Followed by 0x3d bytes of zeros.

Tracks
------
* An array of commands (0xD bytes each):
    * 4 bytes position
    * 1 byte command type
    * 8 bytes of params

### Notes
* The position refers to a position in the song
    * Its meaning is determined by the number of
      positions per measure (in song info)
    * Assumes 4/4 time signature?
* Every track is separated by 0x3d bytes
* There are usually 64 tracks
* The playable tracks for 3 lines are (0 indexed in order of appearance in file):
    * 0: top line
    * 1: middle line
    * 2: bottom line

Command types
-------------
### 0:  Start of a track
#### Params
* 4 bytes num params * 16 (unknown use)
* 4 bytes num params

### 1: Sound
#### Params
* 2 bytes sound index
* 1 byte volume
* 1 byte pan
* 1 byte type
* 1 byte? length (for long notes, unit is a single position)
* 2 bytes unknown

#### Types
* 0: normal note (drag note if length != 0x6)
* a: repeat note
* b: last repeat note
* c: hold note
* 5: first chain note (all notes in between have type 0)
* 6: last chain note
* 2: unknown

### 3: BPM change
#### Params
* 4 bytes float BPM
* 4 bytes unknown

