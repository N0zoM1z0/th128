# Historical reconstruction checkpoint notes

These notes preserve the previous handoff advance section verbatim from local
checkpoint `e3e2036`. Counts, frontiers and receipts describe those historical
snapshots; use [the current handoff](RE_HANDOFF.md) and canonical ledgers for
current state. [The knowledge base](KNOWLEDGE_BASE.md) holds the corresponding
findings and failed approaches.

## Latest advance (2026-11-26)

`SupervisorScreenshot.cpp` now has two complete VC10 RTM `/GL /Ob1 /Oy-`
matches: CaptureSupervisorScreenshot (`0x004343D0`, **520B / 39 fields**)
and WriteSupervisorScreenshot (`0x00434270`, **352B / 34 fields**).
The maintained capture prepares bottom-up 640x480 24-bit BMP data and
starts the real CRT writer callback. The worker uses the maintained
FileSystem stream bodies, frees both buffers and clears its worker slot.
The target's format refusals, ignored HRESULTs and allocation/early-return
behavior remain intact. A natural row-loop/pointer-initialization ordering
change closed capture; no owner flags were varied.

The screenshot handler and four-slot ANM surface-copy consumer also have
source, remaining frontiers at 301/168 and 369/371 bytes. The manager's
four prefix records contain source rectangle coordinates before destination
coordinates, established by the D3DX call arguments. FileExists is inlined
into the handler; reusing the existing FsErrUtil call probe did not help.
The header's one existing exact consumer, AnmViewportTan, passed cold replay.
See the [finding](KNOWLEDGE_BASE.md#screenshot-capture-writer-and-pending-surface-copies-2026-11-26)
and [affected scope](../.analysis/screenshot-final-scope.json).

Present now links the real screenshot/copy/FileSystem bodies, retaining
189/190 and 323/314 bytes; its timer/state support owners remain 266/802B.
The [final seven-owner comparisons](../.analysis/screenshot-final-results.json)
accept capture, writer and the existing tangent wrapper, and reject the
four complete frontiers. The affected scope spans four contexts; current
receipts are valid for the retained sources and profiles. Frame-entry
contexts still need the real Present/screenshot/copy bodies integrated.

Previous render-state checkpoint (`6047666`):

`InitializeSupervisorRenderState` (`0x00455A70`) is newly exact at **802
bytes / 37 DIR32 fields**, with the fixed VC10 RTM `/GL /Ob1 /Oy-` profile.
The first natural implementation reproduces all 34 COM calls, three
float-to-DWORD conversions and the conditional ANM cache reset. The entry
PUSH ECX is a local float slot, not an incoming parameter. Adjacent byte
assignments naturally produce the cache's `0xFF07` word store.

Both Present contexts now include this maintained body. Complete comparison
still rejects Immediate at 189/190 and WithSleep at 323/314 bytes; the real
initializer does not explain those remaining layout differences. Its
support owner stays 802 bytes and the canonical timer stays 266. Screenshot
handling and the post-Present timing/replay hooks remain external. The old
frame-entry contexts still omit Present bodies.

Only the initializer and Present contexts required cold builds. No changed
input is consumed by an existing exact context; the one new exact owner and
two affected frontiers received complete comparisons. See the
[finding](KNOWLEDGE_BASE.md#initial-d3d9-state-and-anm-cache-reset-2026-11-26),
[final results](../.analysis/state-reset-final-results.json) and
[affected scope](../.analysis/state-reset-final-scope.json).

Previous surface/Present checkpoint (`b2a0df4`):

`RestoreSupervisorSurfaces` (`0x00435D20`) is newly exact at 217 bytes,
including all three real BindVmScript fields and the runtime viewport copy.
It gets level-zero texture surfaces, binds `0x52/0x55/0x52`, tests the first
pointer after the first binding, and gets the mono back buffer through the
SDK GetBackBuffer call. The receiver is Supervisor `+8`; the copied
`0x118`-byte blocks remain opaque beyond their reviewed fields.

Immediate/WithSleep Present now have maintained source with complete
21/30-field units. Both remain frontiers at 189/190 and 323/314 bytes.
Immediate differs in final branch/pop layout; tail-expression/null-guard
variants did not close it. The canonical timer profiles restore its support
owner to 266 bytes but leave WithSleep's caller frame. A normal-COFF timer
support diagnostic shrank Present to 313 bytes but also shrank the timer
to 259 and lost its independent alignment. That hypothesis was rejected;
the maintained context retains the canonical all-LTCG timer configuration.
Removing the named double local did not help either. Original TU ownership
is still unproven; pursue real callee/TU context with fixed owner profiles.
Screenshot handling and D3D state initialization remain external; the
latter has no live incoming ECX argument.

The frame header now names its screenshot member and both maintained
Present bodies. Its one affected existing exact owner, AnmViewportTan,
passes complete replay. Final comparison covers that wrapper plus the
three new owners across three cold contexts: surface restoration passes,
both Present frontiers are rejected. See the
[finding](KNOWLEDGE_BASE.md#surface-restoration-and-maintained-present-paths-2026-11-26),
[final results](../.analysis/surfaces-present-preserved-timer-final-results.json) and
[affected scope](../.analysis/surfaces-present-preserved-timer-final-scope.json).

Previous device-texture checkpoint (`60017e8`):

`SupervisorDevice.cpp/.hpp` now maintains both device-loss texture passes:
ReleaseAnmRenderTargets (`0x004536A0`, 124B / one field) and
RecreateAnmRenderTargets (`0x00453720`, 165B / six fields). Both pass the
complete cold linked Oracle, adding two authored exact owners / 289 bytes.
They walk all 38 slots; release rereads the slot/array after COM calls,
while recreation uses current back-buffer dimensions/format and preserves
the flag and HRESULT behavior. Slot advance before count decrement and
direct field expressions recover the target without hints or fake callees.

The new TU/header/probe do not change any existing exact unit's inputs.
Only their one new context was cold-built; both complete comparisons pass.
The existing standalone CreateRenderTarget remains unchanged. See the
[finding](KNOWLEDGE_BASE.md#render-target-release-and-recreation-2026-11-26),
[full comparisons](../.analysis/device-target-final-results.json) and
[affected scope](../.analysis/device-target-final-scope.json).

Previous frame/camera checkpoint (`daee2ac`):

`SupervisorFrame.cpp` now maintains the three real calc/draw frame entries
at `0x004547D0/0x00454BD0/0x00454E50`. Their receiver is the view rooted at
Supervisor `+0xB10`, not the camera. Signed frame-skip counting, clock rollback,
the timed 1.5ms sleep threshold, 60Hz deadline catch-up and the real
BeginScene/reset/fog/draw/EndScene sequence are present. The `0xFF` store
invalidates the fog cache at `0x004D37F8`; it is not a color. The three
presentation bodies remain external reviewed boundaries. Frame entries
remain frontiers at 360/360, 453/443 and 288/288 bytes, including private
presentation-receiver and return-scheduling differences.

`AnmViewport.cpp/.hpp` maintains the default 2D camera, both D3DX imports,
D3D transforms and manager view offsets. The camera remains 339/343 bytes.
The float tangent wrapper at `0x00435EC0` passes its complete 31-byte owner
and real `__CItan` call identity. Its existing compiler/header exclusion
remains; these bytes do not add authored exact credit. Component-wise
vector statements and a 343-byte dimensions-vector variant did not close
the camera; neither was
adopted. The actual frame callers still leave ResetBuffers with its same
23 ordinary differences. See the
[frame/viewport finding](KNOWLEDGE_BASE.md#frame-entry-points-and-the-2d-viewport-2026-11-26).

All 97 affected existing exact owners pass the final current-input replay.
The 116-owner comparison passes 98 owners (97 retained + the compiler
wrapper) and rejects 18 frontiers. Eleven contexts were cold-built; the
unchanged final-input AnmViewport context from the preceding cold build was
reused with its receipt rechecked. This adds four authored mappings and one
excluded compiler/header mapping. Authored exact totals remain 322 owners /
19,844 bytes. See the [full results](../.analysis/frame-current-results.json),
[replay log](../.analysis/frame-current-replay.log),
[existing exact selection](../.analysis/frame-current-selected.json) and
[affected contexts](../.analysis/frame-current-contexts.json).

Previous rectangle checkpoint (`b46df32`):

The real rectangle/border definitions restore all 23 DrawLayer callbacks
to canonical exact status: 342 bytes, each with one real DrawVmGroup REL32
identity. Their forwarding source is unchanged. DrawVmGroup and DrawSingleVm
still fail their complete-owner checks. The synthetic renderer remains removed.

Solid/gradient rectangles, both border wrappers and ResetBuffers now have
maintained source and full linked units. The rectangle path uses the
passed manager's cursor/format cache but flushes the global sprite manager
and uses its stage cache. Borders halve alpha, expand width/height by one,
then draw the original shape. All five remain frontiers: 517/533, 520/535,
141/139, 163/160 and reset 56/56 with 23 ordinary differences. Real sin/cos
calls still differ from target FSINCOS and affect caller registers/alignment.

`AnmDrawBuffers` records the two observed storage regions: 131,072 × 28-byte
sprite vertices and 16,384 × 20-byte primitive vertices, with two counters
and two cursor/start pairs. Reset's zero-materialization schedule remains
unmatched. Five position-vector source probes also failed; the original
108-byte/eight-difference helper is retained. See the
[rectangle/buffer finding](KNOWLEDGE_BASE.md#rectangle-drawing-and-paired-vertex-buffers-2026-11-26).

All 79 affected existing exact owners pass after 11 cold context builds.
The complete 116-owner comparison passes 102 owners (79 retained + 23
restored) and rejects 14 frontiers. Draw's current candidate is 1,452/1,392
bytes. This checkpoint adds five authored source mappings and restores
23 exact rows / 342 bytes. See the
[full results](../.analysis/anm-rectangle-current-results.json),
[replay log](../.analysis/anm-rectangle-current-replay.log),
[selected existing exact owners](../.analysis/anm-rectangle-current-selected.json)
and [cold contexts](../.analysis/anm-rectangle-current-contexts.json).

Previous submission checkpoint (`4ab71e2`):

`AnmDrawLeaf::SubmitSprite` (`0x0045E920`) now has complete natural source
and a 100-field linked unit. The real path covers offset/snap/cache/UV,
viewport culling, texture and vertex-format state, all four color layouts,
modulation, ApplyState and AppendSprite. All linked draw contexts include
the actual BlendColorChannel definition. Direct color statements recover
the target private manager ECX / VM EAX / stack flags ABI; an inline color
helper had stayed out of line and was rejected.

The complete owner remains a frontier at 1,660/1,532 bytes. Mathematical
ambient-mode edge rounding passes 25 bounded scalar checks with the real
selected CRT. Its control-mode query, four helper calls and stack alignment
still differ from target FRNDINT; floating-point exception equivalence is
unverified. Pinned compiler probes reject rint/nearbyint/_frnd intrinsics;
even an accepted floor pragma retains a real call. Do not repeat those
probes or force alignment/registers to hide the mismatch. See the
[submission finding](KNOWLEDGE_BASE.md#maintained-sprite-submission-and-ambient-rounding-2026-11-26)
and [canonical rejection](../.analysis/anm-submit-current-results.json).

All 74 affected existing exact owners pass after 10 cold linked builds.
All 106 selected owners were compared; the 32 frontiers, including the
23 DrawLayer callers, remain rejected. This checkpoint adds one authored
source mapping and no exact credit. See the
[complete replay](../.analysis/anm-submit-current-results.json) and
[affected contexts](../.analysis/anm-submit-current-contexts.json).

Previous state/append checkpoint (`2d2cdee`):

`AnmDrawLeaf::ApplyState` (`0x0045E770`, 416B) and `AppendSprite`
(`0x0045F040`, 138B) now pass the canonical linked Oracle, adding two exact
owners / 554 bytes. ApplyState uses the passed manager, rereads VM flags
after flush, and controls MAG/MIN sampler filtering with SetSamplerState;
the old one-VM approximation now delegates to it. Target-visible case order
6 before 5 naturally recovers the complete code and seven-entry table.
The quad append uses six struct assignments in TL/TR/BL, TR/BL/BR order,
with a cursor reload before each copy; it has no address fields.

Four real sprite wrappers and the initially zero shared transformed quad
now have maintained source. The unrotated entry's stack order is manager
then VM. All four wrappers remain rejected frontiers; submission at
`0x0045E920` stays external. Its complete 1,532-byte extent, UV/color/cull
paths and four-entry table are reviewed. FRNDINT follows the ambient x87
rounding mode, so neither floor nor a fixed nearest-even replacement closes
that source contract. See the [state/append finding](KNOWLEDGE_BASE.md#anm-sprite-state-quad-append-and-real-corner-wrappers-2026-11-26),
[state replay](../.analysis/anm-sprite-state-canonical.json) and
[append replay](../.analysis/anm-sprite-append-canonical.json).

All 77 existing affected exact owners and the two new exact owners pass
after 11 cold context builds. All 87 selected owners were compared; the
remaining eight candidates still fail the complete-owner check (seven
extent rejections and the draw-position helper's eight byte differences).
No former caller credit was restored. See the
[full results](../.analysis/anm-sprite-deps-current-results.json) and
[final scope](../.analysis/anm-sprite-deps-final-scope.json).

Previous draw checkpoint (`2263a9f`):

`AnmVm::Draw` (`0x00461B00`) now has maintained source in `AnmDraw.cpp`,
replacing the probes' volatile `flags ^ nextVmId` renderer. The helpers
probe's fabricated binding results and the old unused one-argument draw
approximation are also removed. Every linked manager/helper context now
includes the real dispatcher; unknown leaf implementations stay external.
Its 1,392-byte target includes 22/7-entry tables and 58 reviewed fields.
That checkpoint candidate was 1,324 bytes; with the real wrappers it is
now 1,348 bytes. The 108-byte draw-position helper still has
eight ordinary X-addition operand-order differences; reversing its outer
operands introduced GS and a 133-byte owner, so that variant was rejected.

`AnmFogState::Enable/Disable` (`0x00434A70/0x00434AB0`) are newly exact:
54 bytes and two fields each. The view aliases Supervisor `+8`, using the
device at `+8` and cached fog at `+0x990`. Both flush before changing the
cache and calling the SDK SetRenderState, returning HRESULT or zero.

All 154 previously exact affected owners and four candidates were compared
after cold-building 16 contexts. Of the old rows, 129 still match; 23
DrawLayer wrappers, DrawVmGroup and DrawSingleVm no longer reproduce.
Their 25 rows / 489 bytes are withdrawn, while their source and reviewed
extents remain frontiers. The real draw's private ABI differs from the
synthetic callee and changes complete caller extents and code. The two new
fog owners add 108 bytes; the current affected exact set is 131 passing units.
Historical comparisons below retain their original scope; their former
renderer-dependent credit is superseded by this correction.
See the [draw finding](KNOWLEDGE_BASE.md#natural-anm-draw-dispatch-and-probe-callee-cleanup-2026-11-26),
[full comparison](../.analysis/anm-draw-current-input-replay.log),
[withdrawn rows](../.analysis/anm-draw-withdrawn-conditional-rows.json) and
[final scope](../.analysis/anm-draw-final-scope.json).

Previous HelpInf checkpoint (`e311d8f`):

HelpInf now has maintained construction, creation, registration and cleanup.
`Destroy` (`0x00427CB0`, 29B) is newly exact, including its global pointer,
Shutdown and operator-delete fields. It deletes a cached pointer after
Shutdown clears the global. Create (138/135B), Initialize (265/267B) and
Shutdown (229/222B) remain complete-owner frontiers with all 6/9/24 target
fields declared. Initialize's private EDI argument differs from the target's
stack argument; Shutdown retains the CutImpl register and epilogue differences.
Its element/Chain snapshots now precede each lock as in the target.

`UpdateTimer.hpp` shares the actual setter at `0x00403280` between ANM and
HelpInf. Its 61-byte canonical owner remains exact; the public `0x00403240`
setter remains a separate function. The reviewed xrefs span 20 caller
functions. Both HelpInf timer edges now use the correct maintained copy;
Update is 1,104/1,184 bytes and still differs elsewhere. The target's
embedded MenuCursor defaults and inline ChainElem construction now have
source; the latter's existing 27-byte standalone owner remains exact.
The shared launcher's third worker also has plain-RET tails. Target CRT
review shows a direct worker call followed immediately by `_endthreadex`
and `ExitThread`, with no normal-path adapter. The source callback type
bridge remains external; see the finding for the reviewed instructions.
See the [lifecycle/setter finding](KNOWLEDGE_BASE.md#helpinf-lifecycle-and-shared-update-setter-2026-11-26).
All 174 affected/new exact owners passed the final current-input comparison
across 15 cold-built contexts. The final constructor-header move required
only the HelpInf and texture contexts to be rebuilt again; the other 13
current contexts were reused. All six HelpInf frontiers were rejected
separately at the complete-owner size check. See the
[final comparison](../.analysis/help-lifecycle-closure-replay.log),
[selected owners](../.analysis/help-lifecycle-closure-selected.json) and
[current observations](../.analysis/help-lifecycle-closure-observations.txt).

The preceding HelpInf checkpoint (`cedc55d`) supplied the viewer and its
first five exact owners:

`HelpInf.hpp/.cpp` now reconstruct the nine-page help viewer at `0x00427CD0`,
its `0x1C0` layout, page/resource worker bodies, UI VM creation and callbacks.
Five complete owners are newly exact (+122 bytes, seven reviewed fields):
`CreateVm` (`0x00427880`, 35B), `LoadPageWorker` (`0x004279C0`, 68B),
`Draw` (`0x00428170`, 6B), `OnTick` (`0x00428180`, 7B) and
`OnDraw` (`0x00428190`, 6B). The real member-returned `AnmId` and a dynamic
probe preserve the generic ID-store helper without an ABI shim. The upload
caller owns and frees its CPU image; the following D3D slot `+0x24` is
`PreLoad`, confirmed against the pinned SDK.

At that checkpoint, three HelpInf owners remained frontiers: Update was
1,092/1,184 bytes, `AnmResource::CreateUiVm` was 202/204, and
`LoadResourcesWorker` was 92/90.
Update differed in frame/register layout, cursor clamping, launcher
edges and the two timer calls fixed above.
The UI helper's binder input/result lifetimes differ; the resource worker's
private Fatal receiver is EDI rather than ECX. Worker bodies consume no
stack arguments and end in plain RET; the launcher's source callback type
bridge remains unresolved. Initialization was not implemented
until the lifecycle work above; native runtime closure is still unproved.

The resource-header change required 11 existing contexts to be cold-built;
the new HelpInf context was already cold-built and receipt-verified. All
143 affected/new exact owners passed the final current-input comparison.
The expanded texture context includes the real HelpInf caller, but image
upload remains 116/156 bytes and RGB repair retains its `0x4C`/`0x50`
frame difference. See the [HelpInf finding](KNOWLEDGE_BASE.md#helpinf-viewer-returned-anm-ids-and-page-ownership-2026-11-26),
[comparison](../.analysis/help-final-replay.log) and
[selected units](../.analysis/help-final-affected-units.json).

`AnmVm::SetColorTransition` (`0x0045E4D0`) and
`SetSecondaryColorTransition` (`0x0045E2C0`) now replay exact as complete
227-byte VC10 RTM `/GL /Ob1` owners (+2 functions, +454 bytes). Each has one
reviewed `DIR32 g_TimeScale` field at `+185`, resolving to `0x004B6F58`.
The natural source uses a `(current, target, duration, mode)` API and local
three-integer color snapshots; both Update callers and the probe preserve
their endpoint roles. The two new owners and 81 existing dependent exact
units all passed cold replay. See the [color snapshot finding](KNOWLEDGE_BASE.md#anm-color-snapshot-and-parameter-order-2026-11-26).

`OnTickWorldCallback` (`0x00466690`, 46 bytes) and `OnTickUiCallback`
(`0x004666C0`, 7 bytes) are now exact as well (+2 functions, +53 bytes).
The world callback honors the flags at state `+0x60` before tail-calling
world update; UI always tail-calls UI update. Keeping these actual callbacks
in the probe, instead of taking the internal updates' addresses, restores
the target's private EAX manager input naturally. All 74 affected exact
units passed cold replay. See the [tick callback finding](KNOWLEDGE_BASE.md#anm-tick-callbacks-and-internal-update-abi-2026-11-26).

`UpdateUiVms` (`0x00466BE0`, 246 bytes) is now exact (+1 function,
+246 bytes). A two-iteration initialization loop reproduces the target's
unrolled head-clear/tail-store order and all 11 reviewed address fields.
All 75 affected exact units passed cold replay. The four registration
methods and `CreateVm` now return `AnmId` by value; child registration uses
independent ID locals instead of overwriting the live `parent` argument.
These API and lifetime repairs grant no extra exact credit. See the
[UI/ID finding](KNOWLEDGE_BASE.md#ui-update-initialization-and-value-returned-anm-ids-2026-11-26).

`AnmResource::SpawnChildVmAtPosition` (`0x00467330`, 230 bytes) and
`AnmManager::StopVmById` (`0x00467480`, 103 bytes) now replay exact
(+2 functions, +333 bytes). Child creation uses natural `AnmId` member
returns and typed Supervisor locks. The positioned path copies the main
position before binding, then script variables and alternate position after
binding. The stop path preserves its signed 16-bit interrupt and callbacks.
The native 89-byte `GetVmById(AnmId)` replaces the old unused-ECX shim's
canonical unit without extra credit; the shim and its probe pointers are
removed. See the [child/lookup finding](KNOWLEDGE_BASE.md#anm-child-creation-and-native-id-lookup-2026-11-26).

The affected check also found five old exact callback rows whose definitions
had been overwritten during effect-10 work. All five natural callbacks are
restored in `AnmVmEffects.cpp`; a local inline timer reset reproduces the
187-byte start callback and its seven relocations, while the four empty
callbacks each reproduce three bytes. These restore existing credit.
The four timer-helper units now share the identical script compilation
context instead of repeating it in four additional output directories.
All 126 affected exact units passed the final current-input comparison
([replay](../.analysis/anm-member-consolidated-final-replay.log)).

`AnmResource::BindVmScript` (`0x004593B0`, 262 bytes) and
`BindVmAtPosition` (`0x004594C0`, 295 bytes) now replay exact
(+2 functions, +557 bytes), each with six reviewed address fields.
They use a real manager member receiver and a local inline timer reset;
the old volatile singleton read, flag cast and absolute timer pointer are
removed. All 128 affected exact units passed current-input comparison; the
mixed wrapper consumer also builds. See the [binding finding](KNOWLEDGE_BASE.md#natural-anm-binding-calls-and-inline-timer-resets-2026-11-26).

Current focused frontiers:

- `FinalizeNextEntry` (`0x00465770`): complete target 1,000 bytes, current
  source 903 bytes with the real GPU and pixel-helper definitions. Its
  resource input uses ECX, closing the scheduler below; the full finalizer
  still differs.
- `FillTransparentTextureColors` (`0x004645F0`): complete 1,774-byte source
  owner and nine fields, including both physical tables. Candidate frame
  `0x4C` vs target `0x50`; cookie/local offsets, register-save placement and
  dispatch destinations still differ. The strict comparison rejects the
  field-position mismatch (`+87` vs target `+89`), granting no exact credit.
- `LoadExternalTexture` (`0x00464FD0`): source 515 vs 511 bytes. With the
  real repair and inline neighbor definitions, an interim owner had 511 bytes
  and `RET 16`, but 57 ordinary differences. Using the real format normalizer
  yields 515 bytes, promotes the format input to ECX and returns with `RET 12`.
  ABI, cropping and register scheduling remain open. The earlier in-place-format
  probe's 477-byte/`RET 12` result is also retained as historical evidence.
- `LoadEmbeddedTexture` (`0x004651D0`): full 296-byte extent and 13 fields
  replay, with 16 raw differences in the failure epilogue and final product.
  Dimension signedness, release expansion and an accumulator did not close it.
- `LoadTextureFile` (`0x00464CE0`): complete 245-byte owner and all 14 fields
  compare, but eight filename/format register bytes differ. Parameter-order
  and in-place-format controls did not change those differences.
- `UploadTextureImage` (`0x00464DE0`): 116 vs 156 bytes; target eight-byte
  stack alignment, GS setup and size-register context remain open. Its real
  caller at `0x00427F09` supplies `textures[1]` and frees the input afterward.
- `UploadTextureRegion` (`0x00464E80`): 329 vs 330 bytes, with ten fields;
  format/data promotion, frame and `RET 8` vs target `RET 12` differ.
- `LoadAnm` (`0x004653F0`): the real 515-byte first-stage loader now has
  source and a linked frontier unit. The candidate is 511 bytes; the
  target retains a loop-top null test instead of the candidate's rotated
  backedge check.
- `CacheExternalTexture` (`0x00465680`): the full 231-byte owner and all
  13 fields replay, with two raw SIB byte differences at `+0xC4/+0xCE`.
  Typed/index-first and byte-offset accessors did not resolve them.
- `UpdateWorldVms`: complete 229-byte owner with all 12 address fields
  replayed; one raw difference at `+8` (`sub esp, 0x84` vs target `0x8C`).
  Resolve the frame/TU context without unused slots or padding.
- `SetEffectState`: the byte-formal hypothesis was rejected (132 bytes,
  mode in DL, `RET 8` vs target 133 bytes / three stack slots / `RET 12`).
  The unsigned-int formal and mask are retained; its frontier symbol and
  inventory signature now agree with the source.

The remaining factory family has maintained linked `/GL` frontier units.
`SpawnChildVm` is 293 vs its complete 266-byte target, with registration
return-pointer promotion and control-flow differences. `CreateVm` was
359 vs 364 in the resource context. Registration owners remain frontiers;
the target keeps physical insertion branches eliminated by the source compiler.
`SetInterruptAndUpdate` (`0x004674F0`, 128 bytes) now has a source mapping
and linked frontier unit: the candidate is 127 bytes with different stack
alignment. The reviewed update wrapper at `0x00407080` now has a source
mapping and linked frontier unit. Its natural member implementation is a
five-byte tail jump in the full GL wrapper context; the target is a 20-byte
stack-VM wrapper with eight-byte alignment and `RET 4`. The original TU/ABI
context is unresolved. The binder matches use the reviewed external-symbol
binding; they do not prove closure with that wrapper body. The current script
`Update` still has extensive differences despite its earlier 13,604-byte size.
Reproduce the interrupt frontier (expected
mismatch, no exact credit):

```bash
scripts/repo-python scripts/build-match-unit.py --unit anm-manager-set-interrupt-and-update
scripts/repo-python scripts/compare-linked-function.py --unit anm-manager-set-interrupt-and-update
```

`AnmVm::Update` remains a frontier.

`AnmManager::PreloadAnm` (`0x00465600`, 124 bytes) is now exact
(+1 function, +124 bytes), including six internal alignment bytes and
eight address fields. It uses a manager snapshot, the actual `LoadAnm`
member, and the real empty ellipsis-only release logger. The fake loader
and volatile logger probe are removed. `AnmLoading.cpp` also implements
the version-7 external texture cache, preserving the target error paths.
The resource's 32-bit file ID, filename and count fields are now named.
Its inline constructor retains the existing exact 19-byte owner in the
primary loading context; this adds no constructor credit. The first-stage
loader and texture cache above remain frontiers. GPU/table finalization
at `0x00465770` now has the frontier implementation described below.
See the [loading finding](KNOWLEDGE_BASE.md#anm-first-stage-loading-release-logging-and-preload-wait-2026-11-26)
and [affected replay](../.analysis/anm-loading-final-replay.log)
(161/161 exact units passed in 12 contexts). Resource fields `+0x124` and
`+0x128` are now named `finalizeCursor` and `releaseRequested`.
Scheduler `0x00465E50` advances only
one archive entry per invocation. See the loading finding for its apparent
null reload on the release branch; preserve the observed ordering.

`AnmResource::SetSprite` (`0x00466090`, 145 bytes) is now exact
(+1 function, +145 bytes). A fixed-size 72-byte `memcpy` and six float
assignments reproduce its complete owner with no address fields. The raw
and resolved sprite layouts now name pixel bounds, actual texture sizes,
UV bounds, logical sizes and scaling ratios. Both `FinalizeNextEntry` and
`ProcessPendingAnms` have maintained source in `AnmFinalization.cpp`.
The texture methods have maintained source in `AnmTextureLoading.cpp`.
The separate 746-byte entry finalizer remains unimplemented.
See the [resolved-sprite finding](KNOWLEDGE_BASE.md#anm-resolved-sprites-and-finalization-array-bounds-2026-11-26)
and [final comparison](../.analysis/anm-finalization-current-input-final.log)
(130/130 exact units passed in ten cold-built contexts).

`CreateEmptyTexture` (`0x00465300`, 70 bytes) and `CreateRenderTarget`
(`0x00465350`, 69 bytes) now replay exactly, each with five reviewed fields.
Both use the pinned SDK interfaces and the actual Supervisor device view.
The batching path's duplicate device storage and 90-slot placeholder view
are removed; its six device fields now bind through `g_Supervisor + 16`.
With all four texture definitions present, `ProcessPendingAnms`
(`0x00465E50`, 140 bytes, four fields) also becomes exact naturally.
These add three functions and 279 bytes. `SetSprite` remains exact.
See the [GPU/state finding](KNOWLEDGE_BASE.md#anm-gpu-texture-loading-and-shared-graphics-state-2026-11-26)
and [canonical comparison](../.analysis/anm-gpu-canonical-final-compare.log)
(189 units across 19 cold-built contexts).

External texture loading now calls the maintained 1,774-byte
`FillTransparentTextureColors` at `0x004645F0`. It repairs transparent
pixels' RGB from nontransparent neighbors; it does not free the CPU file buffer.
The presentation block at `0x004D2F5C` is passed to device `Reset`; the
legacy cursor/windowed field at `0x004D2F7C` corresponds to its `Windowed`.

Five inline helper copies now replay exactly (+5 functions, +195 bytes):
`NormalizeTextureFormat` (`0x00464510`, 40B, two fields) and
`AccumulateTextureColor8888/1555/4444/8332`
(`0x00464540/60/90/C0`, 30/42/42/41B, no fields). Their complete unreferenced
copies survive under VC10 while real callers inline them. The large repair
uses four explicit format loops, signed pitch division and chained reset of
three unsigned sums; alpha remains unchanged. A full-loop template was not
inlined by `/Ob1`, while aggregate initialization retained a larger owner.
All nine affected exact comparisons passed in two cold contexts; no shared
header or other compilation context changed. See the
[pixel finding](KNOWLEDGE_BASE.md#anm-transparent-pixel-repair-and-inline-neighbor-helpers-2026-11-26),
[final comparison](../.analysis/anm-colors-final-compare.log), and
[current frontier observations](../.analysis/anm-colors-final-observations.txt).

`LoadTextureFile`, `UploadTextureImage` and `UploadTextureRegion` now have
complete target-reviewed implementations and full linked frontier units.
The file path retains CPU bytes on success and frees them only on decode
failure; uploads retain caller ownership. External region upload uses an
absolute bottom of texture height, while embedded upload adds the Y origin
to payload height. Upload HRESULTs are ignored as in the target. All four
direct callers of RGB repair are now defined in its linked context, but the
repair still has a `0x4C` frame instead of `0x50`. Shared counters did not
close it; a union for the 16-bit views emitted identical code and was removed.
See the [upload finding](KNOWLEDGE_BASE.md#anm-file-backed-creation-and-existing-texture-uploads-2026-11-26)
for the retained controls and caller-ownership evidence.
The changed entry/D3DX declarations required 11 cold contexts; all 138
affected canonical exact comparisons passed. The three new owners were
rejected separately and add source mappings only. See the
[affected replay](../.analysis/anm-additional-final-replay.log) and
[selected units](../.analysis/anm-additional-final-selected-units.json).

Replay the recent ANM exact owners with:

```bash
scripts/repo-python scripts/replay-exact-units.py \
  --unit anm-vm-set-color-transition --unit anm-vm-set-secondary-color-transition \
  --unit anm-manager-on-tick-world-callback --unit anm-manager-on-tick-ui-callback \
  --unit anm-manager-update-ui-vms --unit anm-manager-stop-vm-by-id \
  --unit anm-vm-spawn-child-at-position --unit anm-manager-get-vm-by-id \
  --unit anm-resource-bind-vm-script --unit anm-resource-bind-vm-at-position \
  --unit anm-manager-preload-anm --unit anm-resource-set-sprite \
  --unit anm-manager-process-pending-anms --unit anm-texture-create-empty \
  --unit anm-texture-create-render-target
```
