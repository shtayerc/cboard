#ifndef HELP_H_
#define HELP_H_
#include <stddef.h>

typedef enum {
    ModeNormal,
    ModeGameList,
    ModeTagFilter,
    ModeEdit,
    ModeFilename,
    ModeNumber,
    ModeSan,
    ModeTag,
    ModeTraining,
    ModeMoveAnnotation,
    ModePositionAnnotation,
    ModeComment,
    ModeClipboard,
    ModePromotion,
    ModeCount
} Mode;

typedef struct {
    char* shortcut;
    char* text;
} HelpRow;

typedef struct {
    HelpRow mode[ModeCount][47];
} Help;

Help help_init();

#endif
