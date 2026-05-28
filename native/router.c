#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 999
#define MAX_SERVERS 10

// 1. Your VTU Dijkstra Logic (Unchanged)
void dijkstra(int c[10][10], int n, int s, int d[10]) {
    int v[10], min, u, i, j;
    
    // Initialization
    for (i = 1; i <= n; i++) {
        d[i] = c[s][i];
        v[i] = 0;
    }
    v[s] = 1; 

    // Calculation Loop
    for (i = 1; i <= n; i++) {
        min = INF;
        for (j = 1; j <= n; j++) {
            if (v[j] == 0 && d[j] < min) {
                min = d[j];
                u = j;
            }
        }
        
        // Prevent array out-of-bounds if graph is disconnected
        if (min == INF) break; 
        
        v[u] = 1;
        for (j = 1; j <= n; j++) {
            if (v[j] == 0 && (d[u] + c[u][j]) < d[j]) {
                d[j] = d[u] + c[u][j];
            }
        }
    }
}

// 2. The Systems Gateway Logic
int main() {
    char input_buffer[512];
    int c[10][10];
    int d[10];
    int n = 0; // Number of servers
    
    // Initialize adjacency matrix to INF
    for (int i = 1; i < MAX_SERVERS; i++) {
        for (int j = 1; j < MAX_SERVERS; j++) {
            c[i][j] = (i == j) ? 0 : INF;
        }
    }

    // Ingest the latency string from Java Gateway via standard input
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
        printf("ERR: Failed to read stdin\n");
        return 1;
    }

    // Parse the string (Format: "1:10,2:45,3:12")
    // We assume the Gateway is Node 0 (or some source node). 
    // In this simplified setup, we'll treat the gateway as connecting directly to nodes 1, 2, 3...
    int source_node = 0;
    
    char *pair = strtok(input_buffer, ",");
    while (pair != NULL && n < MAX_SERVERS - 1) {
        int id, latency;
        sscanf(pair, "%d:%d", &id, &latency);
        
        // Build the edge from Source (0) to the target server (id)
        c[source_node][id] = latency; 
        c[id][source_node] = latency; // Assuming bidirectional for simplicity
        
        if (id > n) n = id; // Track highest node ID for array limits
        pair = strtok(NULL, ",");
    }
    
    // Ensure we include the source node in the total count
    n = n > 0 ? n : 0; 

    // Execute your algorithm
    if (n > 0) {
        dijkstra(c, n, source_node, d);
    } else {
        printf("ERR: Invalid graph topology\n");
        return 1;
    }

    // Find the node with the absolute lowest latency
    int best_server_id = -1;
    int min_latency = INF;

    for (int i = 1; i <= n; i++) {
        if (i != source_node && d[i] < min_latency) {
            min_latency = d[i];
            best_server_id = i;
        }
    }

    // Output ONLY the integer ID of the best server so Java can use it
    if (best_server_id != -1) {
        printf("%d\n", best_server_id); 
    } else {
        printf("ERR: No reachable servers\n");
    }

    return 0;
}
