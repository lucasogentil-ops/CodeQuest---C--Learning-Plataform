#ifndef CODEQUEST_PROGRESS_H
#define CODEQUEST_PROGRESS_H

#include <stdbool.h>

enum { PROGRESS_TOPICS = 8, NO_SCORE = -1 };

/* Chapter badges, earned from the best chapter score (percentage of first answers correct). */
typedef enum { BADGE_NONE, BADGE_AVERAGE, BADGE_EXPERT, BADGE_MASTER } BadgeTier;
enum { AVERAGE_SCORE = 50, EXPERT_SCORE = 80, MASTER_SCORE = 100 };

typedef struct {
    bool viewed, traced;
    int attempts;
    int bestScore;              /* 0-100, or NO_SCORE until the chapter is finished once */
} TopicProgress;

typedef struct {
    TopicProgress topics[PROGRESS_TOPICS];
    const char *notice;
    bool retry;
} Progress;

BadgeTier BadgeForScore(int score);
const char *BadgeName(BadgeTier tier);
void OpenProgress(Progress *progress);
void SaveProgress(Progress *progress);
void CloseProgress(void);

#endif
