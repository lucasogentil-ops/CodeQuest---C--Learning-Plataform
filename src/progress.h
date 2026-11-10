#ifndef CODEQUEST_PROGRESS_H
#define CODEQUEST_PROGRESS_H

#include <stdbool.h>
#include <stdint.h>

/* Same order and count as the topic enum in screens.h. */
enum { PROGRESS_TOPICS = 8 };

/* Badge bits, stored per topic. Setting a bit twice changes nothing,
   so repeated practice can never duplicate a badge. */
enum {
    BADGE_COMPLETED  = 1u << 0, /* answered the practice question correctly */
    BADGE_FIRST_TRY  = 1u << 1, /* correct on the first submission of a question */
    BADGE_PERSISTENT = 1u << 2  /* correct after at least one wrong submission */
};
#define BADGE_KINDS 3
#define BADGE_MASTER (1u << 0)  /* global: every topic completed */

typedef struct {
    uint32_t attempts;  /* submitted answers */
    uint32_t correct;   /* correct submissions */
    uint32_t badges;    /* BADGE_* bit mask */
} TopicProgress;

typedef struct {
    TopicProgress topics[PROGRESS_TOPICS];
    uint32_t globalBadges;
} Progress;

typedef enum {
    PROGRESS_LOADED,   /* file read and verified */
    PROGRESS_MISSING,  /* no save file: fresh progress */
    PROGRESS_CORRUPT   /* bad magic, version, size or checksum: fresh progress */
} ProgressStatus;

void ProgressReset(Progress *progress);
/* Always leaves *progress valid (fresh data on MISSING or CORRUPT). */
ProgressStatus ProgressLoad(const char *path, Progress *progress);
/* Writes a temporary file, then replaces the save. Returns false on failure. */
bool ProgressSave(const char *path, const Progress *progress);
/* Records one submission. firstSubmission: first answer given to the
   current question (a hint only appears after a wrong one). Returns the badges newly earned by it
   (topic badges in the low bits, BADGE_MASTER << 8 for the global one). */
uint32_t ProgressRecordAnswer(Progress *progress, int topic, bool correct,
                              bool firstSubmission);
int ProgressCompletedTopics(const Progress *progress);

#endif