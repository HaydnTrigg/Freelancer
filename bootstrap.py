"""
Freelancer decompilation bootstrap (delink-ida database workflow)
=================================================================

Modeled on Homeworld2Classic/bootstrap.py, but instead of downloading the
original binaries and running `delink pe-split`, this project drives the
delink **IDA database** feature.

The delink exports `config/<id>/delink/<lib>.delink.json` are produced externally
(outside this project) and checked in — they carry no bytes, only metadata. This
script only **consumes** them; it never writes to `config/<id>/delink`:

    for each library:
        delink ida-split config/<id>/delink/<lib>.delink.json orig/<id>/<lib> \
            --idapro config/<id>/splits/<lib>.json        (split)

    The split reads the function/section bytes and the PE `.reloc` table from
    the original binary in `orig/<id>/`.

The idapro grouping files in `config/<id>/splits/<lib>.json` collapse every symbol of a
library into a single `<lib>.obj`, so each library delinks to one target object
that is diffed against one compiled `src/<lib>.cpp`.

The compiler config (`config/<id>/config.json`) targets the MSVC 6.0
toolchain, which this script downloads and installs under `build/msvc6.0/`
on first run (alongside the objdiff diff binaries).

Usage::

    python bootstrap.py                       # split all libs, regen build files
    python bootstrap.py --skip-delink         # only regenerate build.ninja/objdiff/compile_commands
    python bootstrap.py --only ximage.dll      # operate on a single library (repeatable)
"""

import argparse
import datetime
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tarfile
import urllib.request
from pathlib import Path

for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8")
    except (AttributeError, ValueError):
        pass

SOURCE_ROOT = "."

_DIM = "\x1b[2m"
_RESET = "\x1b[0m"
_LEVEL_COLORS = {
    "TRACE": "\x1b[35m",
    "DEBUG": "\x1b[34m",
    "INFO": "\x1b[32m",
    "WARN": "\x1b[33m",
    "ERROR": "\x1b[31m",
}

def _enable_windows_ansi():
    """Turn on virtual-terminal processing so ANSI codes render in cmd.exe."""
    try:
        import ctypes

        kernel32 = ctypes.windll.kernel32
        handle = kernel32.GetStdHandle(-11)
        mode = ctypes.c_uint32()
        if not kernel32.GetConsoleMode(handle, ctypes.byref(mode)):
            return False
        return bool(kernel32.SetConsoleMode(handle, mode.value | 0x0004))
    except Exception:
        return False

def _supports_color():
    if os.environ.get("NO_COLOR") is not None:
        return False
    if not (hasattr(sys.stdout, "isatty") and sys.stdout.isatty()):
        return False
    if os.name == "nt":
        return _enable_windows_ansi()
    return True

_COLOR = _supports_color()

def _now(): return datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%dT%H:%M:%S.%f") + "Z"

def log(message, *, level="INFO"):
    ts = _now()
    lvl = f"{level:>5}"
    if _COLOR:
        ts = f"{_DIM}{ts}{_RESET}"
        lvl = f"{_LEVEL_COLORS.get(level, '')}{lvl}{_RESET}"
    print(f"{ts} {lvl} {message}", flush=True)
def info(message): log(message, level="INFO")
def warn(message): log(message, level="WARN")
def error(message): log(message, level="ERROR")


def load_json(path):
    info(f"reading {path}")
    with open(path, "r", encoding="utf-8") as f:
        text = f.read()
        text = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
        text = re.sub(r"//[^\n]*", "", text)
        return json.loads(text)


def run(cmd, cwd=None):
    info(f"running {' '.join(map(str, cmd))}")
    subprocess.run(cmd, cwd=cwd, check=True)


parser = argparse.ArgumentParser()
parser.add_argument("--config-id", default="052103_release_1149_Ipatch_ver1254",
                    help="Configuration preset (config/<id>/config.json)")
parser.add_argument("--config", default=None,
                    help="Explicit path to config.json (overrides --config-id default)")
parser.add_argument("--delink", default=None,
                    help="Path to the delink executable (skips download)")
parser.add_argument("--objdiff-cli", default=None,
                    help="Path to objdiff-cli executable (skips download)")
parser.add_argument("--skip-delink", action="store_true",
                    help="Skip the ida-split phase; only regenerate build files")
