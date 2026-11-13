# CodeQuest

CodeQuest is an offline Windows desktop app for learning C and selected C++ concepts. Pick a topic, read a short code example, and practise by choosing the missing line. Three interactive walkthroughs show how values change as you step through an example.

![CodeQuest topic map with eight learning topics](docs/images/topic-map.jpg)

## What you can do

- **Explore any topic.** All eight topics are available from the start, each with a lesson, a code example and a practice question.
- **Step through code.** Follow an array update, a pointer swap in C, or a comparison of pass by value and pass by reference in C++. Use **Next**, **Previous** and **Reset** to inspect prepared examples with highlighted lines and visible values.
- **Practise with feedback.** A wrong answer gives you a hint. A second wrong answer on the same question unlocks the solution and its explanation. Use **Try again** to retry the question.
- **Generate more Arrays questions.** **New question** varies the starting values, target index, amount and operation. The answer choices are shuffled.
- **Earn badges and resume your progress.** **My progress** shows badges for topics where you read the lesson, finish its walkthrough if available, and submit a correct answer. Progress is saved under `%LOCALAPPDATA%\CodeQuest`.

## Topics

**Exam** gives you five questions from different topics and shows your score at the end. Exam results are separate from practice badges.

| Topic | Example |
| --- | --- |
| C foundations | Print an integer with `printf` |
| Decisions and loops | Choose the stopping condition for a `for` loop |
| Arrays | Add to or subtract from an element by index |
| Two-dimensional arrays | Read a cell using row and column indexes |
| Strings and scope | Stop counting at the null terminator |
| Functions and parameters | Return a value and compare copies with references |
| Pointers | Assign through a pointer and swap two values |
| Linked lists | Link one node to the next |

## Run on Windows

Download the [Windows x64 ZIP](https://github.com/lucasogentil-ops/CodeQuest---C--Learning-Plataform/releases/tag/v0.1.0), extract it and open `codequest.exe` from the extracted CodeQuest folder. Keep its DLLs, assets and content together. From a source checkout, run `.\scripts\package.ps1` to build `dist/CodeQuest-Windows-x64.zip`.

## Build from source

The current version runs from source. Use MSYS2 UCRT64 with GCC, CMake, Ninja, raylib 6.0 and cJSON. The verified toolchain used GCC 16.2.0, CMake 4.4.4, Ninja 1.13.2 and cJSON 1.7.19.

Open PowerShell in the repository root:

```powershell
.\scripts\build.ps1
.\scripts\run.ps1
```

The scripts default to `C:\msys64\ucrt64`. If MSYS2 is installed elsewhere, pass the same toolchain location to both scripts:

```powershell
.\scripts\build.ps1 -ToolchainRoot 'D:\msys64\ucrt64'
.\scripts\run.ps1 -ToolchainRoot 'D:\msys64\ucrt64'
```

The build places `codequest.exe`, `assets/` and `content/` under `build/`. The run script supplies the MSYS2 runtime path. To try the app, select **Explore Arrays**, then choose **Walkthrough** or **Try a question**. Press **Escape** to exit.

## How it’s built

CodeQuest uses C11 and raylib for the desktop interface. cJSON loads the lessons, questions and prepared walkthrough steps from `content/`. Lesson text and examples can be edited there independently of the screen code. The app runs offline without an account.

The Windows build has passed with compiler warnings enabled. Checks with simulated button presses covered all eight practice flows and the navigation boundaries of all three walkthroughs.

## Roadmap

| Status | Work |
| --- | --- |
| Available | Eight lessons, three visual walkthroughs, randomised Arrays practice, hints, retries and solution explanations |
| Available | Local progress, save recovery and topic badges |
| Release follow-up | Verify the packaged app on a separate Windows machine and record the final demo |

See the [development milestones](MILESTONES.md) for completed work and upcoming release goals.

Created by **Lucas Oliveira and Quentin Allegre**.
