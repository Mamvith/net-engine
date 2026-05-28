import java.io.*;

public class NativeBridge {
    public static void main(String[] args) {
        try {
            // Define the absolute path to your compiled native C binary
            ProcessBuilder pb = new ProcessBuilder("./native/build/router.bin");
            Process process = pb.start();

            // Open the stream to write data directly into the C binary's stdin
            try (BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()))) {
                String mockNetworkState = "server1:10ms,server2:45ms";
                writer.write(mockNetworkState);
                writer.flush(); 
            }

            // Open the stream to read the computed result from the C binary's stdout
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
                String response = reader.readLine();
                System.out.println("Java Gateway Log: " + response);
            }

            int exitCode = process.waitFor();
            if (exitCode != 0) {
                System.err.println("Execution Error: C process exited with code " + exitCode);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