parser.add_argument("--only", action="append", default=None,
                    help="Restrict to the named library (repeatable)")
args = parser.parse_args()

CONFIG_ID = args.config_id
config_path = args.config or f"config/{CONFIG_ID}/config.json"
objects_path = f"config/{CONFIG_ID}/objects.json"

CONFIG_DELINK_DIR = Path(f"config/{CONFIG_ID}/delink")

DELINK_VERSION = "v0.6.0"
DELINK_EXE = Path(args.delink) if args.delink else Path("build/tools/delink-windows-x86_64.exe")
DELINK_URL = f"https://github.com/HaydnTrigg/delink/releases/download/{DELINK_VERSION}/delink-windows-x86_64.exe"
DELINK_SHA1 = "BA201EDE61C0C43C9CF38E161590FDEAA774E2B1"

OBJDIFF_VERSION = "v3.7.2-Monkey"
OBJDIFF_CLI_EXE = Path(args.objdiff_cli) if args.objdiff_cli else Path("build/tools/objdiff-cli-windows-x86_64.exe")
OBJDIFF_CLI_URL = f"https://github.com/HaydnTrigg/objdiff/releases/download/{OBJDIFF_VERSION}/objdiff-cli-windows-x86_64.exe"
OBJDIFF_CLI_SHA1 = "89F5666E23B05C93E4B166A60567A08661028F7A"

OBJDIFF_EXE = Path("build/tools/objdiff-windows-x86_64.exe")
OBJDIFF_URL = f"https://github.com/HaydnTrigg/objdiff/releases/download/{OBJDIFF_VERSION}/objdiff-windows-x86_64.exe"
OBJDIFF_SHA1 = "18665D87721982B33D3CA78AA461B157AEB48A5C"

MSVC6_ARCHIVE = Path("build/tools/msvc6.0.tar.gz")
MSVC6_URL = "https://github.com/OmniBlade/decomp.me/releases/download/msvcwin9x/msvc6.0.tar.gz"
MSVC6_DIR = Path("build/msvc6.0")

SEVENZIP_MSI = Path("build/tools/7z2601.msi")
SEVENZIP_MSI_URL = "https://github.com/ip7z/7zip/releases/download/26.01/7z2601.msi"
SEVENZIP_EXTRACT_DIR = Path("build/tools/7z")
SEVENZIP_EXE = SEVENZIP_EXTRACT_DIR / "Files" / "7-Zip" / "7z.exe"

ORIG_DIR = Path(f"orig/{CONFIG_ID}")
ORIG_ARCHIVE = Path(f"build/tools/{CONFIG_ID}.7z")
ORIG_URL = f"https://f003.backblazeb2.com/file/starport-attachments/dev-uploads/{CONFIG_ID}.7z"

OUTPUT_DIR = Path(f"build/{CONFIG_ID}")
DELINK_OUTPUT_DIR = OUTPUT_DIR / "delink"


def sha1_file(path):
    h = hashlib.sha1()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def verify_hash(required, path_str, expected):
    expected = expected.casefold()
    path = Path(path_str)
    if not path.exists():
        if required:
            raise RuntimeError(f"missing required file: {path}")
        else:
            warn(f"missing required file: {path}")
            return
    actual = sha1_file(path)
    if actual.casefold() != expected:
        if required:
            raise RuntimeError(f"{path} sha1 invalid expected:{expected} actual:{actual}")
        else:
            warn(f"{path} sha1 invalid expected:{expected} actual:{actual}")
            return
    else:
        info(f"{path} sha1 verified")


def download_file(url, dest, expected_sha1=None):
    if dest.exists():
        if expected_sha1 is None or sha1_file(dest).lower() == expected_sha1.lower():
            info(f"{dest} already present{' and verified' if expected_sha1 else ''}")
            return
        warn(f"sha1 mismatch for {dest}, re-acquiring")
        dest.unlink()
    info(f"downloading {url} → {dest}")
    dest.parent.mkdir(parents=True, exist_ok=True)
    req = urllib.request.Request(url, headers={
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36",
    })
    with urllib.request.urlopen(req) as response, open(dest, "wb") as f:
        shutil.copyfileobj(response, f)
    if expected_sha1:
        actual = sha1_file(dest)
        if actual.lower() != expected_sha1.lower():
            raise RuntimeError(f"sha1 mismatch for {dest}: expected {expected_sha1}, got {actual}")
        info(f"{dest} downloaded and verified")
    else:
        info(f"{dest} downloaded")


