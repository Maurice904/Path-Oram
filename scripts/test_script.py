import os
import time
import re
import argparse
import matplotlib.pyplot as plt
from subprocess import Popen, PIPE

'''
Input command below in the terminal, plots will be generated in the scripts folder:
cd scripts
python3 test_script.py --bin ../oram
'''

# ─── Configuration ──────────────────────────────────────────────
TREE_SIZES    = [200000, 1000000]
OPERATE_SIZES = [100000, 200000, 500000, 700000, 1000000]
BUCKET_SIZE   = 4
MAX_SIZE_VAL  = "64"
R_RATIO       = "0.5"

CONFIGS = [
    {"name": "no_opt",             "flags": []},
    {"name": "max_size",           "flags": ["--max-size", MAX_SIZE_VAL]},
    {"name": "ring",               "flags": ["-rp"]},
    {"name": "random_r_plus_ring", "flags": ["--r", R_RATIO, "-rp"]},
    {"name": "maxsize_plus_ring",  "flags": ["--max-size", MAX_SIZE_VAL, "-rp"]},
]

# ─── Regex for parsing ───────────────────────────────────────────
STASH_RE = re.compile(r"stash size[:=]\s*(\d+)", re.IGNORECASE)
CAP_RE   = re.compile(r"has\s+(\d+)\s+capacity",   re.IGNORECASE)

def extract(pattern, text, cast=int, default=0):
    """Search pattern in text and return cast(m.group(1)), or default."""
    m = pattern.search(text)
    return default if not m else cast(m.group(1))

# ─── File generators ──────────────────────────────────────────────
def make_store_file(path, N):
    """Generate store file: N lines 'pos pos*10'."""
    with open(path, "w") as f:
        for pos in range(1, N+1):
            f.write(f"{pos} {pos*10}\n")

def make_ops_file(path, N):
    """Generate operate file: N lines 'R pos' for pos=1..N."""
    with open(path, "w") as f:
        for pos in range(1, N+1):
            f.write(f"R {pos}\n")

# ─── Run commands in one session ──────────────────────────────────
def run_commands(binary, cmds):
    """
    cmds: list of strings, each is one CLI command.
    Returns (full_output, elapsed_time).
    """
    joined = "\n".join(cmds) + "\nexit\n"
    start = time.time()
    proc = Popen([binary], stdin=PIPE, stdout=PIPE, stderr=PIPE, text=True)
    out, err = proc.communicate(joined, timeout=600)
    return out + "\n" + err, time.time() - start

