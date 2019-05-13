#include <stdlib.h>
#include <string.h>
#include "../cardano.h"
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <curl/curl.h>

typedef struct memory_buffer {
    size_t size;
    char *bytes;
} memory_buffer_t;

void print_inputs(cardano_signed_transaction *tx)
{
    printf("%4cInputs\n", ' ');
    cardano_txoptr **inputs;
    size_t inputs_size;

    cardano_signed_transaction_get_inputs(tx, &inputs, &inputs_size);

    printf("%8c-\n", ' ');
    for (unsigned int i = 0; i < inputs_size; ++i)
    {
        uint32_t index = cardano_transaction_txoptr_index(inputs[i]);
        cardano_txid_t txid;
        cardano_transaction_txoptr_txid(inputs[i], &txid);

        printf("%8cOffset %d\n", ' ', index);

        printf("%8cTxid:", ' ');
        for (unsigned int j = 0; j < sizeof(txid); ++j)
        {
            printf("%02x", txid.bytes[j]);
        }
        printf("\n");
    }
    cardano_signed_transaction_delete_inputs(inputs, inputs_size);
}

void print_outputs(cardano_signed_transaction *tx)
{
    printf("%4c" "Outputs\n", ' ');
    cardano_txoutput **outputs;
    size_t outputs_size;

    cardano_signed_transaction_get_outputs(tx, &outputs, &outputs_size);

    for (unsigned int i = 0; i < outputs_size; ++i)
    {
        cardano_address *address = cardano_transaction_txoutput_address(outputs[i]);
        char *address_base58 = cardano_address_export_base58(address);
        uint64_t value = cardano_transaction_txoutput_value(outputs[i]);
        printf("%8c" "-\n", ' ');
        printf("%8c" "Value: %" PRIu64 "\n", ' ', value);
        printf("%8c" "Address: %s\n", ' ', address_base58);
        cardano_account_delete_addresses(&address_base58, 1);
    }
    cardano_signed_transaction_delete_outputs(outputs, outputs_size);
}

void print_transaction(cardano_signed_transaction *transaction)
{
    print_inputs(transaction);
    print_outputs(transaction);
}

void print_block(cardano_block *block)
{
    cardano_block_header *header = cardano_block_get_header(block);

    char *hash = cardano_block_header_compute_hash(header);
    printf("Block id: %s\n", hash);
    cardano_block_delete_hash(hash);

    char *previous_hash = cardano_block_header_previous_hash(header);
    printf("Previous block id: %s\n", previous_hash);
    cardano_block_delete_hash(previous_hash);

    cardano_block_header_delete(header);
}

void print_transactions(cardano_block *block) {
    size_t transactions_size;
    cardano_signed_transaction **transactions;
    cardano_result rc = cardano_block_get_transactions(block, &transactions, &transactions_size);
    assert(rc == CARDANO_RESULT_SUCCESS);

    printf("Transactions: (%zu)\n", transactions_size);
    for (unsigned int i = 0; i < transactions_size; ++i)
    {
        printf("%4c-\n", ' ');
        print_transaction(transactions[i]);
    }

    cardano_block_delete_transactions(transactions, transactions_size);
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    memory_buffer_t *buffer = (memory_buffer_t *) userdata;
    size_t new_size = buffer->size + nmemb;
    char *new_bytes = realloc(buffer->bytes, new_size + 1);

    buffer->bytes = new_bytes;
    memcpy(buffer->bytes + buffer->size, ptr, nmemb);

    buffer->size = new_size;

    buffer->bytes[buffer->size] = 0;

    return nmemb;
}

memory_buffer_t get_block(char *blockid) {
    CURL *curl = curl_easy_init();
    if (curl) {
        char *base_url = "http://localhost:8080/mainnet/block/";
        char *url = malloc(strlen(base_url) + strlen(blockid) + 1); 
        strcpy(url, base_url);
        strcat(url, blockid);

        printf("%s\n", url);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        
        memory_buffer_t buffer;
        buffer.size = 0;
        buffer.bytes = NULL;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        
        CURLcode res = curl_easy_perform(curl);
        /* check for errors */ 
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            exit(1);
        }

        curl_easy_cleanup(curl);
        curl = NULL;

        return buffer;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Expected block id as first parameter\n");
        exit(1);
    }

    memory_buffer_t raw_block = get_block(argv[1]);

    cardano_block *block;
    cardano_result rc = cardano_raw_block_decode(raw_block.bytes, raw_block.size, &block);

    if(rc != CARDANO_RESULT_SUCCESS) {
        printf("Error decoding raw block\n");
        exit(1);
    }

    free(raw_block.bytes);

    print_block(block);

    print_transactions(block);

    cardano_block_delete(block);
}