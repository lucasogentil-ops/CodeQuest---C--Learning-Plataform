# CodeQuest development milestones

## 1. Learning design · Complete

Defined the learning flow from the topic map to a short lesson, an optional walkthrough, practice and feedback. Established the dark interface and selected eight topics spanning C fundamentals, pointers and linked lists.

## 2. Windows desktop foundation · Complete

Set up the C11 and raylib application, built the topic map and added screen navigation. Verified that the project builds and launches from PowerShell using the documented MSYS2 toolchain.

## 3. First lesson and randomised practice · Complete

Built the Arrays lesson and its missing-code question. Added a generator that varies the starting values, target index, amount and operation, then shuffles the answer choices. Checked 1,000 generated questions for distinct choices and exactly one correct answer.

## 4. Visual code walkthroughs · Complete

Added step-by-step examples for an array update, a pointer swap and C++ pass by value versus pass by reference. Each step highlights a line and shows its values or pointer targets. Verified forward and backward navigation, reset and the first and last step boundaries.

## 5. Eight lessons and guided practice · Complete

Added a lesson and question for every topic. The first incorrect answer gives a hint. A second incorrect submission unlocks the solution and its explanation. Only a correct submission counts toward session progress. Checked the practice flow across all eight topics.

## 6. Saved progress and badges · Next

Store learning progress locally and restore it when CodeQuest reopens. Award badges when learners meet a topic’s completion requirements. Verify that progress survives a restart and that repeated practice does not duplicate badges.

## 7. Windows release preparation · Planned

Check readability, navigation and save recovery. Build a standalone Windows package and verify that it launches and supports the full learning flow on a machine without the development toolchain.

## 8. Release and demonstration · Planned

Publish the verified Windows package with launch instructions and known limitations. Prepare a short demonstration of the topic map, visual walkthroughs, guided practice and saved progress.