def to_forward_path(p):
    return p.replace("\\", "/")


config = load_json(config_path)
objects = load_json(objects_path)

if args.only:
    wanted = set(args.only)
    objects = {k: v for k, v in objects.items() if k in wanted}
    missing = wanted - set(objects)
    if missing:
        warn(f"--only names not in objects.json: {', '.join(sorted(missing))}")

COMPILER_ROOT = config.get("compiler_root")
SDK_ROOT = config.get("sdk_root")


def split_all():
    if not DELINK_EXE.exists():
        sys.exit(f"delink not found: {DELINK_EXE} (pass --delink)")

    failures = []
    for lib_name, lib in objects.items():
        idapro = Path(lib["idapro"])
        binary = Path(lib.get("binary", f"orig/{CONFIG_ID}/{lib_name}"))
        delink_name = lib.get("delink", lib_name)
        delink_dir = DELINK_OUTPUT_DIR / delink_name
        delink_dir.mkdir(parents=True, exist_ok=True)
        json_in = CONFIG_DELINK_DIR / f"{lib_name}.delink.json"

        if not json_in.exists():
            warn(f"[{lib_name}] skip — missing export {json_in}")
            failures.append((lib_name, f"missing export {json_in}"))
            continue
        if not idapro.exists():
            warn(f"[{lib_name}] skip — missing idapro {idapro}")
            failures.append((lib_name, f"missing idapro {idapro}"))
            continue
        if not binary.exists():
            warn(f"[{lib_name}] skip — missing binary {binary}")
            failures.append((lib_name, f"missing binary {binary}"))
            continue

        try:
            run([str(DELINK_EXE), "ida-split", str(json_in), str(binary.resolve()),
                 "--idapro", str(idapro), "-o", str(delink_dir)])
        except (subprocess.CalledProcessError, RuntimeError) as e:
            error(f"[{lib_name}] failed: {e}")
            failures.append((lib_name, str(e)))

    ok = len(objects) - len(failures)
    info(f"split phase: {ok}/{len(objects)} libraries ok")
    if failures:
        error(f"{len(failures)} failures:")
        for name, why in failures:
            error(f"  {name}: {why}")


def substitute_flag(flag):
    subs = {
        "$(SOURCE_ROOT)": SOURCE_ROOT,
        "$(COMPILER_ROOT)": COMPILER_ROOT,
        "$(SDK_ROOT)": SDK_ROOT,
    }
    for token, value in subs.items():
        flag = flag.replace(token, value)
    return flag


def flatten_cflags(name, cflags_dict):
    result = []

    def recurse(n):
        entry = cflags_dict[n]
        if "base" in entry:
            recurse(entry["base"])
        result.extend(substitute_flag(f) for f in entry.get("flags", []))

    recurse(name)
    return result


def get_flags(flags_key, flags_dict):
    if not flags_dict or flags_key not in flags_dict:
        return []
    return flatten_cflags(flags_key, flags_dict)


def get_delink_path(lib_name, target):
    return f"build/{CONFIG_ID}/delink/{lib_name}/" + to_forward_path(target)


def get_target_path(lib_name, src):
    base = f"build/{CONFIG_ID}/obj/{lib_name}/" + to_forward_path(src)
    root, _ = os.path.splitext(base)
    return root + ".obj"


def _strip_arg_quotes(flag):
    if len(flag) >= 2 and flag[0] == '"' and flag[-1] == '"':
        return flag[1:-1]
    return flag


_C_EXTENSIONS = {".c"}


