import sys

def main(files, target, header):
  f_target = open(target, "wb")
  f_header = open(header, "w")
  sect = 201
  for file in files:
    f = open(file, "rb")
    elf = f.read()
    f.close()
    elf += b'\0' * (512 - len(elf) % 512)
    f_target.write(elf)
    f_header.write('{"%s", 0, %d, %d},\n' % (file[2:], sect, len(elf)))
    sect += len(elf) // 512
  f_target.close()
  f_header.close()

if __name__ == '__main__':
  main(sys.argv[3:], sys.argv[1], sys.argv[2])
