# Base image (ARM version will be pulled automatically on Raspberry Pi)
FROM ubuntu:22.04

# Install build tools and utilities used by your scripts
RUN apt-get update && apt-get install -y \
    build-essential \
    iproute2 \
    iputils-ping \
    net-tools \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /ipc

# Copy everything (Makefile, ipc-static, scripts, etc.)
COPY . .

# Build the benchmark binary using Makefile
RUN make

# Ensure shell scripts are executable
RUN chmod +x *.sh

# Default entrypoint 
ENTRYPOINT ["./ipc-template.sh"]
