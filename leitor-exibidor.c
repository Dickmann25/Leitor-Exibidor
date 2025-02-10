#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#define MAX_CONSTANTS 1000 // Defina um valor adequado para o número máximo de constantes

// Estrutura para armazenar o índice e a posição
typedef struct {
    int index;
    long position;
    uint8_t tag;
} IndexPosition;

typedef struct {
    long position;
    IndexPosition constant_positions[MAX_CONSTANTS];
    size_t constant_positions_count;
} ConstantPoolResult;

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

// Função para ler o arquivo .class e exibir o magic number
void read_magic_number(const char *filename) {
        FILE *file = fopen(filename, "rb");
        if (!file) {
            perror("Erro ao abrir arquivo");
            exit(EXIT_FAILURE);
        }

    uint32_t magic; // Magic number é um inteiro de 4 bytes
    read_bytes(file, &magic, sizeof(magic), 0); // Lê o magic number

    fclose(file); // Fecha o arquivo após a leitura

    // Ajusta a ordem dos bytes para big-endian, se necessário
    magic = to_big_endian_32(magic);

    // Verifica se o magic number corresponde ao esperado
    if (magic == 0xCAFEBABE) {
        printf("Magic number: 0x%X (Arquivo valido .class)\n", magic);
    } else {
        printf("Magic number: 0x%X (Nao e um arquivo valido .class)\n", magic);
    }
}

// Função para ler o arquivo .class e minor e major
void read_minor_major(const char *filename){
    FILE* file = abre_arquivo(filename);
    uint16_t major_version, minor_version;

    // Lê as versões (major_version e minor_version)
    read_bytes(file, &minor_version, sizeof(minor_version), 4); // Lê o minor_version
    read_bytes(file, &major_version, sizeof(major_version), 6); // Lê o major_version

    // Ajusta a ordem dos bytes para big-endian, se necessário
    minor_version = (uint16_t)to_big_endian_16((uint32_t)minor_version);
    major_version = (uint16_t)to_big_endian_16((uint32_t)major_version);

    // Exibe as versões
    printf("Minor version: %u\n", minor_version);
    printf("Major version: %u\n", major_version);

    fclose(file); // Fecha o arquivo após a leitura
}

// Função para ler o arquivo .class e acess_flags
void read_access_flags(const char *filename, ConstantPoolResult *pos, int id) {
    FILE* file = abre_arquivo(filename);

    uint16_t access_flags;

    read_bytes(file, &access_flags, sizeof(access_flags), pos->position); // Lê os access_flags

    pos->position = pos->position + 2;

    // Ajusta a ordem dos bytes para big-endian, se necessário
    access_flags = to_big_endian_16(access_flags);

    // Exibe os access_flags
    printf("\nAccess Flags: (0x%04X), ", access_flags);

    if(id == 0){ //flags gerais
        if (access_flags & 0x0001) printf("ACC_PUBLIC ");
        if (access_flags & 0x0010) printf("ACC_FINAL");
        if (access_flags & 0x0020) printf("ACC_SUPER ");
        if (access_flags & 0x0200) printf("ACC_INTERFACE ");
        if (access_flags & 0x0400) printf("ACC_ABSTRACT ");
        if (access_flags & 0x1000) printf("ACC_SYNTHETIC ");
        if (access_flags & 0x2000) printf("ACC_ANNOTETION ");
        if (access_flags & 0x4000) printf("ACC_ENUM ");
        if (access_flags & 0x8000) printf("ACC_MODULE ");
    }
    else if(id ==1){ //flags para fields
        if (access_flags & 0x0001) printf("ACC_PUBLIC ");   
        if (access_flags & 0x0002) printf("ACC_PRIVATE ");     
        if (access_flags & 0x0004) printf("ACC_PROTECTED "); 
        if (access_flags & 0x0008) printf("ACC_STATIC ");
        if (access_flags & 0x0010) printf("ACC_FINAL ");
        if (access_flags & 0x0040) printf("ACC_VOLATILE ");   
        if (access_flags & 0x0080) printf("ACC_TRANSIENT ");  
        if (access_flags & 0x1000) printf("ACC_SYNTHETIC ");  
        if (access_flags & 0x4000) printf("ACC_ENUM ");   
    }
    else{ //flags para methods
        if (access_flags & 0x0001) printf("ACC_PUBLIC ");   
        if (access_flags & 0x0002) printf("ACC_PRIVATE ");     
        if (access_flags & 0x0004) printf("ACC_PROTECTED "); 
        if (access_flags & 0x0008) printf("ACC_STATIC ");
        if (access_flags & 0x0010) printf("ACC_FINAL ");
        if (access_flags & 0x0020) printf("ACC_SYNCHRONIZED "); 
        if (access_flags & 0x0040) printf("ACC_BRIDGE ");   
        if (access_flags & 0x0080) printf("ACC_VARARGS "); 
        if (access_flags & 0x0100) printf("ACC_NATIVE ");   
        if (access_flags & 0x0400) printf("ACC_ABSTRACT ");
        if (access_flags & 0x0800) printf("ACC_STRICT ");   
        if (access_flags & 0x1000) printf("ACC_SYNTHETIC ");   
    }
    printf("\n");

    fclose(file); // Fecha o arquivo após a leitura
}

