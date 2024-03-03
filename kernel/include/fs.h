#ifndef __MY_FS__
#define __MY_FS__

#define DISK_SIZE (128 * 1024 * 1024)
#define BLK_SIZE  512
#define BLK_OFF   256
#define BLK_NUM   ((DISK_SIZE / BLK_SIZE) - BLK_OFF)

#define N1 16
#define N2 (BLK_SIZE / sizeof(uint32_t))
#define N3 (N2 * N2)
#define MAX_SIZE ((N1 + N2 + N3) * BLK_SIZE)

#define MAX_NAME (31 - sizeof(uint32_t))

#define TYPE_NONE 0
#define TYPE_FILE 1
#define TYPE_DIR  2
#define TYPE_DEV  3

typedef struct {uint32_t type, device, size, addrs[N1 + 2];} inode_t;
typedef struct dirent {uint32_t node; char name[MAX_NAME + 1];} dirent_t;
typedef struct icache {uint32_t no, ref, del; inode_t node;} icache_t;

void init_fs();
icache_t *idup(icache_t *ip);
void iput(icache_t *ip);
void itrunc(icache_t *ip);
icache_t *iopen(const char *path, icache_t **parent, int create, int type, int dev, uint32_t *off);
uint32_t iread(icache_t *ip, void *buf, uint32_t size, uint32_t off);
uint32_t iwrite(icache_t *ip, const void *buf, uint32_t size, uint32_t off);

#endif