# ─── Main ────────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(
        description="Batch ORAM: N R pos ops + Python timing + capacity plot"
    )
    parser.add_argument("--bin", required=True, help="Path to ORAM binary")
    parser.add_argument("--output-prefix", default="", help="Prefix for output files")
    args = parser.parse_args()

    binary = args.bin
    prefix = args.output_prefix

    # verify binary
    if not os.path.isfile(binary) or not os.access(binary, os.X_OK):
        raise RuntimeError(f"Binary {binary} missing or not executable")

    # collect metrics
    store_metrics    = {}  # cfg -> { N: (stash, time) }
    operate_metrics  = {}  # cfg -> { N: (stash, time) }
    capacity_metrics = {}  # cfg -> { N: capacity }

    for cfg in CONFIGS:
        name  = cfg["name"]
        flags = cfg["flags"]
        print(f"\n=== Config {name}, flags={flags} ===")

        store_metrics[name]    = {}
        operate_metrics[name]  = {}
        capacity_metrics[name] = {}

        for N in TREE_SIZES:
            print(f"-- Data size {N} --")
            # prepare input files
            store_file = f"store_{N}_{name}.txt"
            ops_file   = f"ops_{N}_{name}.txt"
            make_store_file(store_file, N)
            make_ops_file(ops_file, N)

            # 1) measure store
            cmds_store = [f"store {store_file} {BUCKET_SIZE}"] + flags + ["print sizes"]
            out_s, t_store = run_commands(binary, cmds_store)
            stash_s  = extract(STASH_RE, out_s, default=0)
            cap_s    = extract(CAP_RE,   out_s, default=0)
            print(f"Store: stash={stash_s}, time={t_store:.3f}s, capacity={cap_s}")
            store_metrics[name][N]    = (stash_s, t_store)
            capacity_metrics[name][N] = cap_s

            # 2) measure operate (re-store + operate, subtract store time)
            cmds_op = [f"store {store_file} {BUCKET_SIZE}"] + flags + [f"operate {ops_file}"]
            out_o, t_total = run_commands(binary, cmds_op)
            stash_o = extract(STASH_RE, out_o, default=0)
            t_op    = max(t_total - t_store, 0.0)
            print(f"Operate: stash={stash_o}, time={t_op:.3f}s")
            operate_metrics[name][N] = (stash_o, t_op)

    # ─── Plot Store stash vs Data size ──────────────────────────────
    plt.figure()
    for cfg in CONFIGS:
        name = cfg["name"]
        ys   = [ store_metrics[name][N][0] for N in TREE_SIZES ]
        plt.plot(TREE_SIZES, ys, marker='o', label=name)
    plt.xscale("log")
    plt.xlabel("Data size (N)")
    plt.ylabel("Max stash size")
    plt.title("Store Phase: Stash vs Data size")
    plt.grid(True, which="both", ls="--")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{prefix}store_stash_vs_datasize.png")
    print(f"Saved {prefix}store_stash_vs_datasize.png")

    # ─── Plot Store time vs Data size ────────────────────────────────
    plt.figure()
    for cfg in CONFIGS:
        name = cfg["name"]
        ys   = [ store_metrics[name][N][1] for N in TREE_SIZES ]
        plt.plot(TREE_SIZES, ys, marker='o', label=name)
    plt.xscale("log"); plt.yscale("log")
    plt.xlabel("Data size (N)")
    plt.ylabel("Elapsed time (s)")
    plt.title("Store Phase: Time vs Data size")
    plt.grid(True, which="both", ls="--")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{prefix}store_time_vs_datasize.png")
    print(f"Saved {prefix}store_time_vs_datasize.png")

    # ─── Plot Capacity vs Data size ───────────────────────────────────
    plt.figure()
    for cfg in CONFIGS:
        name = cfg["name"]
        ys   = [ capacity_metrics[name][N] for N in TREE_SIZES ]
        plt.plot(TREE_SIZES, ys, marker='o', label=name)
    plt.xscale("log")
    plt.xlabel("Data size (N)")
    plt.ylabel("Tree capacity")
    plt.title("Tree Capacity vs Data size")
    plt.grid(True, which="both", ls="--")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{prefix}capacity_vs_datasize.png")
    print(f"Saved {prefix}capacity_vs_datasize.png")

    # ─── Plot Operate stash vs Data size ─────────────────────────────
    plt.figure()
    for cfg in CONFIGS:
        name = cfg["name"]
        ys   = [ operate_metrics[name][N][0] for N in TREE_SIZES ]
        plt.plot(TREE_SIZES, ys, marker='o', label=name)
    plt.xscale("log")
    plt.xlabel("Data size (N)")
    plt.ylabel("Max stash size")
    plt.title("Operate Phase: Stash vs Data size")
    plt.grid(True, which="both", ls="--")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{prefix}operate_stash_vs_datasize.png")
    print(f"Saved {prefix}operate_stash_vs_datasize.png")

    # ─── Plot Operate time vs Data size ──────────────────────────────
    plt.figure()
    for cfg in CONFIGS:
        name = cfg["name"]
        ys   = [ operate_metrics[name][N][1] for N in TREE_SIZES ]
        plt.plot(TREE_SIZES, ys, marker='o', label=name)
    plt.xscale("log"); plt.yscale("log")
    plt.xlabel("Data size (N)")
    plt.ylabel("Elapsed time (s)")
    plt.title("Operate Phase: Time vs Data size")
    plt.grid(True, which="both", ls="--")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{prefix}operate_time_vs_datasize.png")
    print(f"Saved {prefix}operate_time_vs_datasize.png")

if __name__ == "__main__":
    main()