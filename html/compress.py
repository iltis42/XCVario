import gzip
import glob
import shutil

extensions = ('*.html', '*.css') # files to compress
for extension in extensions:
    for file in glob.glob(extension):
        with open(file, 'rb') as f_in:
            with gzip.open(file + ".gz", 'wb', 9) as f_out:
                shutil.copyfileobj(f_in, f_out)