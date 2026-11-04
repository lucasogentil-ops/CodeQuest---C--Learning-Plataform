# CodeQuest development plan

Planning date: 3 November 2026. Final demo: 17 November 2026.

## Goal

Build a small Windows desktop app in C11 with raylib that helps students revise C and selected C++ concepts. Keep the code clear enough to explain in class: small functions, descriptive names, simple structs, arrays with explicit counts, and comments explaining important decisions.

This plan describes the intended scope. Actual completed work is recorded in the progress note and development journal. The interface concept is now being implemented in C and raylib.

## Agreed interface design

The initial interface concept was approved on 3 November 2026. Use it as the visual reference for the C and raylib implementation:

- CodeQuest branding with purple accents, dark navy backgrounds and clear rounded panels, matching the user's selected dark reference. Use bundled Noto Sans instead of raylib's pixel-style default font.
- A home topic map showing all eight numbered topics, left-aligned titles, status labels and an action to start learning. Use a quiet grid of cards with purple accent bars. Keep every topic accessible; do not draw connections that imply an inaccurate route.
- A lesson screen with the explanation beside a readable monospace code panel.
- A walkthrough screen with the current line highlighted, visible values and Previous, Next and Reset buttons.
- A practice screen with the code question beside answer options, followed by clear feedback, hints and retry controls.
- A progress screen showing topic badges and practice outcomes.

Keep the preview's layout and visual style while adapting sizing, scrolling and drawing to a Windows desktop window. The preview uses illustrative progress and one arrays lesson; the actual app must load the correct content for each topic. Design approval does not mark content planning or implementation as complete.

Maintain README.md as a public product and portfolio introduction for GitHub visitors, including potential employers. Explain what CodeQuest does, the learning experience, its technology and architecture, and how to build and run it. Keep academic deadlines and journal administration in the development documentation. After each milestone, update actual progress, verified setup instructions, validation results and limitations where relevant. Keep planned features separate from completed work; add real screenshots, a demo and release instructions when available.

## Scope and time

Use eight milestones. The brief allows approximately 10 hours across two weeks. The estimates below total 10 hours of focused development; installing tools, fixing unexpected problems and preparing recordings may require extra time. Re-estimate after the first build rather than treating these times as guarantees.

Keep all eight topic groups, with at least one short lesson and one missing-code question each. Include three prepared walkthroughs: changing an array, swapping with pointers, and passing by value versus C++ reference. Do not add a compiler, free-text code editor, accounts, online features, sound or complex animation to the first release.

## Milestones

Progress as of 3 November 2026: the interface concept is approved. Milestone 2 has a verified build and working screen navigation. A Milestone 1 design revision replaces the default font and light theme with readable bundled Noto Sans and the selected dark design. Both updates are recorded in docs/DEVELOPMENT_JOURNAL.md. Milestone 1 content mapping remains to be completed alongside the first lesson. Later learning features are not yet implemented.

### 1. Agree the screens and learning content — 45 minutes

Goal: know what we are building before writing app code.

- Review the interface preview: topic map, lesson, walkthrough, practice and results.
- Choose a simple layout designed for a 1280 by 720 window, with scrolling for longer content.
- List the eight topics and choose one source exercise for each.
- Plan three walkthroughs and their expected values at each step.
- Decide the completion rule: read the lesson, finish its walkthrough if it has one, and answer required questions correctly.

Done when: there is an agreed screen layout and a content checklist linked to the lecture references.

Journal evidence: design screenshot, topic list, and a short reason for the chosen layout.

### 2. Create a working C and raylib project — 1 hour

Goal: build and open the actual desktop app.

- Check existing compiler and build tools before installing anything.
- Set up CMake, raylib and cJSON using the planned Windows toolchain.
- Open a window, draw a title and button, respond to a mouse click and close cleanly.
- Add a screen enum and a simple switch to decide which screen to draw.
- Record working build commands and dependency versions in a README.

Done when: a clean build opens a window and a button changes the visible screen.

Code to understand: main(), the game loop, an enum, a switch and a button function.

Journal evidence: running-window screenshot, build command and any setup problem solved.

### 3. Build one complete learning journey — 1 hour 30 minutes

Goal: make a small usable version before adding every topic.

- Create simple structs for a lesson, question and answer option.
- Load one arrays lesson and one question from JSON through a separate loading function.
- Draw readable lesson text and a code panel.
- Let the learner select an answer, submit it and see feedback.
- Connect Map → Lesson → Practice → Results → Map.
- Keep drawing code separate from answer-checking code.

Done when: the learner can complete one topic journey with correct and incorrect answers. Hint and saving rules will be completed in later milestones.

Code to understand: structs, arrays, IDs, string comparison and a returned true/false result.

Journal evidence: short recording of the whole journey and a description of how an answer is checked.

### 4. Add visual code walkthroughs — 1 hour 15 minutes

Goal: show how code changes values step by step.

- Store prepared steps in JSON: highlighted line, explanation and displayed values.
- Add Previous, Next and Reset buttons with boundary checks.
- Complete the array, pointer swap and value/reference walkthroughs.
- Use labelled boxes for array elements and pointer targets; the addresses are illustrations.
- Clearly label C examples and C++ reference examples.

Done when: each walkthrough can move forward and backward, Reset restores the start, and all displayed states match the example code.

