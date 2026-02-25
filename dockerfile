FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu

# Install GCC and Make
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    make \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
