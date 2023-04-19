import gzip
import shutil
import sys
import os

if __name__ == "__main__":
    full_path = sys.argv[1]
    file_name = os.path.basename(full_path)
    print(f"Compressing: {file_name} > {file_name}.gz")
    with open(full_path, 'rb') as f_in:
        with gzip.open(full_path + ".gz", 'wb', 9) as f_out:
            shutil.copyfileobj(f_in, f_out)