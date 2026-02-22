#include "help.h"

#define TEXTINPUT \
    {.shortcut = "Pressed keys are inserted as text.", .text = NULL}, \
    {.shortcut = "Escape", .text = "Return to previous mode."}, \
    {.shortcut = "Ctrl-u", .text = "Delete all text."}, \
    {.shortcut = "Left Arrow", .text = "Move cursor left."}, \
    {.shortcut = "Right Arrow", .text = "Move cursor right."}, \
    {.shortcut = "Backspace", .text = "Delete character before cursor."}, \
    {.shortcut = "Delete", .text = "Delete character after cursor."}

Help
help_init() {
    return (Help) {
        .mode = {
            [ModeNormal] = {
                {.shortcut = "q", .text = "Quit."},
                {.shortcut = "g", .text = "Enter GAME_LIST mode."},
                {.shortcut = "G", .text = "Toggle Game list statistics."},
                {.shortcut = "e", .text = "Enter EDIT mode."},
                {.shortcut = "f", .text = "Enter FILENAME mode."},
                {.shortcut = "n", .text = "Enter NUMBER mode."},
                {.shortcut = "s", .text = "Enter SAN mode."},
                {.shortcut = "t", .text = "Enter TAG mode."},
                {.shortcut = "T", .text = "Enter TRAINING mode."},
                {.shortcut = "v", .text = "Enter MOVE mode."},
                {.shortcut = "V", .text = "Enter POSITION mode."},
                {.shortcut = "a", .text = "Enter COMMENT mode after current move."},
                {.shortcut = "A", .text = "Enter COMMENT mode before current move."},
                {.shortcut = "c", .text = "Enter CLIPBOARD mode."},
                {.shortcut = "C", .text = "Toggle board coordinates."},
                {.shortcut = "r", .text = "Rotate chessboard."},
                {.shortcut = "W", .text = "Write current game."},
                {.shortcut = "m", .text = "Start/stop machine 1."},
                {.shortcut = "M", .text = "Start/stop machine 2."},
                {.shortcut = "b", .text = "Show/hide machine output."},
                {.shortcut = "Space", .text = "Play best move from machine 1."},
                {.shortcut = "Shift-Space", .text = "Play best move from machine 2."},
                {.shortcut = "o", .text = "Start/stop explorer 1."},
                {.shortcut = "O", .text = "Start/stop explorer 2."},
                {.shortcut = "0-9", .text = "Explorer events."},
                {.shortcut = "l, Arrow Right, Mouse Scroll Up", .text = "Go to next move."},
                {.shortcut = "h, Arrow Left, Mouse Scroll Down", .text = "Go to previous move."},
                {.shortcut = "j, Arrow Down", .text = "Go to first sub variation."},
                {.shortcut = "k, Arrow Up", .text = "Go to parent variation."},
                {.shortcut = "L, Shift-Arrow Right", .text = "Go to last move."},
                {.shortcut = "H, Shift-Arrow Left", .text = "Go to first move."},
                {.shortcut = "J, Shift-Arrow Down", .text = "Go to next sub variation."},
                {.shortcut = "K, Shift-Arrow Up", .text = "Go to previous sub variation."},
                {.shortcut = "u", .text = "Undo last destructive operation."},
                {.shortcut = "Ctrl-r", .text = "Redo last destructive operation."},
                {.shortcut = "z", .text = "Increase size of chessboard."},
                {.shortcut = "Z", .text = "Decrease size of chessboard."},
                {.shortcut = "+", .text = "Increase font size."},
                {.shortcut = "-", .text = "Decrease font size."},
                {.shortcut = "p", .text = "Promote current variation."},
                {.shortcut = "P", .text = "Promote current variation and delete old."},
                {.shortcut = "i", .text = "Insert moves from parent variation to current variation until they are valid."},
                {.shortcut = "d", .text = "Delete current variation."},
                {.shortcut = "D", .text = "Delete remaining moves after current move."},
                {.shortcut = "PageUp, Ctrl+u", .text = "Scroll notation up."},
                {.shortcut = "PageDown, Ctrl+d", .text = "Scroll notation down."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModeGameList] = {
                {.shortcut = "q", .text = "Quit."},
                {.shortcut = "Escape", .text = "Return to NORMAL mode."},
                {.shortcut = "Return", .text = "Read current game."},
                {.shortcut = "r", .text = "Read game list from file."},
                {.shortcut = "p", .text = "Search current position."},
                {.shortcut = "j, Arrow Down", .text = "Move to next game."},
                {.shortcut = "f", .text = "Enter TEXTINPUT tag filter mode."},
                {.shortcut = "k, Arrow Up", .text = "Move to previous game."},
                {.shortcut = "/", .text = "Enter TEXINPUT search mode."},
                {.shortcut = "z", .text = "Increase size of chessboard."},
                {.shortcut = "Z", .text = "Decrease size of chessboard."},
                {.shortcut = "+", .text = "Increase font size."},
                {.shortcut = "-", .text = "Decrease font size."},
                {.shortcut = "c", .text = "Insert game title in comment after current move."},
                {.shortcut = "PageUp, Ctrl+u", .text = "Scroll game list up."},
                {.shortcut = "PageDown, Ctrl+d", .text = "Scroll game list down."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModeTagFilter] = {
                {.shortcut = "First the tag name and operator is inputted. Tag name and operator are split by single space. If it already exists, it will be edited - otherwise new tag filter is created.", .text = NULL},
                TEXTINPUT,
                {.shortcut = "Return", .text = "Confirm tag name and operator. And enter TEXTINPUT mode for editing tag filters. Tag filter will be removed if its value is empty string."},
                {.shortcut = "OPERATORS", .text = NULL},
                {.shortcut = "sort", .text = "Sort game list by tag. Tag can be \"File\" in which case the game list is just read from File. Correct values are Asc or Desc."},
                {.shortcut = "=", .text = "Exact match."},
                {.shortcut = "~", .text = "Case insensitive substring."},
                {.shortcut = ">", .text = "Greater than."},
                {.shortcut = "<", .text = "Lower than."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModeEdit] = {
                {.shortcut = "Mouse Click Left", .text = "Insert current piece at clicked square."},
                {.shortcut = "Mouse Click Right", .text = "Clear piece at clicked square."},
                {.shortcut = "q", .text = "Quit."},
                {.shortcut = "c", .text = "Clear board."},
                {.shortcut = "s", .text = "Set starting position."},
                {.shortcut = "f", .text = "Import FEN from clipboard."},
                {.shortcut = "r", .text = "Rotate chessboard."},
                {.shortcut = "0", .text = "Change color."},
                {.shortcut = "1", .text = "Select pawn of current color."},
                {.shortcut = "2", .text = "Select knight of current color."},
                {.shortcut = "3", .text = "Select bishop of current color."},
                {.shortcut = "4", .text = "Select rook of current color."},
                {.shortcut = "5", .text = "Select queen of current color."},
                {.shortcut = "6", .text = "Select king of current color."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModeFilename] = {
                {.shortcut = "Filename is used for reading and writing.", .text = NULL},
                TEXTINPUT,
                {.shortcut = NULL, .text = NULL}
            },
            [ModeNumber] = {
                {.shortcut = "Number represents game sequence. If value is \"a\" game is added at the end of file.", .text = NULL},
                TEXTINPUT,
                {.shortcut = NULL, .text = NULL}
            },
            [ModeSan] = {
                {.shortcut = "Valid SAN moves are played.", .text = NULL},
                TEXTINPUT,
                {.shortcut = "Return", .text = "If the move is correct it is cleared and played."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModeTag] = {
                {.shortcut = "First the tag name is inputted. If it already exists, it will be edited - otherwise new tag is created.", .text = NULL},
                TEXTINPUT,
                {.shortcut = NULL, .text = NULL},
            },
            [ModeTraining] = {
                {.shortcut = "After user plays move that exists in notation the opponents move will be played automatically - starting with main line. Move can be inserted with mouse or in SAN format.", .text = NULL},
                {.shortcut = "COMMANDS", .text = NULL},
                {.shortcut = "Restart", .text = "Start from begining - first game and its main line."},
                {.shortcut = "Repeat", .text = "Replay current line."},
                {.shortcut = "Next", .text = "Proceed to the next line or next file if all lines in current game are finished."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModeMoveAnnotation] = {
                {.shortcut = "q", .text = "Quit."},
                {.shortcut = "r", .text = "Rotate chessboard."},
                {.shortcut = "j, Arrow Down", .text = "Select next move NAG."},
                {.shortcut = "k, Arrow Up", .text = "Select previous move NAG."},
                {.shortcut = "x, Backspace", .text = "Delete move NAG."},
                {.shortcut = "Escape", .text = "Return to NORMAL mode."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModePositionAnnotation] = {
                {.shortcut = "q", .text = "Quit."},
                {.shortcut = "r", .text = "Rotate chessboard."},
                {.shortcut = "j, Arrow Down", .text = "Select next position NAG."},
                {.shortcut = "k, Arrow Up", .text = "Select previous position NAG."},
                {.shortcut = "x, Backspace", .text = "Delete position NAG."},
                {.shortcut = "Space", .text = "Input position NAG from machine 1."},
                {.shortcut = "Shift-Space", .text = "Input position NAG from machine 2."},
                {.shortcut = "Escape", .text = "Return to NORMAL mode."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModeComment] = {
                {.shortcut = "Comment is inserted after/before current move.", .text = NULL},
                TEXTINPUT,
                {.shortcut = NULL, .text = NULL}
            },
            [ModeClipboard] = {
                {.shortcut = "q", .text = "Quit."},
                {.shortcut = "f", .text = "Copy current FEN to clipboard."},
                {.shortcut = "Escape", .text = "Return to NORMAL mode."},
                {.shortcut = NULL, .text = NULL}
            },
            [ModePromotion] = {
                {.shortcut = NULL, .text = NULL}
            }
        }
    };
}
