# Shared Factory tools

Ghidra, JDK and the Wine executable are provided by the common Factory
installation. This repository's compiler and Ghidra frontends use that runtime.
The local launcher works without Factory MCP or a network connection:

```bash
./scripts/shared_runtime.py --describe ghidra
./scripts/shared_runtime.py java -- -version
./scripts/shared_runtime.py --describe wine
./scripts/shared_runtime.py wine -- cmd /c ver
./scripts/shared_runtime.py winepath -- -w "$PWD/build/probe.obj"
```

Use the existing repository Ghidra script for its target checks and project
lock. Direct headless arguments can also be passed with
`./scripts/shared_runtime.py ghidra -- ...`. Keep the original locked tool
version; moving the installation does not authorize changing tool hashes.

For bounded bytes that Ghidra has not defined as strings or data, use the
attested read-only query path:

```bash
scripts/repo-python scripts/ghidra.py query \
  .analysis/anm-version-message.txt read_bytes 96 0x004A5D14
```

`read_bytes COUNT ADDRESS...` returns hex bytes for each address. Count must
be 1..4096 and each range must stay in one initialized memory block. This
checks mapped memory bounds; it does not establish function ownership.

The ignored `.tools/shared-runtimes.json` chooses the installed versions,
repository-owned Wine prefix and architecture. `.tools/ghidra`, `.tools/jdk`
and `.tools/factory-runtime.py` are local selectors. Factory's provisioner
refreshes them if its checkout moves. Do not copy a complete installation into
this repository or borrow another game's prefix/project. Consult Factory
`docs/shared-runtimes.md` for provisioning and sandbox mounts.

Java home, temporary files, settings and caches stay below this repository's
`.analysis/factory-runtime/ghidra`. The existing analyzer project, compiler
profile, target and exactness evidence stay here too. Wine's normal prefix is
selected from this repository's configuration, independently of an ambient
`WINEPREFIX`. An explicit local probe prefix can be selected with
`./scripts/shared_runtime.py --prefix .analysis/probe-prefix wine -- ...`.
External prefixes must be the exact legacy path configured for this game.
Do not use the operator's global Wine prefix. Factory rejects prefixes already
owned by another repository and serializes Wine launches within this game.
Use existing headless/Xvfb compiler wrappers where required. Historical
verifier recipes naming system Wine retain the same shared binary and their
recorded game-specific prefix/profile.

Read-only source access to other registered games remains available through
Factory for cross-reference. Shared tools do not grant writes to their mutable
state. Ordinary analysis and source browsing do not require a full replay.
