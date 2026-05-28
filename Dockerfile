FROM eclipse-temurin:21-jdk-jammy
RUN apt-get update && apt-get install -y gcc libc-dev && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY gateway/ gateway/
COPY native/ native/
RUN mkdir -p native/build
RUN gcc -O3 native/router.c -o native/build/router.bin
RUN gcc -O3 native/cache_optimizer.c -o native/build/cache_optimizer.bin
RUN javac gateway/src/ApiGateway.java
EXPOSE 8080
CMD ["java", "-cp", "gateway/src", "ApiGateway"]
