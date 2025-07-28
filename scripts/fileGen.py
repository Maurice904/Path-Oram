import random
import sys
import argparse
#command format: python fileGen.py [storage|operation] <n> [--output <filename>] [--max-value <max_value>] [--max-position <max_position>] [--read-ratio <read_ratio>]
def generate_storage_file(n, filename="storage.txt", max_value=100000):
    with open(filename, 'w') as f:
        for i in range(1, n + 1):
            position = i
            value = random.randint(1, max_value)
            f.write(f"{position} {value}\n")
    print(f"Generated storage file '{filename}' with {n} lines")

def generate_operation_file(n, filename="operations.txt", max_position=10000, max_value=100000, read_ratio=0.5):
    with open(filename, 'w') as f:
        for _ in range(n):
            if random.random() < read_ratio:
                position = random.randint(1, max_position)
                f.write(f"R {position}\n")
            else:
                position = random.randint(1, max_position)
                value = random.randint(1, max_value)
                f.write(f"W {position} {value}\n")
    print(f"Generated operation file '{filename}' with {n} lines")

def main():
    parser = argparse.ArgumentParser(description='Generate test files for Path-ORAM')
    parser.add_argument('op', choices=['storage', 'operation'], 
                       help='Type of file to generate: storage or operation')
    parser.add_argument('n', type=int, help='Number of lines to generate')
    parser.add_argument('--output', '-o', default=None, 
                       help='Output filename')
    parser.add_argument('--max-value', type=int, default=100000,
                       help='Maximum value for random numbers (default: 100000)')
    parser.add_argument('--max-position', type=int, default=10000,
                       help='Maximum position for operations (default: 10000)')
    parser.add_argument('--read-ratio', type=float, default=0.5,
                       help='Ratio of read operations (0.0-1.0, default: 0.5)')

    args = parser.parse_args()

    if args.output is None:
        if args.op == 'storage':
            args.output = 'storage.txt'
        else:
            args.output = 'operations.txt'

    if not 0.0 <= args.read_ratio <= 1.0:
        print("Error: read-ratio must be between 0.0 and 1.0")
        sys.exit(1)

    if args.op == 'storage':
        generate_storage_file(args.n, args.output, args.max_value)
    else:
        generate_operation_file(args.n, args.output, args.max_position, 
                              args.max_value, args.read_ratio)

if __name__ == "__main__":
    main()