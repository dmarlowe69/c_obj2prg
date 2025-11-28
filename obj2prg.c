// obj2prg.c
// Compile with MinGW-w64:
//   gcc -obj2prg.c -o obj2prg.exe
//   or
//   x86_64-w64-mingw32-gcc -O2 obj2prg.c -o obj2prg.exe
//
// Usage: obj2prg input.obj  output.prg
//        obj2prg input.obj (outputs input.prg)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned char memory[65536];
unsigned char pad = 0xaa;
uint16_t load_address = 0;
size_t max_addr = 0;

int parse_hex_line(const char *line) {
	printf("\n%s\n",line);
    if (line[0] != ';') {
		printf("\nNot a CBM Interface file\n");
		return 0;  // not a cbm interface record
	}
    unsigned int len, addr, checksum;
    if (sscanf(line + 1, "%2x%4x", &len, &addr) != 2)
        return 0;
    printf("len = %02x address = %04x\n",len,addr); 

    // First data record determines the load address for the .prg
    if (load_address == 0 && len > 0) {
        load_address = addr;
    }

    for (unsigned int i = 0; i < len; i++) {
        unsigned int byte;
        if (sscanf(line + 7 + i*2, "%2x", &byte) != 1)
            return 0;
        memory[addr + i] = (unsigned char)byte;
		printf("%02x",byte);
        if (addr + i > max_addr) max_addr = addr + i;
    }
	if (sscanf(line + 7 + len*2, "%04x", &checksum) != 1)
            return 0;
		printf("\nchecksum %04x",checksum);
	printf("\n");
    return 0;
}

int main(int argc, char *argv[]) {
	printf("\n\n\nOBJ2PRG.C64 V072882\n(C) 1982 BY COMMODORE BUSINESS MACHINES\n");
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "CBM HEX to C64 PRG converter\n");
        fprintf(stderr, "Usage: %s input.obj [output.prg]\n", argv[0]);
        return 1;
    }

    const char *in_name = argv[1];
    const char *out_name = (argc == 3) ? argv[2] : 
        (strrchr(in_name, '.') ? 
         (strcat(strdup(in_name), ".prg"), strrchr(in_name, '.')-in_name+in_name) : 
         strcat(strdup(in_name), ".prg"));

    FILE *f = fopen(in_name, "rb");
    if (!f) {
        perror("Cannot open input file");
        return 1;
    }

    for(unsigned int j=0; j <= 8192; j++)
	{
		memory[j] = pad;
	}
	
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        if (parse_hex_line(line)) break;  // EOF record
    }
    fclose(f);

    if (load_address == 0) {
        fprintf(stderr, "No valid data found in hex file\n");
        return 1;
    }

    FILE *out = fopen(out_name, "wb");
    if (!out) {
        perror("Cannot create output file");
        return 1;
    }

    // Write little-endian load address
    fputc(load_address & 0xFF, out);
    fputc(load_address >> 8,   out);

    // Write the actual data
    size_t bytes_written = fwrite(memory + load_address, 1,
                                  max_addr - load_address + 1, out);

    fclose(out);

    printf("%s -> %s  (load address $%04X, %zu bytes)\n",
           in_name, out_name, load_address, bytes_written);

    return 0;
}
