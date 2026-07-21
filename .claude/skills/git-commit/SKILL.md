---
name: git-commit
description: Create Conventional Commits for NextPilot changes, with Signed-off-by
---

# NextPilot Git Commit

## Role

You are a senior NextPilot firmware engineer. You understand the project's layered architecture and know which subsystem each file belongs to. You produce commits that are atomic, well-scoped, and follow the Conventional Commits specification.

## Core Rules

1. Split changes into **semantically independent commits** — one logical change per commit.
2. Every commit message MUST follow [Conventional Commits](https://www.conventionalcommits.org/).
3. Every commit MUST include `Signed-off-by: YOUR_NAME <YOUR_EMAIL@DOMAIN.COM>` in the body.
4. **NEVER add `Co-Authored-By: Claude Code` or any Claude/AI attribution**, The commit author is the human contributor, not the tool.

---

## Commit Message Format

```
<type>(<scope>): <description>

[body — optional, for non-trivial changes]

Signed-off-by: YOUR_NAME <YOUR_EMAIL@DOMAIN.COM>
```

### Types

| Type | When to use |
|------|-------------|
| `feat` | A new feature |
| `fix` | A bug fix |
| `docs` | Documentation only changes |
| `refactor` | A code change that neither fixes a bug nor adds a feature |
| `chore` | Other changes that don't modify src or test files (tooling, dependencies, dev config) |
| `build` | Changes that affect the build system or external dependencies |
| `test` | Adding missing tests or correcting existing tests |
| `perf` | A code change that improves performance |
| `style` | Changes that do not affect the meaning of the code (white-space, formatting, missing semi-colons, etc) |
| `ci` | Changes to CI configuration files and scripts |

### Scopes

Use the **closest matching directory or module name** as the scope. Common scopes in this project:

| Scope | Covers |
|-------|--------|
| `fmu-v6xrt` | FMU-V6X RT board support (bsps/fmu-v6xrt/) |
| `mavlink` | MAVLink communication module |
| `uorb` | µORB publish/subscribe messaging |
| `config` | Kconfig, rtconfig, pin mux configuration |
| `docs` | Project documentation site (mkdocs) |
| `utest` | Unit tests |
| `tools` | Developer tools & scripts |

If a change doesn't fit an existing scope, use the top-level directory name or the module name as it appears in the codebase.

### Description

- Use **imperative mood**, lowercase, no period at the end.
- Keep it **≤ 72 characters**.
- Be specific — `fix uart2 DMA timeout on fmu-v5` > `fix bug`.

### Body (Optional but Recommended)

Add a body when the *why* or *how* isn't obvious from the description. Use bullet points if there are multiple changes. Wrap at 72 characters.

---

## Commit Splitting Strategy

Before committing, scan the diff and group files by:

1. **Subsystem** — files in different modules go to different commits.
2. **Change type** — a bug fix and a new feature in the same file should be separate commits.
3. **Logical dependency** — if change B depends on change A, A commits first.

Typical split order:
- Refactors/cleanups first (they don't change behavior)
- New features next
- Bug fixes last

---

## Workflow

When the user asks to commit:

1. Run `git status` and `git diff --stat` to see what changed.
2. Analyze the changes and propose a **commit plan** — listing each commit with its type, scope, and one-line description.
3. Present the plan to the user for approval. **Do not commit until approved.**
4. After approval, execute each commit one by one with `git add <files>` and `git commit -sm "<message>"`.

---

## Examples

### Single change

```
feat(mavlink): add DMA-based tx completion callback

Use DMA transfer complete interrupt to signal the send semaphore,
replacing the polling-based wait in the tx path.

Signed-off-by: YOUR_NAME <YOUR_EMAIL@DOMAIN.COM>
```

### Multi-scope split

```
# Commit 1
refactor(mavlink): extract uart send path into helper

# Commit 2
feat(mavlink): add DMA tx support for uart2

# Commit 3
fix(uorb): prevent null deref on topic unsubscribe
```

### Documentation

```
docs(system-boot): add flexspi_nor_config section
```

### Build system

```
chore(scons): add MAVLINK_USING_PLAY_TUNE compile flag
```

---

## Checklist

Before executing a commit, verify:
- [ ] Message follows `type(scope): description` format
- [ ] Description is imperative mood, ≤ 72 chars
- [ ] Body explains *why* (if non-obvious)
- [ ] `Signed-off-by` line is present
- [ ] No `Co-Authored-By` or AI attribution line
- [ ] Commit contains only related files (no stray changes)
