FROM debian:trixie AS builder

WORKDIR /URLShortener

RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    cmake \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-date-time-dev \
    libboost-json-dev \
    libssl-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN mkdir -p build

WORKDIR /URLShortener/build
RUN cmake .. && cmake --build . -- -j$(nproc)

FROM debian:trixie-slim AS staging

WORKDIR /URLShortener

RUN apt-get update && apt-get install -y --no-install-recommends \
    libboost-system1.83.0 \
    libboost-thread1.83.0 \
    libboost-date-time1.83.0 \
    libboost-json1.83.0 \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /URLShortener/build/URLShortener .

EXPOSE 8080

RUN chmod +x URLShortener

CMD ["./URLShortener"]