import com.sun.net.httpserver.HttpServer;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import java.io.*;
import java.net.InetSocketAddress;
import java.util.concurrent.Executors;
import java.util.concurrent.ConcurrentHashMap;
import java.util.Map;

public class ApiGateway {

    // Tracks FileID -> [Weight(Size), Value(HitCount)]
    static ConcurrentHashMap<Integer, int[]> fileStats = new ConcurrentHashMap<>();
    static final int MAX_CACHE_CAPACITY = 50; 

    private static String getCurrentNetworkState() {
        int lat1 = (int)(Math.random() * 50) + 10;
        int lat2 = (int)(Math.random() * 50) + 10;
        int lat3 = (int)(Math.random() * 50) + 10;
        return "1:" + lat1 + ",2:" + lat2 + ",3:" + lat3;
    }

    public static void main(String[] args) throws Exception {
        
        // 1. INITIALIZE STATE BEFORE ACCEPTING TRAFFIC
        fileStats.put(101, new int[]{10, 50}); 
        fileStats.put(102, new int[]{20, 30}); 
        fileStats.put(103, new int[]{30, 40}); 

        HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
        server.createContext("/api/request", new RouteHandler());
        server.createContext("/api/optimize_cache", new CacheOptimizationHandler());
        
        server.setExecutor(Executors.newCachedThreadPool()); 
        System.out.println("GATEWAY ACTIVE: Listening on port 8080.");
        server.start();
    }

    static class RouteHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            String networkState = getCurrentNetworkState();
            String optimalNode = "ERR";
            try {
                ProcessBuilder pb = new ProcessBuilder("./native/build/router.bin");
                Process process = pb.start();
                try (BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()))) {
                    writer.write(networkState + "\n");
                    writer.flush();
                }
                try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
                    optimalNode = reader.readLine();
                }
                process.waitFor();
            } catch (Exception e) {
                System.err.println("IPC Failure (Router)");
            }

            String response = String.format("{\"status\": \"success\", \"routed_to_node\": %s}", optimalNode);
            exchange.sendResponseHeaders(200, response.getBytes().length);
            OutputStream os = exchange.getResponseBody();
            os.write(response.getBytes());
            os.close();
        }
    }

    static class CacheOptimizationHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            StringBuilder payloadBuilder = new StringBuilder();
            payloadBuilder.append(MAX_CACHE_CAPACITY).append(",").append(fileStats.size());
            
            for (Map.Entry<Integer, int[]> entry : fileStats.entrySet()) {
                payloadBuilder.append(",")
                              .append(entry.getKey()).append(":")      
                              .append(entry.getValue()[0]).append(":") 
                              .append(entry.getValue()[1]);            
            }
            
            String cacheStatePayload = payloadBuilder.toString();
            String survivingFiles = "COMPUTATION_FAILED";

            try {
                ProcessBuilder pb = new ProcessBuilder("./native/build/cache_optimizer.bin");
                Process process = pb.start();
                try (BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()))) {
                    writer.write(cacheStatePayload + "\n");
                    writer.flush();
                }
                try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
                    String line = reader.readLine();
                    if (line != null && !line.trim().isEmpty()) {
                        survivingFiles = line;
                    }
                }
                process.waitFor();
            } catch (Exception e) {
                System.err.println("IPC Failure (Cache Optimizer): " + e.getMessage());
            }

            String response = String.format(
                "{\n  \"status\": \"cache_optimized\",\n  \"capacity_mb\": %d,\n  \"files_retained_in_ram\": [%s]\n}\n", 
                MAX_CACHE_CAPACITY, 
                survivingFiles
            );
            
            exchange.sendResponseHeaders(200, response.getBytes().length);
            OutputStream os = exchange.getResponseBody();
            os.write(response.getBytes());
            os.close();
        }
    }
}
