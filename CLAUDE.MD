1. ⁠Ask, don't assume. If something is unclear, ask before writing a  single line. Never make silent assumptions about intent, architecture,  or requirements.
2. ⁠Simplest solution  first. Always implement the simplest thing that could work. Do not add  abstractions or flexibility that weren't explicitly requested.
3. ⁠Don't touch unrelated code. If a file or function is not directly part of the current task, do not modify it, even if you think it could be  improved.
4. ⁠Flag uncertainty explicitly.  If you are not confident about an approach or technical detail, say so  before proceeding. Confidence without certainty causes more damage than  admitting a gap.

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

- [ ] Version number incremented in filename
- [ ] All imports present and correct
- [ ] Error handling added for new operations
- [ ] UI elements use customtkinter (ctk) components
- [ ] Mono conversion preserved in audio processing
- [ ] Cue point calculation logic intact
- [ ] Print statements for user feedback
- [ ] Success/error messageboxes for important operations
- [ ] File paths handled with os.path
- [ ] No hardcoded absolute paths
- [ ] README.md updated with version number
- [ ] Code tested with multiple WAV files
- [ ] Output verified in audio editor

### When Making Changes

1. **Create new version file** - don't overwrite existing working versions
2. **Increment version number appropriately** - follow the established pattern
3. **Add error handling** - use try/except with messagebox.showerror
4. **Maintain UI consistency** - keep customtkinter style and 350x510 window size
5. **Update README.md** - reflect version changes and new features
6. **Test with real M8 hardware if possible** - or at minimum verify cue points in audio editor
7. **Document known issues** - be transparent about limitations