// Função para ler o arquivo .class e exibir o this_class
void read_this_class(const char *filename, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);

    uint16_t this_class_index;

    read_bytes(file, &this_class_index, sizeof(this_class_index), pos->position); // Lê os this_class

    pos->position = pos->position + 2;

    // Ajusta a ordem dos bytes para big-endian, se necessário
    this_class_index = to_big_endian_16(this_class_index);

    // Exibe os this_class
    printf("\nThis Class: Index %u\n          ", this_class_index);

    display_constant(file, this_class_index, pos->constant_positions[this_class_index - 1].position, pos->constant_positions[this_class_index - 1].tag, pos->constant_positions, pos->constant_positions_count, 20);
}

// Função para ler o arquivo .class e exibir o super_class
void read_super_class(const char *filename, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);

    uint16_t super_class_index;

    read_bytes(file, &super_class_index, sizeof(super_class_index), pos->position); // Lê os super_class

    pos->position = pos->position + 2;

    // Ajusta a ordem dos bytes para big-endian, se necessário
    super_class_index = to_big_endian_16(super_class_index);

    // Exibe os super_class
    printf("\nSuper Class: Index %u\n          ", super_class_index);

    display_constant(file, super_class_index, pos->constant_positions[super_class_index - 1].position, pos->constant_positions[super_class_index - 1].tag, pos->constant_positions, pos->constant_positions_count,  20);
}

// Função para ler o arquivo .class e exibir o interfaces_count
uint16_t read_interfaces_count(const char *filename, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);

    uint16_t interfaces_count;

    read_bytes(file, &interfaces_count, sizeof(interfaces_count), pos -> position); // Lê os interfaces_count

    pos->position = pos->position + 2;

    // Ajusta a ordem dos bytes para big-endian, se necessário
    interfaces_count = to_big_endian_16(interfaces_count);

    // Exibe os interfaces_count
    printf("\nInterfaces: %u\n", interfaces_count);

    return interfaces_count;

}

// Função para ler o arquivo .class e exibir o fields_count
uint16_t read_fields_count(const char *filename, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);

    uint16_t fields_count;

    read_bytes(file, &fields_count, sizeof(fields_count), pos -> position); // Lê os fields_count

    pos->position = pos->position + 2;

    // Ajusta a ordem dos bytes para big-endian, se necessário
    fields_count = to_big_endian_16(fields_count);

    // Exibe os fields_count
    printf("\nFields: %u\n", fields_count);

    return fields_count;

}

// Função para ler o arquivo .class e exibir o method_count
uint16_t read_method_count(const char *filename, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);

    uint16_t method_count;

    read_bytes(file, &method_count, sizeof(method_count), pos -> position); // Lê os method_count

    pos->position = pos->position + 2;

    // Ajusta a ordem dos bytes para big-endian, se necessário
    method_count = to_big_endian_16(method_count);

    // Exibe os method_count
    printf("\nMetodos: %u\n", method_count);

    return method_count;

}

// Função para ler o arquivo .class e exibir o atribute_count
uint16_t read_atribute_count(const char *filename, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);

    uint16_t atribute_count;

    read_bytes(file, &atribute_count, sizeof(atribute_count), pos -> position); // Lê os atribute_count

    pos->position = pos->position + 2;

    // Ajusta a ordem dos bytes para big-endian, se necessário
    atribute_count = to_big_endian_16(atribute_count);

    // Exibe os atribute_count;

    printf("\nAtributos: %u\n", atribute_count);

    return atribute_count;

}

