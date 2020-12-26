# cboard(1) - Simple chess game editor

```
cboard [OPTIONS] [PGN_FILE]
```

<a name="description"></a>

# Description

**cboard**
is chess game editor done with SDL.

It is designed to be controlled by keyboard using different modes.
Current mode is shown in lower left corner. See **MODES**

<a name="options"></a>

# Options


* **-c, --config _file_**  
  Path configuration file.
* **-f, --font _file_**  
  Path to TrueType font file.
* **--FEN**  
  Starting position in FEN format.
* **-h, --help**  
  Prints usage information.
* **-n, --number _number_**  
  Game number (starting from 0).
* **-o, --output _format_**  
  Output format (default none). Supported formats are: FEN, PGN.
* **-p, --pieces _dir_**  
  Path to directory containing images of pieces, named with lowercase first
  character of color and piece. Example: black biship = bb.png
* **-v, --version**  
  Prints version.

<a name="modes"></a>

# Modes


<a name="normal"></a>

### NORMAL

Main mode, other modes are entered from here.

* **q**  
  Quit.
* **g**  
  Enter GAME_LIST mode.
* **e**  
  Enter EDIT mode.
* **f**  
  Enter FILENAME mode.
* **n**  
  Enter NUMBER mode.
* **s**  
  Enter SAN mode.
* **t**  
  Enter TAG mode.
* **T**  
  Enter TRAINING mode.
* **v**  
  Enter MOVE mode.
* **V**  
  Enter POSITION mode.
* **a**  
  Enter COMMENT mode after current move.
* **A**  
  Enter COMMENT mode before current move.
* **c**  
  Enter CLIPBOARD mode.
* **r**  
  Rotate chessboard.
* **W**  
  Write current game.
* **m**  
  Start/stop machine 1.
* **M**  
  Start/stop machine 2.
* **l, Arrow Right**  
  Go to next move.
* **h, Arrow Left**  
  Go to previous move.
* **j, Arrow Down**  
  Go to first sub variation.
* **k, Arrow Up**  
  Go to parent variation.
* **L, Shift-Arrow Right**  
  Go to last move.
* **H, Shift-Arrow Left**  
  Go to first move.
* **J, Shift-Arrow Down**  
  Go to next sub variation.
* **K, Shift-Arrow Up**  
  Go to previous sub variation.
* **u**  
  Undo last destructive operation.
* **Ctrl-r**  
  Redo last destuctive operation.
* **z**  
  Increase size of chessboard.
* **Z**  
  Decrease size of chessboard.
* **p**  
  Promote current variation.
* **d**  
  Delete current variation.
* **D**  
  Delete remaining moves after current move.
* **PageUp**  
  Scroll notation up.
* **PageDown**  
  Scroll notation down.

<a name="game_list"></a>

### GAME_LIST


* **q**  
  Quit.
* **Escape**  
  Return to NORMAL mode.
* **Return**  
  Read current game.
* **r**  
  Read reversed game list from file.
* **R**  
  Read game list from file.
* **j, Arrow Down**  
  Move to next game.
* **k, Arrow Up**  
  Move to previous game.
* **/**  
  Enter TEXINPUT search mode.
* **c**  
  Insert game title in comment after current move.
* **PageUp**  
  Scroll game list up.
* **PageDown**  
  Scroll game list down.

<a name="textinput"></a>

### TEXTINPUT

Presed keys are inserted as text.

* **Escape**  
  Return to previous mode.
* **Ctrl-u**  
  Delete all text.
* **Left Arrow**  
  Move cursor left.
* **Right Arrow**  
  Move cursor right.
* **Backspace**  
  Delete character before cursor.
* **Delete**  
  Delete character after cursor.

<a name="edit"></a>

### EDIT


* **Mouse Click Left**  
  Insert current piece at clicked square.
* **Mouse Click Right**  
  Clear piece at clicked square.
* **c**  
  Clear board.
* **s**  
  Set starting position.
* **f**  
  Import FEN from clipboard.
* **r**  
  Rotate chessboard.
* **0**  
  Change color.
* **1**  
  Select pawn of current color.
* **2**  
  Select knight of current color.
* **3**  
  Select bishop of current color.
* **4**  
  Select rook of current color.
* **5**  
  Select queen of current color.
* **6**  
  Select king of current color.

<a name="filename"></a>

### FILENAME

TEXTINPUT mode. Filename is used for reading and writing.

<a name="number"></a>

### NUMBER

TEXTINPUT mode. Number represents game sequence.
If value is "a" game is added at the end of file.

<a name="san"></a>

### SAN

It is TEXINPUT mode. Correct SAN moves are accepted and played.

* **Return**  
  If move is correct it is played.

<a name="tag"></a>

### TAG

TEXTINPUT mode. First tag name is inputed.
If it already exists, it will be editer - otherwise new tag is created.

* **Return**  
  Confirm tag name. And enter TEXTINPUT mode for editing tags.
  Tag will be removed it its value is empty string (exept for required 7 tags).

<a name="training"></a>

### TRAINING

TEXTINPUT mode. Notation is hidden.
After user plays move that exists in notation, random
opponents move will be played.
Move can be inserted with mouse or in SAN format.
Training can be restarted with "Restart" value.

<a name="move"></a>

### MOVE

Move annotation.

* **j, Arrow Down**  
  Select next move NAG.
* **k, Arrow Up**  
  Select previous move NAG.
* **x, Backspace**  
  Delete move NAG.
* **Escape**  
  Return to NORMAL mode.

<a name="position"></a>

### POSITION

Position annotation.

* **j, Arrow Down**  
  Select next position NAG.
* **k, Arrow Up**  
  Select previous position NAG.
* **x, Backspace**  
  Delete position NAG.
* **Escape**  
  Return to NORMAL mode.

<a name="comment"></a>

### COMMENT

TEXTINPUT mode. Comment is inserted after/before current move.

<a name="clipboard"></a>

### CLIPBOARD


* **q**  
  Quit.
* **f**  
  Copy current FEN to clipboard.
* **Escape**  
  Return to NORMAL mode.

<a name="configuration"></a>

# Configuration

Configuration is done by line separated key=value.
Lines starting with # are skipped.

* **machine_1_exe, machine_2_exe**  
  Absolute path to engine executable.
  **machine_1_exe=/bin/stockfish**
* **machine_1_param,machine_2_param**  
  Engine command line parameters, can be used multiple times.
  **machine_2_param=--logfile=/home/user/log**
* **machine_1_uci_option_start, machine_2_uci_option_start**  
  Option does not take value.
  Lines after this are passed to engine.
* **machine_1_uci_option_end, machine_2_uci_option_end**  
  End literal input.

<a name="files"></a>

# Files

_$HOME/.config/cboard/config_
Default config location.