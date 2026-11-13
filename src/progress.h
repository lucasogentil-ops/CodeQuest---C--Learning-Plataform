#ifndef CODEQUEST_PROGRESS_H
#define CODEQUEST_PROGRESS_H

#include <stdbool.h>

enum { PROGRESS_TOPICS = 8 };

typedef struct {
    bool viewed, traced, solved, solution;
    int attempts;
} TopicProgress;

typedef struct {
    TopicProgress topics[PROGRESS_TOPICS];
    const char *notice;
    bool retry;
} Progress;

void OpenProgress(Progress *progress);
void SaveProgress(Progress *progress);
void CloseProgress(void);

#endif
