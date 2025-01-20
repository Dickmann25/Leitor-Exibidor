
#include "auxiliar.h"

uint32_t to_big_endian_32(uint32_t value) {
    uint8_t *bytes = (uint8_t *)&value;
    return (uint32_t)bytes[0] << 24 |
           (uint32_t)bytes[1] << 16 |
           (uint32_t)bytes[2] << 8 |
           (uint32_t)bytes[3];
}

uint16_t to_big_endian_16(uint16_t value) {
    uint8_t *bytes = (uint8_t *)&value;
    return (uint16_t)bytes[0] << 8 | (uint16_t)bytes[1];
}

FILE* abre_arquivo(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }
    return file;
}

long find_position_by_index(IndexPosition *constant_pool_map, uint16_t index, uint16_t constant_pool_count) {
    for (int i = 1; i < constant_pool_count; i++) {
        if (constant_pool_map[i].index == index) {
            return constant_pool_map[i].position;  // Retorna a posição correspondente ao índice
        }
    }
    return -1;  // Retorna -1 se o índice não for encontrado
}

// Função para ler bytes de um arquivo
void read_bytes(FILE *file, void *buffer, size_t size, long position) {
    if (fseek(file, position, SEEK_SET) != 0) { // Move o ponteiro para a posição especificada
        perror("Erro ao mover o ponteiro de leitura");
        exit(EXIT_FAILURE);
    }

    if (fread(buffer, size, 1, file) != 1) { // Lê os bytes da posição atual
        perror("Erro ao ler arquivo");
        exit(EXIT_FAILURE);
    }
}