// Função para ler o arquivo .class e exibir constant_pool_count
uint16_t read_constant_pool_count(const char *filename){
    FILE* file = abre_arquivo(filename);
    uint16_t constant_pool_count;

    read_bytes(file, &constant_pool_count, sizeof(constant_pool_count), 8); // Lê o constant_pool_count

    // Ajusta a ordem dos bytes para big-endian, se necessário
    constant_pool_count = (uint16_t)to_big_endian_16((uint32_t)constant_pool_count);

    // Exibe as versões
    printf("Constant Pool Count: %u\n", constant_pool_count);

    fclose(file); // Fecha o arquivo após a leitura

    return constant_pool_count;
}

// Função para ler o arquivo .class e exibir constant_pool
ConstantPoolResult read_constant_pool(const char *filename){
    FILE* file = abre_arquivo(filename);
    uint16_t constant_pool_count;

    constant_pool_count = read_constant_pool_count(filename);

    IndexPosition  constant_positions[MAX_CONSTANTS];
    size_t constant_positions_count = 0;

    long position = 10; // A posição inicial da tabela de constantes
    for (int i = 1; i < constant_pool_count; i++) {
        uint8_t tag;
        read_bytes(file, &tag, sizeof(tag), position); // Lê o tag da constante
        uint16_t entry_size = 0;
        if (tag == 1) { // CONSTANT_Utf8
            uint16_t length;
            read_bytes(file, &length, sizeof(length), position + 1);
            length = to_big_endian_16(length);
            entry_size = 3 + length; // Tag + Length + String content
            } else if (tag == 3 || tag == 4) { // CONSTANT_Integer or CONSTANT_Float
                entry_size = 5; // Tag + 4 bytes for value
            } else if (tag == 5 || tag == 6) { // CONSTANT_Long or CONSTANT_Double
                entry_size = 9; // Tag + 8 bytes for value
            } else if (tag == 7 || tag == 8) { // CONSTANT_Class or CONSTANT_String
                entry_size = 3; // Tag + 2 bytes for index
            } else if (tag == 9 || tag == 10 || tag == 11) { // CONSTANT_Fieldref, CONSTANT_Methodref, CONSTANT_InterfaceMethodref
                entry_size = 5; // Tag + 2 bytes for class_index + 2 bytes for name_and_type_index
            } else if (tag == 12) { // CONSTANT_NameAndType
                entry_size = 5; // Tag + 2 bytes for name_index + 2 bytes for descriptor_index
            } else if (tag == 15) { // CONSTANT_MethodHandle
                entry_size = 4; // Tag + 1 byte for reference_kind + 2 bytes for reference_index
            } else if (tag == 16) { // CONSTANT_MethodType
                entry_size = 3; // Tag + 2 bytes for descriptor_index
            } else if (tag == 17 || tag == 18) { // CONSTANT_Dynamic or CONSTANT_InvokeDynamic
                entry_size = 5; // Tag + 2 bytes for bootstrap_method_attr_index + 2 bytes for name_and_type_index
            } else if (tag == 19 || tag == 20) { // CONSTANT_Module or CONSTANT_Package
                entry_size = 3; // Tag + 2 bytes for name_index
            } else {
                entry_size = 1; // Tag alone for unknown constants
            }   
        constant_positions[constant_positions_count].index = i;
        if(tag == 5 || tag == 6){
            i = i + 1;
        }
        constant_positions[constant_positions_count].position = position;
        constant_positions[constant_positions_count].tag = tag; // Salva a tag
        constant_positions_count++;

        position += entry_size; // Avança para a próxima constante

    }
    
    for (int i = 0; i < constant_positions_count; i++){
        printf("\n#%i = ", constant_positions[i].index);
        if (constant_positions[i].tag == 1){
            printf("Utf8: ");
        }
        display_constant(file, constant_positions[i].index, constant_positions[i].position, constant_positions[i].tag, constant_positions, constant_positions_count, 10);
    }

    fclose(file); // Fecha o arquivo após a leitura

    ConstantPoolResult result;
    result.position = position;
    memcpy(result.constant_positions, constant_positions, sizeof(constant_positions));
    result.constant_positions_count = constant_positions_count;
    return result;
}

