import hashlib

def get_file_md5(file):
    MD5 = hashlib.new('md5')
    with open(file, 'r') as fp:
        MD5.update(fp.read().encode('utf8'))
        fp_md5 = MD5.hexdigest()
        return fp_md5