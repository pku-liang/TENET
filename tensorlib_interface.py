#! /bin/python3
import argparse
import json
import os
import shutil
import subprocess


variables = {
            "GEMM": ['i', 'j', 'k'],
            "CONV": ['k', 'c', 'y', 'x', 'r', 's']
        }
    
input_dir = "./.tenet_temp_input"

def generate_pe_array(x, y, l1, l2, bandwidth, avg_latency):
    s = [
        "{" + f"PE[i,j]:0<=i<{x} and 0<=j<{y}" + "}\n",
        "{PE[i,j]->PE[i-1,j]}\n",
        f"{l1} {l2} {bandwidth} {avg_latency}\n"
    ]

    with open(input_dir+"/temp.p", "w") as fout:
        fout.writelines(s)

def generate_mapping(stt, t):
    def make_notation(mat, var):
        s = []
        for vec in mat:
            s.append("+".join([
                v if i == 1 else "-" + v if i == -1 else f"{i}v"
                for i, v in zip(vec, var) if i != 0
            ]))
        return ",".join(s)

    S = "S[" + ",".join(variables[t]) + "]"

    strs = [
        "{" + S +
        f"->PE[{make_notation(stt[:2], variables[t])}]" + "}\n",
        "{" + S +
        f"->PE[{make_notation(reversed(stt[2:]), variables[t])}]" + "}\n",
    ]
    with open(input_dir+"/temp.m", "w") as fout:
        fout.writelines(strs)

def generate_statement(t,l):
    S = "S[" + ",".join(variables[t]) + "]"
    strs = [
        "2 1\n",
        "{" + S + ":" + \
        " and ".join([f"0<={v}<{l[v]}" for v in range(len(l))]) + \
        "}\n",
    ]
    if acc.type == "GEMM":
        strs.extend([
            "{" + S + "->A[i,k]}\n",
            "{" + S + "->B[k,j]}\n",
            "{" + S + "->C[i,j]}\n",
        ])
    elif acc.type == "CONV":
        strs.extend([
            "{" + S + "->I[y+r,x+s,c]}\n",
            "{" + S + "->W[r,s,c,k]}\n",
            "{" + S + "->O[y,x,k]}\n",
        ])

    with open(input_dir+"/temp.s", "w") as fout:
        fout.writelines(strs)

if __name__=="__main__":
    parser = argparse.ArgumentParser(epilog="Contact semiwaker@pku.edu.cn for bugs")

    parser.add_argument('--tenet', type=str, default='./bin/tenet',
        help="path of Tenet")
    parser.add_argument('--tensorlib', type=str, default='~/tensorlib',
        help="path of Tensorlib")
    parser.add_argument('--main', type=str, default="tensorlib.ParseJson",
        help="main class of tensorlib")
    parser.add_argument('-b','--bandwidth',type=int,default=128,
        help="bandwidth for TENET")
    parser.add_argument('-l','--latency',type=int,default=16,
        help="average latency for TENET")
    parser.add_argument('-x', type=int, default=8,
        help="width of pe array")
    parser.add_argument('-y', type=int, default=8,
        help="height of pe array")
    parser.add_argument('-o', type=str, default="output",
        help="name of output")
    parser.add_argument('input', type=str, help="Input JSON file")

    args = parser.parse_args()

    with open(args.input, "r", encoding="utf-8") as fin:
        data = json.load(args.fin)

    if not os.path.exists(input_dir):
        os.mkdir(input_dir)
    
    generate_pe_array(args.x, args.y, 1, 1, args.bandwidth, args.latency)
    generate_mapping(data["stt"], data["type"])
    generate_mapping(data["type"], data["length"])

    try:
        subprocess.run([args.tenet, '-p', input_dir+'temp.p', '-m', input_dir+'temp.m', 
            '-s', input_dir+"temp.s", '-o', args.output+".csv"])
        subprocess.run(f'(cd {args.tensorlib}; sbt runMain {args.main} {args.input})', shell=True)
    except Exception as e:
        print(e)

    shutil.rmtree(input_dir)