def write_ninja(config, objects):
    lines = []
    cxx = config.get("compiler", "cl.exe")
    cc = config.get("compiler_c", cxx)
    is_msvc = cxx.lower().endswith("cl.exe")

    lines.append(f"cc = {cc}\n\n")
    lines.append(f"cxx = {cxx}\n\n")

    if is_msvc:
        lines.append("rule compile_c\n")
        lines.append('  command = $cc $cflags /c $in "/Fo$out"\n')
        lines.append("  description = Compiling $in\n\n")
        lines.append("rule compile_cxx\n")
        lines.append('  command = $cxx $cxxflags /c $in "/Fo$out"\n')
        lines.append("  description = Compiling $in\n\n")
    else:
        lines.append("rule compile_c\n")
        lines.append("  command = $cc $cflags -c $in -o $out\n")
        lines.append("  description = Compiling $in\n\n")
        lines.append("rule compile_cxx\n")
        lines.append("  command = $cxx $cxxflags -c $in -o $out\n")
        lines.append("  description = Compiling $in\n\n")

    c_dict = config.get("cflags", {})
    cxx_dict = config.get("cxxflags", config.get("cflags", {}))

    all_objs = []
    for lib_name, lib in objects.items():
        flags_key = lib["cflags"]
        c_flags = " ".join(get_flags(flags_key, c_dict))
        cxx_flags = " ".join(get_flags(flags_key, cxx_dict))

        for src in lib["objects"]:
            obj = get_target_path(lib_name, src)
            all_objs.append(obj)
            ext = Path(src).suffix.lower()
            if ext in _C_EXTENSIONS:
                rule, flags_var, flags_str = "compile_c", "cflags", c_flags
            else:
                rule, flags_var, flags_str = "compile_cxx", "cxxflags", cxx_flags
            lines.append(f"build {obj}: {rule} {to_forward_path(SOURCE_ROOT + '/' + src)}\n")
            lines.append(f"  {flags_var} = {flags_str}\n\n")

    lines.append("build all: phony $\n")
    for obj in all_objs:
        lines.append(f"  {obj} $\n")
    lines.append("\ndefault all\n")
    return "".join(lines)


def write_objdiff(config, objects):
    units = []
    for lib_name, lib in objects.items():
        category = lib.get("progress_category", "default")
        delink_name = lib.get("delink", lib_name)
        for src, target in lib["objects"].items():
            units.append({
                "name": to_forward_path(src).rsplit(".", 1)[0],
                "target_path": get_delink_path(delink_name, target or src),
                "base_path": get_target_path(lib_name, src),
                "metadata": {
                    "complete": False,
                    "reverse_fn_order": False,
                    "source_path": SOURCE_ROOT + "/" + to_forward_path(src),
                    "progress_categories": [category],
                    "auto_generated": False,
                },
            })
    return {
        "min_version": "2.0.0-beta.5",
        "custom_make": "ninja",
        "build_target": False,
        "watch_patterns": [
            "*.c", "*.cp", "*.cpp", "*.h", "*.hpp",
            "*.inc", "*.py", "*.yml", "*.txt", "*.json",
        ],
        "units": units,
        "progress_categories": [
            {"id": k, "name": v}
            for k, v in config.get("progress_categories", {}).items()
        ],
    }


def write_compile_commands(config, objects):
    cxx = config.get("compiler", "cl.exe")
    cc = config.get("compiler_c", cxx)
    is_msvc = cxx.lower().endswith("cl.exe")

    c_dict = config.get("cflags", {})
    cxx_dict = config.get("cxxflags", config.get("cflags", {}))

    directory = str(Path(SOURCE_ROOT).resolve())
    entries = []
    for lib_name, lib in objects.items():
        flags_key = lib["cflags"]
        c_flags = [_strip_arg_quotes(f) for f in get_flags(flags_key, c_dict)]
        cxx_flags = [_strip_arg_quotes(f) for f in get_flags(flags_key, cxx_dict)]
        for src in lib["objects"]:
            ext = Path(src).suffix.lower()
            compiler, flags = (cc, c_flags) if ext in _C_EXTENSIONS else (cxx, cxx_flags)
            file_path = str(Path(SOURCE_ROOT, src).resolve())
            compile_flag = "/c" if is_msvc else "-c"
            entries.append({
                "directory": directory,
                "file": file_path,
                "arguments": [compiler, compile_flag] + flags + [file_path],
            })
    return entries


def ensure_7z():
    if SEVENZIP_EXE.exists():
        info(f"{SEVENZIP_EXE} already present, skipping 7zip download")
    else:
        download_file(SEVENZIP_MSI_URL, SEVENZIP_MSI)
        extract_dir = SEVENZIP_EXTRACT_DIR.resolve()
        info(f"extracting {SEVENZIP_MSI} → {extract_dir}")
        extract_dir.mkdir(parents=True, exist_ok=True)
        run(["msiexec", "/a", str(SEVENZIP_MSI.resolve()), "/qn", f"TARGETDIR={extract_dir}"])
    return str(SEVENZIP_EXE)


