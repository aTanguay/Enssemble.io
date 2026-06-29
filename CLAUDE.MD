1. Ask, don't assume. If something is unclear, ask before writing a single line. Never make silent assumptions about intent, architecture, or requirements.
2. Simplest solution first. Always implement the simplest thing that could work. Do not add abstractions or flexibility that weren't explicitly requested.
3. Don't touch unrelated code. If a file or function is not directly part of the current task, do not modify it, even if you think it could be improved.
4. Flag uncertainty explicitly. If you are not confident about an approach or technical detail, say so before proceeding. Confidence without certainty causes more damage than admitting a gap.

## AI Assistant Session Workflow

**MANDATORY FIRST STEPS**:

1. Read `PLANNING.md` (architecture, design decisions, calibration procedure)
2. Read `TASKS.md` (current priorities and status)
3. Understand task context before coding
4. Read README.MD to get familiar with the goals of the project

**During Development**:

- Follow naming conventions and safety patterns above
- Mark tasks complete in TASKS.md immediately
- Document architectural decisions in PLANNING.md
- Safety-first: validate inputs, fail-safe defaults, test on hardware

**Session History**: See end of `PLANNING.md` for development session logs

### Code Review Checklist

Before committing changes, verify:

- [ ] Code compiles cleanly with `idf.py build`
- [ ] All `#include` directives present and correct
- [ ] Error handling uses ESP-IDF patterns (`ESP_ERROR_CHECK`, `esp_err_t`)
- [ ] Platform-specific code isolated in appropriate components
- [ ] Shared BLE/MIDI layer remains platform-agnostic
- [ ] No hardcoded absolute paths
- [ ] README.md updated if user-facing behavior changed
- [ ] Tested on target hardware if available

### When Making Changes

1. **Build before committing** — `idf.py build` must pass
2. **Keep platform code separate** — XIAO/SAM2695 and AMYboard/AMY in their own components
3. **Add error handling** — use ESP-IDF error checking patterns
4. **Update README.md** — reflect any user-facing changes
5. **Test with real hardware if possible** — or at minimum verify BLE advertising and MIDI parsing in logs
6. **Document known issues** — be transparent about limitations
