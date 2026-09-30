# Net-Engine

**Net-Engine** is a hybrid API Gateway and Network Routing Engine designed with a two-tier architecture. It combines the web-handling capabilities of **Java** with the high-performance computational power of **C**.

## Architecture Overview

The project is split into two primary layers that communicate with each other using Inter-Process Communication (IPC) via `stdin` and `stdout`.

### 1. Java API Gateway (`gateway/`)
A front-facing web server built using Java's native `HttpServer` (running on port `8080`). It acts as the orchestrator, accepting HTTP requests and delegating complex algorithmic tasks to the native C binaries.

**Key Endpoints:**
- `GET /api/request`: Simulates incoming network traffic, fetches current network server latencies, and asks the native router engine to compute the optimal node to route the request to.
- `GET /api/optimize_cache`: Manages file states (size/weight and hits/value). It passes this data to the native cache optimizer to figure out exactly which files to keep in memory to maximize efficiency without exceeding the maximum capacity (50 MB).

### 2. Native Compute Engine (`native/`)
A collection of C programs designed to handle intensive algorithmic processing. By offloading these tasks to C, the system avoids bogging down the Java web server with heavy computations.

**Components:**
- **`cache_optimizer.c`**: Implements the **0/1 Knapsack Dynamic Programming** algorithm. It receives cache capacity and file statistics from the Java Gateway, computes the optimal set of files to retain in memory, and returns the surviving file IDs. Note: The DP logic and backtracking are provided as skeleton code for educational/lab completion.
- **`router.bin`**: Computes the optimal network node based on the simulated server latencies provided by the Gateway.

## How it works

1. The Java `ApiGateway` receives an HTTP request.
2. It formats the required state (e.g., current cache capacity and file stats) into a serialized string.
3. It spawns a native C process using `ProcessBuilder` (e.g., `./native/build/cache_optimizer.bin`).
4. It pipes the serialized data directly into the C binary's `stdin`.
5. The C binary parses the string, runs its algorithm (like the Knapsack problem), and prints the result to `stdout`.
6. The Java Gateway reads the `stdout` response, parses it, and returns the final JSON response to the user over HTTP.

## Educational Note
Some parts of the native algorithms (like the DP table generation and backtracking in `cache_optimizer.c`) are left intentionally blank as an exercise for lab work.