// Função para ler o arquivo .class e exibir as constantes
void display_constant(FILE *file, uint16_t index, long position, uint8_t tag, IndexPosition *dic, size_t constant_positions_count, int indent) {
    
    switch (tag) {
        case 1: { // CONSTANT_Utf8
            uint16_t length;
            
            // Lê o comprimento da string UTF-8
            read_bytes(file, &length, sizeof(length), position + 1);
            length = to_big_endian_16(length); // Ajuste de endianess

            // Aloca memória para a string UTF-8
            char *utf8_string = malloc(length + 1); // +1 para o terminador nulo '\0'
            if (utf8_string == NULL) {
                perror("Erro ao alocar memória para a string UTF-8");
                exit(EXIT_FAILURE);
            }

            // Lê os bytes da string UTF-8
            read_bytes(file, utf8_string, length, position + 3);

            // Adiciona o terminador nulo para a string
            utf8_string[length] = '\0';
            
            // Exibe a string UTF-8
            printf("%s\n", utf8_string);

            // Libera a memória alocada
            free(utf8_string);

            break;
        }
        case 3: { // CONSTANT_Integer
            uint32_t value;
            read_bytes(file, &value, sizeof(value), position + 1); // Lê o valor inteiro
            value = to_big_endian_32(value); // Ajuste de endianess
            printf("Integer: %u\n", value);
            break;
        }
        case 4: { // CONSTANT_Float
            uint32_t value;
            read_bytes(file, &value, sizeof(value), position + 1); // Lê o valor float
            value = to_big_endian_32(value); // Ajuste de endianess
            printf("Float: %f\n", *((float*)&value)); // Converte para float
            break;
        }
        case 5: { // CONSTANT_Long
            uint32_t high_bytes, low_bytes;
            
            // Lê os 4 bytes para high_bytes e low_bytes
            read_bytes(file, &high_bytes, sizeof(high_bytes), position + 1); // Lê o high_bytes
            read_bytes(file, &low_bytes, sizeof(low_bytes), position + 5);  // Lê o low_bytes
            


            // Ajuste de endianess para ambos os valores
            high_bytes = to_big_endian_32(high_bytes);
            low_bytes = to_big_endian_32(low_bytes);
            
            // Combina os 2 valores de 32 bits para formar o valor de 64 bits
            uint64_t value = ((uint64_t)high_bytes << 32) | (uint64_t)low_bytes;

            // Converte o valor combinado para um inteiro com sinal de 64 bits
            int64_t long_value = (int64_t)value;

            // Exibe o valor longo
            printf("Long: %" PRId64 "\n", long_value);
            break;
        }
        case 6: { // CONSTANT_Double
            uint32_t high_bytes, low_bytes;
            
            // Lê os 4 bytes para high_bytes e low_bytes
            read_bytes(file, &high_bytes, sizeof(high_bytes), position + 1); // Lê o high_bytes
            read_bytes(file, &low_bytes, sizeof(low_bytes), position + 5);  // Lê o low_bytes
            
            // Ajuste de endianess para ambos os valores
            high_bytes = to_big_endian_32(high_bytes);
            low_bytes = to_big_endian_32(low_bytes);
            
            // Combina os 2 valores de 32 bits para formar o valor de 64 bits
            uint64_t value = ((uint64_t)high_bytes << 32) | (uint64_t)low_bytes;
            
            // Converte o valor de 64 bits para um double
            double double_value;
            memcpy(&double_value, &value, sizeof(double)); // Converte o valor de 64 bits para double
            
            // Exibe o valor double
            printf("Double: %f\n", double_value);
    
            break;
        }
        case 7: { // CONSTANT_Class
        uint16_t class_index;
        
        // Lê o índice da classe
        read_bytes(file, &class_index, sizeof(class_index), position + 1);
        // Ajusta a ordem dos bytes para big-endian
        class_index = to_big_endian_16(class_index);
        printf("Class: Name index %u\n%*s",class_index, indent, "");
        // Exibe o nome da classe
        printf("Name(Utf8): ");

        position = find_position_by_index(dic, class_index, constant_positions_count);
        display_constant(file, class_index, position, 1, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Utf8 associado ao índice

        break;
        }
        case 8: { // CONSTANT_String
            uint16_t string_index;
            read_bytes(file, &string_index, sizeof(string_index), position + 1); // Lê o string_index
            string_index = to_big_endian_16(string_index);
            printf("String: String index %u\n", string_index);

            position = find_position_by_index(dic, string_index, constant_positions_count);
            printf("String(Utf8): ");
            display_constant(file, string_index, position, 1, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Utf8 associado ao índice

            break;
        }
        case 9: { // CONSTANT_Fieldref      
            uint16_t class_index, name_and_type_index;
            // Lê os índices da classe e do name_and_type
            read_bytes(file, &class_index, sizeof(class_index), position + 1); // Lê o class_index
            read_bytes(file, &name_and_type_index, sizeof(name_and_type_index), position + 3); // Lê o name_and_type_index

            // Ajusta a ordem dos bytes para big-endian
            class_index = to_big_endian_16(class_index);
            name_and_type_index = to_big_endian_16(name_and_type_index);

            // Exibe os índices
            printf("Fieldref: Class index %u, Name and Type index %u\n", class_index, name_and_type_index);

            position = find_position_by_index(dic, class_index, constant_positions_count);
            // Exibe os detalhes da classe
            display_constant(file, class_index, position, 7, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Class associado ao índice

            position = find_position_by_index(dic, name_and_type_index, constant_positions_count);
            // Exibe os detalhes do name_and_type
            display_constant(file, name_and_type_index, position, 12, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_NameAndType associado ao índice

            break;
        }
        case 10: { // CONSTANT_Methodref
            uint16_t class_index, name_and_type_index;
            // Lê os índices da classe e do name_and_type
            read_bytes(file, &class_index, sizeof(class_index), position + 1); // Lê o class_index
            read_bytes(file, &name_and_type_index, sizeof(name_and_type_index), position + 3); // Lê o name_and_type_index

            // Ajusta a ordem dos bytes para big-endian
            class_index = to_big_endian_16(class_index);
            name_and_type_index = to_big_endian_16(name_and_type_index);

            // Exibe os índices
            printf("Methodref: Class index %u, Name and Type index %u\n%*s", class_index, name_and_type_index, indent, "");

            position = find_position_by_index(dic, class_index, constant_positions_count);
            // Exibe os detalhes da classe
            display_constant(file, class_index, position, 7, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Class associado ao índice

            printf("%*s", indent, "");
            position = find_position_by_index(dic, name_and_type_index, constant_positions_count);
            // Exibe os detalhes do name_and_type
            display_constant(file, name_and_type_index, position, 12, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_NameAndType associado ao índice

            break;
        }
        case 11: { // CONSTANT_InterfaceMethodref
            uint16_t class_index, name_and_type_index;
            // Lê os índices da classe e do name_and_type
            read_bytes(file, &class_index, sizeof(class_index), position + 1); // Lê o class_index
            read_bytes(file, &name_and_type_index, sizeof(name_and_type_index), position + 3); // Lê o name_and_type_index

            // Ajusta a ordem dos bytes para big-endian
            class_index = to_big_endian_16(class_index);
            name_and_type_index = to_big_endian_16(name_and_type_index);

            // Exibe os índices
            printf("InterfaceMethodref: Class index %u, Name and Type index %u\n", class_index, name_and_type_index);

            position = find_position_by_index(dic, class_index, constant_positions_count);
            // Exibe os detalhes da classe
            display_constant(file, class_index, position, 7, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Class associado ao índice

            position = find_position_by_index(dic, name_and_type_index, constant_positions_count);
            // Exibe os detalhes do name_and_type
            display_constant(file, name_and_type_index, position, 12, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_NameAndType associado ao índice

            break;
        }
        case 12: { // CONSTANT_NameAndType
            uint16_t name_index, type_index;

            // Lê os índices para o nome e o tipo
            read_bytes(file, &name_index, sizeof(name_index), position + 1); // Lê o name_index
            read_bytes(file, &type_index, sizeof(type_index), position + 3); // Lê o type_index
            
            // Ajusta a ordem dos bytes para big-endian
            name_index = to_big_endian_16(name_index);
            type_index = to_big_endian_16(type_index);

            // Exibe os índices de nome e tipo
            printf("Name and Type: Name index %u, Descriptor index %u\n%*s", name_index, type_index, indent, "");
            
            position = find_position_by_index(dic, name_index, constant_positions_count);
            // Exibe o nome
            printf("Name(Utf8): ");
            display_constant(file, name_index, position, 1, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Utf8 associado ao nome
            
            position = find_position_by_index(dic, type_index, constant_positions_count);
            // Exibe o tipo
            printf("%*sDescriptor(Utf8): ", indent, "");
            display_constant(file, type_index, position, 1, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Utf8 associado ao tipo

            break;
        }
        case 15: { // CONSTANT_MethodHandle
            uint8_t reference_kind;
            uint16_t reference_index;
            // Lê os índices da classe e do name_and_type
            read_bytes(file, &reference_kind, sizeof(reference_kind), position + 1); // Lê o class_index
            read_bytes(file, &reference_index, sizeof(reference_index), position + 2); // Lê o name_and_type_index

            // Ajusta a ordem dos bytes para big-endian
            reference_index = to_big_endian_16(reference_index);

            // Exibe os índices
            printf("MethodHandle: Reference Kind: %u", reference_kind);

            position = find_position_by_index(dic, reference_index, constant_positions_count);
            // Exibe os detalhes do name_and_type
            if(reference_kind == 1 || reference_kind == 2 || reference_kind == 3 || reference_kind == 4 ){
            display_constant(file, reference_index, position, 9, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Fieldref associado ao índice
            }
            else if(reference_kind == 5 || reference_kind == 8){
            display_constant(file, reference_index, position, 10, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Methodref associado ao índice
            }
            else if(reference_kind == 6 || reference_kind == 7){
            display_constant(file, reference_index, position, 9, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Fieldref associado ao índice
            }
            else if(reference_kind == 9){
            display_constant(file, reference_index, position, 11, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_InterfaceMethodref associado ao índice
            }
            break;
        }
        case 16: { // CONSTANT_MethodType
        uint16_t descriptor_index;
        
        // Lê o índice da classe
        read_bytes(file, &descriptor_index, sizeof(descriptor_index), position + 1);
        // Ajusta a ordem dos bytes para big-endian
        descriptor_index = to_big_endian_16(descriptor_index);
        printf("MethodType: Descriptor index %u\n",descriptor_index);
        // Exibe o nome da classe
        printf("Descriptor(Utf8): ");

        position = find_position_by_index(dic, descriptor_index, constant_positions_count);
        display_constant(file, descriptor_index, position, 1, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Utf8 associado ao índice

        break;

        }
        case 17: { // CONSTANT_Dynamic
            uint16_t name_and_type_index;
            
            // Lê os índices da classe e do name_and_type
            read_bytes(file, &name_and_type_index, sizeof(name_and_type_index), position + 3); // Lê o name_and_type_index

            // Ajusta a ordem dos bytes para big-endian
            name_and_type_index = to_big_endian_16(name_and_type_index);

            printf("CONSTANT_Dynamic: Name and Type index %u\n",  name_and_type_index);

            position = find_position_by_index(dic, name_and_type_index, constant_positions_count);
            // Exibe os detalhes do name_and_type
            display_constant(file, name_and_type_index, position, 12, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_NameAndType associado ao índice

            break;
        }
        case 18: { // CONSTANT_InvokeDynamic
            uint16_t name_and_type_index;

            // Lê os índices da classe e do name_and_type
            read_bytes(file, &name_and_type_index, sizeof(name_and_type_index), position + 3); // Lê o name_and_type_index

            // Ajusta a ordem dos bytes para big-endian
            name_and_type_index = to_big_endian_16(name_and_type_index);

            printf("CONSTANT_InvokeDynamic: Name and Type index %u\n",  name_and_type_index);

            position = find_position_by_index(dic, name_and_type_index, constant_positions_count);

            // Exibe os detalhes do name_and_type
            display_constant(file, name_and_type_index, position, 12, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_NameAndType associado ao índice

            break;
        }
        case 19: { // CONSTANT_Module
            uint16_t name_index;

            read_bytes(file, &name_index, sizeof(name_index), position + 1); // Lê o name_index
            name_index = to_big_endian_16(name_index);
            printf("Module: Name index %u\n", name_index);

            position = find_position_by_index(dic, name_index, constant_positions_count);
            printf("Name(Utf8): ");
            display_constant(file, name_index, position, 1, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Utf8 associado ao índice

            break;
        }
        case 20:{ // CONSTANT_Package
            uint16_t name_index;

            read_bytes(file, &name_index, sizeof(name_index), position + 1); // Lê o name_index
            name_index = to_big_endian_16(name_index);
            printf("Package: Name index %u\n", name_index);

            position = find_position_by_index(dic, name_index, constant_positions_count);
            printf("Name(Utf8): ");
            display_constant(file, name_index, position, 1, dic, constant_positions_count, indent + 10); // Exibe o CONSTANT_Utf8 associado ao índice

            break;
        }
        default:
            printf("Unknown constant type (tag %d)\n", tag);
            break;
        
    }
}

// Função para ler o arquivo .class e exibir as interfaces
void display_interfaces(const char *filename, uint16_t interfaces_count, ConstantPoolResult *dic) {
    FILE* file = abre_arquivo(filename);
    if(interfaces_count == 0){    
        printf("Nenhuma Interface Encontrada\n");

    }
    else{
        for (int i = 0; i < interfaces_count; i++) {
            printf("\n#%i\n", i + 1);

            uint16_t index;
            read_bytes(file, &index, sizeof(index), dic->position);
            index = to_big_endian_16(index);
        
            for (int i = 0; i < dic->position; i++){
                if(index == dic->constant_positions[i].index){
                    printf("Class Index: %u\n          ", index);
                    display_constant(file, dic->constant_positions[i].index, dic->constant_positions[i].position, dic->constant_positions[i].tag, dic->constant_positions, dic->constant_positions_count, 20);
                    dic->position = dic->position + 2;  
                    break;             
                }
            }
        }
    }
}

// Função para ler o arquivo .class e exibir os fields
void display_fields(const char *filename, uint16_t count, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);
    if(count == 0){
        printf("Nenhum Fields Encontrado\n");
    }
    else{
        for (int i = 0; i < count; i++){
            printf("\n#%i", i + 1);

            read_access_flags(filename, pos, 1);

            uint16_t f_index;

            read_bytes(file, &f_index, sizeof(f_index), pos->position);

            f_index = to_big_endian_16(f_index);
            printf("          Name Index: %u\n                    Name(Utf8):", f_index);
            for (int i = 0; i < pos->position; i++){
                if(f_index == pos->constant_positions[i].index){
                    display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                    pos->position = pos->position + 2;
                    break;
                }
            }

            read_bytes(file, &f_index, sizeof(f_index), pos->position);

            f_index = to_big_endian_16(f_index);
            printf("          Descriptor Index: %u\n                    Descriptor(Utf8):", f_index);

            for (int i = 0; i < pos->position; i++){
                if(f_index == pos->constant_positions[i].index){
                    display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                    pos->position = pos->position + 2;
                    break;
                }
            }

            read_bytes(file, &f_index, sizeof(f_index), pos->position);

            f_index = to_big_endian_16(f_index);

            printf("Atribute Count: %u\n", f_index);
            pos->position = pos->position + 2;
            if (f_index != 0){
                
                read_bytes(file, &f_index, sizeof(f_index), pos->position);

                f_index = to_big_endian_16(f_index);

                printf("          Atribute Name Index: %u\n                    Utf8: ", f_index);
                for (int i = 0; i < pos->position; i++){
                    if(f_index == pos->constant_positions[i].index){
                        display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                        pos->position = pos->position + 2;
                        break;
                    }
                }

                uint32_t f_atribute;

                read_bytes(file, &f_atribute, sizeof(f_atribute), pos->position);

                f_atribute = to_big_endian_32(f_atribute);

                printf("          Atribute Lenght: %u\n", f_atribute);
                pos->position = pos->position + 4;

                read_bytes(file, &f_index, sizeof(f_index), pos->position);

                f_index = to_big_endian_16(f_index);
                for (int i = 0; i < pos->position; i++){
                    if(f_index == pos->constant_positions[i].index){
                        display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                        pos->position = pos->position + f_atribute ;
                        break;
                    }
                }
            }
        }
    }
}

// Função para ler o arquivo .class e exibir os metodos
void display_method(const char *filename, uint16_t count, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);
    if(count == 0){
        printf("Nenhum Method Encontrado\n");
    }
    else{
        for (int i = 0; i < count; i++){

            printf("\n#%i", i + 1);

            read_access_flags(filename, pos, 2);

            uint16_t f_index;

            read_bytes(file, &f_index, sizeof(f_index), pos->position);

            f_index = to_big_endian_16(f_index);
            printf("          Name Index: %u\n                    Name(Utf8):", f_index);
            for (int i = 0; i < pos->position; i++){
                if(f_index == pos->constant_positions[i].index){
                    display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                    pos->position = pos->position + 2;
                    break;
                }
            }

            read_bytes(file, &f_index, sizeof(f_index), pos->position);

            f_index = to_big_endian_16(f_index);
            printf("          Descriptor Index: %u\n                    Descriptor(Utf8):", f_index);

            for (int i = 0; i < pos->position; i++){
                if(f_index == pos->constant_positions[i].index){
                    display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                    pos->position = pos->position + 2;
                    break;
                }
            }

            read_bytes(file, &f_index, sizeof(f_index), pos->position);

            f_index = to_big_endian_16(f_index);

            printf("Atribute Count: %u\n", f_index);
            pos->position = pos->position + 2;
            if (f_index != 0){
                
                read_bytes(file, &f_index, sizeof(f_index), pos->position);

                f_index = to_big_endian_16(f_index);

                printf("          Atribute Name Index: %u\n                    Utf8: ", f_index);
                for (int i = 0; i < pos->position; i++){
                    if(f_index == pos->constant_positions[i].index){
                        display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                        pos->position = pos->position + 2;
                        break;
                    }
                }

                uint32_t f_atribute;

                read_bytes(file, &f_atribute, sizeof(f_atribute), pos->position);

                f_atribute = to_big_endian_32(f_atribute);

                printf("          Atribute Lenght: %u\n", f_atribute);
                pos->position = pos->position + 4;

                pos->position = pos->position + f_atribute;

            }
        }
    }
}

// Função para ler o arquivo .class e exibir os atributos
void display_atribute(const char *filename, uint16_t count, ConstantPoolResult *pos){
    FILE* file = abre_arquivo(filename);
    if(count == 0){
        printf("Nenhum Atribute Encontrado\n");
    }
    else{
        for (int i = 0; i < count; i++){
            printf("\n#%i\n", i + 1);

            uint16_t f_index;

            read_bytes(file, &f_index, sizeof(f_index), pos->position);

            f_index = to_big_endian_16(f_index);
            printf("Atribute Name Index: %u\n          Name(Utf8): ", f_index);
            for (int i = 0; i < pos->position; i++){
                if(f_index == pos->constant_positions[i].index){
                    display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                    pos->position = pos->position + 2;
                    break;
                }
            }

            uint32_t f_atribute;

            read_bytes(file, &f_atribute, sizeof(f_atribute), pos->position);

            f_atribute = to_big_endian_32(f_atribute);
            printf("          Atribute Lenght: %u\n          Name(Utf8):", f_atribute);
            pos->position = pos->position + 4;

            read_bytes(file, &f_index, sizeof(f_index), pos->position);

            f_index = to_big_endian_16(f_index);

            for (int i = 0; i < pos->position; i++){
                if(f_index == pos->constant_positions[i].index){
                    display_constant(file, pos->constant_positions[i].index, pos->constant_positions[i].position, pos->constant_positions[i].tag, pos->constant_positions, pos->constant_positions_count, 10);
                    break;
                }
            }
        }
    
    }
}
// Função principal
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.class>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    read_magic_number(argv[1]); // Lê e exibe o magic number do arquivo .class
    read_minor_major(argv[1]); // Lê e exibe o minor major number
    ConstantPoolResult pos = read_constant_pool(argv[1]);
    printf("\n--------------------------------------------------------------------------------------------------------------------------------------\n");
    read_access_flags(argv[1], &pos, 0);
    read_this_class(argv[1], &pos);
    read_super_class(argv[1], &pos);
    printf("\n--------------------------------------------------------------------------------------------------------------------------------------\n");
    uint16_t i_count = read_interfaces_count(argv[1], &pos);
    display_interfaces(argv[1], i_count, &pos);
    printf("\n--------------------------------------------------------------------------------------------------------------------------------------\n");
    uint16_t f_count = read_fields_count(argv[1], &pos);
    display_fields(argv[1], f_count, &pos);
    printf("\n--------------------------------------------------------------------------------------------------------------------------------------\n");
    uint16_t m_count = read_method_count(argv[1], &pos);
    display_method(argv[1], m_count, &pos);
    printf("\n--------------------------------------------------------------------------------------------------------------------------------------\n");
    uint16_t a_count = read_atribute_count(argv[1], &pos);
    display_atribute(argv[1], a_count, &pos);
    return EXIT_SUCCESS;
}