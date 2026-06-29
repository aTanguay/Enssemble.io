# Development Instructions

## Framework

- Use native ESP-IDF.
- Do not introduce Arduino dependencies unless explicitly approved.
- Target board: ESP32-S3.
- Write new application code in C++ where appropriate.
- Keep hardware drivers in separate components.

## Build and Validation

After making code changes, run:

    idf.py build

Do not consider a task complete unless the project builds without errors.

When hardware is attached, use:

    idf.py -p <PORT> flash monitor

## Project Documentation

- README.md describes currently implemented behavior.
- PLANNING.md contains design decisions and planned behavior.
- TASKS.md is the authoritative task list.
- Update TASKS.md when completing work.
- Do not describe planned features as currently implemented.