def ensure_orig():
    if ORIG_DIR.is_dir() and any(ORIG_DIR.iterdir()):
        info(f"{ORIG_DIR} already present, skipping orig download")
        return
    download_file(ORIG_URL, ORIG_ARCHIVE)
    seven_zip = ensure_7z()
    ORIG_DIR.mkdir(parents=True, exist_ok=True)
    info(f"extracting {ORIG_ARCHIVE} → {ORIG_DIR}")
    run([seven_zip, "x", str(ORIG_ARCHIVE), f"-o{ORIG_DIR}", "-y"])


def ensure_tools():
    if args.delink:
        info(f"using overridden delink: {DELINK_EXE}")
    else:
        download_file(DELINK_URL, DELINK_EXE, DELINK_SHA1)

    if args.objdiff_cli:
        info(f"using overridden objdiff-cli: {OBJDIFF_CLI_EXE}")
    else:
        download_file(OBJDIFF_CLI_URL, OBJDIFF_CLI_EXE, OBJDIFF_CLI_SHA1)
    download_file(OBJDIFF_URL, OBJDIFF_EXE, OBJDIFF_SHA1)

    if (MSVC6_DIR / "Bin" / "CL.EXE").exists():
        info(f"{MSVC6_DIR} already present, skipping MSVC 6.0 download and extraction")
    else:
        download_file(MSVC6_URL, MSVC6_ARCHIVE)
        info(f"extracting {MSVC6_ARCHIVE} → {MSVC6_DIR}")
        MSVC6_DIR.mkdir(parents=True, exist_ok=True)
        with tarfile.open(MSVC6_ARCHIVE) as tar:
            tar.extractall(MSVC6_DIR)


