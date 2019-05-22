#include "../cardano.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

static uint32_t PROTOCOL_MAGIC = 764824073;
const uint64_t MAX_COIN = 45000000000000000;

static uint8_t xprv[XPRV_SIZE] = {
    136,
    33,
    70,
    123,
    239,
    210,
    196,
    225,
    74,
    10,
    105,
    125,
    208,
    221,
    238,
    69,
    242,
    138,
    178,
    155,
    111,
    254,
    21,
    98,
    67,
    255,
    58,
    238,
    92,
    83,
    246,
    70,
    242,
    135,
    47,
    192,
    13,
    99,
    195,
    154,
    150,
    81,
    160,
    27,
    1,
    192,
    118,
    190,
    209,
    137,
    8,
    95,
    93,
    180,
    84,
    133,
    150,
    4,
    15,
    71,
    253,
    181,
    102,
    3,
    43,
    27,
    19,
    240,
    3,
    60,
    44,
    57,
    30,
    242,
    158,
    241,
    139,
    51,
    140,
    17,
    111,
    94,
    7,
    147,
    133,
    102,
    40,
    6,
    139,
    95,
    54,
    195,
    204,
    130,
    18,
    178,
};

int main(int argc, char *argv[]) {

    cardano_address *input_address = cardano_address_import_base58("Ae2tdPwUPEYxnas8ediApdeepciYUGv7jTGFveEkzHDMtjNq4xdF78wCBps");
    cardano_address *output_address = cardano_address_import_base58("Ae2tdPwUPEZN5M1TeLiNF2wuCTDUH4VQk9k4xFZzuEnN8saHWM8j18tZ7dg");

    cardano_transaction_builder *txbuilder = cardano_transaction_builder_new();

    uint8_t fake_id[32] = {
        0
    };

    cardano_txoptr *input = cardano_transaction_output_ptr_new(fake_id, 1);

    cardano_txoutput *output = cardano_transaction_output_new(output_address, 1000);

    if(cardano_transaction_builder_add_input(txbuilder, input, 1000)
        != CARDANO_RESULT_SUCCESS ) {
            printf("Error adding input");
            exit(1);
        }
    cardano_transaction_builder_add_output(txbuilder, output);

    uint8_t txid[32] = {
        83,
        129,
        146,
        65,
        156,
        168,
        180,
        159,
        87,
        157,
        241,
        49,
        141,
        189,
        188,
        241,
        156,
        172,
        53,
        44,
        97,
        203,
        236,
        94,
        176,
        172,
        21,
        214,
        165,
        88,
        95,
        52
    };

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

    if(system("base64 -w 0 tx.binary") == -1) {
        printf("System error\n");
    };

    cardano_signed_transaction_serialized_delete(bytes, size);
    cardano_transaction_delete(tx);
    cardano_transaction_finalized_delete(tf);
    cardano_transaction_signed_delete(txaux);
}