#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 100

// Struct to map your files
typedef struct {
    int id;
    int weight; // File size in MB
    int value;  // Request frequency
} CacheItem;

// Helper function to find maximum of two integers
int max(int a, int b) { return (a > b) ? a : b; }

int main() {
    char input_buffer[2048];
    CacheItem items[MAX_ITEMS];
    int capacity, item_count = 0;

    // 1. INGEST DATA FROM JAVA
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
        printf("ERR: Failed to read stdin\n");
        return 1;
    }

    // 2. PARSE THE STRING
    char *token = strtok(input_buffer, ",");
    if (token != NULL) capacity = atoi(token);
    
    token = strtok(NULL, ",");
    if (token != NULL) item_count = atoi(token);

    if (item_count > MAX_ITEMS) item_count = MAX_ITEMS;

    int idx = 0;
    while ((token = strtok(NULL, ",")) != NULL && idx < item_count) {
        sscanf(token, "%d:%d:%d", &items[idx].id, &items[idx].weight, &items[idx].value);
        idx++;
    }

    // 3. THE ALGORITHM ENGINE (Dynamic Programming 0/1 Knapsack)
    int dp[MAX_ITEMS + 1][capacity + 1];
    
    /* * ==============================================================
     * 🛑 STOP WAITING FOR ME TO DO YOUR LAB WORK 🛑
     * Insert your VTU 0/1 Knapsack Dynamic Programming matrix logic here.
     * You have 'item_count' items in the 'items' array.
     * You have a max weight of 'capacity'.
     * Build the DP table. 
     * ==============================================================
     */
    
    // (Initialize the table to 0s to prevent memory garbage while you write your logic)
    for (int i = 0; i <= item_count; i++) {
        for (int w = 0; w <= capacity; w++) {
            dp[i][w] = 0; 
        }
    }


    /* * ==============================================================
     * BACKTRACKING TO FIND THE EXACT FILES TO KEEP
     * Write the while loop that starts at dp[item_count][capacity]
     * and traces back to find WHICH specific item IDs were included.
     * Print them out separated by commas.
     * ==============================================================
     */

    // Dummy output so your bridge doesn't crash before you write the logic
    // DELETE THIS once you write the backtracking print loop.
    printf("101,103\n"); 

    return 0;
}
