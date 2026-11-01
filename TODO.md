# DOS port TODO

## Runtime blockers and validation

- [ ] Verify a usable 80x25 interface under DOSEMU and real DOS hardware, and
  exercise both VGA-only operation and optional VESA modes. The current
  `dosemu -dumb ocp.exe` smoke run remains active until timeout without a
  reported CPU exception, but does not visually confirm the display.
- [ ] Verify sustained Sound Blaster 16 playback under DOSEMU and, if possible,
  real DOS hardware. Test WAV playback past the earlier `read failed #14`
  report, check for choppiness, and tune the current 500 ms player buffer and
  2048-frame SDL chunks for a workable latency/reliability balance.
- [ ] Test actual playback with each included player, especially AdPlug and
  SID, rather than only confirming their code and plugin labels are linked.
- [ ] Recheck DOS file selection and filesystem behavior: drive-root paths,
  `TEMP`/`TMP`, opening files, and temporary-file handling.

## Playback formats and compatibility

- [ ] Audit the remaining player modules and codec dependencies for DOS
  compatibility, then add only those that can be built and statically linked
  reliably. The DOS target now includes Ogg/Vorbis, QOA, SNDH, and TiMidity.
- [ ] Keep the supported-format list and the documented limitations in
  [README-DOS.md](./README-DOS.md) aligned with the executable and runtime
  tests.

## Build and release readiness

- [ ] Repeat the documented configure and full build from a clean build state,
  then verify the resulting executable and generated `ocp.ini`. The current
  dependency prefix is under `/tmp/djgpp-deps` and may need to be rebuilt.
- [ ] Confirm the local reSIDfp DJGPP compatibility edits are preserved and
  reproducible when the SID submodules are initialized again.
- [ ] Resolve the whitespace warnings reported by `git diff --check` for the
  DJGPP compatibility block in `playym/stsoundlib/YmTypes.h`, preserving that
  file's line-ending convention.
- [ ] Review generated build/runtime files and test artifacts before preparing
  a release; preserve user-provided test files such as `quest.wav`.