Code to understand: an array of steps, a currentStep index and checks that stop it leaving the array.

Journal evidence: screenshots of before/after states and one manually checked trace.

### 5. Add all topics and the full practice rules — 1 hour 30 minutes

Goal: cover the required material and help learners recover from mistakes.

- Add foundations, decisions/loops, arrays, 2D arrays, strings/scope, functions/parameters, pointers and linked lists.
- Provide at least one lesson and one question per topic, each with one correct answer and distinct wrong options.
- First wrong submission: show a concept hint without exposing the correct option.
- Allow a second submission before enabling Show solution.
- Viewing a solution does not count as success. Retry starts a fresh unanswered attempt.
- Keep an earlier successful completion even if later practice is wrong.
- Validate content types, IDs, references, gap tokens and walkthrough line numbers. Report useful errors.

Done when: all topics open freely and the question/retry/solution rules behave as described in the PRD.

Code to understand: conditions, attempt counters, practice states and loading data in a loop.

Journal evidence: content checklist and a recording showing an incorrect answer, hint, retry and solution.

### 6. Save progress and award badges — 1 hour 15 minutes

Goal: remember progress after closing the app.

- Track lesson viewing, completed walkthroughs, attempts, solved questions and viewed solutions.
- Calculate topic completion from these records; award each badge once.
- Save progress under the Windows local application data folder, separate from packaged lessons.
- Write and validate a temporary save, preserve a valid backup, then replace the current save.
- Show save failures and allow Retry. Keep learning usable if saving fails.
- Handle missing, damaged and unsupported saves without silently destroying them.
- Prevent two running instances from writing the same profile.

Done when: restarting restores progress and badges; deliberate invalid-save and failed-save checks produce clear messages and preserve valid data.

Code to understand: reading/writing files, JSON fields, return values and why temporary/backup files are used. This is likely the most involved milestone.

Journal evidence: before/after restart screenshots and notes from save/recovery checks.

### 7. Check usability and package the app — 1 hour

Goal: produce a dependable offline demo.

- Check readable text, wrapping, scrolling, mouse controls and back navigation at the target resolution.
- Make status clear using text or symbols as well as colour.
- Check every answer and walkthrough against the source material, including array boundaries, null terminators and linked-list edge cases.
- Run focused checks for answer rules, walkthrough bounds, completion and persistence.
- Package the executable, required runtime files, fonts and lesson content.
- Start the package from another working directory and verify it works offline without development tools where a clean Windows environment is available. Record any unverified packaging assumptions.

Done when: the packaged app completes the learning flow and remaining limitations are documented honestly.

Journal evidence: a completed check list, bug fixes and package screenshot.

### 8. Prepare the explanation and final demo — 45 minutes

Goal: confidently explain the app and the code you understand.

- At this final stage, create a visual guide showing which code sections are easiest to explain to the teacher.
- Start with screen switching, button clicks, answer checking and walkthrough indexes; then cover content loading and saving.
- Draw a small diagram linking the screen, learning functions and data files.
- Rehearse: open a topic → read → step through code → answer incorrectly → retry correctly → show badge → restart to show saved progress.
- Finish the journal and record the final screencast. Credit external libraries and describe AI assistance according to course requirements.

Done when: the final package, journal and demo are ready, and you can explain the main code in your own words.

Journal evidence: final recording, explanation diagram, limitations and a reflection on what you learned.

## Working towards 17 November

The only deadline used for this development plan is 17 November 2026. There are no Tuesday or Friday submission checkpoints. Work through the eight milestones in order and write a journal entry whenever a milestone is completed, regardless of the day.

Aim to finish the main features before the final few days so there is time for testing, fixes, packaging and rehearsal. The milestone time estimates are working estimates, not separate deadlines.

By 17 November, have the working app, final package, completed journal and demo explanation ready. Keep screenshots and short recordings as evidence while developing; organise them by milestone.

## Journal entry template

Copy this after each milestone. Write what actually happened; do not paste planned features as completed work.

- Date and milestone:
- My goal:
- What I completed:
- Files or functions I changed:
- How the code works, in my own words:
- A problem I met and how I handled it:
- How I checked it, and the result:
- Evidence: screenshot, recording or code example:
- Time spent:
- What is still unfinished:
- What I learned and what I will do next:

## Suggested code structure

Start small and split files when they gain a clear responsibility. Avoid creating many empty modules at the beginning.

```text
src/
  main.c             Open the app, run the loop and clean up
  screens.c/.h       Draw screens and handle navigation
  ui.c/.h            Shared buttons, code panels and scrolling
  learning.c/.h      Answers, walkthrough steps and completion
  content.c/.h       Load and validate lesson JSON
  progress.c/.h      Load, save and recover progress
content/
  catalogue.json
  lessons/
assets/
tests/
docs/
CMakeLists.txt
README.md
```

The architecture document uses more detailed folders. These smaller starting modules keep the same separation and can move into those folders as the app grows.

## If time runs short

Cut decorative animation, custom artwork, extra questions and additional walkthroughs first. Keep the eight required topic groups, three required walkthroughs, correct feedback, progress rules and core checks. If the remaining work exceeds available time, record the gap and revise the schedule explicitly rather than claiming incomplete requirements are finished.
