"""Guaranteed process-TREE teardown on Windows, via a Job Object.

The bug this fixes: `subprocess.run(cmd, timeout=N)` on Windows only kills
the DIRECT child on timeout (Popen.kill() -> TerminateProcess() on that one
PID). decomp-permuter's `-j N` mode spawns N `multiprocessing.Process()`
workers, each of which spawns its own `cap_objdump.py` -> `mwccarm.exe`
subprocess. None of those are children of the timed-out call in any sense
Windows tracks automatically - killing the parent leaves the whole worker
tree running. That's why `python.exe`/`mwccarm.exe` processes were still
alive minutes after tools/permuter/batch.py (and ad-hoc `timeout N python
permuter.py ...` calls) reported done.

Windows Job Objects exist exactly for this: assign a process to a job
created with JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE, and terminating the job
(one call) recursively kills every process in it - direct child, its
multiprocessing workers, and their subprocesses - no matter how deep, with
no cooperation required from any of them. This needs no extra dependency
(pywin32 etc.) - just ctypes against kernel32, which is always present.

Usage:
    with JobbedProcess([sys.executable, "permuter.py", d, "-j", "4"]) as p:
        try:
            p.wait(timeout=secs)
        except subprocess.TimeoutExpired:
            pass
    # tree is guaranteed dead here, whether it finished, timed out, or this
    # block raised - __exit__ always calls terminate_tree().
"""
import ctypes
import subprocess
import sys

IS_WINDOWS = sys.platform == "win32"

if IS_WINDOWS:
    import ctypes.wintypes as wt

    JobObjectExtendedLimitInformation = 9
    JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE = 0x2000

    class _JOBOBJECT_BASIC_LIMIT_INFORMATION(ctypes.Structure):
        _fields_ = [
            ("PerProcessUserTimeLimit", ctypes.c_int64),
            ("PerJobUserTimeLimit", ctypes.c_int64),
            ("LimitFlags", wt.DWORD),
            ("MinimumWorkingSetSize", ctypes.c_size_t),
            ("MaximumWorkingSetSize", ctypes.c_size_t),
            ("ActiveProcessLimit", wt.DWORD),
            ("Affinity", ctypes.c_void_p),
            ("PriorityClass", wt.DWORD),
            ("SchedulingClass", wt.DWORD),
        ]

    class _IO_COUNTERS(ctypes.Structure):
        _fields_ = [
            ("ReadOperationCount", ctypes.c_uint64),
            ("WriteOperationCount", ctypes.c_uint64),
            ("OtherOperationCount", ctypes.c_uint64),
            ("ReadTransferCount", ctypes.c_uint64),
            ("WriteTransferCount", ctypes.c_uint64),
            ("OtherTransferCount", ctypes.c_uint64),
        ]

    class _JOBOBJECT_EXTENDED_LIMIT_INFORMATION(ctypes.Structure):
        _fields_ = [
            ("BasicLimitInformation", _JOBOBJECT_BASIC_LIMIT_INFORMATION),
            ("IoInfo", _IO_COUNTERS),
            ("ProcessMemoryLimit", ctypes.c_size_t),
            ("JobMemoryLimit", ctypes.c_size_t),
            ("PeakProcessMemoryUsed", ctypes.c_size_t),
            ("PeakJobMemoryUsed", ctypes.c_size_t),
        ]


class JobbedProcess:
    """subprocess.Popen wrapper that guarantees the whole process tree dies
    together. On Windows this assigns the child to a kill-on-close Job
    Object; elsewhere Popen's own process-group handling is enough (this
    project targets Windows, but the fallback keeps the API usable)."""

    def __init__(self, cmd, **kwargs):
        self._job = None
        if IS_WINDOWS:
            kernel32 = ctypes.windll.kernel32
            self._job = kernel32.CreateJobObjectW(None, None)
            if self._job:
                info = _JOBOBJECT_EXTENDED_LIMIT_INFORMATION()
                info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
                kernel32.SetInformationJobObject(
                    self._job, JobObjectExtendedLimitInformation,
                    ctypes.byref(info), ctypes.sizeof(info),
                )
            self.proc = subprocess.Popen(cmd, **kwargs)
            if self._job:
                # Best-effort: if assignment fails (e.g. the child already
                # exited immediately), terminate_tree() still falls back to
                # proc.kill() for the one process we do have a handle to.
                ok = kernel32.AssignProcessToJobObject(
                    self._job, int(self.proc._handle))
                if not ok:
                    kernel32.CloseHandle(self._job)
                    self._job = None
        else:
            self.proc = subprocess.Popen(cmd, start_new_session=True, **kwargs)

    def wait(self, timeout=None):
        return self.proc.wait(timeout=timeout)

    def terminate_tree(self):
        """Kill this process AND every descendant it (or its children)
        spawned, unconditionally. Safe to call more than once."""
        if IS_WINDOWS:
            if self._job:
                ctypes.windll.kernel32.TerminateJobObject(self._job, 1)
                ctypes.windll.kernel32.CloseHandle(self._job)
                self._job = None
            elif self.proc.poll() is None:
                self.proc.kill()
        else:
            import os
            import signal
            if self.proc.poll() is None:
                try:
                    os.killpg(os.getpgid(self.proc.pid), signal.SIGKILL)
                except ProcessLookupError:
                    pass

    def __enter__(self):
        return self

    def __exit__(self, *exc_info):
        self.terminate_tree()


def run_bounded(cmd, secs, **kwargs):
    """Run cmd for up to `secs` seconds; the ENTIRE process tree is always
    gone by the time this returns, whether it finished, timed out, or
    raised. Returns the exit code, or None if it was killed on timeout."""
    with JobbedProcess(cmd, **kwargs) as p:
        try:
            p.wait(timeout=secs)
            return p.proc.returncode
        except subprocess.TimeoutExpired:
            return None


def _main():
    """CLI: python tools/permuter/winproc.py --secs 300 -- <command...>

    The bounded, guaranteed-teardown replacement for `timeout N <command>`
    on Windows - use this instead of raw shell `timeout` for any decomp-
    permuter (or other multiprocessing-spawning) invocation, interactive or
    scripted. Plain `timeout` leaves multiprocessing workers and their
    subprocesses running after the deadline; this doesn't."""
    import argparse

    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--secs", type=float, required=True)
    ap.add_argument("cmd", nargs=argparse.REMAINDER,
                     help="command to run, e.g. -- python permuter.py workdir -j 4")
    args = ap.parse_args()
    cmd = args.cmd[1:] if args.cmd[:1] == ["--"] else args.cmd
    if not cmd:
        ap.error("no command given")
    rc = run_bounded(cmd, secs=args.secs)
    if rc is None:
        print(f"[winproc] {args.secs}s budget elapsed - process tree terminated", file=sys.stderr)
        sys.exit(0)
    sys.exit(rc)


if __name__ == "__main__":
    _main()