def verify_orig():
    verify_hash(True, f"orig/{CONFIG_ID}/Common.dll", "99FBBA9AAF30299857838C6C3B7443A804E860D3")
    verify_hash(True, f"orig/{CONFIG_ID}/Content.dll", "8A9571B9708AABEFF4D1A3615F0642F6715D699D")
    verify_hash(True, f"orig/{CONFIG_ID}/DALib.dll", "521C6D14A5DD2F73C788F700D2A02C6F25B40D2D")
    verify_hash(True, f"orig/{CONFIG_ID}/Freelancer.exe", "EAB2F12172058795F6CD6A8E175E0266824BDE50")
    verify_hash(True, f"orig/{CONFIG_ID}/RemoteClient.dll", "D630D0A98283E73FDC51ADBE84D0BB35E157F7FE")
    verify_hash(True, f"orig/{CONFIG_ID}/Server.dll", "B98E932306DE6D1924AC52B464F660CC1949587B")
    verify_hash(True, f"orig/{CONFIG_ID}/alchemy.dll", "038F028FFC49CBFFC7FD3CBEAC2B4F9AAE0CB029")
    verify_hash(True, f"orig/{CONFIG_ID}/dacom.dll", "30E031B76AC9D419A74AFEB6F48B1316973E3A30")
    verify_hash(True, f"orig/{CONFIG_ID}/debuglib.dll", "C10ECD7CF951C5A57D4EA397E132B5F5CFBB02F5")
    verify_hash(True, f"orig/{CONFIG_ID}/deformable2.dll", "F9FB4BC26B863980A5559E8E8A9077C48F5F19A9")
    verify_hash(True, f"orig/{CONFIG_ID}/ebueula.dll", "4A5EAABA03DDF231E49FE5F56FA69A75F7167505")
    verify_hash(True, f"orig/{CONFIG_ID}/engbase.dll", "ACDF853ADA0DF921C55B7D00C1B46D5CA64F819E")
    verify_hash(True, f"orig/{CONFIG_ID}/equipresources.dll", "ABC81093F8CD0766306D9DF9E38479E10B3860D7")
    verify_hash(True, f"orig/{CONFIG_ID}/flmaterials.dll", "F65E915290ADF078E0CB3C91DC8514FB00BE4F36")
    verify_hash(True, f"orig/{CONFIG_ID}/flserver.exe", "B736C37CEA42B31A676F73A669A71419B94127CB")
    verify_hash(True, f"orig/{CONFIG_ID}/gundll.dll", "BC4845F73A0BBE5009957D759B8BB755B5C22C9D")
    verify_hash(True, f"orig/{CONFIG_ID}/imeui.dll", "4601521E9631F67CA13DF59A4DDB986F8D5E6AAF")
    verify_hash(True, f"orig/{CONFIG_ID}/infocards.dll", "41C4B45B0EBF88ECC881135B7BEFEECEE5C63F42")
    verify_hash(True, f"orig/{CONFIG_ID}/luaprofile.dll", "B281B357F76C23EEB0A4D22866AAD6C5952DEB07")
    verify_hash(True, f"orig/{CONFIG_ID}/misctext.dll", "81455C99D1BFE53121AD10A81A91425E6E51F5C4")
    verify_hash(True, f"orig/{CONFIG_ID}/misctextinfo2.dll", "AE9D98D10369B3839AD06114302AF0B3B24DAD01")
    verify_hash(True, f"orig/{CONFIG_ID}/movie.dll", "D5C46B16CF6FAB19D678BC7F9CBFEC3639276FC2")
    verify_hash(True, f"orig/{CONFIG_ID}/nameresources.dll", "48582F9752401213E3C27C93A527A5CDBBB88AFA")
    verify_hash(True, f"orig/{CONFIG_ID}/offerbriberesources.dll", "C96B05944205E8D2A999F125DBBFE97086301A94")
    verify_hash(True, f"orig/{CONFIG_ID}/readfile.dll", "A23BC7EA656009E21785CCD606852ACB93F22560")
    verify_hash(True, f"orig/{CONFIG_ID}/remoteserver.dll", "2BFAD641D3B38EC5113C27B5D2948DFC9B865399")
    verify_hash(True, f"orig/{CONFIG_ID}/rendcomp.dll", "EEE80001A36F4E2CC90E54A9B847D406614C332E")
    verify_hash(True, f"orig/{CONFIG_ID}/resources.dll", "8294374D437AE6281FE97EA3A2E5267867526570")
    verify_hash(True, f"orig/{CONFIG_ID}/rp8.dll", "FBF6985E396AB37FE95689DD39F9FBCED37B3AAD")
    verify_hash(True, f"orig/{CONFIG_ID}/rpclocal.dll", "AF5846D06D1745CF26E209AC92408F857A2E905D")
    verify_hash(True, f"orig/{CONFIG_ID}/serverresources.dll", "D410D7235C659B16450EDBF54FF3676FA2193297")
    verify_hash(True, f"orig/{CONFIG_ID}/shading.dll", "E09DC7C6404950DF30D1AC453476498280A57965")
    verify_hash(True, f"orig/{CONFIG_ID}/soundmanager.dll", "A0A2E1E5382014C259CC85E4B344A92FCB1AAA43")
    verify_hash(True, f"orig/{CONFIG_ID}/soundstreamer.dll", "9D21FDDA73BEFDECBE56AD08366F62925E161887")
    verify_hash(True, f"orig/{CONFIG_ID}/system.dll", "49BF7B6E954EBD7DF537785E16104857F0B44264")
    verify_hash(True, f"orig/{CONFIG_ID}/thorn.dll", "2CD301BA4CF1247A727278C4EF29D8DD08E0C927")
    verify_hash(True, f"orig/{CONFIG_ID}/unicows.dll", "436D68AB440545000FE7A207115A2B0329E64F07")
    verify_hash(True, f"orig/{CONFIG_ID}/x86math.dll", "B99761ECE4CE6F232BF5B155FC5BD62F4009C4ED")
    verify_hash(True, f"orig/{CONFIG_ID}/ximage.dll", "505CFF05303D7543099C4279FE9EEA7F678DBDDA")
    verify_hash(True, f"orig/{CONFIG_ID}/zlib.dll", "50439B99CE525ECB74C554E1DC43DDB39481DFA4")


ensure_tools()

ensure_orig()

verify_orig()

if not args.skip_delink:
    split_all()

with open("build.ninja", "w", encoding="utf-8") as f:
    f.write(write_ninja(config, objects))
info("generated → build.ninja")

with open("objdiff.json", "w", encoding="utf-8") as f:
    json.dump(write_objdiff(config, objects), f, indent=2)
info("generated → objdiff.json")

with open("compile_commands.json", "w", encoding="utf-8") as f:
    json.dump(write_compile_commands(config, objects), f, indent=2)
info("generated → compile_commands.json")
