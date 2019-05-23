#include "../cardano.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

static uint32_t PROTOCOL_MAGIC = 1097911063;
const uint64_t MAX_COIN = 45000000000000000;

static uint8_t xprv[XPRV_SIZE] = {
    184,195,6,129,151,72,167,110,161,41,199,230,41,14,134,69,252,102,11,130,239,118,96,47,105,222,121,161,224,33,3,68,230,204,178,226,18,0,127,241,73,38,210,197,140,106,121,228,121,166,197,93,214,61,143,205,137,228,68,192,184,187,225,91,130,170,22,189,10,143,116,141,251,211,193,214,99,168,8,83,253,69,66,180,22,64,33,47,64,200,62,157,254,89,11,0
};

int main(int argc, char *argv[]) {

    cardano_address *input_address = cardano_address_import_base58("2cWKMJemoBaiKK4RN2M5r1KZuKpCZadsfDb9po961jkXUQ5rJdMPs2rkYLVTZrvMMm2Ce");
    cardano_address *output_address = cardano_address_import_base58("2cWKMJemoBaiKK4RN2M5r1KZuKpCZadsfDb9po961jkXUQ5rJdMPs2rkYLVTZrvMMm2Ce");

    cardano_transaction_builder *txbuilder = cardano_transaction_builder_new();

    uint8_t input_txid[32] = 
    {
        0xdb,0x52,0xe6,0xde,0x75,0x06,0xcb,0x47,0x6e,0x65,
        0x46,0x3e,0x34,0xb7,0x48,0x40,0x60,0x37,0x46,0x4e,
        0xe5,0x3d,0xff,0xaf,0xbc,0x95,0x34,0x37,0xf2,0xed,0xf1,0x61
    };

    cardano_txoptr *input = cardano_transaction_output_ptr_new(input_txid, 0);

    cardano_txoutput *output = cardano_transaction_output_new(output_address, 2000);

    if(cardano_transaction_builder_add_input(txbuilder, input, 975120064)
        != CARDANO_RESULT_SUCCESS ) {
            printf("Error adding input");
            exit(1);
        }
    cardano_transaction_builder_add_output(txbuilder, output);

    uint8_t txid[32] = {
        29,
        63,
        139,
        96,
        63,
        81,
        168,
        30,
        171,
        157,
        66,
        238,
        253,
        201,
        81,
        133,
        75,
        100,
        145,
        162,
        109,
        152,
        240,
        112,
        190,
        19,
        23,
        248,
        101,
        85,
        133,
        191
    };

    printf("%8cTxid:", ' ');
    for (unsigned int j = 0; j < sizeof(txid); ++j)
    {
        printf("%02x", txid[j]);
    }
    printf("\n");


    cardano_transaction *tx;
    cardano_transaction_builder_finalize(txbuilder, &tx);

    cardano_transaction_finalized *tf = cardano_transaction_finalized_new(tx);

    if(cardano_transaction_finalized_add_witness(tf, xprv, PROTOCOL_MAGIC, txid)
        != CARDANO_RESULT_SUCCESS)
    {
        printf("Error adding witness\n");
    }

    cardano_signed_transaction *txaux;
    cardano_transaction_finalized_output(tf, &txaux);

    uint8_t *bytes;
    size_t size;
    if(cardano_signed_transaction_serialize(txaux, &bytes, &size)
    != CARDANO_RESULT_SUCCESS) {
        printf("Error serializing\n");
    }

    FILE *file = fopen("tx.binary", "w");
    if(fwrite(bytes, sizeof(uint8_t), size, file) == 0) {
        printf("Error writing to file\n");
    }
    fclose(file);

    printf("Encoded transaction:\n");
    printf("--------------------\n");
    if(system("base64 -w 0 tx.binary") == -1) {
        printf("System error\n");
    };
    printf("\n");

    cardano_signed_transaction_serialized_delete(bytes, size);
    cardano_transaction_delete(tx);
    cardano_transaction_finalized_delete(tf);
    cardano_transaction_signed_delete(txaux);
}