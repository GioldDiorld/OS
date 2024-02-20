typedef struct crc_checksum {
    unsigned int crc;
    char filename[200]; 
}CHECKSUM;

extern unsigned int file_checksum_count;
extern CHECKSUM file_checksums[];

unsigned int crc_32_calc(const void* data, unsigned int length, unsigned int crc);
int check_validdata(const char* filename, const void* buf, int length);

